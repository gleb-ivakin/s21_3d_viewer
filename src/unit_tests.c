#include <check.h>

#include "s21_3d_viewer.h"

START_TEST(count_vertexes_in_facet_test) {
  unsigned result;
  result = count_vertexes_in_facet("1 2 5 4");
  ck_assert_uint_eq(result, 4);

  result = count_vertexes_in_facet("1 2 5 4 # some comment ");
  ck_assert_uint_eq(result, 4);

  result = count_vertexes_in_facet("3.4 5.6 3.2 8.7");
  ck_assert_uint_eq(result, 4);
}
END_TEST

START_TEST(count_vertexes_and_facets_test) {
  unsigned v, f;
  count_vertexes_and_facets("objects/cube.obj", &v, &f, 0);
  ck_assert_uint_eq(v, 8);
  ck_assert_uint_eq(f, 12);

  count_vertexes_and_facets("objects/special/group_cube.obj", &v, &f, 1);
  ck_assert_uint_eq(v, 8);
  ck_assert_uint_eq(f, 6);
}
END_TEST

START_TEST(count_groups_test) {
  unsigned g;
  g = count_groups("objects/cube.obj");
  ck_assert_uint_eq(g, 0);

  g = count_groups("objects/special/group_cube.obj");
  ck_assert_uint_eq(g, 2);
}
END_TEST

START_TEST(create_data_test) {
  data_t res_data;
  unsigned v, f, g;
  g = count_groups("objects/cube.obj");
  count_vertexes_and_facets("objects/cube.obj", &v, &f, g);
  create_data(&res_data, v, f, "objects/cube.obj", g);
  ck_assert_uint_eq(res_data.amount_of_facets, f);
  ck_assert_uint_eq(res_data.amount_of_vertexes, v);
  ck_assert_uint_eq(res_data.indexes, 36);
  ck_assert_uint_eq(res_data.matrix_3d.columns, 3);
  ck_assert_uint_eq(res_data.matrix_3d.rows, 9);  // первый ряд - пустой
  ck_assert_double_eq(res_data.matrix_3d.matrix[1][0], -0.5);
  ck_assert_int_eq(res_data.polygons[1].vertexes[1], 7);
  free_data(&res_data);

  g = count_groups("objects/special/group_cube.obj");
  count_vertexes_and_facets("objects/special/group_cube.obj", &v, &f, g);
  create_data(&res_data, v, f, "objects/special/group_cube.obj", g);
  ck_assert_uint_eq(res_data.amount_of_facets, f);
  ck_assert_uint_eq(res_data.amount_of_vertexes, v);
  ck_assert_uint_eq(res_data.indexes, 18);
  ck_assert_uint_eq(res_data.matrix_3d.columns, 3);
  ck_assert_uint_eq(res_data.matrix_3d.rows, 9);  // первый ряд - пустой
  ck_assert_double_eq(res_data.matrix_3d.matrix[1][0], -0.5);
  ck_assert_int_eq(res_data.polygons[1].vertexes[1], 7);
  free_data(&res_data);
}
END_TEST

START_TEST(get_min_and_max_x_y_test) {
  data_t res_data;
  unsigned v, f, g;
  double a, b, c, d;
  g = count_groups("objects/cube.obj");
  count_vertexes_and_facets("objects/cube.obj", &v, &f, g);
  create_data(&res_data, v, f, "objects/cube.obj", g);
  get_min_and_max_x_y(res_data, &a, &b, &c, &d);
  ck_assert_double_eq(a, -0.5);
  ck_assert_double_eq(b, -0.5);
  ck_assert_double_eq(c, 0.5);
  ck_assert_double_eq(d, 0.5);
  free_data(&res_data);
}
END_TEST

START_TEST(rotation_by_ox_test) {
  data_t original_data, res_data;
  matrix_t *A, *B;
  unsigned v, f, g;
  g = count_groups("objects/cube.obj");
  double angle = 20, radian_angle, temp_y, temp_z, res_1, res_2;
  count_vertexes_and_facets("objects/cube.obj", &v, &f, g);
  create_data(&original_data, v, f, "objects/cube.obj", g);
  create_data(&res_data, v, f, "objects/cube.obj", g);

  A = &original_data.matrix_3d;
  B = &res_data.matrix_3d;
  radian_angle = angle * M_PI / 180;

  rotation_by_ox(&res_data.matrix_3d, angle);
  for (int i = 1; i < A->rows; i++) {
    temp_y = A->matrix[i][1];
    temp_z = A->matrix[i][2];
    res_1 = cos(radian_angle) * temp_y - sin(radian_angle) * temp_z;
    ck_assert_double_eq(res_1, B->matrix[i][1]);
    res_2 = sin(radian_angle) * temp_y + cos(radian_angle) * temp_z;
    ck_assert_double_eq(res_2, B->matrix[i][2]);
  }

  free_data(&res_data);
}
END_TEST

START_TEST(rotation_by_oy_test) {
  data_t original_data, res_data;
  matrix_t *A, *B;
  unsigned v, f, g;
  g = count_groups("objects/cube.obj");
  double angle = 20, radian_angle, temp_x, temp_z, res_1, res_2;
  count_vertexes_and_facets("objects/cube.obj", &v, &f, g);
  create_data(&original_data, v, f, "objects/cube.obj", g);
  create_data(&res_data, v, f, "objects/cube.obj", g);

  A = &original_data.matrix_3d;
  B = &res_data.matrix_3d;
  radian_angle = angle * M_PI / 180;

  rotation_by_oy(&res_data.matrix_3d, angle);
  for (int i = 1; i < A->rows; i++) {
    temp_x = A->matrix[i][0];
    temp_z = A->matrix[i][2];
    res_1 = cos(radian_angle) * temp_x - sin(radian_angle) * temp_z;
    ck_assert_double_eq(res_1, B->matrix[i][0]);
    res_2 = sin(radian_angle) * temp_x + cos(radian_angle) * temp_z;
    ck_assert_double_eq(res_2, B->matrix[i][2]);
  }

  free_data(&res_data);
}
END_TEST

START_TEST(rotation_by_oz_test) {
  data_t original_data, res_data;
  matrix_t *A, *B;
  unsigned v, f, g;
  g = count_groups("objects/cube.obj");
  double angle = 20, radian_angle, temp_x, temp_y, res_1, res_2;
  count_vertexes_and_facets("objects/cube.obj", &v, &f, g);
  create_data(&original_data, v, f, "objects/cube.obj", g);
  create_data(&res_data, v, f, "objects/cube.obj", g);

  A = &original_data.matrix_3d;
  B = &res_data.matrix_3d;
  radian_angle = angle * M_PI / 180;

  rotation_by_oz(&res_data.matrix_3d, angle);
  for (int i = 1; i < A->rows; i++) {
    temp_x = A->matrix[i][0];
    temp_y = A->matrix[i][1];
    res_1 = cos(radian_angle) * temp_x - sin(radian_angle) * temp_y;
    ck_assert_double_eq(res_1, B->matrix[i][0]);
    res_2 = sin(radian_angle) * temp_x + cos(radian_angle) * temp_y;
    ck_assert_double_eq(res_2, B->matrix[i][1]);
  }

  free_data(&res_data);
}
END_TEST

START_TEST(move_object_test) {
  data_t original_data, res_data;
  matrix_t *A, *B;
  unsigned v, f, g;
  g = count_groups("objects/cube.obj");
  double bias = 3.50;
  count_vertexes_and_facets("objects/cube.obj", &v, &f, g);
  create_data(&original_data, v, f, "objects/cube.obj", g);
  create_data(&res_data, v, f, "objects/cube.obj", g);

  A = &original_data.matrix_3d;
  B = &res_data.matrix_3d;

  move_object(&res_data.matrix_3d, bias, 'x');
  for (int i = 1; i < A->rows; i++)
    ck_assert_double_eq(A->matrix[i][0] + bias, B->matrix[i][0]);
  move_object(A, bias, 'x');

  move_object(&res_data.matrix_3d, bias, 'y');
  for (int i = 1; i < A->rows; i++)
    ck_assert_double_eq(A->matrix[i][1] + bias, B->matrix[i][1]);
  move_object(A, bias, 'y');

  move_object(&res_data.matrix_3d, bias, 'z');
  for (int i = 1; i < A->rows; i++)
    ck_assert_double_eq(A->matrix[i][2] + bias, B->matrix[i][2]);

  free_data(&res_data);
}
END_TEST

START_TEST(zoom_test) {
  data_t original_data, res_data;
  matrix_t *A, *B;
  unsigned v, f, g;
  g = count_groups("objects/cube.obj");
  double coef = 3.50;
  count_vertexes_and_facets("objects/cube.obj", &v, &f, g);
  create_data(&original_data, v, f, "objects/cube.obj", g);
  create_data(&res_data, v, f, "objects/cube.obj", g);

  A = &original_data.matrix_3d;
  B = &res_data.matrix_3d;

  zoom(B, coef);
  for (int i = 1; i < A->rows; i++) {
    ck_assert_double_eq(A->matrix[i][0] * coef, B->matrix[i][0]);
    ck_assert_double_eq(A->matrix[i][1] * coef, B->matrix[i][1]);
    ck_assert_double_eq(A->matrix[i][2] * coef, B->matrix[i][2]);
  }

  free_data(&res_data);
}
END_TEST

START_TEST(make_vert_arr_test) {
  data_t data;
  unsigned v, f, g;
  g = count_groups("objects/cube.obj");
  count_vertexes_and_facets("objects/cube.obj", &v, &f, g);
  create_data(&data, v, f, "objects/cube.obj", g);

  matrix_t A = data.matrix_3d;
  double *res_arr = (double *)malloc(A.rows * A.columns);
  make_vert_arr(A, res_arr);

  int k = 0;
  for (int i = 0; i < A.rows; i++)
    for (int j = 0; j < A.columns; j++)
      ck_assert_double_eq(A.matrix[i][j], res_arr[k++]);

  free_data(&data);
}
END_TEST

START_TEST(make_ind_arr_test) {
  data_t data;
  unsigned v, f, g;
  g = count_groups("objects/cube.obj");
  count_vertexes_and_facets("objects/cube.obj", &v, &f, g);
  create_data(&data, v, f, "objects/cube.obj", g);
  int *res_arr = (int *)malloc(sizeof(int) * data.indexes * 2);
  int test_arr[72] = {1, 7, 7, 5, 5, 1, 1, 3, 3, 7, 7, 1, 1, 4, 4, 3, 3, 1,
                      1, 2, 2, 4, 4, 1, 3, 8, 8, 7, 7, 3, 3, 4, 4, 8, 8, 3,
                      5, 7, 7, 8, 8, 5, 5, 8, 8, 6, 6, 5, 1, 5, 5, 6, 6, 1,
                      1, 6, 6, 2, 2, 1, 4, 6, 6, 8, 8, 4, 4, 2, 2, 6, 6, 4};

  make_ind_arr(data, res_arr);

  for (unsigned i = 0; i < data.indexes * 2; i++)
    ck_assert_int_eq(res_arr[i], test_arr[i]);

  free_data(&data);
  free(res_arr);
}
END_TEST

int main(void) {
  Suite *s1 = suite_create("3d_viewer");
  TCase *tc1 = tcase_create("3d_viewer");
  SRunner *sr = srunner_create(s1);
  int nf;

  suite_add_tcase(s1, tc1);
  tcase_add_test(tc1, count_vertexes_in_facet_test);
  tcase_add_test(tc1, count_vertexes_and_facets_test);
  tcase_add_test(tc1, count_groups_test);
  tcase_add_test(tc1, create_data_test);
  tcase_add_test(tc1, get_min_and_max_x_y_test);
  tcase_add_test(tc1, rotation_by_ox_test);
  tcase_add_test(tc1, rotation_by_oy_test);
  tcase_add_test(tc1, rotation_by_oz_test);
  tcase_add_test(tc1, move_object_test);
  tcase_add_test(tc1, zoom_test);
  tcase_add_test(tc1, make_vert_arr_test);
  tcase_add_test(tc1, make_ind_arr_test);

  srunner_run_all(sr, CK_ENV);
  nf = srunner_ntests_failed(sr);
  srunner_free(sr);

  return nf == 0 ? 0 : 1;
}