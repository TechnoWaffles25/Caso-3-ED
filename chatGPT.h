#ifndef chatGPT_h
#define chatGPT_h

#include <iostream>
#include <string>
#include <curl/curl.h>
#include "json.hpp"

using json = nlohmann::json;



class chatGPT{
    private:
        std::string& apiKey;
    public:
        chatGPT(std::string& papiKey) : apiKey(papiKey) {
}



    std::string makeApiRequest(const std::string& requestBody){
    CURL* curl = curl_easy_init();
    
    // URL de la API de ChatGPT
    std::string apiUrl = "https://api.openai.com/v1/engines/davinci/completions";  // Reemplaza con la URL correcta
    
    // Cabeceras de la solicitud HTTP
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Authorization: Bearer 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");  // Reemplaza con tu clave de API

    // Configurar la solicitud POST
    curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestBody.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    
    // Almacenar la respuesta
    std::string response;

    // Realizar la solicitud
    CURLcode res = curl_easy_perform(curl);
    
    if (res == CURLE_OK) {
        // La solicitud fue exitosa, obtener la respuesta
        char* data = NULL;
        long length;
        curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &length);
        data = (char*)malloc(length + 1);
        data[length] = '\0';
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &length);
        response = data;
        free(data);
    } else {
        // La solicitud falló, manejar el error
        response = "Error en la solicitud HTTP";
    }
    
    // Limpieza y liberación de recursos
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    
    return response;
}

        //funcion en donde el usuario le da un input
        std::string userInput;
        std::string convertirFrase(const std::string& userInput){
            // Construye la solicitud HTTP con el input del usuario sin procesar.
            std::string requestBody = "{\"messages\": [{\"role\": \"system\", \"content\": \"mm\"}, {\"role\": \"user\", \"content\": \"" + userInput + "\"}]}";

            std::string jsonResponse = makeApiRequest(requestBody);

            // Analiza la respuesta JSON.
            json responseJson = json::parse(jsonResponse);

            // Extrae la respuesta generada por el modelo.
            std::string modelResponse = responseJson["choices"][0]["message"]["content"];

            // Devuelve la respuesta generada por el modelo.
            return modelResponse;
        }
};

#endif 