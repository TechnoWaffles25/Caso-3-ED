#ifndef PROCESSBOOK_H
#define PROCESSBOOK_H

#include <string>
#include <vector>
#include <unordered_set>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <stdexcept>
#include <iostream>

#include "token.h"

using namespace std;


// Sobrecargamos el operador << para que maneje objetosTokenPosition
std::ostream& operator<<(std::ostream& os, const TokenPosition& position) {
    os << "Paragraph: " << position.paragraphIndex << ", Word: " << position.wordIndex;
    return os;
}

// Sobrecargamos el operador << para que maneje objetos TokenInfo
std::ostream& operator<<(std::ostream& os, const TokenInfo& info) {
    // Imprime el token
    os << info.token;

    // Imprime las posiciones del token si hay alguna
    if (!info.positions.empty()) {
        os << " [";
        for (size_t i = 0; i < info.positions.size(); ++i) {
            if (i > 0) {
                os << ", ";
            }
            os << info.positions[i];
        }
        os << "]";
    }

    // Regresa os para poder encadenar impresiones
    return os;
}

// Sobrecargamos el operador << para que maneje objetos pair<int, vector<TokenInfo>>
bool operator==(const TokenPosition& lhs, const TokenPosition& rhs) {
    return lhs.paragraphIndex == rhs.paragraphIndex && lhs.wordIndex == rhs.wordIndex;
}

// Sobrecargamos el operador << para que maneje objetos pair<int, vector<TokenInfo>>
bool operator<(const TokenPosition& lhs, const TokenPosition& rhs) {
    if (lhs.paragraphIndex == rhs.paragraphIndex) {
        return lhs.wordIndex < rhs.wordIndex;
    }
    return lhs.paragraphIndex < rhs.paragraphIndex;
}

// Clase que procesa el texto de un archivo txt
class TextProcessing {
private:
    // StopWords a procesar
    unordered_set<string> stopWords;

public:
    // Regresa las stopWords que se están utilizando
    unordered_set<string> getStopWords() const {
        return stopWords;
    }
    // Setter para las StopWords por medio de un unordered_set
    void setStopWords(const unordered_set<string>& stopWords) {
        this->stopWords = stopWords;
    }

    // Tokeniza un texto quitandole la puntuación
    vector<string> tokenize(const string& text, bool keepPunctuation = false) const {
        vector<string> tokens;
        istringstream iss(text);
        string token;
        while (iss >> token) {
            if (!keepPunctuation) {
                token = removePunctuation(token);
            }
            if (!token.empty()) {
                tokens.push_back(token);
            }
        }
        return tokens;
    }

    // Convierte un string a minúsculas
    string toLower(const string& str) const {
        string lowerStr = str;
        transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
                       [](unsigned char c) { return tolower(c); });
        return lowerStr;
    }

    // Remueve la puntuación de un token
    string removePunctuation(const string& token) const {
        string newToken;
        remove_copy_if(token.begin(), token.end(), back_inserter(newToken),
                            [](char c) { return ispunct(c); });
        return newToken;
    }

    // Remueve las StopWords de un vector de tokens
    vector<string> removeStopWords(const vector<string>& tokens) const {
        vector<string> filteredTokens;
        for (const auto& token : tokens) {
            if (stopWords.find(token) == stopWords.end()) {
                filteredTokens.push_back(token);
            }
        }
        return filteredTokens;
    }

    // Sin utilizar
    vector<string> processText(const string& text) const {
        string lowerText = toLower(text);
        auto tokens = tokenize(lowerText);
        return removeStopWords(tokens);
    }

    // Sin utilizar
    vector<string> tokenizeFileFromLine4(const string& filepath) const {
        ifstream file(filepath);
        if (!file.is_open()) {
            throw runtime_error("Unable to open file: " + filepath);
        }

        // Skip the first three lines
        string line;
        for (int i = 0; i < 3; ++i) {
            if (!getline(file, line)) {
                throw runtime_error("File does not have enough lines.");
            }
        }

        // Read and tokenize the rest of the file
        vector<string> allTokens;
        while (getline(file, line)) {
            vector<string> tokens = processText(line); // processText already converts to lower case and removes stop words
            allTokens.insert(allTokens.end(), tokens.begin(), tokens.end());
        }

        file.close();
        return allTokens; // No need to remove stop words again
    }

    // Función modificada para procesar el archivo por párrafos y registrar la posición de los tokens
    vector<pair<int, vector<TokenInfo>>> tokenizeFileByParagraphs(const string& filepath) const {
        // Abre el archivo
        ifstream file(filepath);
        if (!file.is_open()) {
            throw runtime_error("Unable to open file: " + filepath);
        }

        // Salta las primeras 3 lineas par allegar al contenido
        string line;
        for (int i = 0; i < 3; ++i) {
            getline(file, line);
        }

        // Creamos un vector de pares de enteros y vectores de TokenInfo para guardar los párrafos por numero y los tokens que contiene
        vector<pair<int, vector<TokenInfo>>> paragraphs;
        unordered_map<string, TokenInfo> paragraphTokens;
        string paragraph;
        int paragraphIndex = 0;

        // Mientras haya lineas en el archivo
        while (getline(file, line)) {
            if (line.empty()) { // Si la linea esta vacia, significa que es un nuevo parrafo
                if (!paragraph.empty()) {
                    // Vector de tokens dentro del parrafo
                    vector<TokenInfo> tokensInfo;
                    // Tokenizamos el parrafo y remueve puntuacion
                    vector<string> tokens = tokenize(paragraph, true); 
                    int wordIndex = 0; // Posicion del token dentro del parrafo
                    for (const auto& token : tokens) {
                        TokenPosition position = {paragraphIndex, wordIndex++};
                        // Si el token no existe en el mapa de tokens del parrafo, lo agrega
                        if (paragraphTokens.find(token) == paragraphTokens.end()) {
                            paragraphTokens[token] = TokenInfo{token};
                        }
                        // Agrega la posicion del token al vector de posiciones del token
                        paragraphTokens[token].positions.push_back(position);
                    }
                    // Agrega los tokens del parrafo al vector de tokens del parrafo
                    for (auto& pair : paragraphTokens) {
                        tokensInfo.push_back(std::move(pair.second));
                    }
                    paragraphs.push_back({paragraphIndex++, tokensInfo});
                    paragraph.clear();
                    paragraphTokens.clear(); // Limpia los tokens para el siguiente parrafo
                }
            } else {
                paragraph += line + " ";
            }
        }

        // Maneja el ultimo parrafo si no termina con una linea vacia
        if (!paragraph.empty()) {
            vector<TokenInfo> tokensInfo;
            vector<string> tokens = tokenize(paragraph, true);
            int wordIndex = 0;
            for (const auto& token : tokens) {
                TokenPosition position = {paragraphIndex, wordIndex++};
                if (paragraphTokens.find(token) == paragraphTokens.end()) {
                    paragraphTokens[token] = TokenInfo{token};
                }
                paragraphTokens[token].positions.push_back(position);
            }
            for (auto& pair : paragraphTokens) {
                tokensInfo.push_back(std::move(pair.second));
            }
            paragraphs.push_back({paragraphIndex, tokensInfo});
        }

        file.close();
        return paragraphs;
    }
};

#endif