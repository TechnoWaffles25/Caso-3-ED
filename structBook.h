#ifndef BOOK_H
#define BOOK_H

#include <string>

using namespace std;

struct Book {
    private:
        string title;
        string author;
        string genre;
        string filepath;
        int score;

    public:
        Book(string t, string a, string g, string f)
            : title(t), author(a), genre(g), filepath(f), score(0) {}

        // Getters
        string getTitle() const { return title; }
        string getAuthor() const { return author; }
        string getGenre() const { return genre; }
        string getFilepath() const { return filepath; }
        int getScore() const { return score; }

        // Setters
        void setTitle(const string& t) { title = t; }
        void setAuthor(const string& a) { author = a; }
        void setGenre(const string& g) { genre = g; }
        void setFilepath(const string& f) { filepath = f; }
        void setScore(int s) { score = s; }
};

#endif