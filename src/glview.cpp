#include "glview.h"

// эта функция срабатывает в начале программы
glView::glView(QWidget *parent)
    : QOpenGLWidget(parent)

{
  colorBackground = QColor(Qt::black);
  colorLine = QColor(Qt::white);
  colorPoint = QColor(Qt::green);
}

// начальные настройки OpenGL
void glView::initializeGL() {
  glEnable(GL_DEPTH_TEST);  // разрешили использование буфера глубины
}

// настройки OpenGL при изменении размеров окна
void glView::resizeGL(int w, int h) {
  glViewport(0, 0, w, h);  // область видимости окна, в котором мы рисуем
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
}

// рисование в OpenGL
void glView::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // очищаем вот эти буфера
  glClearColor(colorBackground.redF(), colorBackground.greenF(),
               colorBackground.blueF(), 0);  // цвет фона

  glMatrixMode(GL_MODELVIEW);  // показываем, какую матрицу мы будем
                               // использовать (видовую в данном случае)
  glLoadIdentity();

  // для вращения мышкой:
  glTranslatef(0, 0, 0);
  glRotatef(xRot, 1, 0, 0);
  glRotatef(yRot, 0, 1, 0);

  drawObject();
  update();
}

// рисует объект
void glView::drawObject() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glScaled(scale, scale, scale);

  if (projection == 0) {
    // ортогональная проекция:
    // от и до по х, от и до по у,
    // расстояние от камеры до передней плоскости,
    // расстояние от камеры до задней плоскости.
    glOrtho(min_x, max_x, min_y, max_y, -1000, 1000);
  } else if (projection == 1) {
    // перспективная проекция:
    // значение параметров аналогичное.
    glFrustum(min_x, max_x, min_y, max_y, -1000, 1000);
  }

  if (point_type != NO_POINTS) drawPoints();
  drawLines();
}

// рисует точки
void glView::drawPoints() {
  if (point_type == CIRCLE_POINTS)
    glEnable(GL_POINT_SMOOTH);
  else if (point_type == SQUARE_POINTS)
    glDisable(GL_POINT_SMOOTH);

  glPointSize(point_size);
  glColor3d(colorPoint.redF(), colorPoint.greenF(), colorPoint.blueF());

  for (unsigned i = 0; i < g; i++) {
    glVertexPointer(3, GL_DOUBLE, 0, vert_array[i]);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawElements(GL_POINTS, data_arr[i].indexes * 2, GL_UNSIGNED_INT,
                   indexes_arr[i]);
    glDisableClientState(GL_VERTEX_ARRAY);
  }
}

// рисует линии
void glView::drawLines() {
  if (line_type == DASHED_LINE) {
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(7, 0x3333);
  }
  glColor3d(colorLine.redF(), colorLine.greenF(), colorLine.blueF());
  glLineWidth(line_size);

  for (unsigned i = 0; i < g; i++) {
    glVertexPointer(3, GL_DOUBLE, 0, vert_array[i]);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawElements(GL_LINES, data_arr[i].indexes * 2, GL_UNSIGNED_INT,
                   indexes_arr[i]);
    glDisableClientState(GL_VERTEX_ARRAY);
  }

  glDisable(GL_LINE_STIPPLE);
}

// инициализирует объект: создаёт массивы индексов и вершин
void glView::initializeObject() {
  vert_array = (double **)malloc(sizeof(double *) * g);
  indexes_arr = (int **)malloc(sizeof(int *) * g);

  for (unsigned i = 0; i < g; i++) {
    vert_arr_len = data_arr[i].matrix_3d.rows * data_arr[i].matrix_3d.columns;
    vert_array[i] = (double *)malloc(sizeof(double) * vert_arr_len);
    make_vert_arr(data_arr[i].matrix_3d, vert_array[i]);
    indexes_arr[i] = (int *)malloc(sizeof(int) * data_arr[i].indexes * 2);
    make_ind_arr(data_arr[i], indexes_arr[i]);
  }

  update();
}

int glView::open_file(char *file_name) {
  int error = 0;

  // если в файле нет букв g, то нужна особая обработка
  g = count_groups(file_name);
  if (g == 0) {
    no_groups = 1;
    g = 1;
  }

  data_arr = (data_t *)malloc(sizeof(data_t) * g);

  if (no_groups) {
    count_vertexes_and_facets(file_name, &v, &f, 0);
    error = create_data(&data_arr[0], v, f, file_name, 0);
  } else {
    for (unsigned i = 1; i <= g; i++) {
      count_vertexes_and_facets(file_name, &v, &f, i);
      error = create_data(&data_arr[i - 1], v, f, file_name, i);
    }
  }

  return error;
}

// две функции для возможности крутить ось координат мышкой
void glView::mousePressEvent(QMouseEvent *mo) { mPos = mo->pos(); }

void glView::mouseMoveEvent(QMouseEvent *mo) {
  xRot += 1 / (M_PI) * (mo->pos().y() - mPos.y());
  yRot += 1 / (M_PI) * (mo->pos().x() - mPos.x());
  mPos = mo->pos();
  update();
}

// функция для масштабирования колёсиком
void glView::wheelEvent(QWheelEvent *event) {
  if (event->angleDelta().y() > 0) {
    scale += 0.05;
  } else {
    scale -= 0.05;
  }
  if (scale < 0) scale = 0.01;
  update();
}
