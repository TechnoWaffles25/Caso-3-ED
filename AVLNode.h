#include "structLibro.h"

struct AVLNode {
    Book book;
    AVLNode* left;
    AVLNode* right;
    int height; // Altura del nodo en el árbol

    AVLNode(const Book& book) : book(book), left(nullptr), right(nullptr), height(1) {}
};