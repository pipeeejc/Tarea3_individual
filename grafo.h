#ifndef GRAFO_H
#define GRAFO_H

#include "list.h"

typedef struct Grafo Grafo;

typedef struct {
    int destino;
    char *etiqueta;
} AristaGrafo;

Grafo *grafo_crear(int capacidad);

void grafo_agregarNodo(Grafo *g, int id, void *dato);

void grafo_agregarArista(Grafo * g, int origen, int destino, const char * etiqueta);

void *grafo_obtenerDato(Grafo *g, int id);

int grafo_vecinoPorEtiqueta(Grafo *g, int origen, const char *etiqueta);

List *grafo_aristasDe(Grafo *g, int origen);

int grafo_capacidad(Grafo *g);

void grafo_destruir(Grafo *g);

#endif