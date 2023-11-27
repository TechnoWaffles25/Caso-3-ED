#ifndef BHANDLER_H
#define BHANDLER_H

#include "processTxt.h"
#include "ParagraphScore.h"
#include "structBook.h"
#include "B+Tree.h"
#include "token.h"
#include "Top10.h"

#include <unordered_set>
#include <map>
#include <string>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <vector>
#include <fstream>

using namespace std;
const int GRADO = 50;

class BHandler {
private:
    vector<BPlusTree<TokenInfo>> vectorTrees;
    TextProcessing tp;
    map<string, Book> books;
    Top10 top10;

public:
    BHandler(string directoryPath) {
        vectorTrees = vector<BPlusTree<TokenInfo>>();
        loadBooksFromDirectory(directoryPath);
    }
    

    void setStopWords(unordered_set<string>& stopWords) {
        tp.setStopWords(stopWords);
    }

    void loadBooksFromDirectory(const string& directoryPath) {
        for (const auto& file : filesystem::directory_iterator(directoryPath)) {
            ifstream bookFile(file.path().string());
            string title, author, genre, filepath;

            // Leer las primeras cuatro líneas para obtener la información del libro
            getline(bookFile, title);
            getline(bookFile, author);
            getline(bookFile, genre);
            filepath = file.path().filename().string();

            // Crear una instancia de Book y almacenarla
            Book book(title, author, genre, filepath);
            books[filepath] = book;

            // Procesar el archivo para construir el árbol B+
            BPlusTree<TokenInfo> bpt(GRADO, file.path().string());
            auto paragraphs = tp.tokenizeFileByParagraphs(file.path().string());
            for (const auto& paragraphPair : paragraphs) {
                for (const auto& tokenInfo : paragraphPair.second) {
                    bpt.insert(tokenInfo);
                }
            }
            vectorTrees.push_back(bpt);
        }
    }


    vector<TokenInfo*> findTokens(const vector<string>& tokens, BPlusTree<TokenInfo>& bpt) {
        vector<TokenInfo*> foundTokenInfos;
        bool allTokensFound = true;

        for (const auto& token : tokens) {
            auto tokenInfos = bpt.searchAll(TokenInfo{token, {}});
            if (!tokenInfos.empty()) {
                foundTokenInfos.insert(foundTokenInfos.end(), tokenInfos.begin(), tokenInfos.end());
            } else {
                allTokensFound = false;
                break;
            }
        }

        if (!allTokensFound) {
            cout << "!! No se encontraron todos los tokens" << endl;
            foundTokenInfos.clear();
        }
        return foundTokenInfos;
    }

    vector<ParagraphScore> calculateScores(const vector<string>& tokens, const vector<TokenInfo*>& foundTokenInfos) {
        map<int, ParagraphScore> paragraphScores;
        for (const TokenInfo* tokenInfo : foundTokenInfos) {
            for (const auto& position : tokenInfo->positions) {
                auto it = paragraphScores.find(position.paragraphIndex);
                if (it == paragraphScores.end()) {
                    it = paragraphScores.insert({position.paragraphIndex, ParagraphScore(position.paragraphIndex)}).first;
                }
                it->second.addTokenInfo(tokenInfo);
            }
        }

        for (auto& pair : paragraphScores) {
            pair.second.calculateScore(tokens);
        }

        vector<ParagraphScore> sortedScores;
        for (const auto& pair : paragraphScores) {
            sortedScores.push_back(pair.second);
        }

        sort(sortedScores.begin(), sortedScores.end(), 
            [](const ParagraphScore& a, const ParagraphScore& b) {
                return a.score > b.score;
            });

        return sortedScores;
    }

    vector<Book> buscarFrase(const string& phraseToSearch) {
        vector<Book> top10Books;
        vector<string> tokens = tp.tokenize(phraseToSearch, true);

        for (BPlusTree<TokenInfo>& bpt : vectorTrees) {
            vector<TokenInfo*> foundTokenInfos = findTokens(tokens, bpt);
            if (!foundTokenInfos.empty()) {
                vector<ParagraphScore> sortedScores = calculateScores(tokens, foundTokenInfos);

                // Sumar los puntajes de los 3 mejores párrafos
                int totalScore = 0;
                for (int i = 0; i < 3 && i < sortedScores.size(); ++i) {
                    totalScore += sortedScores[i].score;
                }

                // Actualizar la puntuación del libro correspondiente
                string filepath = bpt.getFileName(); // Asegúrate de que BPlusTree tenga un método getFileName
                if (books.find(filepath) != books.end()) {
                    books[filepath].setScore(totalScore);
                    top10.addBook(books[filepath]);
                }
            }
        }

        // Obtener los top 10 libros
        top10Books = top10.getTop10();

        return top10Books;
    }

};

#endif
