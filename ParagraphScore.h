#ifndef PARAGRAPH_SCORE_H
#define PARAGRAPH_SCORE_H

#include <vector>
#include "token.h"

// Struct que guarda el score de un parrafo individualmente
struct ParagraphScore {
    int paragraphIndex;
    double score;
    std::vector<const TokenInfo*> tokenInfos; // Almacena punteros a TokenInfo, o sea las palabras que se encontraron en el parrafo

    // Constructor
    ParagraphScore(int index) : paragraphIndex(index), score(0.0) {}

    // Agrega un TokenInfo (Palabra) al vector de tokenInfos
    void addTokenInfo(const TokenInfo* tokenInfo) {
        tokenInfos.push_back(tokenInfo);
    }

    // Calcula el puntaje del parrafo si encuentra los tokens que se estan buscando en un mismo parrafo
    void calculateScore(const std::vector<std::string>& searchTokens) {
        if (tokenInfos.empty()) {
            return;
        }

        int matchCount = 0; // Cantidad de tokens encontrados en el parrafo
        int lastWordIndex = -1; // Posicion de la ultima palabra encontrada en el parrafo
        int totalDistance = 0; // Distancia entre las palabras encontradas en el parrafo

        // Itera sobre los tokens que se estan buscando
        for (const auto& searchToken : searchTokens) {
            bool tokenFound = false;
            // Itera sobre los tokens del parrafo que se esta trabajando actualmente
            for (const auto& tokenInfo : tokenInfos) {
                if (tokenInfo->token == searchToken) {
                    // Itera sobre las posiciones del token en el parrafo
                    for (const auto& position : tokenInfo->positions) {
                        if (position.paragraphIndex == paragraphIndex) {
                            // Calcula la distancia entre palabras consecutivas que coinciden
                            if (lastWordIndex != -1) {
                                totalDistance += abs(position.wordIndex - lastWordIndex);
                            }
                            lastWordIndex = position.wordIndex;
                            matchCount++;
                            tokenFound = true;
                            break;
                        }
                    }
                    if (tokenFound) {
                        break;
                    }
                }
            }
        }

        if (matchCount > 0) {
            // Calcula la puntuacion basada a la cantidad de coincidencias y la distancia entre palabras
            score = (double)matchCount / searchTokens.size() * 100.0 / (1 + totalDistance);
        } else {
            score = 0.0; // Si no hay coincidencias la puntuacion es 0
        }
    }


};
#endif