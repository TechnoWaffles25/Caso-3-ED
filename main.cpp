//  g++ -o main.o main.cpp -lcurl -std=c++17
#include "chatGPT.h"
#include <iostream>

using namespace std;

int main(){
string pApiKey = "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";
chatGPT ChatGPT (pApiKey);
string respuesta = ChatGPT.convertirFrase("Simplifique la siguiente frase, resalte las palabras más importante= Hola hola pero no se");
cout<<respuesta;
return 0;
}