#include "s21_3d_viewer.h"

// подсчитывает количество вершин в одной грани
unsigned count_vertexes_in_facet(char *str) {
  unsigned num = 0;
  for (size_t i = 0; str[i] && str[i] != '#'; i++) {
    if (((str[i] >= '0' && str[i] <= '9') || str[i] == '.') &&
        (str[i + 1] == ' ' || str[i + 1] == '#' || str[i + 1] == '\0' ||
         str[i + 1] == '\n'))
      num++;
  }

  return num;
}

// подсчитывает количество вершин в файле или группе
void count_vertexes_and_facets(const char *filename, unsigned *vertexes,
                               unsigned *facets, unsigned g) {
  *vertexes = 0;
  *facets = 0;
  FILE *file;

  if ((file = fopen(filename, "r")) == NULL) {
    printf("Не удалось открыть файл\n");
  } else {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    unsigned groups = 0;

    if (g != 0) {
      while ((read = getline(&line, &len, file)) != -1) {
        char str[read];
        sscanf(line, "%s", str);
        if (strcmp(str, "g") == 0) groups++;
        if (groups == g) break;
      }
    }

    while ((read = getline(&line, &len, file)) != -1) {
      char str[read];
      sscanf(line, "%s", str);
      if (strcmp(str, "v") == 0)
        (*vertexes)++;
      else if (strcmp(str, "f") == 0)
        (*facets)++;
      else if (g != 0 && strcmp(str, "g") == 0)
        break;
    }

    if (line) free(line);

    fclose(file);
  }
}

// подсчитывает количество групп в файле
unsigned count_groups(const char *filename) {
  FILE *file;
  unsigned g = 0;

  // если блок вершин один, то в файле, по сути, одна группа, сколько бы букв g
  // ни было. для отслеживания этого момента используются данные переменные:
  unsigned vertex_blocks = 0, flag = 0;

  if ((file = fopen(filename, "r")) == NULL) {
    printf("Не удалось открыть файл\n");
  } else {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, file)) != -1) {
      char str[read];
      sscanf(line, "%s", str);

      if (!flag && strcmp(str, "v") == 0) {
        vertex_blocks++;
        flag = 1;
      } else if (flag && strcmp(str, "f") == 0) {
        flag = 0;
      } else

          if (strcmp(str, "g") == 0) {
        g++;
      }
    }

    if (line) free(line);

    fclose(file);
  }

  if (vertex_blocks == 1) g = 0;

  return g;
}

// функция читает файл и заполняет полученными данными структуру data_t
int create_data(data_t *d, unsigned v, unsigned f, const char *filename,
                unsigned g) {
  d->amount_of_vertexes = v;
  d->amount_of_facets = f;
  d->indexes = 0;

  int return_code = 0;

  matrix_t *m = &(d->matrix_3d);
  s21_create_matrix(v + 1, 3, m);

  d->polygons = malloc(sizeof(polygon_t) * (f + 1));

  FILE *file;
  if ((file = fopen(filename, "r")) == NULL) {
    printf("Не удалось открыть файл\n");
    return_code = 1;
  } else {
    unsigned vertexes = 1, facets = 1, groups = 0;
    double x, y, z;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    // если в объекте несколько групп, то мы ищем нужную.
    if (g != 0) {
      while ((read = getline(&line, &len, file)) != -1) {
        char str[read];
        sscanf(line, "%s", str);

        if (strcmp(str, "g") ==
            0)  // с буквы g в файлах начинается описание групп (groups)
          groups++;
        if (groups == g) break;
      }
    }

    while ((read = getline(&line, &len, file)) != -1) {
      char str[read];
      sscanf(line, "%s", str);

      if (strcmp(str, "v") ==
          0) {  // с буквы v в файлах начинается описание вершин (vertexes)
        sscanf(line, "%s %lf %lf %lf", str, &x, &y, &z);
        m->matrix[vertexes][0] = x;
        m->matrix[vertexes][1] = y;
        m->matrix[vertexes][2] = z;
        vertexes++;
      } else if (strcmp(str, "f") == 0) {  // с буквы f в файлах начинается
                                           // описание полигонов/граней (facets)
        unsigned v_in_f = count_vertexes_in_facet(line);

        d->indexes += v_in_f;
        d->polygons[facets].amount_of_vertexes_in_facet = v_in_f;
        d->polygons[facets].vertexes = malloc(sizeof(int) * v_in_f);

        char *p_line = line;
        int *p_vertexes = d->polygons[facets].vertexes;

        for (unsigned i = 0; i < v_in_f; i++) {
          while ((*p_line < '0' || *p_line > '9') && *p_line != '-') p_line++;

          int vert;
          sscanf(p_line, "%d", &vert);
          p_vertexes[i] = vert;

          while (*p_line != ' ' && *p_line != '\n') p_line++;
        }
        facets++;
      } else if (g != 0 && strcmp(str, "g") == 0) {
        break;
      }
    }

    if (line) free(line);
  }

  return return_code;
}

void free_data(data_t *d) {
  if (d->polygons) {
    for (unsigned j = 1; j < d->amount_of_facets + 1; j++) {
      if (d->polygons[j].vertexes) {
        free(d->polygons[j].vertexes);
      }
    }
    free(d->polygons);
  }

  if (d->matrix_3d.matrix) s21_remove_matrix(&d->matrix_3d);
}

// находит минимальные значения x и y в структуре (для оптимизации
// масштабирования)
void get_min_and_max_x_y(data_t data, double *a, double *b, double *c,
                         double *d) {
  double min_x = 1.79769313486231570E+308, min_y = 1.79769313486231570E+308;
  double max_x = -1.79769313486231570E+308, max_y = -1.79769313486231570E+308;

  for (int i = 1; i < data.matrix_3d.rows; i++) {
    double x = data.matrix_3d.matrix[i][0];
    double y = data.matrix_3d.matrix[i][1];
    if (x < min_x) min_x = x;
    if (y < min_y) min_y = y;
    if (x > max_x) max_x = x;
    if (y > max_y) max_y = y;
  }

  *a = min_x;
  *b = min_y;
  *c = max_x;
  *d = max_y;
}

// вращение
void rotation_by_ox(matrix_t *A, double angle) {
  angle = angle * M_PI / 180;  // переводим градусы в радианы
  for (int i = 1; i < A->rows; i++) {
    double temp_y = A->matrix[i][1];
    double temp_z = A->matrix[i][2];
    A->matrix[i][1] = cos(angle) * temp_y - sin(angle) * temp_z;
    A->matrix[i][2] = sin(angle) * temp_y + cos(angle) * temp_z;
  }
}

void rotation_by_oy(matrix_t *A, double angle) {
  angle = angle * M_PI / 180;
  for (int i = 1; i < A->rows; i++) {
    double temp_x = A->matrix[i][0];
    double temp_z = A->matrix[i][2];
    A->matrix[i][0] = cos(angle) * temp_x - sin(angle) * temp_z;
    A->matrix[i][2] = sin(angle) * temp_x + cos(angle) * temp_z;
  }
}

void rotation_by_oz(matrix_t *A, double angle) {
  angle = angle * M_PI / 180;
  for (int i = 1; i < A->rows; i++) {
    double temp_x = A->matrix[i][0];
    double temp_y = A->matrix[i][1];
    A->matrix[i][0] = cos(angle) * temp_x - sin(angle) * temp_y;
    A->matrix[i][1] = sin(angle) * temp_x + cos(angle) * temp_y;
  }
}

// перемещение
void move_object(matrix_t *A, double bias, char axis_char) {
  int axis = -1;
  if (axis_char == 'x')
    axis = 0;
  else if (axis_char == 'y')
    axis = 1;
  else if (axis_char == 'z')
    axis = 2;

  for (int i = 1; i < A->rows; i++) A->matrix[i][axis] += bias;
}

// увеличение модели
void zoom(matrix_t *A, double coef) {
  if (coef != 0)
    for (int i = 1; i < A->rows; i++) {
      A->matrix[i][0] = A->matrix[i][0] * coef;
      A->matrix[i][1] = A->matrix[i][1] * coef;
      A->matrix[i][2] = A->matrix[i][2] * coef;
    }
}

// создаёт массив вершин для передачи в OpenGL
void make_vert_arr(matrix_t A, double *vert_arr) {
  for (int i = 0; i < A.rows; i++)
    for (int j = 0; j < A.columns; j++) *(vert_arr++) = A.matrix[i][j];
}

// создаёт массив индексов для передачи в OpenGL.
// если, например, в нашем полигоне индексы 0, 10, 350, 4,
// то в массив нам нужно записать 0, 10, 10, 350, 350, 4, 4, 0, чтобы нарисовать
// четыре линии. именно таким образом в массив индексов записываются все
// полигоны.
void make_ind_arr(data_t d, int *ind_arr) {
  for (unsigned int i = 1; i < d.amount_of_facets + 1; i++) {
    int first = 0;
    for (unsigned int j = 0; j < d.polygons[i].amount_of_vertexes_in_facet;
         j++) {
      int current_index = d.polygons[i].vertexes[j];

      // существует отрицательная индексация индексов.
      // если элемент (индекс) меньше нуля,
      // то нужно прибавить к нему количество вершин в группе/файле + 1
      if (current_index < 0) current_index += d.amount_of_vertexes + 1;

      if (j == 0) {
        first = current_index;
        *(ind_arr++) = first;
      } else {
        *(ind_arr++) = current_index;
        *(ind_arr++) = current_index;
      }
    }
    *(ind_arr++) = first;
  }
}
