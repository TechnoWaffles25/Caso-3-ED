#include <iostream>
#include "json.hpp"
#include "cpp-httplib-master/httplib.h"
using json = nlohmann::json;

// Define la clase ChatGPT (asegúrate de definirla correctamente)
class ChatGPT {
public:
    std::string convertirFrase(const std::string& userInput) {
        // Implementa la lógica de conversión aquí
        // Esta es solo una implementación de ejemplo
        return "Respuesta a: " + userInput;
    }
};

int main() {
    // Crea una instancia de la clase ChatGPT
    ChatGPT chatGPTInstance;

    httplib::Server svr;

    svr.Post("/api/convertirFrase", [&chatGPTInstance](const httplib::Request& req, httplib::Response& res) {
        // Obtiene el cuerpo de la solicitud POST (que debe contener la frase del usuario)
        std::string userInput = req.get_param_value("frase");

        // Realiza la conversión de la frase utilizando la instancia de ChatGPT
        std::string respuesta = chatGPTInstance.convertirFrase(userInput);

        // Crea una respuesta JSON
        json respuestaJson = {
            {"respuesta", respuesta}
        };

        res.set_content(respuestaJson.dump(), "application/json");
    });

    std::cout << "Servidor REST en ejecución en http://localhost:8080/" << std::endl;

    svr.listen("localhost", 8085);

    return 0;
}  // Fin de main()