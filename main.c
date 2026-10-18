#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "juego.h"
#include "grafo.h"
#include "list.h"

#define TAM_BUFFER 512


static int leerEntrada(char *buffer, int tam) {
    if (fgets(buffer, tam, stdin) == NULL) return -1;
    buffer[strcspn(buffer, "\r\n")] = '\0';
    return 0;
}

static void imprimirItemsDisponibles(Escenario *e) {
    int hay = 0;
    int idx = 1;
    printf("Items disponibles en este escenario:\n");
    for (Item *item = list_first(e->items); item != NULL; item = list_next(e->items)) {
        if (!item->tomado) {
            printf("  %d) %s (%d pts, %d kg)\n", idx, item->nombre, item->valor, item->peso);
            hay = 1;
        }
        idx++;
    }
    if(!hay) printf("  (no hay items disponibles aqui)\n");
}

static void imprimirInventario(Jugador *j) {
    printf("Inventario (peso total: %d kg | puntaje acumulado: %d pts):\n", j->pesoTotal, j->puntajeTotal);
    if (list_size(j->inventario) == 0) {
        printf("  (vacio)\n");
        return;
    }
    int idx = 1;
    for (Item *item = list_first(j->inventario); item != NULL; item =list_next(j->inventario)) {
        printf("  %d) %s (%d pts, %d kg)\n", idx, item->nombre, item->valor, item->peso);
        idx++;
    }
}

static void imprimirDirecciones(Grafo *g, int escenarioId) {
    printf("Direcciones disponibles: ");
    List *aristas = grafo_aristasDe(g, escenarioId);
    if (list_size(aristas) == 0) {
        printf("(ninguna)\n");
        return;
    }
    int primero = 1;
    for (AristaGrafo *a = list_first(aristas); a != NULL; a = list_next(aristas)) {
        if (!primero) printf(", ");
        printf("%s", a->etiqueta);
        primero = 0;
    }
    printf("\n");
}

static void mostrarEstado(Grafo *g, Jugador *j) {
    Escenario *e = (Escenario *)grafo_obtenerDato(g, j->escenarioActualId);

    printf("\n========================================\n");
    printf("Escenario: %s\n", e->nombre);
    printf("----------------------------------------\n");
    printf("%s\n\n", e->descripcion);

    imprimirItemsDisponibles(e);
    printf("\n");
    printf("Tiempo restante: %d\n", j->tiempoRestante);
    imprimirInventario(j);
    printf("\n");
    imprimirDirecciones(g, j->escenarioActualId);
    printf("========================================\n");
}

static int leerIndices(char *entrada, int indices[], int maxIndices) {
    int n = 0;
    char *token = strtok(entrada, " ,");
    while (token != NULL && n < maxIndices) {
        indices[n++] = atoi(token);
        token = strtok(NULL, " ,");
    }
    return n;
}

static void accionRecoger(Escenario *e, Jugador *j) {
    if (list_size(e->items) == 0) {
        printf("No hay items en este escenario.\n");
        return;
    }
    char entrada[TAM_BUFFER];
    printf("Ingrese el/los numero(s) de item(s) a recoger (ej: 1 3): ");
    if (leerEntrada(entrada, TAM_BUFFER) != 0) return;

    int indices[TAM_BUFFER];
    int cantidad = leerIndices(entrada, indices ,TAM_BUFFER);
    if (cantidad == 0) {
        printf("No se selecciono ningun item.\n");
        return;
    }
    int algunoRecogido = 0;
    for (int k = 0; k < cantidad; k++) {
        int idx = 1;
        for (Item *item = list_first(e->items); item != NULL; item = list_next(e->items)) {
            if (!item->tomado && idx == indices[k]) {
                item->tomado = 1;
                list_pushBack(j->inventario, item);
                j->pesoTotal += item->peso;
                j->puntajeTotal += item->valor;
                printf("Recogiste: %s\n", item->nombre);
                algunoRecogido = 1;
                break;
            }
            idx++;
        }
    }
    if (algunoRecogido) {
        j->tiempoRestante -= 1;
        printf("(-1 de tiempo)\n");
    } else {
        printf("Ninguno de esos numeros corresponde a un item disponible.\n");
    }
}

static void accionDescartar(Jugador *j) {
    if (list_size(j->inventario) == 0) {
        printf("Tu inventario esta vacio.\n");
        return;
    }
    char entrada[TAM_BUFFER];
    printf("Ingrese el/los numero(s) de item(s) a descartar (ej: 1 2): ");
    if (leerEntrada(entrada, TAM_BUFFER) != 0) return;

    int indices[TAM_BUFFER];
    int cantidad = leerIndices(entrada, indices, TAM_BUFFER);
    if (cantidad == 0) {
        printf("No se selecciono ningun item\n");
        return;
    }

    //se marcan los items a descartar; luego se reconstruye le inventario
    //sin ellos (evita problemas de indices al ir eliminando en el camino).
    List *nuevoInventario = list_create();
    int idx = 1;
    int algunoDescartado = 0;
    for (Item *item = list_first(j->inventario); item != NULL; item = list_next(j->inventario)) {
        int descartar = 0;
        for (int k = 0; k < cantidad; k++) {
            if (indices[k] == idx) { descartar = 1; break; }
        }
        if (descartar) {
            j->pesoTotal -= item->peso;
            j->puntajeTotal -= item->valor;
            printf("Descartgaste: %s\n", item->nombre);
            algunoDescartado = 1;
        } else {
            list_pushBack(nuevoInventario, item);
        }
        idx++;
    }
    list_clean(j->inventario);
    free(j->inventario);
    j->inventario = nuevoInventario;

    if (algunoDescartado) {
        j->tiempoRestante -= 1;
        printf("(-1 de tiempo)\n");
    } else {
        printf("Ninguno de esos numeros corresponde a un item de tu inventario.\n");
    }
}

static void accionAvanzar(Grafo *g, Jugador *j) {
    char direccion[TAM_BUFFER];  
    printf("Ingrese la direccion (Arriba/Abajo/Izquierda(Derecha): ");
    if (leerEntrada (direccion, TAM_BUFFER) != 0) return;

    int destino = grafo_vecinoPorEtiqueta(g, j->escenarioActualId, direccion);

    if (destino < 0) {
        printf("No puedes avanzar en esa direccion.\n");
        return;
    }
    int costo = juego_costoMovimiento(j->pesoTotal);
    j->tiempoRestante -= costo;
    j->escenarioActualId = destino;
    printf("Avanzas hacia %s. (-%d de tiempo)\n", direccion, costo);

    Escenario *nuevoEscenario = (Escenario *)grafo_obtenerDato(g, destino);
    if (nuevoEscenario->esFinal) {
        j->estado = GANADO;
    } else if (j->tiempoRestante <= 0) {
        j->estado = PERDIDO;
    }
}
static void mostrarFinDePartida(Jugador *j) {
    if (j->estado == GANADO) {
        printf("\n*** ¡Llegaste a la salida! ***\n");
        printf("Puntaje final: %d pts (peso transportado: %d kg)\n", j->puntajeTotal, j->pesoTotal);
    } else if (j->estado == PERDIDO) {
        printf("\n*** Se acabo el tiempo. Has perdido. ***\n");
        printf("Puntaje Obtenido: %d pts\n", j->puntajeTotal);
    }
    printf("Puedes elegir 'Reiniciar Partida' para intentarlo de nuevo.\n");
}

static void mostrarMenuJuego() {
    printf("\nOpciones:\n");
    printf("1) Recoger Item(s)\n");
    printf("2) Descartar Item(s)\n");
    printf("3) Avanzar en una Direccion\n");
    printf("4) Reiniciar Partida\n");
    printf("5) Salir del juego\n");
    printf("Elige una opcion: ");
}

static void iniciarPartida(Grafo *g, int idInicial) {
    Jugador *j = jugador_crear(idInicial);
    char opcion[TAM_BUFFER];
    int salir = 0;

    while (!salir) {
        mostrarEstado(g, j);

        if (j->estado != JUGANDO) {
            mostrarFinDePartida(j);
        }

        mostrarMenuJuego();
        if (leerEntrada(opcion, TAM_BUFFER) != 0) break;

        Escenario *e = (Escenario *)grafo_obtenerDato(g, j->escenarioActualId);

        if (strcmp(opcion, "1") == 0) {
            if (j->estado != JUGANDO) printf("La partida termino. Reinicia para seguir jugando.\n");
            else accionRecoger(e, j);
          
        } else if (strcmp(opcion, "2") == 0) {
            if (j->estado != JUGANDO) printf("La partida termino. Reinicia para seguir jugando.\n");
            else accionDescartar(j);
          
        } else if (strcmp(opcion, "3") == 0) {
            if (j->estado != JUGANDO) printf("La partida termino. Reinicia para seguir jugando.\n");
            else accionAvanzar(g, j);
    
        } else if (strcmp(opcion, "4") == 0) {
            jugador_reiniciar(j, g);
            printf("Partida reiniciada.\n");
          
        } else if (strcmp(opcion, "5") == 0) {
            printf("Hasta pronto!\n");
            salir = 1;
          
        } else {
            printf("Opcion invalida. Intente nuevamente.\n");
        }
    }
    jugador_destruir(j);
}

static void mostrarMenuPrincipal() {
    printf("========================================\n");
    printf("              GRAPHQUEST\n");
    printf("========================================\n");
    printf("1) Cargar Laberinto desde Archivo CSV\n");
    printf("2) Iniciar Partida\n");
    printf("3) Salir\n");
    printf("Ingrese su opcion: ");
}

int main () {
    Grafo *g = NULL;
    int idInicial = -1;
    char opcion[TAM_BUFFER];
    int salir = 0;

    while (!salir) {
        mostrarMenuPrincipal();
        if (leerEntrada(opcion, TAM_BUFFER) != 0) break;
        printf("\n");

        if (strcmp(opcion, "1") == 0) {
            char ruta[TAM_BUFFER];
            printf("Ingrese la rita del archivo CSV: ");
            if (leerEntrada(ruta, TAM_BUFFER) != 0) continue;

            if (g != NULL) juego_destruirGrafo(g); // se descarta laberinto anterior, si habia

            g = juego_cargarCSV(ruta, &idInicial);
            if (g == NULL) {
                printf("No se pudo cargar el archivo '%s'.\n", ruta);
            } else {
                printf("Laberinto cargado exitosamente (%d escenarios).\n", grafo_capacidad(g) - 1);
            } 
              
            } else if (strcmp(opcion, "2") == 0) {
                if (g == NULL) {
                    printf("Primero debe cargar un laberinto (opcion 1).\n");
                } else {
                    iniciarPartida(g, idInicial);
                    salir = 1;
                }
            } else if (strcmp(opcion, "3") == 0) {
                printf("Hasta pronto!\n");
                salir = 1;
            } else {
                printf("Opcion invalida. Intente nuevamente.\n");
            }
            printf("\n");
        }

        if (g != NULL) juego_destruirGrafo(g);
        return 0;
    }