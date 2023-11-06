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
        /* std::string userInput = req.body;
        std::string respuesta = chatGPTInstance.convertirFrase(userInput);
        json respuestaJson = {
            {"respuesta", respuesta}
        };
        res.set_header("Content-Type", "application/json");
        res.status = 200;
        res.set_content(respuestaJson.dump(), "application/json");
    });*/
        json respuestaJson = {
            {"respuesta", "Hola mundo"}
        };
        res.set_content(respuestaJson.dump(), "application/json");
        cout << "Hola mundo" << endl;
    });
    std::cout << "Servidor REST en ejecución en http://localhost:8080/" << std::endl;
    svr.listen("localhost", 8080);
    return 0;
}
// http://localhost:8080/api/convertirFrase?frase=Hola