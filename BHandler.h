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
    Top10 top10;
public:
    BHandler(string directoryPath);{
        vectorTrees = vector<BPlusTree<TokenInfo>>();
        loadBooksFromDirectory(directoryPath);
    }

    void setStopWords(unordered_set<string>& stopWords) {
        tp.setStopWords(stopWords);
    }

    void loadBooksFromDirectory(const string& directoryPath) {
        for (const auto& file : std::filesystem::directory_iterator(directoryPath)) {
            BPlusTree <TokenInfo> bpt(GRADO);
            auto paragraphs = tp.tokenizeFileByParagraphs(ifstream bookFile(file.path()));
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
            foundTokenInfos = bpt.searchAll(TokenInfo{token, {}});

            if (foundTokenInfos.empty()) {
                allTokensFound = false;
            }
        }

        if (!allTokensFound) {
            cout << "!! No se encontraron todos los tokens" << endl;
        }
        return foundTokenInfos;
    }

    vector<ParagraphScore> calculateScores(const vector<string>& tokens, const vector<TokenInfo*>& foundTokenInfos) {
        map<int, ParagraphScore> paragraphScores;
        for (TokenInfo* tokenInfo : foundTokenInfos) {
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

    /*
        Funcion que retorna un vector con los top 10 libros
        - para saber si un libro entra en el top10, la suma del score de sus top 3 parrafos debe ser mayor a la del ultimo libro en el top10
    */ 

    vector<Top10> buscarFrase(const string& phraseToSearch) {
        vector<Book> top10;
        vector<string> tokens = tp.tokenize(phraseToSearch, true);
        for (BPlusTree<TokenInfo>& bpt : vectorTrees) {
            vector<TokenInfo*> foundTokenInfos = findTokens(tokens, bpt);
            vector<ParagraphScore> sortedScores = calculateScores(tokens, foundTokenInfos);
            for (const auto& score : sortedScores) {
                cout << "Archivo" << bpt.getFileName() << " - Parrafo " << score.paragraphIndex << " - Puntuación: " << score.score << endl;
            }
            cout << "\n---------------------------------------------------------------------------------"<<endl;
        } return top10;
    }
}
#endif