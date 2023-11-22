#ifndef TOP10_H
#define TOP10_H

#include "structBook.h"
#include <string>
#include <vector>
#include <algorithm>

class Top10 {
    private:
        vector<Book> top10;
    public:
        Top10(){}

        void addBook(const Book& book) {
            // Insert the new book in the correct position
            auto it = std::lower_bound(top10.begin(), top10.end(), book, 
                                    [](const Book& a, const Book& b) {
                                        return a.getScore() > b.getScore();
                                    });
            top10.insert(it, book);

            // If there are more than 10 books, remove the one with the lowest score
            if (top10.size() > 10) {
                top10.pop_back();
            }
        }

        vector<Book> getTop10() {
            return top10;
        }

        void printTop10() {
            // Implement this method to print the top 10 books
        }
};

#endif