#include <iostream>
#include <string>
#include <cstring>
#include <winsock2.h>
#include "chatGPT.h"

#pragma comment(lib, "ws2_32.lib") // Enlazar con la librería Winsock

const int PORT = 8080;

class RestServer {
public:
    RestServer(const std::string& apiKey) : chatGPT(apiKey) {}

    void start() {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "Error al inicializar Winsock" << std::endl;
            return;
        }

        int serverSocket, newSocket;
        struct sockaddr_in serverAddr;
        struct sockaddr_storage serverStorage;
        socklen_t addr_size;
        char buffer[1024];

        serverSocket = socket(AF_INET, SOCK_STREAM, 0); // Cambiado a AF_INET
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(PORT);
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));
        bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

        if (listen(serverSocket, 50) == 0) {
            std::cout << "Servidor REST en ejecución en http://localhost:" << PORT << std::endl;
        } else {
            std::cerr << "Error al iniciar el servidor" << std::endl;
            WSACleanup(); // Limpieza de Winsock
            return;
        }

        addr_size = sizeof(serverStorage);
        newSocket = accept(serverSocket, (struct sockaddr*)&serverStorage, &addr_size);

        while (true) {
            recv(newSocket, buffer, 1024, 0);

            if (std::string(buffer).find("POST /api/convertirFrase") != std::string::npos) {
                // Procesa la solicitud POST para /api/convertirFrase
                std::string content = "Ejemplo de cuerpo de solicitud POST";  // Aquí obtener el cuerpo de la solicitud

                std::string respuesta = chatGPT.convertirFrase(content);

                std::string response = "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(respuesta.length()) + "\r\n\r\n" + respuesta;

                send(newSocket, response.c_str(), response.length(), 0);
            } else {
                // Si la solicitud no coincide con ninguna ruta conocida, puede responder con un error 404.
                std::string not_found_response = "HTTP/1.1 404 Not Found\r\n\r\n";
                send(newSocket, not_found_response.c_str(), not_found_response.length(), 0);
            }
        }

        close(newSocket);
        close(serverSocket);

        WSACleanup(); // Limpieza de Winsock
    }

private:
    chatGPT chatGPT;
};

int main() {
    RestServer restServer("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"); // Asegúrate de proporcionar la clave de la API correcta
    restServer.start();

    return 0;
}