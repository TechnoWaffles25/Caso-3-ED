#include "AVLNode.h"
#include <iostream>

class AVLTree{
    private:
        AVLNode *root;
        int size;
        void insert(AVLNode *node, int value);
        void remove(AVLNode *node, int value);
        void print(AVLNode *node);
        void clear(AVLNode *node);
        int height(AVLNode *node);
        int balanceFactor(AVLNode *node);
        void rotateLeft(AVLNode *node);
        void rotateRight(AVLNode *node);
        void rebalance(AVLNode *node);
        AVLNode* findMin(AVLNode *node);
        AVLNode* findMax(AVLNode *node);
        AVLNode* find(AVLNode *node, int value);
    public:
        AVLTree();
        ~AVLTree();
        void insert(int value);
        void remove(int value);
        void print();
        void clear();
        int height();
        int balanceFactor();
        int size();
        bool empty();
        bool find(int value);
        int findMin();
        int findMax();
        
        AVLTree::AVLTree() {
            root = nullptr;
            size = 0;
        }

        AVLTree::~AVLTree() {
            clear();
        }

        void AVLTree::insert(Book value) {
            if (root == nullptr) {
                root = new AVLNode(value);
            } else {
                insert(root, value);
            }
            size++;
        }

        void AVLTree::insert(AVLNode *node, Book value) {
            if (value < node->book) {
                if (node->left == nullptr) {
                    node->left = new AVLNode(value);
                } else {
                    insert(node->left, value);
                }
            } else {
                if (node->right == nullptr) {
                    node->right = new AVLNode(value);
                } else {
                    insert(node->right, value);
                }
            }
            rebalance(node);
        }

        void AVLTree::remove(int value) {
            if (root == nullptr) {
                return;
            }
            remove(root, value);
            size--;
        }

        void AVLTree::remove(AVLNode *node, int value) {
            if (node == nullptr) {
                return;
            }
            if (value < node->book) {
                remove(node->left, value);
            } else if (value > node->book) {
                remove(node->right, value);
            } else {
                if (node->left == nullptr && node->right == nullptr) {
                    delete node;
                    node = nullptr;
                } else if (node->left == nullptr) {
                    AVLNode *temp = node;
                    node = node->right;
                    delete temp;
                } else if (node->right == nullptr) {
                    AVLNode *temp = node;
                    node = node->left;
                    delete temp;
                } else {
                    AVLNode *temp = findMin(node->right);
                    node->book = temp->book;
                    remove(node->right, temp->book);
                }
            }
            rebalance(node);
        }

        void AVLTree::print() {
            if (root == nullptr) {
                std::cout << "Tree is empty" << std::endl;
            } else {
                print(root);
            }
        }

        void AVLTree::print(AVLNode *node) {
            if (node != nullptr) {
                print(node->left);
                std::cout << node->book << " ";
                print(node->right);
            }
        }

        void AVLTree::clear() {
            clear(root);
            root = nullptr;
            size = 0;
        }

        void AVLTree::clear(AVLNode *node) {
            if (node != nullptr) {
                clear(node->left);
                clear(node->right);
                delete node;
            }
        }

        int AVLTree::height() {
            return height(root);
        }

        int AVLTree::height(AVLNode *node) {
            if (node == nullptr) {
                return -1;
            }
            return 1 + std::max(height(node->left), height(node->right));
        }

        int AVLTree::balanceFactor() {
            return balanceFactor(root);
        }

        int AVLTree::balanceFactor(AVLNode *node) {
            if (node == nullptr) {
                return 0;
            }
            return height(node->left) - height(node->right);
        }

        void AVLTree::rotateLeft(AVLNode *node) {
            AVLNode *temp = node->right;
            node->right = temp->left;
            temp->left = node;
            node = temp;
        }

        void AVLTree::rotateRight(AVLNode *node) {
            AVLNode *temp = node->left;
            node->left = temp->right;
            temp->right = node;
            node = temp;
        }

        void AVLTree::rebalance(AVLNode *node) {
            if (node == nullptr) {
                return;
            }
            if (balanceFactor(node) == -2) {
                if (balanceFactor(node->right) == 1) {
                    rotateRight(node->right);
                }
                rotateLeft(node);
            } else if (balanceFactor(node) == 2) {
                if (balanceFactor(node->left) == -1) {
                    rotateLeft(node->left);
                }
                rotateRight(node);
            }
        }

        int AVLTree::size() {
            return size;
        }

        bool AVLTree::empty() {
            return size == 0;
        }

        bool AVLTree::find(int value) {
            return find(root, value) != nullptr;
        }

        AVLNode* AVLTree::find(AVLNode *node, int value) {
            if (node == nullptr) {
                return nullptr;
            }
            if (value == node->book) {
                return node;
            } else if (value < node->book) {
                return find(node->left, value);
            } else {
                return find(node->right, value);
            }
        }

        Book AVLTree::findMin() {
            if (root == nullptr) {
                return -1;
            }
            return findMin(root)->book;
        }

        AVLNode* AVLTree::findMin(AVLNode *node) {
            if (node->left == nullptr) {
                return node;
            }
            return findMin(node->left);
        }

        Book AVLTree::findMax() {
            if (root == nullptr) {
                return;
            }
            return findMax(root)->book;
        }

        AVLNode* AVLTree::findMax(AVLNode *node) {
            if (node->right == nullptr) {
                return node;
            }
            return findMax(node->right);
        }
};