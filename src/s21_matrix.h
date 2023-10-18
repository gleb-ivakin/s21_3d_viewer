#ifndef SRC_S21_MATRIX_H_
#define SRC_S21_MATRIX_H_
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define EPS 0.0000001

typedef struct matrix_struct {
  double **matrix;
  int rows;
  int columns;
} matrix_t;

// Основные:
int s21_create_matrix(int rows, int columns, matrix_t *result);
void s21_remove_matrix(matrix_t *A);
int s21_eq_matrix(matrix_t *A, matrix_t *B);
int s21_sum_matrix(matrix_t *A, matrix_t *B, matrix_t *result);
int s21_sub_matrix(matrix_t *A, matrix_t *B, matrix_t *result);
int s21_mult_number(matrix_t *A, double number, matrix_t *result);
int s21_mult_matrix(matrix_t *A, matrix_t *B, matrix_t *result);
int s21_transpose(matrix_t *A, matrix_t *result);
int s21_calc_complements(matrix_t *A, matrix_t *result);
int s21_determinant(matrix_t *A, double *result);
int s21_inverse_matrix(matrix_t *A, matrix_t *result);

// Вспомогательные:
void get_minor(matrix_t *A, matrix_t *result, int oy, int ox);
void print_matrix(matrix_t A);
int matrix_is_empty(matrix_t *A);

#endif  //  SRC_S21_MATRIX_H_
