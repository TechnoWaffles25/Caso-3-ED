docker run -it --rm -v "C:\Users\Ale\Desktop\Ale's Stuff\Prog\C++\Caso-3-ED:/home" gcc bash

g++ C:\MinGW\include\winsock2.h -o main.o main.cpp
./main.o

g++ -o REST.o REST.h
./REST.o

docker run -it --rm -v "C:\Users\cococo\3erCasoED:/home" gcc bash

g++ -g -o REST.o REST.cpp

./REST.o

docker run -it --rm -v "C:\Users\cococo\3nuevo\Caso-3-ED:/home" -p 8087:808 gcc bash

g++ REST.cpp -o REST.o -lcurl 

./caso2.o