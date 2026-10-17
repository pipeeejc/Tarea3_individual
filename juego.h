#ifndef JUEGO_H
#define JUEGO_H  

#include "list.h"
#include "grafo.h"

#define TIEMPO_INICIAL 10

typedef enum {
    JUGANDO,
    GANADO,
    PERDIDO
} EstadoPartida;

typedef struct {
    char *nombre;
    int valor; //puntos que aporta al puntaje
    int peso; //kg que aporta al inventario
    int tomado; // 1 si ya esta en el inventario de alguna partida en curso
} Item;

typedef struct {
    int id;
    char *nombre;
    char *descripcion;
    List *items;
    int esFinal;
} Escenario;

typedef struct {
    int escenarioActualId;
    int escenarioInicialId;
    List *inventario;
    int pesoTotal;
    int puntajeTotal;
    int tiempoRestante;
    EstadoPartida estado;
} Jugador;

/* Carga el laberinto desde un archivo CSV y construye el grafo de escenarios. */
Grafo *juego_cargarCSV(const char *ruta, int *idInicial);

/* Crea un jugador nuevo*/
Jugador *jugador_crear(int idInicial);

/* Reinicia la partida*/
void jugador_reiniciar(Jugador *j, Grafo *g);

/* Calcula el costo en tiempo de avanzar, segun el peso total de inventario */
int juego_costoMovimiento(int pesoTotal);

/* Libera toda la memoria del laberinto*/
void juego_destruirGrafo(Grafo *g);

/* Libera la memoria propia del jugador (su lista de inventario; los Item* no se liberan aqui, son propiedad de los Escenario). */
void jugador_destruir(Jugador *j);

#endif