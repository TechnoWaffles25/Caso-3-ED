#include "AVLTree.h"
#include <iostream>

using namespace std;
int main() {
    AVLTree avlTree;

    string directoryPath = "Books/";

    // Load books from the directory into the AVL tree
    avlTree.loadBooksFromDirectory(directoryPath, avlTree);
    avlTree.printInOrder(avlTree.root);

    cout << endl;

    vector<AVLNode*> libros = avlTree.search("Novel");
    for (int i = 0; i < libros.size(); i++) {
        cout << libros[i]->title << endl;
    }
    return 0;
}
