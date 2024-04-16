/*
  Fecha: 11/04/04
  Autor: María José Cárdenas Machaca
  Materia: Sistemas Operativos
  Tema: Concurrencia
  Objetivo: Metodologia para la implementacion de la
  multiplicacion de matrices haciendo uso de paralelismo
  por medio de la biblioteca Pthreads. Se hace uso del
  algoritmo clásico de multiplicación de matrices, en este
  caso, matrices cuadradas.
  ----mm_v01.c
    -Reserva de memoria.
    -Creación de punteros para matrices del tipo double.
    -Asignación de memoria.
    -Ingreso de argumentos de entrada.
    -Validación argumentos de entrada
  ----mm_v02.c
    -Inicializar las matrices.
    -Imprimir las matrices.
    -Funcion para inicializar las matrices.
    -Funcion para imprimir las matrices.
    -Algoritmo de multiplicacion de matrices.
    -Verificación del resultado
  ----mm_v03.c
    -Delaracion vector de hilos.
    -Creacion de hilos segun tamaño de vector de hilos
    -Crear estructura de datos que encapsule de entrada para la función de
  multiplicación -Adaptar la función multiplicaciónMariz -Creación de estrucutra
  de datos que encapsule
  ----mm_v04.C
    -Implementación del paralelismo de la multiplicacion de matrices
    -Pendiente la exlusion de los hilos
    -Se pasa a globales las matrices
    -Encapsular los datos para enviarlos a la función multiplciacion
    -Desencapsular los datos de la fución de multiplicación
*/

// Librerias usadas en el programa
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

/*--- Reserva de memoria --*/
// Asignación de un valor a la variable RESERVA
#define RESERVA (1024 * 128 * 64 * 8)

/*--- Estructura de datos ---*/
struct datosMM {
  int N;   // Tamaño de la matriz (N*N)
  int H;   // Número de hilos
  int idH; // Identidad de los hilos
};

// Variables globales que representan los parametros necesarios para el
// algoritmo de multiplicación de matrices
double *mA, *mB, *mC;

// Creación de un vector cuyo tamaño sera igual a RESERVA
static double MEM_CHUNK[RESERVA];

/*--- Funcición para inicializar matrices ---*/
void inicializarMatrices(int n) {
  for (int i = 0; i < n * n; i++) {
    mA[i] = i * 1.1;
    mB[i] = i * 2.2;
    mC[i] = i;
  }
}

/*--- Función para imprimir una matriz ---*/
void imprimirMatriz(int n, double *m) {
  // La función solo recibe el tamaño de la matriz y matriz resultante, ya que
  // la matriz A y B so variables globales
  if (n < 9) {
    for (int i = 0; i < n * n; i++) {
      if (i % n == 0) {
        printf("\n");
      }
      printf("%f ", m[i]);
    }
    printf("\n***************************************\n");
  } else {
    printf("\n***************************************\n");
  }
}

/*--- Función que implementa el algoritmo de multiplicación de matrices ---*/
void *multiplicacionMatriz(
    void *argMM) { // El parametro void de la función apunta a la estructura de
                   // datosMM que contiene: el tamaño de la matriz, el número de
                   // hilos y el identificador de los hilos
  struct datosMM *valoresINT = (struct datosMM *)argMM;
  // Descomprimir la estructora para extraer los valores
  int N = valoresINT->N;     // Tamaño de la matriz
  int H = valoresINT->H;     // Número de hilos
  int idH = valoresINT->idH; // Identificador de los hilos

  // Calculo de los indices de inicion o fin para el hilo
  int ini = (N / H) * idH; // Inicio del hilo actual
  int fin;                 // Fin del hilo actual
  if (idH == H - 1) { // Si el hilo actual es el último se encargará hasta la
                      // última fila de la matriz A (N)
    fin = N;
  } else { // En caso contrario, el hilo actual se encargará hasta la fila media
           // de la matriz A
    fin = ini + (N / H);
  }

  // Impresión que verifica la correcta asignación de filas de la matriz A para
  // cada hilo
  printf("\nHilos= %d ; IdHilo= %d ; ini = %d ; fin = %d \n", H, idH, ini, fin);

  // Multiplicación de matricez asignando el rango del hilo
  for (int i = ini; i < fin; i++) {
    for (int j = 0; j < N; j++) {
      double *pA, *pB, sumaTemp;
      sumaTemp = 0.0;
      for (int k = 0; k < N; k++) { // Recorrido de la matriz resultado
        sumaTemp +=
            mA[i * N + k] *
            mB[k * N +
               j]; // Multiplicación de las variables de la matriz A y matriz B
      }
      mC[j + i * N] = sumaTemp; // Guardar el resultado en la matriz C
    }
  }
  free(argMM);        // Liberar memoria
  pthread_exit(NULL); // Finalizar el hilo actual
}

int main(int argc, char *argv[]) {
  // Verficacion de la cantidad de argumentos ingresados
  if (argc < 3) {
    printf("No se ingresaron suficientes elementos\n");
    return 1;
  } else if (atoi(argv[1]) <= 0 && atoi(argv[2]) <= 0) {
    printf("Los elementos son menores o iguales a 0\n");
    return 1;
  }
  // Captura de los valores ingresados como argumento.
  int N = atoi(argv[1]);
  int H = atoi(argv[2]);

  // Validación del número de hilos
  if (H > N) {
    printf("El número de hilos no puede ser mayor que N.\n");
    return 1;
  } else if (H < 0) {
    printf("El número de hilos no puede ser negativo.\n");
    return 1;
  } else {
    printf("\n");
  }

  // Declaración de un arreglo de indentificadores de tipo pthread_t
  pthread_t hilos[H];

  // Definicion de los espacios de memoria a los que van a hacer referencia los
  // punteros
  mA = MEM_CHUNK;
  mB = mA + (N * N);
  mC = mB + (N * N);

  // Inicializacion de matrices.
  inicializarMatrices(N);

  // Impresion de matrices
  printf("Matriz A\n");
  imprimirMatriz(N, mA);
  printf("Matriz B\n");
  imprimirMatriz(N, mB);

  // Creación de hilos
  for (int h = 0; h < H; h++) {
    // Implementación de memoria dinámica para almacenar la estrcutura datosMM
    struct datosMM *valoresMM =
        (struct datosMM *)malloc(sizeof(struct datosMM));
    // Se comprimen los datos para que sean guardados en la estructura
    valoresMM->N = N;
    valoresMM->H = H;
    valoresMM->idH = h;
    // Creación de un hilo que tiene por parámetros: Un identificador, atributos
    // NULL, la función d multiplicación y lo parametros que necesita la función
    pthread_create(&hilos[h], NULL, multiplicacionMatriz, valoresMM);
  }

  // Esperando a que los hilos finalicen para porder imprimir la matriz C
  for (int h = 0; h < H; h++) {
    pthread_join(hilos[h], NULL);
  }
  // Impresión de matriz C
  printf("\nMatriz Producto mA * mB\n");
  imprimirMatriz(N, mC);

  printf("\nFin del programa\n");

  pthread_exit(NULL);
}