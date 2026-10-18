# GraphQuest

Juego de consola donde se recorre un laberinto representado como un grafo. El jugador decide que items recoger y que caminos seguir antes de que se acabe el tiempo, usando el TDA Grafo.

## Como compilar y ejecutar

En la carpeta raiz:

gcc *.c -Wno-unused-result -o graphquest -lm

y luego:

./graphquest

**Importante:** el '-lm' es obligatorio (se usa 'ceil()' para calcular el costo de movimiento). Sin el, la compilacion falla.

## Uso

### Menu principal
1. Cargar Laberinto: ingresa la ruta del CSV (ej: `graphquest.csv`).
2. Iniciar Partida: comienza el juego desde el escenario inicial.
3. Salir.

### Menu del juego (en cada escenario)
1. Recoger item(s): indica el/los numero(s) del listado (ej: '1 3').
2. Descartar item(s): indica el/los numero(s) del inventario a soltar.
3. Avanzar: escribe la direccion (Arriba/Abajo/Izquierda/Derecha). El costo en tiempo depende del peso del inventario: 'ceil((peso+1)/10)'.
4. Reiniciar Partida: vuelve al escenario inicial, inventario vacio y tiempo completo (los items recogidos o descartados vuelven a estar disponibles en el mundo).
5. Salir del Juego: termina la aplicacion.

## Ejemplo de interaccion

```
========================================
              GRAPHQUEST
========================================
1) Cargar Laberinto desde Archivo CSV
2) Iniciar Partida
3) Salir
Ingrese su opcion: 1
Ingrese la ruta del archivo CSV: graphquest.csv
Laberinto cargado exitosamente (16 escenarios).

Ingrese su opcion: 2

========================================
Escenario: Entrada principal
----------------------------------------
Una puerta rechinante abre paso a esta mansion olvidada...

Items disponibles en este escenario:
  (no hay items disponibles aqui)

Tiempo restante: 10
Inventario (peso total: 0 kg | puntaje acumulado: 0 pts):
  (vacio)

Direcciones disponibles: Abajo
========================================

Opciones:
1) Recoger Item(s)
2) Descartar Item(s)
3) Avanzar en una Direccion
4) Reiniciar Partida
5) Salir del Juego
Elige una opcion: 3
Ingrese la direccion (Arriba/Abajo/Izquierda/Derecha): Abajo
Avanzas hacia Abajo. (-1 de tiempo)

========================================
Escenario: Cocina
...
Items disponibles en este escenario:
  1) Cuchillo (3 pts, 1 kg)
  2) Pan (2 pts, 1 kg)
...
Elige una opcion: 1
Ingrese el/los numero(s) de item(s) a recoger (ej: 1 3): 1 2
Recogiste: Cuchillo
Recogiste: Pan
(-1 de tiempo)
...
```

## TDAs utilizados

- **Grafo** ('grafo.c'): grafo dirigido y etiquetado (arreglo de nodos + lista de aristas salientes por nodo). Representa el laberinto:
cada nodo es un escenario, cada arista una direccion posible.
- **Lista** ('list.c'): aristas salientes de cada nodo del grafo, items de cada escenario, e inventario del jugador.

## Notas y supuestos

- Se asume que los IDs del CSV van de 1 a N sin huecos.
- El primer escenario leido del CSV se toma como punto de partida.
- Un item descartado no vuelve al escenario durante esa partida; solo "Reiniciar Partida" devuelve todos los items al mundo.
- Todas las opciones del menu funcionan correctamente (probado con el CSV de ejemplo, incluyendo victoria, derrota por tiempo y reinicio).