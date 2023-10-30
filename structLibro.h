#include <string>
#include <vector>

using namespace std;

struct Book {
    string author;
    string title;
    string content;
    string path;
    vector<string> keywords;

    Book(string author, string title, string content, string path, vector<string> keywords) {
        this->author = author;
        this->title = title;
        this->content = content;
        this->path = path;
        this->keywords = keywords;
    }

    string getAuthor(){
        return author;
    }
    string getTitle(){
        return title;
    }
    string getContent(){
        return content;
    }
    string getPath(){
        return path;
    }
    vector<string> getKeywords(){
        return keywords;
    }
};