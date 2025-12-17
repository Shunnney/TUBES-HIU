#include "tree.h"
#include <algorithm> // For std::transform
#include <cctype>    // For ::tolower
#include <iostream>  // For std::cerr and std::cout
#include <vector>    // For std::vector

/**
 * @brief Helper function to convert a string to lowercase for case-insensitive comparison.
 */
std::string toLower(const std::string& str) {
    std::string data = str;
    std::transform(data.begin(), data.end(), data.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return data;
}

/**
 * @brief Creates a new tree node.
 * @param name The taxonomic name (e.g., "Carcharodon").
 * @param level The taxonomic level (e.g., "Genus").
 * @return Node* A pointer to the newly created node.
 */
Node* createNode(const std::string& name, const std::string& level) {
    Node* newNode = new Node();
    newNode->name = name;
    newNode->level = level;
    newNode->commonName = ""; 
    newNode->wikiLink = ""; // Initialize new member
    // children vector is initialized as empty
    return newNode;
}

/**
 * @brief Recursively finds a child node with a specific name.
 * @param parent The current node to search its children.
 * @param name The name of the child to find.
 * @return Node* The pointer to the found child, or nullptr if not found.
 */
Node* findChild(Node* parent, const std::string& name) {
    // Using case-insensitive comparison for finding existing nodes
    std::string lowerName = toLower(name);
    for (Node* child : parent->children) {
        if (toLower(child->name) == lowerName) {
            return child;
        }
    }
    return nullptr;
}

/**
 * @brief Inserts a full taxonomic path (Class down to Species) into the tree.
 * @param root The root of the overall tree or the current node for recursion.
 * @param path A vector of strings representing the 5 taxonomic names (Class to Species).
 * @param commonName The common name of the species to be stored on the Species node.
 * @param wikiLink The Wikipedia link for the species. <-- NEW PARAMETER
 * @return Node* The (possibly new) root of the tree.
 */
Node* addSpeciesPath(Node* root, 
                     const std::vector<std::string>& path, 
                     const std::string& commonName,
                     const std::string& wikiLink) { // <-- FIX: Added wikiLink
    
    if (path.size() != REQUIRED_TAX_LEVELS) {
        std::cerr << "Internal Error: Path size mismatch in addSpeciesPath.\n";
        return root;
    }

    Node* currentNode = root;
    
    // Safety check for the root name (Class level)
    if (root != nullptr) {
        std::string newClassNameLower = toLower(path[0]);
        if (toLower(currentNode->name) != newClassNameLower) {
             std::cerr << "Error: The tree already has a Class: " << currentNode->name 
                       << ". All species must belong to the same Class.\n";
             return root;
        }
    }

    for (size_t i = 0; i < path.size(); ++i) {
        const std::string& name = path[i];
        std::string level = TAX_LEVELS[i];

        if (i == 0) {
            // Handling the absolute root (Class level)
            if (currentNode == nullptr) {
                currentNode = createNode(name, level);
                root = currentNode;
            }
            // If currentNode is not null and the Class names match (case-insensitive), continue.
        } else {
            // Handling nodes from Order down to Species
            Node* existingChild = findChild(currentNode, name);
            
            if (existingChild) {
                currentNode = existingChild;
                
                // If the Species node already exists, update its common name and wiki link
                if (i == path.size() - 1) {
                    if (existingChild->commonName != commonName || existingChild->wikiLink != wikiLink) {
                         std::cout << "[INFO] Species '" << name << "' already exists. Updating common name/link.\n";
                         existingChild->commonName = commonName;
                         existingChild->wikiLink = wikiLink; // <-- FIX: Update wikiLink
                    } else {
                         std::cout << "[INFO] Species '" << name << "' already exists with the same details.\n";
                    }
                }
            } else {
                // Node does not exist, create it and link it
                Node* newNode = createNode(name, level);
                currentNode->children.push_back(newNode);
                
                // If this is the final Species node, set the common name and link
                if (i == path.size() - 1) {
                    newNode->commonName = commonName;
                    newNode->wikiLink = wikiLink; // <-- FIX: Set wikiLink
                    std::cout << "--> Successfully added new species: " << commonName << " (" << name << ")\n";
                } else {
                    std::cout << "--> Inserting new " << level << ": " << name << "\n";
                }
                
                currentNode = newNode;
            }
        }
    }
    
    return root;
}


/**
 * @brief Recursively searches the tree for a node with a matching taxonomic OR common name.
 * @param root The current node to start the search from.
 * @param name The name to search for.
 * @return Node* The pointer to the found node, or nullptr if not found.
 */
Node* searchNode(Node* root, const std::string& name) {
    if (root == nullptr) {
        return nullptr;
    }

    // Use case-insensitive comparison for search
    std::string lowerName = toLower(name);

    // 1. Check current node (Taxonomic name)
    if (toLower(root->name) == lowerName) {
        return root;
    }
    
    // 2. Check current node (Common name - only present on Species nodes)
    if (!root->commonName.empty() && toLower(root->commonName) == lowerName) {
        return root;
    }

    // Recursively check children
    for (Node* child : root->children) {
        Node* result = searchNode(child, name);
        if (result != nullptr) {
            return result; // Found the node in a subtree
        }
    }

    return nullptr; // Not found in this branch
}

/**
 * @brief Updates the common name and Wikipedia link for an existing species node.
 * @param speciesNode The pointer to the Species node to update.
 * @param newCommonName The new common name.
 * @param newWikiLink The new Wikipedia link.
 * @return true if the update was successful, false otherwise.
 */
bool updateSpecies(Node* speciesNode, const std::string& newCommonName, const std::string& newWikiLink) {
    if (speciesNode == nullptr || speciesNode->level != "Species") {
        std::cerr << "[ERROR] Cannot update: Node is null or not a Species.\n";
        return false;
    }
    
    if (newCommonName.empty()) {
        std::cerr << "[ERROR] Common Name cannot be updated to empty.\n";
        return false;
    }

    speciesNode->commonName = newCommonName;
    speciesNode->wikiLink = newWikiLink; 
    
    std::cout << "[SUCCESS] Species '" << speciesNode->name << "' details updated.\n";
    return true;
}


/**
 * @brief Searches for and deletes a Species node.
 * * NOTE: Since nodes lack parent pointers, this function traverses the tree 
 * until the species node is found in its parent's (Genus) children vector 
 * and then deletes it. It does *not* delete empty parent nodes.
 * * @param root The root of the tree.
 * @param speciesName The name (taxonomic or common) of the species to delete.
 * @return Node* The (possibly updated) root of the tree.
 */
Node* deleteSpecies(Node* root, const std::string& speciesName) {
    if (root == nullptr) {
        std::cout << "[INFO] Tree is empty.\n";
        return nullptr;
    }

    Node* targetNode = searchNode(root, speciesName);
    if (!targetNode || targetNode->level != "Species") {
        // searchNode already prints if not found, main.cpp handles the case.
        return root;
    }

    // Use the exact taxonomic name of the found node for case-sensitive vector removal
    std::string targetTaxName = targetNode->name;
    
    // Start traversal from the root (Class)
    // Level Path: Class (root) -> Order -> Family -> Genus (Parent) -> Species (Target)
    
    // Loop through Order nodes (children of the Class root)
    for (Node* orderNode : root->children) {
        // Loop through Family nodes (children of Order)
        for (Node* familyNode : orderNode->children) {
            // Loop through Genus nodes (children of Family, these are the potential parents)
            for (Node* genusNode : familyNode->children) {
                
                // Search the children of the Genus node for the Species target
                for (auto it = genusNode->children.begin(); it != genusNode->children.end(); ++it) {
                    // Check if the current child pointer points to the target node
                    // Use case-sensitive name check here because we are iterating the exact vector
                    if ((*it) == targetNode) {
                        delete *it; // Free the memory
                        genusNode->children.erase(it); // Remove the pointer from the vector
                        std::cout << "[SUCCESS] Species '" << speciesName << "' deleted successfully.\n";
                        return root; // Success
                    }
                }
            }
        }
    }
    
    // Should ideally not be reached if searchNode worked, but serves as a failsafe
    std::cerr << "[ERROR] Could not find the parent of species '" << speciesName << "' for deletion.\n";
    return root;
}


/**
 * @brief Displays the tree structure using a simple indentation for hierarchy.
 * @param root The current node to display.
 * @param depth The current depth (for indentation).
 */
void displayTree(Node* root, int depth) {
    if (!root) return;

    // Print indentation
    for (int i = 0; i < depth; ++i) {
        std::cout << (i == depth - 1 ? "  |--" : "  |  ");
    }

    // Print the node information
    std::cout << "(" << root->level << ") " << root->name;
    
    // If it's a species node and has a common name, display it
    if (root->level == "Species") {
        if (!root->commonName.empty()) {
            std::cout << " [" << root->commonName << "]";
        }
        if (!root->wikiLink.empty()) {
            // Display link abbreviation
            std::cout << " {W}";
        }
    }
    std::cout << "\n";

    // Recursively call for all children
    for (Node* child : root->children) {
        displayTree(child, depth + 1);
    }
}

/**
 * @brief Cleans up all dynamically allocated memory in the tree.
 * @param root The root of the tree to be deleted.
 */
void deleteTree(Node* root) {
    if (!root) return;
    for (Node* child : root->children) {
         deleteTree(child);
    }
    delete root;
}
