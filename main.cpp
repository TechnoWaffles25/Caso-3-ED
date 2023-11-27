#include "BHandler.h"
#include <iostream>

using namespace std;
int main() {
    // Ruta al directorio donde se encuentran los archivos de los libros
    string directoryPath = "Books/";

    // Crear una instancia de BHandler
    BHandler bHandler(directoryPath);

    // Establecer palabras para remover
    unordered_set<string> stopWords = {"the", "and", "or","when"};
    bHandler.setStopWords(stopWords);

    // Buscar una frase
    string phraseToSearch = "fighting for hedgehogs";
    vector<Book> top10Books = bHandler.buscarFrase(phraseToSearch);

    // Mostrar los resultados
    cout << "Top 10 libros para la frase: '" << phraseToSearch << "'" << endl;
    for (const auto& book : top10Books) {
        cout << "Título: " << book.getTitle() << ", Autor: " << book.getAuthor() << ", Puntuación: " << book.getScore() << endl;
    }

    return 0;
}
