CLASS TREE.H

#ifndef TREE_H
#define TREE_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm> 

//menjelaskan taxonomic yg fix untuk level strukturnya
const std::vector<std::string> TAX_LEVELS = {
    "Class", "Order", "Family", "Genus", "Species"
};
const size_t REQUIRED_TAX_LEVELS = TAX_LEVELS.size(); 
const size_t REQUIRED_TOTAL_INPUTS = TAX_LEVELS.size() + 1;

// sruktur nodenya
struct Node {
    std::string name;             
    std::string level;            
    std::string commonName;       
    std::string wikiLink;         // Link Wikipedia
    std::vector<Node*> children;  
};

// --- FUNGSI UTILITY ---
std::string toLower(const std::string& str); 
Node* createNode(const std::string& name, const std::string& level);

// --- FUNGSI CRUD: CREATE (Add) ---
Node* addSpeciesPath(Node* root, const std::vector<std::string>& path, const std::string& commonName, const std::string& wikiLink);

// --- FUNGSI CRUD: READ (Search/Display) ---
Node* searchNode(Node* root, const std::string& name);
void displayTree(Node* root, int depth = 0);

// --- FUNGSI CRUD: UPDATE ---
bool updateSpecies(Node* speciesNode, const std::string& newCommonName, const std::string& newWikiLink);

// --- FUNGSI CRUD: DELETE ---
// Parent diperlukan untuk menghapus node dari children vector
Node* deleteSpecies(Node* root, const std::string& speciesName, Node* parent = nullptr);
void deleteTree(Node* root);

// --- FUNGSI TRAVERSAL ---
void preOrderTraversal(Node* root);
void postOrderTraversal(Node* root);
void levelOrderTraversal(Node* root); 

#endif