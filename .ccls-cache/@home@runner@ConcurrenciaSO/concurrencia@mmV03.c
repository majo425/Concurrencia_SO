/*
  Fecha: 04/04/04
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
  multiplicación -Adaptar la función multiplicaciónMariz
  ----mm_v04.C
*/

// Librerias usadas en el programa.
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

/*--- Reserva de memoria --*/
// Asignación de un valor a la variable RESERVA
#define RESERVA (1024 * 128 * 64 * 8)
// Creación de un vector cuyo tamaño sera igual a RESERVA
static double MEM_CHUNK[RESERVA];

/*--- Estructura de datos ---*/
struct datosMM {
  int N;
  int H;
  double *mA;
  double *mB;
  double *mC;
};

// Funcición para inicializar matrices
void inicializarMatrices(int n, double *mA, double *mB, double *mC) {
  for (int i = 0; i < n * n; i++) {
    mA[i] = i * 1.1;
    mB[i] = i * 2.2;
    mC[i] = i;
  }
}

// Función para imprimir una matriz
void imprimirMatriz(int n, double *m) {
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

// Función para multiplicar matrices
void *multiplicacionMatriz(void *arg) {
  struct datosMM *valores = (struct datosMM *)arg;
  int N = valores->N;
  double *mA = valores->mA;
  double *mB = valores->mB;
  double *mC = valores->mC;

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      double *pA, *pB, sumaTemp;
      sumaTemp = 0.0;
      pA = mA + (i * N);
      pB = mB + j;
      for (int k = 0; k < N; k++, pA++, pB += N) {
        sumaTemp += *pA * *pB;
      }
      mC[i * N + j] = sumaTemp;
    }
  }
  return NULL;
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

  // Declaración de un arreglo de indentificadores de tipo pthread_t
  pthread_t hilos[H];

  // Creacion de variables apuntadoras para hacer referencias a diferentes
  // espacios de memoria
  double *mA, *mB, *mC;

  // Definicion de los espacios de memoria a los que van a hacer referencia los
  // punteros
  mA = MEM_CHUNK;
  mB = mA + (N * N);
  mC = mB + (N * N);

  // Inicializacion de matrices.
  inicializarMatrices(N, mA, mB, mC);

  // Impresion de matrices
  printf("Matriz A\n");
  imprimirMatriz(N, mA);
  printf("Matriz B\n");
  imprimirMatriz(N, mB);

  // Multiplicación de matrices e impresión de resultado de mC
  printf("Matriz Producto mA * mB\n");

  // Inicialización de la estructura de datos
  struct datosMM dataM;
  dataM.N = N;
  dataM.mA = mA;
  dataM.mB = mB;
  dataM.mC = mC;

  // Creación de hilos
  for (int h = 0; h < H; h++) {
    pthread_create(&hilos[h], NULL, multiplicacionMatriz, &dataM);
  }

  // Esperando a que los hilos finalicen para porder imprimir la matriz C
  for (int h = 0; h < H; h++) {
    pthread_join(hilos[h], NULL);
  }
  // Impresión de matriz C
  imprimirMatriz(N, mC);

  printf("\nFin del programa\n");
  return 0;
}