#ifndef AVL_TREE_H
#define AVL_TREE_H
#include "AVLNode.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

class AVLTree {
private:
    // Function to search nodes by title, author, or genre
    void search(AVLNode* node, const std::string& searchString, std::vector<AVLNode*>& results) {
        if (node == nullptr) {
            return;
        }

        // Check if the current node's title, author, or genre matches the search string
        if (node->title.find(searchString) != std::string::npos ||
            node->author.find(searchString) != std::string::npos ||
            node->genre.find(searchString) != std::string::npos) {
            results.push_back(node);
        }

        // Recursively search in the left and right subtrees
        search(node->left, searchString, results);
        search(node->right, searchString, results);
    }
public:
    // Public interface to search the tree
    std::vector<AVLNode*> search(const std::string& searchString) {
        std::vector<AVLNode*> results;
        search(root, searchString, results);
        return results;
    }
    AVLNode* root;

    AVLTree() : root(nullptr) {}

    int height(AVLNode* N) {
        if (N == nullptr)
            return 0;
        return N->height;
    }

    AVLNode* rightRotate(AVLNode* y) {
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;

        // Realizar rotación
        x->right = y;
        y->left = T2;

        // Actualizar alturas
        y->height = max(height(y->left), height(y->right)) + 1;
        x->height = max(height(x->left), height(x->right)) + 1;

        // Devolver nueva raíz
        return x;
    }

    AVLNode* leftRotate(AVLNode* x) {
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;

        // Realizar rotación
        y->left = x;
        x->right = T2;

        // Actualizar alturas
        x->height = max(height(x->left), height(x->right)) + 1;
        y->height = max(height(y->left), height(y->right)) + 1;

        // Devolver nueva raíz
        return y;
    }

    int getBalance(AVLNode* N) {
        if (N == nullptr)
            return 0;
        return height(N->left) - height(N->right);
    }

    AVLNode* insert(AVLNode* node, string title, string author, string genre, string filepath) {
        // 1. Realizar la inserción normal de BST
        if (node == nullptr)
            return(new AVLNode(title, author, genre, filepath));

        if (title < node->title)
            node->left = insert(node->left, title, author, genre, filepath);
        else if (title > node->title)
            node->right = insert(node->right, title, author, genre, filepath);
        else // Los títulos duplicados no están permitidos en el BST
            return node;

        // 2. Actualizar la altura del nodo ancestro
        node->height = 1 + max(height(node->left), height(node->right));

        // 3. Obtener el factor de equilibrio del nodo ancestro para verificar si este nodo se desequilibró
        int balance = getBalance(node);

        // Si el nodo se desequilibra, hay 4 casos

        // Caso izquierda izquierda
        if (balance > 1 && title < node->left->title)
            return rightRotate(node);

        // Caso derecha derecha
        if (balance < -1 && title > node->right->title)
            return leftRotate(node);

        // Caso izquierda derecha
        if (balance > 1 && title > node->left->title) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }

        // Caso derecha izquierda
        if (balance < -1 && title < node->right->title) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }

        // Devolver el puntero del nodo (sin cambios)
        return node;
    }

    void printInOrder(AVLNode* node) {
        if (node != nullptr) {
            printInOrder(node->left);
            std::cout << "Title: " << node->title << ", Author: " << node->author << ", Genre: " << node->genre << std::endl;
            printInOrder(node->right);
        }
    }

    void loadBooksFromDirectory(const string& directoryPath, AVLTree& avlTree) {
        for (const auto& file : std::filesystem::directory_iterator(directoryPath)) {
            std::ifstream bookFile(file.path());
            std::string title, author, genre;
            getline(bookFile, title);
            getline(bookFile, author);
            getline(bookFile, genre);
            avlTree.root = avlTree.insert(avlTree.root, title, author, genre, file.path());

        }
    }
};
#endif
