#include "s21_matrix.h"

int s21_create_matrix(int rows, int columns, matrix_t *result) {
  int ret = 0;
  result->rows = rows;
  result->columns = columns;

  if (rows < 1 || columns < 1) {
    ret = 1;
    result->matrix = NULL;
  } else {
    result->matrix = (double **)calloc(rows, sizeof(double *));

    if (result->matrix == NULL) {
      ret = 2;
    } else {
      for (int i = 0; i < rows && !ret; i++) {
        result->matrix[i] = (double *)calloc(columns, sizeof(double));
        if (result->matrix[i] == NULL) {
          ret = 2;
          for (int j = 0; j < i; j++) free(result->matrix[j]);
          free(result->matrix);
        }
      }

      for (int y = 0; y < result->rows; y++)
        for (int x = 0; x < result->columns; x++) result->matrix[y][x] = 0;
    }
  }

  return ret;
}

void s21_remove_matrix(matrix_t *A) {
  if (!matrix_is_empty(A)) {
    for (int i = 0; i < A->rows; i++)
      if (A->matrix[i]) free(A->matrix[i]);
    if (A->matrix) free(A->matrix);
  }
  if (A->columns) A->columns = 0;
  if (A->rows) A->rows = 0;
}

int s21_eq_matrix(matrix_t *A, matrix_t *B) {
  int ret = 1;

  if (matrix_is_empty(A) || matrix_is_empty(B) || A->columns != B->columns ||
      A->rows != B->rows) {
    ret = 0;
  } else {
    for (int y = 0; (y < A->rows) && ret; y++)
      for (int x = 0; (x < A->columns) && ret; x++)
        if (fabs(A->matrix[y][x] - B->matrix[y][x]) > EPS) ret = 0;
  }

  return ret;
}

int s21_sum_matrix(matrix_t *A, matrix_t *B, matrix_t *result) {
  int ret = 0;

  if (matrix_is_empty(A) || matrix_is_empty(B)) {
    ret = 1;
  } else if (A->rows != B->rows || A->columns != B->columns) {
    ret = 2;
  } else {
    s21_create_matrix(A->rows, A->columns, result);
    for (int y = 0; y < A->rows; y++)
      for (int x = 0; x < A->columns; x++)
        result->matrix[y][x] = A->matrix[y][x] + B->matrix[y][x];
  }

  return ret;
}

int s21_sub_matrix(matrix_t *A, matrix_t *B, matrix_t *result) {
  int ret = 0;

  if (matrix_is_empty(A) || matrix_is_empty(B)) {
    ret = 1;
  } else if (A->rows != B->rows || A->columns != B->columns) {
    ret = 2;
  } else {
    s21_create_matrix(A->rows, A->columns, result);
    for (int y = 0; y < A->rows; y++)
      for (int x = 0; x < A->columns; x++)
        result->matrix[y][x] = A->matrix[y][x] - B->matrix[y][x];
  }

  return ret;
}

int s21_mult_number(matrix_t *A, double number, matrix_t *result) {
  int ret = 0;

  if (matrix_is_empty(A)) {
    ret = 1;
  } else {
    s21_create_matrix(A->rows, A->columns, result);

    for (int y = 0; y < A->rows; y++)
      for (int x = 0; x < A->columns; x++)
        result->matrix[y][x] = A->matrix[y][x] * number;
  }

  return ret;
}

int s21_mult_matrix(matrix_t *A, matrix_t *B, matrix_t *result) {
  int ret = 0;

  if (matrix_is_empty(A) || matrix_is_empty(B)) {
    ret = 1;
  } else if (A->columns != B->rows) {
    ret = 2;
  } else {
    s21_create_matrix(B->columns, A->rows, result);
    for (int y = 0; y < result->rows; y++)
      for (int x = 0; x < result->columns; x++)
        for (int n = 0; n < A->columns; n++)
          result->matrix[y][x] += (A->matrix[y][n] * B->matrix[n][x]);
  }

  return ret;
}

int s21_transpose(matrix_t *A, matrix_t *result) {
  int ret = 0;

  if (matrix_is_empty(A)) {
    ret = 1;
  } else {
    s21_create_matrix(A->columns, A->rows, result);

    for (int y = 0; y < A->rows; y++)
      for (int x = 0; x < A->columns; x++)
        result->matrix[x][y] = A->matrix[y][x];
  }

  return ret;
}

int s21_calc_complements(matrix_t *A, matrix_t *result) {
  int ret = 0;

  if (matrix_is_empty(A)) {
    ret = 1;
  } else if (A->rows != A->columns) {
    ret = 2;
  } else {
    matrix_t M;
    double minor_det = 0;

    s21_create_matrix(A->rows, A->columns, result);

    for (int y = 0; y < A->rows; y++)
      for (int x = 0; x < A->columns; x++) {
        get_minor(A, &M, y, x);
        s21_determinant(&M, &minor_det);
        s21_remove_matrix(&M);
        result->matrix[y][x] = minor_det * pow(-1, y + x);
      }
  }

  return ret;
}

int s21_determinant(matrix_t *A, double *result) {
  int ret = 0;

  if (matrix_is_empty(A)) {
    ret = 1;
  } else if (A->rows != A->columns) {
    ret = 2;
  } else if (A->rows == 1) {
    *result = A->matrix[0][0];
  } else if (A->rows == 2) {
    *result = (A->matrix[0][0] * A->matrix[1][1]) -
              (A->matrix[1][0] * A->matrix[0][1]);
  } else {
    matrix_t M;
    double minor_det = 0;
    *result = 0;

    for (int x = 0; x < A->columns; x++) {
      // Идём по первой строке, четвёртый параметр - вычёркиваемый столбец.
      get_minor(A, &M, 0, x);
      s21_determinant(&M, &minor_det);
      s21_remove_matrix(&M);
      if (x % 2 == 0)
        *result += (A->matrix[0][x] * minor_det);
      else
        *result -= (A->matrix[0][x] * minor_det);
    }
  }

  return ret;
}

int s21_inverse_matrix(matrix_t *A, matrix_t *result) {
  int ret = 0;
  result->rows = 0;
  result->columns = 0;

  if (matrix_is_empty(A)) {
    ret = 1;
  } else {
    double A_det = 0;
    if (s21_determinant(A, &A_det) != 0 || A_det == 0) {
      ret = 2;
    } else {
      matrix_t R, T;
      s21_calc_complements(A, &R);
      s21_transpose(&R, &T);
      s21_mult_number(&T, 1.0 / A_det, result);
      s21_remove_matrix(&R);
      s21_remove_matrix(&T);
    }
  }

  return ret;
}

void get_minor(matrix_t *A, matrix_t *result, int oy, int ox) {
  s21_create_matrix(A->rows - 1, A->columns - 1, result);

  for (int y = 0; y < A->rows; y++)
    for (int x = 0; x < A->columns; x++) {
      if (y == oy || x == ox)
        continue;
      else if (y < oy && x < ox)
        result->matrix[y][x] = A->matrix[y][x];
      else if (y > oy && x < ox)
        result->matrix[y - 1][x] = A->matrix[y][x];
      else if (y < oy && x > ox)
        result->matrix[y][x - 1] = A->matrix[y][x];
      else if (y > oy && x > ox)
        result->matrix[y - 1][x - 1] = A->matrix[y][x];
    }
}

void print_matrix(matrix_t A) {
  printf("-----------\n");
  for (int y = 0; y < A.rows; y++) {
    printf("%d", y);
    for (int x = 0; x < A.columns; x++) {
      printf("%15.7lf ", A.matrix[y][x]);
    }
    printf("\n");
  }
  printf("-----------\n");
}

int matrix_is_empty(matrix_t *A) {
  int ret = 1;

  if (A && A->matrix && A->columns > 0 && A->rows > 0) {
    ret = 0;
  }

  return ret;
}
