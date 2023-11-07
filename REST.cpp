#include <iostream>
#include "json.hpp"
#include "cpp-httplib-master/httplib.h"

using json = nlohmann::json;
using namespace std;

class ChatGPT {
public:
    std::string convertirFrase(const std::string& userInput) {
        return "Respuesta a: " + userInput;
    }
};

int main() {
    ChatGPT chatGPTInstance;
    httplib::Server svr;

    svr.Post("/api/convertirFrase", [&chatGPTInstance](const httplib::Request& req, httplib::Response& res) {
        cout << "Solicitud recibida: " << req.body << endl;
        
        // Procesamiento comentado. Descomentar para uso real.
        // std::string userInput = req.body;
        // std::string respuesta = chatGPTInstance.convertirFrase(userInput);
        // json respuestaJson = {{"respuesta", respuesta}};

        json respuestaJson = {{"respuesta", "Hola mundo"}};
        res.set_content(respuestaJson.dump(), "application/json");
        cout << "Respuesta enviada: " << respuestaJson.dump() << endl;
    });

    try {
        cout << "Intentando iniciar el servidor REST en http://localhost:8080/" << endl;
        
        if (!svr.listen("localhost", 8080)) {
            cerr << "Error al iniciar el servidor en http://localhost:8080" << endl;
            return -1;
        }

        cout << "Servidor REST en ejecución en http://localhost:8080/" << endl;
    } catch (const std::exception& e) {
        cerr << "Excepción al iniciar el servidor: " << e.what() << endl;
        return -1;
    }

    return 0;
}