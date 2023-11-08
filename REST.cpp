#include <iostream>
#include "json.hpp"
#define CROW_MAIN
#include "crow.h"

using json = nlohmann::json;
using namespace std;

class ChatGPT {
public:
    string convertirFrase(const string& userInput) {
        return "Respuesta a: " + userInput;
    }
};

int main() {
    crow::SimpleApp app;
    ChatGPT chatGPTInstance;

     CROW_ROUTE(app, "/api/convertirFrase")([&](const crow::request& req){
        // Este bloque es donde puedes procesar la solicitud.
        // Si tienes una solicitud POST y necesitas procesar datos JSON, por ejemplo, puedes hacerlo aquí.
        // std::string userInput = req.body;
        // std::string respuesta = "Respuesta a: " + userInput;
        // crow::json::wvalue respuestaJson;
        // respuestaJson["respuesta"] = respuesta;

        // Pero para un simple "Hola mundo", simplemente respondemos con un JSON así:
        crow::json::wvalue respuestaJson;
        respuestaJson["respuesta"] = "Hola mundo";

        // Enviar la respuesta
        return respuestaJson;
    });

    cout << "Intentando iniciar el servidor Crow en http://localhost:8080" << endl;
    app.port(8080).multithreaded().run();

    return 0;
}