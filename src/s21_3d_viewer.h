#ifndef S21_3D_VIEWER_H
#define S21_3D_VIEWER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "s21_matrix.h"

// полигон a.k.a. грань
typedef struct polygon_struct {
  int *vertexes;                         // массив вершин
  unsigned amount_of_vertexes_in_facet;  // количество вершин в грани
} polygon_t;

// все данные 3d-модели
typedef struct data_struct {
  unsigned amount_of_vertexes;  // количество вершин
  unsigned amount_of_facets;    // количество граней
  unsigned indexes;  // количество индексов (передаётся в отрисовку)
  matrix_t matrix_3d;   // матрица значений вершин
  polygon_t *polygons;  // массив полигонов
} data_t;

// хранит данные о вращении объекта
typedef struct rotation_struct {
  double ox;
  double oy;
  double oz;
} rotation;

// хранит данные о перемещении объекта
typedef struct moving_struct {
  double ox;
  double oy;
  double oz;
} moving;

void count_vertexes_and_facets(const char *filename, unsigned *vertexes,
                               unsigned *facets, unsigned g);
unsigned count_vertexes_in_facet(char *str);
unsigned count_groups(const char *filename);

int create_data(data_t *d, unsigned v, unsigned f, const char *filename,
                unsigned g);
void free_data(data_t *d);
void make_vert_arr(matrix_t A, double *vert_arr);
void make_ind_arr(data_t d, int *ind_arr);

void get_min_and_max_x_y(data_t data, double *a, double *b, double *c,
                         double *d);

void rotation_by_ox(matrix_t *A, double angle);
void rotation_by_oy(matrix_t *A, double angle);
void rotation_by_oz(matrix_t *A, double angle);
void zoom(matrix_t *A, double coef);
void move_object(matrix_t *A, double bias, char axis_char);

#ifdef __cplusplus
}
#endif

#endif  // S21_3D_VIEWER_H
