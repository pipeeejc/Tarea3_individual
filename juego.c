#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "juego.h"
#include "extra.h"

static int contarFilas(const char *ruta) {
    FILE *archivo = fopen(ruta, "r");
    if (archivo == NULL) return -1;

    int contador = 0;
    char **campos = leer_linea_csv(archivo, ',');
    if (campos == NULL) {
        fclose(archivo);
        return -1;
    }
    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        contador++;
    }
    fclose(archivo);
    return contador;
}

static void parsearItems(const char *campoItems, List *listaItems) {
    if (campoItems == NULL || strlen(campoItems) == 0) return;

    char *copia = strdup(campoItems);
    List *descriptores = split_string(copia, ";");

    for (char *desc = list_first(descriptores); desc != NULL; desc = list_next(descriptores)) {
        char *copiaDesc = strdup(desc);
        char *tokNombre = strtok(copiaDesc, ",");
        char *tokValor = strtok(NULL, ",");
        char *tokPeso = strtok(NULL, ",");

        if (tokNombre != NULL & tokValor != NULL & tokPeso != NULL) {
            Item *item = (Item *)malloc(sizeof(Item));
            item->nombre = strdup(tokNombre);
            item->valor = atoi(tokValor);
            item->peso = atoi(tokPeso);
            item->tomado = 0;
            list_pushBack(listaItems, item);
        }
        free(copiaDesc);
        free(desc);
    }
    list_clean(descriptores);
    free(descriptores);
    free(copia);
}

Grafo *juego_cargarCSV(const char *ruta, int *idInicial) {
    int filas = contarFilas(ruta);
    if (filas <= 0) return NULL;

    FILE *archivo = fopen(ruta, "r");
    if (archivo == NULL) return NULL;

    char **header = leer_linea_csv(archivo, ',');
    if (header == NULL) {
        fclose(archivo);
        return NULL;
    }

    int colId = -1, colNombre = -1, colDesc = -1, colItems = -1;
    int colArriba = -1, colAbajo = -1, colIzq = -1, colDer = -1, colFinal = -1;
    for (int i = 0; header[i] != NULL; i++) {
        if(strcmp(header[i], "ID") == 0) colId = i;
        else if (strcmp(header[i], "Nombre") == 0) colNombre = i;
        else if (strcmp(header[i], "Descripcion") == 0) colDesc = i;
        else if (strcmp(header[i], "Items") == 0) colItems = i;
        else if (strcmp(header[i], "Arriba") == 0) colArriba = i;
        else if (strcmp(header[i], "Abajo") == 0) colAbajo = i;
        else if (strcmp(header[i], "Izquierda") == 0) colIzq = i;
        else if (strcmp(header[i], "Derecha") == 0) colDer = i;
        else if (strcmp(header[i], "EsFinal") == 0) colFinal= i;
    }

    if (colId < 0 || colNombre < 0 || colDesc < 0 || colItems < 0 || colArriba < 0 || colAbajo < 0 || colIzq < 0 || colDer < 0 || colFinal < 0) {
        fprintf(stderr, "Error: el CSV no tiene todas las columnas esperadas.\n");
        fclose(archivo);
        return NULL;
    }

    Grafo *g = grafo_crear(filas + 1);

    int esElPrimero = 1;
    char **campos;
    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        int id = atoi(campos[colId]);

        Escenario *e = (Escenario *)malloc(sizeof(Escenario));
        e->id = id;
        e->nombre = strdup(campos[colNombre]);
        e->descripcion = strdup(campos[colDesc]);
        e->items = list_create();
        e->esFinal = (strcasecmp(campos[colFinal], "Si") == 0 || strcasecmp(campos[colFinal], "Sí") == 0);

        parsearItems(campos[colItems], e->items);

        grafo_agregarNodo(g, id, e);

        if (esElPrimero) {
            *idInicial = id;
            esElPrimero = 0;
        }
        int arriba = atoi(campos[colArriba]);
        int abajo = atoi(campos[colAbajo]);
        int izquierda = atoi(campos[colIzq]);
        int derecha = atoi(campos[colDer]);

        if (arriba >= 0) grafo_agregarArista(g, id, arriba ,"Arriba");
        if (abajo >= 0) grafo_agregarArista(g, id, abajo ,"Abajo");
        if (izquierda >= 0) grafo_agregarArista(g, id, izquierda ,"Izquierda");
        if (derecha >= 0) grafo_agregarArista(g, id, derecha ,"Derecha");
    }

    fclose(archivo);
    return g;  
}

Jugador *jugador_crear(int idInicial) {
    Jugador *j = (Jugador *)malloc(sizeof(Jugador));
    j->escenarioActualId = idInicial;
    j->escenarioInicialId = idInicial;
    j->inventario = list_create();
    j->pesoTotal = 0;
    j->puntajeTotal = 0;
    j->tiempoRestante = TIEMPO_INICIAL;
    j->estado = JUGANDO;
    return j;
}

void jugador_reiniciar(Jugador *j, Grafo *g) {
    for (int id = 0; id < grafo_capacidad(g); id++) {
        Escenario *e = (Escenario *)grafo_obtenerDato(g, id);
        if (e == NULL) continue;
        for (Item *item = list_first(e->items); item != NULL; item = list_next(e->items)) {
            item->tomado = 0;  
        }
    }
    list_clean(j->inventario);
    j->escenarioActualId = j->escenarioInicialId;
    j->pesoTotal = 0;
    j->puntajeTotal = 0;
    j->tiempoRestante = TIEMPO_INICIAL;
    j->estado = JUGANDO;
}

int juego_costoMovimiento(int pesoTotal) {
    return (int)ceil((pesoTotal +1) / 10.0);
}
void juego_destruirGrafo(Grafo *g) {
    if (g == NULL) return;
    for (int id = 0; id < grafo_capacidad(g); id++) {
        Escenario *e = (Escenario *)grafo_obtenerDato(g, id);
        if (e == NULL) continue;

        for (Item *item = list_first(e->items); item != NULL; item = list_next(e->items)) {
            free(item->nombre);
            free(item);
        }
        list_clean(e->items);
        free(e->items);
        free(e->nombre);
        free(e->descripcion);
        free(e);
    }
    grafo_destruir(g);
}

void jugador_destruir(Jugador *j) {
    if (j == NULL) return;
    list_clean(j->inventario);
    free(j->inventario);
    free(j);
}