#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grafo.h"

typedef struct {
    void *dato;
    List *aristas;
} NodoGrafo;

struct Grafo {
    NodoGrafo *nodos;
    int capacidad;
};

Grafo *grafo_crear(int capacidad) {
    Grafo *g = (Grafo *)malloc(sizeof(Grafo));
    g->capacidad = capacidad;
    g->nodos = (NodoGrafo *)malloc(sizeof(NodoGrafo) * capacidad);
    for (int i = 0; i < capacidad; i++) {
        g->nodos[i].dato = NULL;
        g->nodos[i].aristas = list_create();
    }
    return g;
}

static int idValido(Grafo *g, int id) {
    return g != NULL && id >= 0 && id < g->capacidad;
}

void grafo_agregarNodo(Grafo *g, int id, void *dato) {
    if (!idValido(g, id)) return;
    g->nodos[id].dato = dato;
}

void grafo_agregarArista(Grafo *g, int origen, int destino, const char *etiqueta) {
    if (!idValido(g, origen) || !idValido(g, destino)) return;

    AristaGrafo *arista = (AristaGrafo *)malloc(sizeof(AristaGrafo));
    arista->destino = destino;
    arista->etiqueta = strdup(etiqueta);
    list_pushBack(g->nodos[origen].aristas, arista);
  
}

void *grafo_obtenerDato(Grafo *g, int id) {
    if (!idValido(g, id)) return NULL;
    return g->nodos[id].dato;
}

int grafo_vecinoPorEtiqueta(Grafo *g, int origen, const char *etiqueta) {
    if (!idValido(g, origen)) return -1;

    List *aristas = g->nodos[origen].aristas;
    for (AristaGrafo *a = list_first(aristas); a != NULL; a = list_next(aristas)) {
        if (strcasecmp(a->etiqueta, etiqueta) == 0) {
            return a->destino;
        }
    }
    return -1;
}

List *grafo_aristasDe(Grafo *g, int origen) {
    if (!idValido(g, origen)) return NULL;
    return g->nodos[origen].aristas;
}

int grafo_capacidad(Grafo *g) {
    return g == NULL ? 0 : g->capacidad;
}

void grafo_destruir(Grafo *g) {
    if (g == NULL) return;
    for (int i = 0; i < g->capacidad; i++) {
        List *aristas = g->nodos[i].aristas;
        for (AristaGrafo *a = list_first(aristas); a != NULL; a = list_next(aristas)) {
            free(a->etiqueta);
            free(a);
        }
        list_clean(aristas);
        free(aristas);
    }
    free(g->nodos);
    free(g);
}