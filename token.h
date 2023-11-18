#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <vector>
#include <unordered_set>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <stdexcept>
#include <iostream>

// Struct que guarda el indice del parrafo y el la posicion de la palabra dentro del parrafo
struct TokenPosition {
    int paragraphIndex;
    int wordIndex;
};

struct TokenInfo {
    std::string token;
    std::vector<TokenPosition> positions;

    // Sobrecargamos el operados < para que maneje objetos TokenInfo
    bool operator<(const TokenInfo& other) const {
        return token < other.token;
    }

    // Sobrecargamos el operados > para que maneje objetos TokenInfo
    bool operator>(const TokenInfo& other) const {
        return token > other.token;
    }

    // Sobrecargamos el operados <= para que maneje objetos TokenInfo
    bool operator<=(const TokenInfo& other) const {
        return !(*this > other);
    }

    // Sobrecargamos el operados >= para que maneje objetos TokenInfo
    bool operator>=(const TokenInfo& other) const {
        return !(*this < other);
    }

    // Sobrecargamos el operador == para que maneje objetos TokenInfo
    bool operator==(const TokenInfo& other) const {
        return token == other.token && positions == other.positions;
    }

    // Sobrecargamos el operador != para que maneje objetos TokenInfo
    bool operator!=(const TokenInfo& other) const {
        return !(*this == other);
    }
};
#endif