// Implementacion del B sacado de https://github.com/solangii/b-plus-tree
#ifndef BPlusTree_H
#define BPlusTree_H

#include "token.h"
#include <iostream>

using namespace std;
/*
    Los templates se utilizan cuando se desea mandar el tipo de dato por parametro para no tener que
    escribir muchas versiones del codigo para cada tipo de dato. La implementacion del B sacada de github
    viene asi, presumiblemente para que el arbol sea generico.
*/

template <typename T>

struct Node {
    bool is_leaf; // booleano que indica si el nodo es hoja
    std::size_t degree; // numero maximo de hijos que el nodo puede tener
    std::size_t size; // numero actual de claves que tiene el nodo
    T* item; // Son las claves del nodo, en nuestro caso los tokeninfo. Aca es muy util el template
    Node<T>** children; // Un puntero a un arreglo de punteros a mas nodos hijo. CLAVE PARA EL ARBOL
    Node<T>* parent; // Un puntero al al nodo padre, es util para balancear el arbol.

public:
    Node(std::size_t _degree) { // Constructor
        this->is_leaf = false; // Por defecto no es hoja
        this->degree = _degree; // Asigna el grado que viene del arbol
        this->size = 0; // Por defecto no tiene llaves

        /* 
            Crea un arreglo de tokens del tamaño del grado menos 1 
            (el grado menos 1 es el numero maximo de tokens que puede tener un nodo)
        */
        T* _item = new T[degree-1];
        for(int i=0; i<degree-1; i++){
            T* _item = new T[degree-1]; 
                _item[i] = T();
            }
        this->item = _item;

        /*
            Crea el arreglo de punteros a sus hijos, los cuales son inicializados en nullptr, esto 
            reserva la memoria para eventualmente ser llenada. Sin embargo hay desventajas con esto
            ya que si no se usa toda la memoria reservada se desperdicia.
        */ 

        Node<T>** _children = new Node<T>*[degree];
        for(int i=0; i<degree; i++){
            _children[i] = nullptr;
        }
        this->children = _children;

        // El nodo padre se inicializa en nullptr inicialmente.
        this->parent = nullptr;

    }
};

template <typename T>
class BPlusTree {
    Node<T>* root; // Nodo raiz
    std::size_t degree; // Grado del arbol, max numero de hijos
    string fileName; // Nombre del archivo al que esta asociado el arbol

public:
    //? Constructor
    BPlusTree(std::size_t _degree, string pFile) { // Constructor, inicializa el arbol con un grado especifico
        this->root = nullptr; // Por defecto no hay raiz
        this->degree = _degree;
        this->fileName = pFile;
    }
    ~BPlusTree() { // Destructor
        clear(this->root);
    }

    string getFileName() const {
        return fileName;
    }

    //? Devuelve la raiz    
    Node<T>* getroot(){
        return this->root;
    }

    //? Funcion para buscar un solo token en el arbol
    TokenInfo* BPlusTreeSearch(Node<T>* node, T key){
        if(node == nullptr) {
            return nullptr;
        } else {
            Node<T>* cursor = node;

            // Navegar por el arbol hasta llegar a un nodo hoja
            while(!cursor->is_leaf){
                for(int i = 0; i < cursor->size; i++){
                    if(key.token < cursor->item[i].token){ 
                        cursor = cursor->children[i];
                        break;
                    }
                    if(i == cursor->size - 1){
                        cursor = cursor->children[i + 1];
                        break;
                    }
                }
            }

            // Buscar el token en el nodo hoja
            for (int i = 0; i < cursor->size; i++) {
                if (cursor->item[i].token == key.token) {
                    return &cursor->item[i]; // Devuelve el puntero al TokenInfo encontrado
                }
            }

            return nullptr;
        }
    }

    //? Usado para insert y para remove. Busca el nodo hoja deseado creando un cursor y enviando el valor deseado
    Node<T>* BPlusTreeRangeSearch(Node<T>* node, T key){
        // Caso base
        if(node == nullptr) {
            return nullptr;
        } else {
            Node<T>* cursor = node; // Crea el cursor comenzando en el nodo dado

            // Navega hasta abajo en el arbol hasta llegar a un nodo hoja
            while(!cursor->is_leaf){
                for(int i = 0; i < cursor->size; i++){// Si llega a un nodo hoja, itera sobre sus elementos (tokens) y los compara con el actual
                    // Si la clave es menor, se mueve hacia abajo a un nodo que potencialmente podria tener el token
                    if(key.token < cursor->item[i].token){ 
                        cursor = cursor->children[i];
                        break;
                    }
                    // Esto es en caso de que el token sea mayor que todos los tokens del nodo, 
                    // se mueve al ultimo nodo hijo
                    if(i == cursor->size - 1){
                        cursor = cursor->children[i + 1];
                        break;
                    }
                }
            }
            return cursor; // Devuelve el nodo hoja donde se puede encontrar el token
        }
    }

    //?? Funcion que busca un rango de tokens en el arbol
        int range_search(T start, T end, T* result_data, int arr_length) {
            int index=0;

            Node<T>* start_node = BPlusTreeRangeSearch(this->root,start);
            Node<T>* cursor = start_node;
            T temp= cursor->item[0];

            while(temp<=end){
                if(cursor == nullptr){
                    break;
                }
                for(int i=0; i< cursor->size;i++){
                    temp = cursor->item[i];
                    if((temp >= start)&&(temp <= end)){
                        result_data[index] = temp;
                        index++;
                    }
                }
                cursor = cursor->children[cursor->size];
            }
            return index;
        }
    
    //?? Funcion que busca un token en el arbol
    TokenInfo* search(T data) {
        return BPlusTreeSearch(this->root, data);
    }

    //? Esta funcion encuentra la posicion en la que data debe ser insertado en un arreglo para mantener el orden
    int find_index(T* arr, T data, int len){
        int index = 0;
        for(int i=0; i<len; i++){
            if(data < arr[i]){
                index = i;
                break;
            }
            if(i==len-1){
                index = len;
                break;
            }
        }
        return index;
    }

    //? Funcion que busca todas las instancias de un token especifio en el arbol
    std::vector<TokenInfo*> BPlusTreeSearchAll(Node<T>* node, T key) {
        // Vector de palabras encontradas
        std::vector<TokenInfo*> results;

        if (node == nullptr) {
            return results;
        }

        /*
            Se crea un nodo cursor para moverse hacia abajo por el arbol.
            Mientras el cursor no sea un nodo hoja, se itera sobre los elementos del nodo para 
            encontrar el hijo que potencialmente podria tener el token que se busca.
        */ 
        Node<T>* cursor = node;
        while (!cursor->is_leaf) {
            for (int i = 0; i < cursor->size; i++) {
                if (key.token < cursor->item[i].token) {
                    cursor = cursor->children[i];
                    break;
                }
                if (i == cursor->size - 1) {
                    cursor = cursor->children[i + 1];
                    break;
                }
            }
        }

        // Si encuentra alguna coincidencia en los elementos del nodo, los agrega al vector de resultados
        for (int i = 0; i < cursor->size; i++) {
            if (cursor->item[i].token == key.token) {
                results.push_back(&cursor->item[i]);
            }
        }
        // Devuelve el vector de resultados
        return results;
    }

    //? El que se llama en el main
    std::vector<TokenInfo*> searchAll(T data) {
        return BPlusTreeSearchAll(this->root, data);
    }
    
    //? Usado en RemovePar
    /*
        Esta funcion inserta un dato en el arreglo de datos de un nodo. Busca la posicion donde 
        se debe insertar el dato y ordena los demas datos para que se mantenga el orden.
        Devuelve el arreglo con el nuevo dato insertado.
    */ 
    T* item_insert(T* arr, T data, int len){
        int index = 0;
        // Averigua la posicion donde se debe insertar el dato para mantener el orden
        for(int i=0; i<len; i++){
            if(data < arr[i]){
                index = i;
                break;
            }
            // Si el indice es igual a la cantidad maxima permitida, se inserta al final
            if(i==len-1){
                index = len;
                break;
            }
        }

        // Se mueven los elementos a la derecha para hacer espacio para el nuevo dato
        for(int i = len; i > index; i--){
            arr[i] = arr[i-1];
        }
        // Se inserta el dato en la posicion deseada
        arr[index] = data;

        /*
            Se devuelve el arreglo con el nuevo dato insertado, sin embargo ya se modifico el 
            arreglo original por lo que podria no devolver nada.
        */ 
        return arr;
    }
    
    //? Usado en RemovePar
    /*
        Esta funcion inserta un puntero a un nodo hijo en un arreglo de punteros a nodos hijos. Es util
        ya que a menudo se deben insertar nodos hijos en posiciones especificas para mantener el orden.
        Devuelve el arreglo con el nuevo puntero insertado.
    */
    Node<T>** child_insert(Node<T>** child_arr, Node<T>*child,int len,int index){ 
        // La funcion comienza en el final del arreglo (len) y se mueve hacia la izquierda
        for(int i= len; i > index; i--){
            child_arr[i] = child_arr[i - 1]; // Va moviendo los elementos a la derecha para hacer espacio
        }
        // Inserta el puntero al nodo hijo en la posicion deseada
        child_arr[index] = child;
        return child_arr;
    }
    
    //!! Usado en InsertPar
    Node<T>* child_item_insert(Node<T>* node, T data, Node<T>* child){
        int item_index=0;
        int child_index=0;
        for(int i=0; i< node->size; i++){
            if(data < node->item[i]){
                item_index = i;
                child_index = i+1;
                break;
            }
            if(i==node->size-1){
                item_index = node->size;
                child_index = node->size+1;
                break;
            }
        }
        for(int i = node->size;i > item_index; i--){
            node->item[i] = node->item[i-1];
        }
        for(int i=node->size+1;i>child_index;i--){
            node->children[i] = node->children[i-1];
        }

        node->item[item_index] = data;
        node->children[child_index] = child;

        return node;
    }
    
    //!! Usado en el caso especial del insert.
    void InsertPar(Node<T>* par,Node<T>* child, T data){
        //overflow check
        Node<T>* cursor = par;
        if(cursor->size < this->degree-1){//not overflow, just insert in the correct position
            //insert item, child, and reallocate
            cursor = child_item_insert(cursor,data,child);
            cursor->size++;
        }
        else{//overflow
            //make new node
            auto* Newnode = new Node<T>(this->degree);
            Newnode->parent = cursor->parent;

            //copy item
            T* item_copy = new T[cursor->size+1];
            for(int i=0; i<cursor->size; i++){
                item_copy[i] = cursor->item[i];
            }
            item_copy = item_insert(item_copy,data,cursor->size);

            auto** child_copy = new Node<T>*[cursor->size+2];
            for(int i=0; i<cursor->size+1;i++){
                child_copy[i] = cursor->children[i];
            }
            child_copy[cursor->size+1] = nullptr;
            child_copy = child_insert(child_copy,child,cursor->size+1,find_index(item_copy,data,cursor->size+1));

            //split nodes
            cursor->size = (this->degree)/2;
            if((this->degree) % 2 == 0){
                Newnode->size = (this->degree) / 2 -1;
            }
            else{
                Newnode->size = (this->degree) / 2;
            }

            for(int i=0; i<cursor->size;i++){
                cursor->item[i] = item_copy[i];
                cursor->children[i] = child_copy[i];
            }
            cursor->children[cursor->size] = child_copy[cursor->size];
            //todo 안지워짐. 뒤에것.

            for(int i=0; i < Newnode->size; i++){
                Newnode->item[i] = item_copy[cursor->size + i +1];
                Newnode->children[i] = child_copy[cursor->size+i+1];
                Newnode->children[i]->parent=Newnode;
            }
            Newnode->children[Newnode->size] = child_copy[cursor->size+Newnode->size+1];
            Newnode->children[Newnode->size]->parent=Newnode;

            T paritem = item_copy[this->degree/2];

            delete[] item_copy;
            delete[] child_copy;

            //parent check
            if(cursor->parent == nullptr){//if there are no parent node(root case)
                auto* Newparent = new Node<T>(this->degree);
                cursor->parent = Newparent;
                Newnode->parent = Newparent;

                Newparent->item[0] = paritem;
                Newparent->size++;

                Newparent->children[0] = cursor;
                Newparent->children[1] = Newnode;

                this->root = Newparent;

                //delete Newparent;
            }
            else{//if there already have parent node
                InsertPar(cursor->parent, Newnode, paritem);
            }
        }
    }
    
    //* Funcion para insertar un token en el arbol.
    void insert(T data) {
        // Caso 1: Arbol Vacio, se inserta en la raiz
        if(this->root == nullptr){  
            this->root = new Node<T>(this->degree); // Crea el nuevo nodo raiz
            this->root->is_leaf = true; // Lo marca como hoja
            this->root->item[0] = data; // Asigna el dato
            this->root->size = 1; // Ajusta el tamaño
        }
        else{ // Caso 2: El arbol tiene la menos un nodo

            // Creamos un cursor que busca el nodo donde se insertara el token
            Node<T>* cursor = this->root;
            cursor = BPlusTreeRangeSearch(cursor, data);

            // Revisa si hay espacio en el nodo hoja
            if(cursor->size < (this->degree-1)){
                // Si hay espacio, inserta el token en el nodo hoja
                cursor->item = item_insert(cursor->item,data,cursor->size);
                cursor->size++;
                // Ajusta el cursor al siguiente nodo
                cursor->children[cursor->size] = cursor->children[cursor->size-1];
                cursor->children[cursor->size-1] = nullptr;
            }
            else{ /*
                    Caso 3: Desbordamiento del nodo hoja, al intentar insertar un nuevo 
                    nodo en un nodo hoja lleno se debe manejar el desbordamiento
                */ 
                
                // Se hace un nuevo nodo hoja, se establece el padre del nuevo nodo como el padre del nodo actual
                auto* Newnode = new Node<T>(this->degree);
                Newnode->is_leaf = true;
                Newnode->parent = cursor->parent;

                // Copiamos elementos al nuevo nodo
                T* item_copy = new T[cursor->size+1]; // arreglo temporal con la info del nodo actual mas el nuevo elemento
                for(int i=0; i<cursor->size; i++){ // copia los elementos
                    item_copy[i] = cursor->item[i];
                }
                item_copy = item_insert(item_copy,data,cursor->size); // los inserta

                // Dividimos el arreglo temporal en dos, uno para cada nodo
                cursor->size = (this->degree)/2;
                if((this->degree) % 2 == 0){
                    Newnode->size = (this->degree) / 2;
                }
                else{
                    Newnode->size = (this->degree) / 2 + 1;
                }

                // Reasignamos los elementos a los nodos, la mitad para cada uno
                for(int i=0; i<cursor->size;i++){
                    cursor->item[i] = item_copy[i];
                }
                for(int i=0; i < Newnode->size; i++){
                    Newnode->item[i] = item_copy[cursor->size + i];
                }
                // Ajustamos los punteros y enlaces
                cursor->children[cursor->size] = Newnode;
                Newnode->children[Newnode->size] = cursor->children[this->degree-1];
                cursor->children[this->degree-1] = nullptr;

                // Libera la memoria del arreglo temporal
                delete[] item_copy;

                // Caso 4: Maneja la insercion en el nodo padre
                // Despues de dividir el nodo hoja hay que insertar un elemento en el nodo padre para
                // mantener las propiedades del arbol b+
                // El elemento a insertar en el nodo padre es el primer elemento del nuevo nodo hoja
                T paritem = Newnode->item[0];

                if(cursor->parent == nullptr){
                    // Caso especial: No hay nodo padre, 
                    auto* Newparent = new Node<T>(this->degree); // Creamos un nuevo nodo
                    
                    // Asignamos los enlaces entre nodos
                    cursor->parent = Newparent; 
                    Newnode->parent = Newparent;

                    // Asigna los valores al nuevo nodo padre, aumenta el tamaño para representar la inserción
                    Newparent->item[0] = paritem;
                    Newparent->size++;
                    
                    Newparent->children[0] = cursor;
                    Newparent->children[1] = Newnode;

                    // Actualiza la raiz del arbol para que sea el nuevo nodo padre
                    this->root = Newparent;
                }
                else{ 
                    // Insertamos en el nodo padre existente, puede ocasionar una cascada de divisiones
                    InsertPar(cursor->parent, Newnode, paritem);
                }
            }
        }
    }

    //? Funcion para remover un token del arbol
    /*
        La funcion de remove recibe un token a eliminar, y realiza un proceso de busqueda, identificacion
        de hermanos, eliminacion, reorganizacion de punteros y manejo de underflow y balanceo. Hace uso de 
        "REMOVEPAR".
     */
    void remove(T data) {
        // Crea un cursor que empieza en la raiz
        Node<T>* cursor = this->root;

        // Extrae el nodo donde se encuentra el token a eliminar
        cursor = BPlusTreeRangeSearch(cursor,data);

        // Busca el indice del token dentro del arreglo del nodo donde se encuentra el token
        int sib_index =-1;
        for(int i=0; i<cursor->parent->size+1;i++){
            if(cursor == cursor->parent->children[i]){
                sib_index = i;
            }
        }
        // Extrae el indice de los hermanos inmediatos a su izq y der (UTIL A LA HORA DE BALANCEAR)
        int left=sib_index-1;
        int right=sib_index+1;


        // Encuentra el indice de los datos que se desean eliminar recorriendo el nodo padre
        int del_index=-1;
        for(int i=0; i< cursor->size; i++){
            if(cursor->item[i] == data){
                del_index = i;
                break;
            }
        }
        // Si los datos NO existen regresa sin hacer nada
        if(del_index==-1){
            return;
        }

        // De lo contrario, elimina los datos y los punteros
        for(int i=del_index; i<cursor->size-1;i++){ // size-1 porque tiene los nodos tienen grado-1 hijos
            cursor->item[i] = cursor->item[i+1];
        }
        cursor->item[cursor->size-1] = 0;
        cursor->size--;

        // Si el cursor es la raiz del arbol y ya no tiene datos, se limpia el arbol
        if(cursor == this->root && cursor->size==0){
            clear(this->root); // Limpia la raiz
            this->root = nullptr; // establece que la raiz es nula
            return;
        }
        cursor->children[cursor->size] = cursor->children[cursor->size+1];
        cursor->children[cursor->size+1] = nullptr;


        // Revisa el underflow, que es el caso donde un nodo no cumpla tener la cantidad de hijos minima requerida
        if(cursor == this->root){
            return;
        }
        if(cursor->size < degree/2){// Aca se ve donde verifica usando el grado/2 

            // En caso de que exista un token hermano izquierdo, 
            if(left >= 0){
                // Extrae el nodo entre el hermano izquierdo y el cursor
                Node<T>* leftsibling= cursor->parent->children[left];

                /*
                    Verifica si el hermano tiene suficientes elementos para redistribuir, si es asi, puede
                    ceder un elemento al nodo con underflow
                */ 
                
                if(leftsibling->size > degree/2){ 
                    // Crea un arreglo temporal que recupera los tokens del cursor + 1 para el token que va a recibir
                    T* temp = new T[cursor->size+1]; 

                    // Copia TODOS los items del cursor al arreglo temporal
                    for(int i=0; i<cursor->size; i++){
                        temp[i]=cursor->item[i];
                    }

                    // Inserta el token mas a la derecha del hermano izquierdo en el arreglo temporal
                    item_insert(temp,leftsibling->item[leftsibling->size -1],cursor->size);
                    for(int i=0; i<cursor->size+1; i++){
                        cursor->item[i] = temp[i];
                    }
                    // Aumenta el tamaño del cursor al recibir el nuevo token
                    cursor->size++;
                    delete[] temp;

                    // Reasigna los punteros del cursor
                    // El puntero del final antiguo se vuelve el puntero del nuevo final
                    cursor->children[cursor->size] = cursor->children[cursor->size-1];
                    cursor->children[cursor->size-1] = nullptr;

                    // Actualiza las propiedades del hermano izquierdo
                    leftsibling->item[leftsibling->size-1] = 0; // Elimina el token que se cedio
                    leftsibling->size--; // Refleja el cedimiento
                    // El ultimo puntero del hermano izquierda avanza 1 y ese se pone en null para reflejar el cedimiento
                    leftsibling->children[leftsibling->size] = leftsibling->children[leftsibling->size+1];
                    leftsibling->children[leftsibling->size+1]= nullptr;

                    /*
                        se actualiza el elemento correspondiente en el nodo padre de cursor. Esto se hace 
                        para reflejar el nuevo primer elemento en cursor después de la reorganizacion. 
                        El índice left indica la posición del elemento en el nodo padre que actúa 
                        como clave para el rango de valores en cursor. Al cambiar este elemento en 
                        el padre, aseguramos que la estructura del árbol B siga siendo válida 
                        y refleje correctamente los rangos de valores en sus nodos hijos.
                    */
                    cursor->parent->item[left] = cursor->item[0];

                    return;
                }
            }
            // En caso de que exista un token hermano derecho se hace lo mismo pero con logica para el nodo del hermano derecho
            if(right <= cursor->parent->size){// right_sibiling exists
                Node<T>* rightsibling = cursor->parent->children[right];

                if(rightsibling->size >degree/2){//if data number is enough to use this node
                    T* temp = new T[cursor->size+1];

                    //copy item
                    for(int i=0; i<cursor->size; i++){
                        temp[i]=cursor->item[i];
                    }
                    //insert and rearrange
                    item_insert(temp,rightsibling->item[0],cursor->size);
                    for(int i=0; i<cursor->size+1; i++){
                        cursor->item[i] = temp[i];
                    }
                    cursor->size++;
                    delete[] temp;

                    //pointer edit
                    cursor->children[cursor->size] = cursor->children[cursor->size-1];
                    cursor->children[cursor->size-1] = nullptr;

                    //sibling property edit
                    for(int i=0; i<rightsibling->size-1;i++){
                        rightsibling->item[i] = rightsibling->item[i+1];
                    }
                    rightsibling->item[rightsibling->size-1] = 0;
                    rightsibling->size--;
                    rightsibling->children[rightsibling->size] = rightsibling->children[rightsibling->size+1]; //cursor
                    rightsibling->children[rightsibling->size+1]= nullptr;

                    //parent property edit
                    cursor->parent->item[right-1] = rightsibling->item[0];

                    return;
                }
            }

            /*
                Si no existe ningun hermano con suficientes elementos para ceder, se hace un merge
            */

            if(left>=0){ // MERGE CON EL HERMANO IZQUIERDO
                // Recupera el nodo hermano izquierdo
                Node<T>* leftsibling = cursor->parent->children[left];

                // Fusiona los dos nodos hoja, literalmente los vuelve uno solo
                for(int i=0; i<cursor->size; i++){
                    leftsibling->item[leftsibling->size+i]=cursor->item[i];
                }
                //Se ajustan los tamannos de los nodos,
                leftsibling->children[leftsibling->size] = nullptr;
                // Se aumenta el tamaño ya que se fusionaron directamente
                leftsibling->size = leftsibling->size+cursor->size;
                // El ultimo puntero de leftsibling ahora apunta al ultimo puntero del nodo cursor
                leftsibling->children[leftsibling->size] = cursor->children[cursor->size];

                Removepar(cursor, left, cursor->parent);
                
                // Se limpia el nodo cursor liberando sus punteros y estableciendo los items en 0.
                for(int i=0; i<cursor->size;i++){
                    cursor->item[i]=0;
                    cursor->children[i] = nullptr;
                }
                cursor->children[cursor->size] = nullptr;
                
                // Elimina de la memoria la lista de ptrs a items, la lista de ptrs a hijos y el nodo cursor
                delete[] cursor->item;
                delete[] cursor->children;
                delete cursor;

                return;

            }
            if(right<=cursor->parent->size){ // right_sibiling exists
                Node<T>* rightsibling = cursor->parent->children[right];

                //merge two leaf node
                for(int i=0; i<rightsibling->size; i++){
                    cursor->item[i+cursor->size]=rightsibling->item[i];
                }
                //edit pointer
                cursor->children[cursor->size] = nullptr;
                cursor->size = rightsibling->size+cursor->size;
                cursor->children[cursor->size] = rightsibling->children[rightsibling->size];

                //parent property edit
                Removepar(rightsibling, right-1, cursor->parent);

                for(int i=0; i<rightsibling->size;i++){
                    rightsibling->item[i]=0;
                    rightsibling->children[i] = nullptr;
                }
                rightsibling->children[rightsibling->size] = nullptr;

                delete[] rightsibling->item;
                delete[] rightsibling->children;
                delete rightsibling;
                return;

            }

        }
        else{
            return;
        }
    }
    
    //? Usada en REMOVE
    /*
        La funcion removpar actualiza el nodo padre luego de sufrir alguna fusion en un nodo hijo. Es vital
        ya que mantiene la integridad del arbol y sus propiedades.
    */
    void Removepar(Node<T>* node, int index, Node<T>* par){ // Recibe el nodo a eliminar, su indice en el nodo padre y el nodo padre
        // Se definen varios nodos, el nodo a eliminar, el nodo padre y el token a eliminar
        Node<T>* remover = node;
        Node<T>* cursor = par;
        T target = cursor->item[index];

        // CASO ESPECIAL, si el el nodo raiz tiene un TOKEN, permitido para la raiz
        if(cursor == this->root && cursor->size==1){
            // Reorganizamos en base a si el nodo a eliminar es el hijo izquierdo o derecho
            /*
                Borra los recursos del nodo a eliminar.
                Al eliminar uno de los hijos, la raíz se queda con un solo hijo y ya no cumple 
                con la definición de un nodo interno en un árbol B. Por lo tanto, el árbol 
                se reestructura haciendo que el hijo restante sea la nueva raíz. 
                El elemento de la raíz original se vuelv redundante ya que el nuevo arbol seria el hijo
                restante y por lo tanto se elimina.
            */
            if(remover == cursor->children[0]){
                delete[] remover->item;
                delete[] remover->children;
                delete remover;
                this->root = cursor->children[1];
                delete[] cursor->item;
                delete[] cursor->children;
                delete cursor;
                return;
            }
            if(remover == cursor->children[1]){
                delete[] remover->item;
                delete[] remover->children;
                delete remover;
                this->root = cursor->children[0];
                delete[] cursor->item;
                delete[] cursor->children;
                delete cursor;
                return;
            }
        }

        /*
            El objetivo de este for es desplazar todos los ELEMENTOS que estan a la derecha del indice del
            elemento a eliminar hacia la izuierda para "cerrar" el hueco que deja esa eliminacion. Se coloca
            la posicion size-1 como 0 para representar que ahora hay un espacio al final del arreglo.
        */
        for(int i=index; i<cursor->size-1;i++){
            cursor->item[i] = cursor->item[i+1];
        }
        cursor->item[cursor->size-1] = 0;

        // En este bucle se obtiene el indice del puntero al nodo a eliminar
        int rem_index = -1;
        for(int i=0; i<cursor->size+1;i++){
            if(cursor->children[i] == remover){
                rem_index = i;
            }
        }
        // Si llega a este caso no se encontro el nodo a eliminar, por lo que no se hace nada
        if(rem_index == -1){
            return;
        }
        // Se desplazan los PUNTEROS a la izquierda para "cerrar" el hueco que deja la eliminacion, igual que antes
        for(int i=rem_index; i<cursor->size;i++){
            cursor->children[i] = cursor->children[i+1];
        }
        // Se ajusta el ultimo puntero y se reduce el tamaño del nodo padre
        cursor->children[cursor->size] = nullptr;
        cursor->size--;

        // Verificacion de underflow, si el nodo tiene menos de la mitad de sus hijos, se debe balancear
        if(cursor == this->root){ // En este caso termina ya que la raiz es un caso especial y puede tener menos elementos por definicion
            return;
        }

        if(cursor->size < degree/2){ // Si el nodo tiene menos de la mitad de sus hijos, se debe balancear

            // Se busca el indice de los hermanos inmediatos a su izq y der (UTIL A LA HORA DE BALANCEAR)
            int sib_index =-1;
            for(int i=0; i<cursor->parent->size+1;i++){
                if(cursor == cursor->parent->children[i]){
                    sib_index = i; // Encuentra el indice
                }
            }
            // Define los hermanos izq y der del nodo a eliminar
            int left=sib_index-1;
            int right=sib_index+1;

            // Manejo de underflow en caso izquierdo
            if(left >= 0){
                Node<T>* leftsibling= cursor->parent->children[left]; // Dereferenciamos el hermano izquierdo

                // Entra al if solo si el hermano puede dar un token al nodo con underflow
                if(leftsibling->size > degree/2){
                    // Crea un arreglo temporal que recupera el token del padre + 1 para el token que va a recibir
                    T* temp = new T[cursor->size+1];

                    // Copia TODOS los items del cursor al arreglo temporal
                    for(int i=0; i<cursor->size; i++){
                        temp[i]=cursor->item[i];
                    }

                    // Inserta el elemento mas a la derecha del hermano izquierdo en el arreglo temporal
                    item_insert(temp, cursor->parent->item[left],cursor->size);
                    for(int i=0; i<cursor->size+1; i++){
                        cursor->item[i] = temp[i];
                    }
                    // Actualiza el puntero correspondiente del padre con el ultimo de leftsibling.
                    cursor->parent->item[left] = leftsibling->item[leftsibling->size-1];
                    delete[] temp;

                    // REDISTRIBUCION DE PUNTEROS, se copian los elementos a un arreglo temporal
                    Node<T>** child_temp = new Node<T>*[cursor->size+2];
                    for(int i=0; i<cursor->size+1; i++){
                        child_temp[i]=cursor->children[i];
                    }
                    // Se inserta el puntero del hijo más a la derecha de leftsibling en la posición inicial de child_temp.
                    child_insert(child_temp,leftsibling->children[leftsibling->size],cursor->size,0);

                    /*
                        Cada puntero a hijo en cursor se actualiza con los valores de child_temp. 
                        Esto es necesario porque se ha insertado un nuevo puntero a hijo (proveniente del hermano 
                        izquierdo) al principio de los punteros de cursor.
                    */ 
                    for(int i=0; i<cursor->size+2; i++){
                        cursor->children[i] = child_temp[i];
                    }
                    delete[] child_temp;

                    // Se actualizan los tamaños para reflejar las rotaciones y balanceo
                    cursor->size++;
                    leftsibling->size--;
                    return;

                }
            }

            if(right <= cursor->parent->size){// right_sibiling exists
                Node<T>* rightsibling = cursor->parent->children[right];

                if(rightsibling->size > degree/2){//if data number is enough to use this node
                    T* temp = new T[cursor->size+1];

                    //copy item
                    for(int i=0; i<cursor->size; i++){
                        temp[i]=cursor->item[i];
                    }
                    //insert and rearrange at cursor
                    item_insert(temp,cursor->parent->item[sib_index],cursor->size);
                    for(int i=0; i<cursor->size+1; i++){
                        cursor->item[i] = temp[i];
                    }
                    cursor->parent->item[sib_index] = rightsibling->item[0];
                    delete[] temp;

                    //insert and reaarange at child

                    cursor->children[cursor->size+1] = rightsibling->children[0];
                    for(int i=0; i<rightsibling->size; i++){
                        rightsibling->children[i] = rightsibling->children[i+1];
                    }
                    rightsibling->children[rightsibling->size] = nullptr;

                    cursor->size++;
                    rightsibling->size--;
                    return;

                }
            }

            //if sibling is not enought to use their data
            //we have to merge step
            if(left>=0){ // left_sibling exists
                Node<T>* leftsibling = cursor->parent->children[left];

                leftsibling->item[leftsibling->size] = cursor->parent->item[left];
                //merge two leaf node
                for(int i=0; i<cursor->size; i++){
                    leftsibling->item[leftsibling->size+i+1]=cursor->item[i];
                }
                for(int i=0; i<cursor->size+1;i++){
                    leftsibling->children[leftsibling->size+i+1] = cursor->children[i];
                    cursor->children[i]->parent = leftsibling;
                }
                for(int i=0; i<cursor->size+1; i++){
                    cursor->children[i] = nullptr;
                }
                leftsibling->size = leftsibling->size+cursor->size+1;
                /*
                    En este caso recursivo se llama a la funcion de removepar para que se actualice
                    el nodo padre en caso de necesitar balanceo ya que los merges o fusiones pueden 
                    desencadenar mas operaciones hacia arriba en el arbol que necesitan manejarse para
                    manetener las propiedades y el balance del arbol B.
                */
                Removepar(cursor, left,cursor->parent);
                return;

            }
            if(right<=cursor->parent->size){ // right_sibiling exists
                Node<T>* rightsibling = cursor->parent->children[right];

                cursor->item[cursor->size] = cursor->parent->item[right-1];
                //merge two leaf node
                for(int i=0; i<rightsibling->size; i++){
                    cursor->item[cursor->size+1+i]=rightsibling->item[i];
                }
                for(int i=0; i<rightsibling->size+1;i++){
                    cursor->children[cursor->size+i+1] = rightsibling->children[i];
                    rightsibling->children[i]->parent=rightsibling;
                }
                for(int i=0; i<rightsibling->size+1; i++){
                    rightsibling->children[i] = nullptr;
                }
                //edit pointer
                rightsibling->size = rightsibling->size+cursor->size+1;
                //parent property edit
                Removepar(rightsibling, right-1,cursor->parent);
                return;
            }
        }
        else{
            return;
        }
    }
    
    //? Funcion para limpiar un nodo del arbol con sus items e hijos.
    void clear(Node<T>* cursor){
        if(cursor != nullptr){
            if(!cursor->is_leaf){
                for(int i=0; i <= cursor->size; i++){
                    clear(cursor->children[i]);
                }
            }
            delete[] cursor->item;
            delete[] cursor->children;
            delete cursor;
        }
    }
    
    //? Prints
    void bpt_print(){
        print(this->root);
    }

    //? Prints
    void print(Node<T>* cursor) {
        // You must NOT edit this function.
        if (cursor != NULL) {
            for (int i = 0; i < cursor->size; ++i) {
                std::cout << cursor->item[i] << " ";
            }
            std::cout << "\n";

            if (!cursor->is_leaf) {
                for (int i = 0; i < cursor->size + 1; ++i) {
                    print(cursor->children[i]);
                }
            }
        }
    }
};

#endif