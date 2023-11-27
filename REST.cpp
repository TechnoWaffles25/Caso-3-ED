#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "B+Tree.h"
#include "BHandler.h"
#include "json.hpp"  // Asegúrate de incluir la biblioteca JSON

using json = nlohmann::json;

const int PORT = 8087;

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);

    std::string directoryPath = "Books/";
    BHandler bHandler(directoryPath);

    // Crear un socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Error al crear el socket");
        return 1;
    }

    // Configurar la dirección del servidor
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    // Enlazar el socket a la dirección y puerto
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Error al enlazar el socket");
        return 1;
    }

    // Escuchar conexiones entrantes
    if (listen(serverSocket, 5) < 0) {
        perror("Error al escuchar");
        return 1;
    }

    std::cout << "Servidor HTTP escuchando en el puerto " << PORT << std::endl;

    while (true) {
        // Aceptar una conexión entrante
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);

        if (clientSocket < 0) {
            perror("Error al aceptar la conexión");
            continue;
        }

        char buffer[1024];
        std::memset(buffer, 0, sizeof(buffer));

        // Leer la solicitud HTTP del cliente
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead < 0) {
            perror("Error al leer la solicitud HTTP");
            close(clientSocket);
            continue;
        }

        // Analizar la solicitud HTTP para encontrar el cuerpo
        std::string request(buffer);
        size_t bodyStart = request.find("\r\n\r\n");
        if (bodyStart != std::string::npos) {
            // Extraer el cuerpo de la solicitud
            std::string requestBody = request.substr(bodyStart + 4);
            
            // Imprimir el cuerpo de la solicitud en la consola
            std::cout << "Cuerpo de la solicitud: " << requestBody << std::endl;

            // Buscar libros basados en la frase
            auto libros = bHandler.buscarFrase(requestBody);

            json respuestaJson = json::array();
            for (const auto& libro : libros) {
                json libroJson;
                libroJson["titulo"] = libro.getTitle();
                libroJson["calificacion"] = libro.getScore();
                respuestaJson.push_back(libroJson);
            }

            // Enviar la respuesta JSON
            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n" + respuestaJson.dump();
            send(clientSocket, response.c_str(), response.size(), 0);
        } else {
            // Si no se encuentra un cuerpo, responder con un mensaje de error
            std::string response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nNo se encontró un cuerpo en la solicitud.";
            send(clientSocket, response.c_str(), response.size(), 0);
        }

        // Cerrar la conexión con el cliente
        close(clientSocket);
    }

    // Cerrar el socket del servidor
    close(serverSocket);

    return 0;
}