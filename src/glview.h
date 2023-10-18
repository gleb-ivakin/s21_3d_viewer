#ifndef GLVIEW_H
#define GLVIEW_H

#define GL_SILENCE_DEPRECATION

#include <QColorDialog>
#include <QWidget>
#include <QtOpenGLWidgets>

extern "C" {
#include "s21_3d_viewer.h"
#include "s21_matrix.h"
}

QT_BEGIN_NAMESPACE
namespace Ui {
class glView;
}
QT_END_NAMESPACE

class glView : public QOpenGLWidget {
  Q_OBJECT

 public:
  glView(QWidget *parent = nullptr);

  enum { NO_POINTS, CIRCLE_POINTS, SQUARE_POINTS, SOLID_LINE, DASHED_LINE };

  int point_type = NO_POINTS;  // споcоб отборажения точек
  int line_type = SOLID_LINE;  // споcоб отборажения точек
  int point_size = 1;
  int line_size = 1;

  QColor colorBackground;
  QColor colorLine;
  QColor colorPoint;

  rotation rotate;
  moving move;

  int projection = 0;

  data_t *data_arr = {nullptr};
  double **vert_array = {nullptr};  // массив вершин для OpenGL
  int **indexes_arr = {nullptr};  // массив индексов для OpenGL
  unsigned v = 0, f = 0, g = 0;  // количество вершин, граней, групп
  unsigned no_groups = 0;  // переменная на случай отсутствия групп в файле
  int vert_arr_len = 0;    // длина массива вершин

  double min_x = -1, min_y = -1, max_x = 1, max_y = 1;

  double scale = 0.5;       // масштабирование
  double scale_step = 0.1;  // шаг масштабирования

  void initializeObject();
  int open_file(char *file_name);

 private:
  Ui::glView *ui;
  QPoint mPos;
  QTimer tmr;
  float xRot, yRot, zRot;  // переменные для вращения осей координат мышкой

  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

  void drawObject();
  void drawPoints();
  void drawLines();

  void mousePressEvent(QMouseEvent *) override;
  void mouseMoveEvent(QMouseEvent *) override;
  void wheelEvent(QWheelEvent *event) override;
};
#endif  // GLVIEW_H
