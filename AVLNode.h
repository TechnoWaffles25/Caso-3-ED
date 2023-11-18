#ifndef AVLNODE_H
#define AVLNODE_H
#include <string>
using namespace std;

struct AVLNode {
    string title;
    string author;
    string genre;
    string filepath;
    AVLNode *left, *right;
    int height;
    AVLNode(string t, string a, string g, string f)
        : title(t), author(a), genre(g), filepath(f), left(nullptr), right(nullptr), height(1) {}
};
#endif