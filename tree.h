#ifndef TREE_H
#define TREE_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm> // Diperlukan untuk std::remove di tree.cpp

// Fixed taxonomic structure levels
const std::vector<std::string> TAX_LEVELS = {
    "Class", "Order", "Family", "Genus", "Species"
};
const size_t REQUIRED_TAX_LEVELS = TAX_LEVELS.size(); // 5 taxonomic levels

// Node structure
struct Node {
    std::string name;             //"Chondrichthyes", "carcharias" (taxonomic name)
    std::string level;            //"Class", "Species" 
    std::string commonName;       //"Great White Shark" (only for Species level)
    std::string wikiLink;         // <-- NEW: Link Wikipedia
    std::vector<Node*> children;  // list of child node pointers
};

// --- UTILITY FUNCTIONS ---
std::string toLower(const std::string& str); 
Node* createNode(const std::string& name, const std::string& level);
Node* findChild(Node* parent, const std::string& name); // Re-declared for completeness

// --- CRUD FUNCTIONS: CREATE (Add) ---
// Updated parameter to accept wikiLink
Node* addSpeciesPath(Node* root, const std::vector<std::string>& path, const std::string& commonName, const std::string& wikiLink);

// --- CRUD FUNCTIONS: READ (Search/Display) ---
Node* searchNode(Node* root, const std::string& name);
void displayTree(Node* root, int depth = 0);

// --- CRUD FUNCTIONS: UPDATE ---
// Implemented to address the linker error
bool updateSpecies(Node* speciesNode, const std::string& newCommonName, const std::string& newWikiLink);

// --- CRUD FUNCTIONS: DELETE ---
// Implemented to address the linker error
Node* deleteSpecies(Node* root, const std::string& speciesName); // Simplified signature for main.cpp call

// Function to delete the entire tree
void deleteTree(Node* root);

#endif
