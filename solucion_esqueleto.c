// Uso:  ./tetris           corre los tests y despues el juego
//       ./tetris --test    corre solo los tests (valgrind --leak-check=full ./tetris --test)

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// ========= CONSTANTES DEL JUEGO =========
#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 800
#define GRID_ROWS 20
#define GRID_COLS 10
#define CELL_SIZE 35
#define OFFSET_X 75
#define OFFSET_Y 50
#define TICK_RATE 30

const int TETROMINOS[7][4][4][4] = {
    { {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}}, {{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0}}, {{0,0,0,0},{0,0,0,0},{1,1,1,1},{0,0,0,0}}, {{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}} },
    { {{2,0,0,0},{2,2,2,0},{0,0,0,0},{0,0,0,0}}, {{0,2,2,0},{0,2,0,0},{0,2,0,0},{0,0,0,0}}, {{0,0,0,0},{2,2,2,0},{0,0,2,0},{0,0,0,0}}, {{0,2,0,0},{0,2,0,0},{2,2,0,0},{0,0,0,0}} },
    { {{0,0,3,0},{3,3,3,0},{0,0,0,0},{0,0,0,0}}, {{0,3,0,0},{0,3,0,0},{0,3,3,0},{0,0,0,0}}, {{0,0,0,0},{3,3,3,0},{3,0,0,0},{0,0,0,0}}, {{3,3,0,0},{0,3,0,0},{0,3,0,0},{0,0,0,0}} },
    { {{0,4,4,0},{0,4,4,0},{0,0,0,0},{0,0,0,0}}, {{0,4,4,0},{0,4,4,0},{0,0,0,0},{0,0,0,0}}, {{0,4,4,0},{0,4,4,0},{0,0,0,0},{0,0,0,0}}, {{0,4,4,0},{0,4,4,0},{0,0,0,0},{0,0,0,0}} },
    { {{0,5,5,0},{5,5,0,0},{0,0,0,0},{0,0,0,0}}, {{0,5,0,0},{0,5,5,0},{0,0,5,0},{0,0,0,0}}, {{0,0,0,0},{0,5,5,0},{5,5,0,0},{0,0,0,0}}, {{5,0,0,0},{5,5,0,0},{0,5,0,0},{0,0,0,0}} },
    { {{0,6,0,0},{6,6,6,0},{0,0,0,0},{0,0,0,0}}, {{0,6,0,0},{0,6,6,0},{0,6,0,0},{0,0,0,0}}, {{0,0,0,0},{6,6,6,0},{0,6,0,0},{0,0,0,0}}, {{0,6,0,0},{6,6,0,0},{0,6,0,0},{0,0,0,0}} },
    { {{7,7,0,0},{0,7,7,0},{0,0,0,0},{0,0,0,0}}, {{0,0,7,0},{0,7,7,0},{0,7,0,0},{0,0,0,0}}, {{0,0,0,0},{7,7,0,0},{0,7,7,0},{0,0,0,0}}, {{0,7,0,0},{7,7,0,0},{7,0,0,0},{0,0,0,0}} }
};

SDL_Color colors[8] = {
    {30, 30, 30, 255}, {0, 255, 255, 255}, {0, 0, 255, 255}, {255, 165, 0, 255},
    {255, 255, 0, 255}, {0, 255, 0, 255}, {128, 0, 128, 255}, {255, 0, 0, 255}
};

// ========= ESTRUCTURAS DE DATOS =========

// 1. Nodo para la fila del tablero (Lista Enlazada)
typedef struct RowNode {
    int cells[GRID_COLS]; 
    struct RowNode* next; 
} RowNode;

// 2. Nodo para la cola de próximas piezas (Queue)
typedef struct PieceQueueNode {
    int type; 
    struct PieceQueueNode* next;
} PieceQueueNode;

// 3. Nodo para el sistema de partículas 
typedef struct ParticleNode {
    float x, y;
    float vel_x, vel_y;
    int color;
    int life_timer;
    struct ParticleNode* next;
} ParticleNode;

typedef struct ActivePiece {
    int col, row;
    int type;      
    int rotation;  
    int active;
} ActivePiece;

typedef struct GameBoard {
    RowNode* first_row;             
    PieceQueueNode* queue_head;     
    PieceQueueNode* queue_tail;     
    ParticleNode* first_particle;   
    ActivePiece current_piece;
    int score;
    int tick_timer;
} GameBoard;

// ========= VARIABLES GLOBALES =========
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
GameBoard* game_board = NULL;

// ========= EJERCICIO 1: STRINGS (NO USAR string.h) =========


char* strDuplicate(char* src) {

    if(src==NULL){
        return NULL; //caso base, si src es NULL, no hay nada que copiar, se devuelve NULL
    }

    int totallen=0;// almacenamos el largo de src en la variable total len 
    for(;src[totallen]!='\0';totallen++){
      // totallen ya es el contador predeterminado   
    }

    char* copia= (char*)malloc((totallen+1)*sizeof(char)); // se pide memoria por malloc, se le suma 1 para que la medida sea la correcta.

    if(copia==NULL){
        return NULL; // verifica si el malloc funciono. 
    }

    for(int i=0;i<totallen+1;i++){
        copia[i]=src[i];  // se le asigna a copia lo que contiene src
    }

    return copia;
}


int strCompare(char* s1, char* s2) {
    
    int i=0; //asignamos un i como indice, luego un bucle for con la condicion de que avance si las palabras son iguales.
    while(s1[i]!='\0'&& s2[i]!='\0'&& s1[i]==s2[i]){
        i++;
    } 
    // si al finalizar el bucle se llega al final de ambas palabras entonces son iguales, por lo tanto devolvemos 0. 
    if(s1[i]=='\0'&& s2[i]=='\0'){
        return 0;
    }
    // si no, evaluamos si s1 es menor a s2, si es asi se devuelve 1.
    if(s1[i]<s2[i]){
        return 1;
    }else{ 
        // por descarte s2 es mayor a s1, devolvemos -1.
        return -1;
      
    }

}





char* strConcatenate(char* src1, char* src2) {
    

    int longitud=0; // creamos la variable longitud para ver la longitud de la palabra concatenada.
    for(int i=0;src1[i]!='\0';i++){
        longitud++;
        //realizamos un bucle para comenzar a contar las longitudes de los string.
    }
    // realizamos otro bucle para saber cuanto sera la longitud total para la concatenacion. 
    for(int j=0; src2[j]!='\0';j++){
        longitud++;
    }
    // ya que tenemos la longitud que vamos a necesitar solicitamos memoria con malloc. 
    char* res=(char*)malloc((longitud+1)*sizeof(char));
    //evaluamos si solicitar a memoria falla. 
    if(res==NULL){
        return NULL;
    }
    int indice=0;
    // realizamos un bucle con src1 para concatenar src1 en res indice por indice. 
    for(int g=0; src1[g]!='\0';g++){
        res[indice]=src1[g];
        indice++;
    }
    //al ser punteros distintos realizamos otro bucle para concatenarlo en res. 
    for(int h=0;src2[h]!='\0';h++){
        res[indice]=src2[h];
        indice++;
    }
    // se cierra el string con el caracter nulo.
    res[indice]='\0';
    // liberamos src1 y src2 como pide la consigna. 
    free(src1);
    free(src2);
    //devolvemos la concatenacion total.
    return res;

}




// ========= EJERCICIO 2: FUNCIONES A IMPLEMENTAR (EL TP) =========

void gameBoardDelete(GameBoard* board); // funcion implementada más abajo, se declara acá para que gameBoardNew pueda llamarla en caso de error.

GameBoard* gameBoardNew() {
    
    GameBoard* board= (GameBoard*)malloc(sizeof(GameBoard)); // solicitamos memoria dinamica

    if(board==NULL){ // verificamos si la memoria fue solicitada correctamente, si no es asi devolvemos NULL.
        return NULL;
    }

    // iniciamos campos base  para poder crear un nuevo game board 

    board-> score=0;
    board-> tick_timer = 0;
    board-> queue_head = NULL;
    board-> queue_tail = NULL;
    board-> first_particle = NULL;
    board-> current_piece.active = 0; 

    // luego creamos las filas mediante una lista enlazada 

    board-> first_row =NULL;
    RowNode* ultimo_nodo = NULL;
    
    // utilizamos memoria dinamica para crear los nodos, ademas de que luego revisamos si hay algun error al solicitar memoria. 
    for(int i=0; i<GRID_ROWS; i++){
        RowNode* nuevo_nodo= (RowNode*)malloc(sizeof(RowNode));

        if(nuevo_nodo==NULL){
            gameBoardDelete(board); // si hay un error al solicitar memoria, se libera la memoria solicitada y se devuelve NULL.
            return NULL;
        }
    

    // Inicializar las celdas de la fila en 0
        for (int c = 0; c < GRID_COLS; c++) {
            nuevo_nodo->cells[c] = 0;
        }
        nuevo_nodo->next = NULL;

        // Conectar el nodo a la lista
        if (board->first_row == NULL) {
            board->first_row = nuevo_nodo; // Es la primera fila
        } else {
            ultimo_nodo->next = nuevo_nodo; // La conectamos a la anterior
        }
        ultimo_nodo = nuevo_nodo;
    }
    return board;
}

    



void gameBoardDelete(GameBoard* board) {
    // TODO: Debe tolerar board == NULL.
    // TODO: Recorrer y liberar todos los RowNode con free().
    // TODO: Recorrer y liberar todos los PieceQueueNode.
    // TODO: Recorrer y liberar todos los ParticleNode.
    // TODO: Finalmente, liberar la estructura GameBoard.

    if(board==NULL){
        return; // si el board esta vacio no se realiza nada 
    }

    // ahora recorremos y liberamos todas las filas
    RowNode* curr_row = board-> first_row;
    while(curr_row!= NULL){
        RowNode* next_row = curr_row->next; //guardamos el siguiente 
        free(curr_row); // borramos el actual
        curr_row = next_row; // asignamos el actual al siguiente para luego borrarlo
    }

    // Recorrer y liberar la cola de proximas piezas (PieceQueueNode)
    PieceQueueNode* curr_q = board->queue_head;
    while (curr_q != NULL) {
        PieceQueueNode* next_q = curr_q->next;
        free(curr_q);
        curr_q = next_q;
    }

    //Recorrer y liberar todas las particulas activas utilizando la misma estructura que en los otros bucles 
    ParticleNode* curr_part = board->first_particle;
    while (curr_part != NULL) {
        ParticleNode* next_part = curr_part->next;
        free(curr_part);
        curr_part = next_part;
    }

     //Finalmente, liberar la estructura principal del GameBoard
    free(board);
}



    


void gameBoardAddParticle(GameBoard* board, float x, float y, int color) {
    // TODO: Crear un ParticleNode con memoria dinámica.
    // TODO: Inicializar x, y, velocidades aleatorias y un life_timer.
    // TODO: Agregarlo al principio de la lista enlazada simple de partículas.

    if(board == NULL){
        return ; // al ser del tipo void, se retorna sin valor 
    }
    // creamos la variable particle node con memoria dinamica.
    ParticleNode* nueva_part= (ParticleNode*)malloc(sizeof(ParticleNode));

    if(nueva_part == NULL){
        return ; // devuelve nada al ser void. 
    }

    // inicializamos x, y, velocidades aleatorias y un life timer
    
    nueva_part->x = x;
    nueva_part-> y= y;
    nueva_part->color = color;
    nueva_part->life_timer = 30 + (rand() % 30); // entre 30 y 59 tics como se aclara en juego base.c

    // Darles velocidades aleatorias (similares al prototipo estatico del esqueleto)
    nueva_part->vel_x = ((rand() % 100) / 10.0f) - 5.0f; // Velocidad horizontal variada
    nueva_part->vel_y = ((rand() % 50) / -10.0f) - 2.0f;  // Impulso hacia arriba

    // Agregarlo al PRINCIPIO de la lista enlazada simple de particulas
    nueva_part->next = board->first_particle;
    board->first_particle = nueva_part;

    
}

int gameBoardCheckAndClearLines(GameBoard* board) {
    // TODO: Recorrer la lista de RowNode.
    // TODO: Si un RowNode tiene todas sus celdas > 0, desenlazarlo y hacer free().
    // TODO: Crear un nuevo RowNode vacío (lleno de ceros).
    // TODO: Insertar el nuevo RowNode en la CABEZA del tablero (board->first_row).
    // TODO: Por cada celda destruida, invocar a gameBoardAddParticle().
    int llenas = 0;
    RowNode* actual = board->first_row;
    RowNode* antes = NULL;
    int fila = 0;
    //revisamos si las filas estan ocupadas
    while (actual != NULL){
        int ocupada = 1;
        for (int z=0; z < GRID_COLS; z++){
            if (actual->cells[z]==0){
                ocupada=0;
            }
        }
        //definimos proxima fila
        RowNode* proxima = actual->next;

        //hacemos un if para ver si la fila esta ocupada, si lo esta, se borra y se agrega una nueva fila vacia al inicio del tablero
        if (ocupada){
            for (int z=0; z < GRID_COLS; z++){
                gameBoardAddParticle(board, (OFFSET_X + z * CELL_SIZE + CELL_SIZE / 2), (OFFSET_Y + fila * CELL_SIZE + CELL_SIZE / 2), actual->cells[z]);
            }
            if (antes==NULL){
                board->first_row = proxima;
            } else {
                antes->next = proxima;
            }
            free(actual);
            llenas= llenas + 1;
         

        //si la fila no esta ocupada, se pasa a la siguiente
        }else {
            antes = actual;
        }
        

        actual = proxima;
        fila++;
    }
    //agregamos las filas vacias al inicio del tablero
    for (int i=0; i < llenas; i++){
        RowNode* nuevaFila = (RowNode*) malloc(sizeof(RowNode));
        if (nuevaFila==NULL){
            return llenas;
        }
        for (int z=0; z < GRID_COLS; z++){
            nuevaFila->cells[z]=0;
        }
        nuevaFila->next = board->first_row;
        board->first_row = nuevaFila;
    }
    return llenas;
}


void gameBoardEnqueuePiece(GameBoard* board, int type) {
    // TODO: Crear un PieceQueueNode con el 'type'.
    // TODO: Insertarlo al final de la cola (queue_tail) y actualizar punteros.
    if(board==NULL){
        return;
    }
     PieceQueueNode* nuevo = (PieceQueueNode*)malloc(sizeof(PieceQueueNode));
    if (nuevo==NULL) {
        return;
    }

    nuevo -> type = type;
    nuevo -> next = NULL;

    if (board->queue_head==NULL) {
        //lista vacia
        board -> queue_head = nuevo;
        board -> queue_tail = nuevo;
    } else {
        //ya habia nodos dentro de la lista
        board -> queue_tail -> next = nuevo;
        board ->queue_tail = nuevo;

    }
}

int gameBoardDequeuePiece(GameBoard* board, int* out_type) {
    // TODO: Si la cola está vacía, retornar 0.
    // TODO: Extraer el primer nodo (queue_head).
    // TODO: Guardar el type en la variable out_type, liberar el nodo y actualizar el head.
    if (board->queue_head==NULL){
        return 0;
    }
    PieceQueueNode* viejo = board -> queue_head;
    *out_type = viejo -> type;
    board -> queue_head = viejo -> next;
    free (viejo);
    if (board -> queue_head==NULL){
    board -> queue_tail=NULL;
    }
    return 1;
}


// ========= LOGICA DE DIBUJADO Y COLISIONES (NO MODIFICAR) =========

RowNode* getRowAt(GameBoard* board, int index) {
    RowNode* curr = board->first_row;
    for(int i = 0; i < index && curr != NULL; i++) curr = curr->next;
    return curr;
}

int canMove(GameBoard* board, int target_col, int target_row, int target_rot) {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (TETROMINOS[board->current_piece.type][target_rot][r][c] != 0) {
                int board_col = target_col + c;
                int board_row = target_row + r;
                if (board_col < 0 || board_col >= GRID_COLS || board_row >= GRID_ROWS) return 0;
                if (board_row >= 0) {
                    RowNode* row_node = getRowAt(board, board_row);
                    if (row_node && row_node->cells[board_col] != 0) return 0;
                }
            }
        }
    }
    return 1;
}

void lockPiece(GameBoard* board) {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            int val = TETROMINOS[board->current_piece.type][board->current_piece.rotation][r][c];
            if (val != 0) {
                RowNode* row_node = getRowAt(board, board->current_piece.row + r);
                if (row_node) row_node->cells[board->current_piece.col + c] = val;
            }
        }
    }
    board->current_piece.active = 0;
    board->score += gameBoardCheckAndClearLines(board) * 100;
}

// Devuelve 0 si la pieza nueva no entra (GAME OVER), 1 en otro caso.
int spawnNextPiece(GameBoard* board) {
    int type;
    if (gameBoardDequeuePiece(board, &type)) {
        board->current_piece.type = type;
        board->current_piece.rotation = 0;
        board->current_piece.col = GRID_COLS / 2 - 2;
        board->current_piece.row = 0;
        board->current_piece.active = 1;
        gameBoardEnqueuePiece(board, rand() % 7);
        if (!canMove(board, board->current_piece.col, board->current_piece.row, board->current_piece.rotation)) {
            printf("GAME OVER. Puntaje final: %d\n", board->score);
            board->current_piece.active = 0;
            return 0;   // main sale del loop y libera tablero y SDL
        }
    }
    return 1;
}

// Devuelve 0 cuando termina la partida.
int gameBoardUpdate(GameBoard* board) {
    if (!board) return 0;
    if (!board->current_piece.active && !spawnNextPiece(board)) return 0;

    board->tick_timer++;
    if (board->tick_timer >= TICK_RATE) {
        board->tick_timer = 0;
        if (canMove(board, board->current_piece.col, board->current_piece.row + 1, board->current_piece.rotation)) {
            board->current_piece.row++;
        } else {
            lockPiece(board);
        }
    }

    ParticleNode* curr_part = board->first_particle;
    ParticleNode* prev_part = NULL;
    while (curr_part) {
        curr_part->x += curr_part->vel_x;
        curr_part->y += curr_part->vel_y;
        curr_part->vel_y += 0.5f;
        curr_part->life_timer--;

        if (curr_part->life_timer <= 0) {
            ParticleNode* to_delete = curr_part;
            if (prev_part == NULL) {
                board->first_particle = curr_part->next;
                curr_part = board->first_particle;
            } else {
                prev_part->next = curr_part->next;
                curr_part = prev_part->next;
            }
            free(to_delete);
        } else {
            prev_part = curr_part;
            curr_part = curr_part->next;
        }
    }
    return 1;
}

void gameBoardDraw(GameBoard* board) {
    if (!board) return;
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect board_rect = {OFFSET_X - 2, OFFSET_Y - 2, GRID_COLS * CELL_SIZE + 4, GRID_ROWS * CELL_SIZE + 4};
    SDL_RenderDrawRect(renderer, &board_rect);

    RowNode* current_row = board->first_row;
    int r = 0;
    while (current_row) {
        for (int c = 0; c < GRID_COLS; c++) {
            if (current_row->cells[c] > 0) {
                SDL_Color col = colors[current_row->cells[c]];
                SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, 255);
                SDL_Rect cell_rect = {OFFSET_X + c * CELL_SIZE, OFFSET_Y + r * CELL_SIZE, CELL_SIZE - 1, CELL_SIZE - 1};
                SDL_RenderFillRect(renderer, &cell_rect);
            }
        }
        current_row = current_row->next;
        r++;
    }

    if (board->current_piece.active) {
        for(int pr = 0; pr < 4; pr++) {
            for(int pc = 0; pc < 4; pc++) {
                int cell_val = TETROMINOS[board->current_piece.type][board->current_piece.rotation][pr][pc];
                if (cell_val != 0) {
                    SDL_Color col = colors[cell_val];
                    SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, 255);
                    SDL_Rect p_rect = {OFFSET_X + (board->current_piece.col + pc) * CELL_SIZE, 
                                       OFFSET_Y + (board->current_piece.row + pr) * CELL_SIZE, 
                                       CELL_SIZE - 1, CELL_SIZE - 1};
                    SDL_RenderFillRect(renderer, &p_rect);
                }
            }
        }
    }

    ParticleNode* curr_part = board->first_particle;
    while (curr_part) {
        SDL_Color col = colors[curr_part->color];
        int alpha = curr_part->life_timer * 8;   // se satura: SDL recibe un Uint8
        if (alpha > 255) alpha = 255;
        SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, alpha);
        SDL_Rect part_rect = {(int)curr_part->x, (int)curr_part->y, 6, 6};
        SDL_RenderFillRect(renderer, &part_rect);
        curr_part = curr_part->next;
    }
    SDL_RenderPresent(renderer);
}

int inicializar() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 0;
    window = SDL_CreateWindow("Tetris Base - TP", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) return 0;
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) return 0;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    return 1;
}



// ========= TESTS (COMPLETAR) =========

int contarFilas(GameBoard* board) {         //funcion para contar la cantidad de filas que tiene el tablero, se utiliza para testear que la funcion gameBoardCheckAndClearLines funcione correctamente.
    int contador = 0;                       //contador de filas, empieza en 0
    RowNode* actual = board->first_row;     //puntero que apunta a la primera fila del tablero
    while (actual != NULL) {
        contador++;
        actual = actual->next;
    }
    return contador;
}


void correrTests() {
    // TODO: Implementar los casos minimos del enunciado. Por ejemplo:
    //  - strDuplicate: "", "x", un string con distintos caracteres.
    int fallos = 0;
    char* d1 =strDuplicate("");             // un string vacio 
    char* d2 = strDuplicate("x");           // Un solo caracter
    char* d3 = strDuplicate("hola mundo");  // Distintos caracteres
    
    // Liberamos la memoria de los duplicados y chequeamos que esten bien 
    if (d1 == NULL || d1[0] != '\0') { printf("FALLO: strDuplicate vacio\n"); fallos++; }
    if (d2 == NULL || d2[0] != 'x' || d2[1] != '\0') { printf("FALLO: strDuplicate 'x'\n"); fallos++; }
    if (d3 == NULL || strCompare(d3, "hola mundo") != 0) { printf("FALLO: strDuplicate 'hola mundo'\n"); fallos++; }
    free(d1); free(d2); free(d3);


    
    // ================= strCompare =================
    // Creamos los strings con strDuplicate para poder liberarlos despues.
    char* a1 = strDuplicate("a");
    char* a2 = strDuplicate("a");
    char* v1 = strDuplicate("");
    char* v2 = strDuplicate("");
    char* b  = strDuplicate("b");

    // Iguales: "" vs "" y "a" vs "a" deben dar 0.
    if (strCompare(v1, v2) != 0) { printf("FALLO: '' vs ''\n"); fallos++; }
    if (strCompare(a1, a2) != 0) { printf("FALLO: 'a' vs 'a'\n"); fallos++; }

    // Desigualdad en ambos sentidos. Segun tu convencion:
    // s1 < s2 devuelve 1, y s1 > s2 devuelve -1.
    if (strCompare(a1, b) != 1)  { printf("FALLO: 'a' vs 'b'\n"); fallos++; }
    if (strCompare(b, a1) != -1) { printf("FALLO: 'b' vs 'a'\n"); fallos++; }

    free(a1); free(a2); free(v1); free(v2); free(b);



    // casos bases de strConcatenate
    // los argumentos se crean con strDuplicate porque strConcatenate los libera internamente.
    
    char* r1 = strConcatenate(strDuplicate(""), strDuplicate("abc"));
    char* r2 = strConcatenate(strDuplicate("hola "), strDuplicate("mundo"));

    // Verificamos que el resultado sea el esperado.
    if (strCompare(r1, "abc") != 0) { printf("FALLO: '' + 'abc'\n"); fallos++; }
    if (strCompare(r2, "hola mundo") != 0) { printf("FALLO: 'hola ' + 'mundo'\n"); fallos++; }

    // Lo unico que hay que liberar es el resultado final.
    free(r1); free(r2);


    // ================= Cola =================
    //  - Cola: 5 push y 6 pop; el 6to debe devolver 0 y dejar queue_head y queue_tail en NULL.

    GameBoard* test_board = gameBoardNew(); //creamos un nuevo tablero para testear la cola
    int valor = -1;         //para guardar el valor que se saca de la cola (-1 es un valor distinto a los posibles para las piezas)

    if (test_board == NULL) {
        printf("FALLO: gameBoardNew devuelve NULL\n");
        fallos++;
    }

    for(int i = 0; i < 5; i++) {
        gameBoardEnqueuePiece(test_board, i);   //entran las piezas 0,1,2,3,4
    }

    for(int i = 0; i < 5; i++) {
        int resultado = gameBoardDequeuePiece(test_board, &valor); //sale una pieza y se guarda en valor
        if (resultado != 1 || valor != i) {
            printf("FALLO: orden FIFO\n");
            fallos++;
        }
    }

    //6to pop
    if (gameBoardDequeuePiece(test_board, &valor) != 0) { 
        printf("FALLO: 6to pop no devuelve 0\n");
        fallos++;
    }

    if (test_board->queue_head != NULL || test_board->queue_tail != NULL) {
        printf("FALLO: queue_head o queue_tail no son NULL despues de vaciar la cola\n");
        fallos++;
    }

    if (valor != 4) { //el valor de la variable valor no deberia haber cambiado, ya que el 6to pop no modifica el valor
        printf("FALLO: valor de variable 'valor' cambio despues de 6to pop\n");
        fallos++;
    }

    //    Verificar tambien que un push posterior funcione.
    gameBoardEnqueuePiece(test_board, 6);
    if (gameBoardDequeuePiece(test_board, &valor) != 1 || valor != 6) {
        printf("FALLO: push despues de vaciar la cola no funciona\n");
        fallos++;
    }

    gameBoardDelete(test_board); //liberamos el tablero de prueba


    //  - Tablero: llenar la fila de indice 19 (la de abajo), llamar a gameBoardCheckAndClearLines y comprobar que first_row es la fila nueva vacia y que la lista sigue teniendo 20 filas.

    GameBoard* test_board2 = gameBoardNew();//creamos un nuevo tablero con 20 filas vacias
    RowNode* primera_fila = test_board2->first_row; //guardamos cual es la primera fila al principio

    RowNode* ultima_fila = getRowAt(test_board2, 19); //buscamos la fila 19 (la última)
    for(int c = 0; c < GRID_COLS; c++) {
        ultima_fila->cells[c] = 1;  //llenamos la fila de abajo con 1s
    }

    int lineas_borradas = gameBoardCheckAndClearLines(test_board2); //llamamos a la funcion para borrar la fila llena

    if (lineas_borradas != 1) { //deberia haber borrado 1 fila
        printf("FALLO: gameBoardCheckAndClearLines no devuelve 1 al borrar una fila\n");
        fallos++;
    }

    if(test_board2->first_row == primera_fila) { //la primera fila deberia ser distinta a la que habia al principio, ya que se borro la fila de abajo y se agrego una nueva al inicio
        printf("FALLO: gameBoardCheckAndClearLines no actualiza first_row\n");
        fallos++;
    }

    for (int c= 0; c < GRID_COLS; c++) {
        if (test_board2->first_row->cells[c] != 0) { //la nueva primera fila deberia estar vacia
            printf("FALLO: gameBoardCheckAndClearLines no crea una nueva fila vacia\n");
            fallos++;
        }
    }

    if (contarFilas(test_board2) != GRID_ROWS) { //deberia seguir habiendo 20 filas
        printf("FALLO: gameBoardCheckAndClearLines no mantiene la cantidad de filas en 20\n");
        fallos++;
    }

    gameBoardDelete(test_board2);


    // ================= filas 0 y 1 =================

    GameBoard* test_board3 = gameBoardNew(); //creamos un nuevo tablero con 20 filas vacias

    RowNode* fila0 = getRowAt(test_board3, 0); //primera fila
    RowNode* fila1 = getRowAt(test_board3, 1); //segunda fila
    for(int c = 0; c < GRID_COLS; c++) {
        fila0->cells[c] = 1; //llenamos la primera fila con 1s
        fila1->cells[c] = 1; //llenamos la segunda fila con 1s
    }

    int lineas_borradas2 = gameBoardCheckAndClearLines(test_board3); //llamamos a la funcion para borrar las filas llenas

    if (lineas_borradas2 != 2) { //deberia haber borrado 2 filas
        printf("FALLO: gameBoardCheckAndClearLines no devuelve 2 al borrar dos filas\n");
        fallos++;
    }

    if (contarFilas(test_board3) != GRID_ROWS) { //deberia seguir habiendo 20 filas
        printf("FALLO: gameBoardCheckAndClearLines no mantiene la cantidad de filas en 20 despues de borrar dos filas\n");
        fallos++;
    }

    for(int c= 0; c < GRID_COLS; c++) {
        if(getRowAt(test_board3, 0)->cells[c] != 0 || getRowAt(test_board3, 1)->cells[c] != 0) { //las nuevas filas 0 y 1 deberian estar vacias
            printf("FALLO: gameBoardCheckAndClearLines no crea nuevas filas vacias despues de borrar dos filas\n");
            fallos++;
        }
    }

    gameBoardDelete(test_board3);

    // Liberar toda la memoria pedida en los tests (strings y tableros).


    // ====== chequeo de resultados ======
    if (fallos == 0){ //si no hubo fallos, se imprime que todos los tests pasaron
        printf("Todos los tests pasaron.\n");
    }
    else {
        printf("Hubo %d fallos en los tests.\n", fallos);
    }
}


// Compara contra "--test" sin depender de string.h ni del Ejercicio 1.
int esFlagTest(char* arg) {
    char* flag = "--test";
    int i = 0;
    while (arg[i] != '\0' && arg[i] == flag[i]) i++;
    return arg[i] == '\0' && flag[i] == '\0';
}

int main(int argc, char* args[]) {
    srand(time(NULL));

    // Los tests corren antes de SDL. Con --test termina aca, asi valgrind solo
    // reporta la memoria del TP (SDL tiene bloques propios que no se liberan).
    correrTests();
    if (argc > 1 && esFlagTest(args[1])) return 0;

    if (!inicializar()) return 1;

    game_board = gameBoardNew();
    if (game_board == NULL) {
        printf("ADVERTENCIA: Implementa gameBoardNew() para iniciar el juego.\n");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }

    for(int i=0; i<3; i++) gameBoardEnqueuePiece(game_board, rand() % 7);

    SDL_Event e;
    int game_over = 0;

    while (!game_over) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) game_over = 1;
            if (e.type == SDL_KEYDOWN && game_board->current_piece.active) {
                if (e.key.keysym.sym == SDLK_LEFT && canMove(game_board, game_board->current_piece.col - 1, game_board->current_piece.row, game_board->current_piece.rotation)) game_board->current_piece.col--;
                if (e.key.keysym.sym == SDLK_RIGHT && canMove(game_board, game_board->current_piece.col + 1, game_board->current_piece.row, game_board->current_piece.rotation)) game_board->current_piece.col++;
                if (e.key.keysym.sym == SDLK_DOWN && canMove(game_board, game_board->current_piece.col, game_board->current_piece.row + 1, game_board->current_piece.rotation)) game_board->current_piece.row++;
                if (e.key.keysym.sym == SDLK_UP) {
                    int next_rot = (game_board->current_piece.rotation + 1) % 4;
                    if (canMove(game_board, game_board->current_piece.col, game_board->current_piece.row, next_rot)) game_board->current_piece.rotation = next_rot;
                }
                if (e.key.keysym.sym == SDLK_SPACE) {
                     while(canMove(game_board, game_board->current_piece.col, game_board->current_piece.row + 1, game_board->current_piece.rotation)) game_board->current_piece.row++;
                     lockPiece(game_board);
                }
            }
        }
        if (!gameBoardUpdate(game_board)) game_over = 1;
        gameBoardDraw(game_board);
        SDL_Delay(16);
    }
    gameBoardDelete(game_board);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
