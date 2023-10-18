#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileDialog>
#include <QGuiApplication>
#include <QMainWindow>
#include <QPixmap>
#include <QScreen>
#include <QTimer>

#include "qtgifimage/src/gifimage/qgifimage.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 private slots:
  void on_button_choose_file_clicked();

  void on_dial_rotate_x_valueChanged(int value);
  void on_dial_rotate_y_valueChanged(int value);
  void on_dial_rotate_z_valueChanged(int value);

  void on_spinbox_rotate_x_valueChanged(int value);
  void on_spinbox_rotate_y_valueChanged(int value);
  void on_spinbox_rotate_z_valueChanged(int value);

  void on_slider_move_x_valueChanged(int value);
  void on_slider_move_y_valueChanged(int value);
  void on_slider_move_z_valueChanged(int value);

  void on_spinbox_move_x_valueChanged(int value);
  void on_spinbox_move_y_valueChanged(int value);
  void on_spinbox_move_z_valueChanged(int value);

  void on_spinbox_model_scale_valueChanged(double value);
  void on_slider_model_scale_valueChanged(int value);

  void on_radioButton_ortho_clicked();
  void on_radioButton_frustum_clicked();

  void on_rb_no_points_clicked();
  void on_rb_circle_points_clicked();
  void on_rb_square_points_clicked();

  void on_rb_dashed_lines_clicked();
  void on_rb_solid_lines_clicked();

  void on_spinbox_point_size_valueChanged(int value);
  void on_slider_point_size_valueChanged(int value);

  void on_spinbox_line_size_valueChanged(int value);
  void on_slider_line_size_valueChanged(int value);

  void on_button_point_color_clicked();
  void on_button_line_color_clicked();
  void on_button_bg_color_clicked();

  void on_button_save_settings_clicked();
  void on_button_load_settings_clicked();

  void on_button_screenshot_clicked();
  void on_button_gif_clicked();
  void record_gif();

 private:
  Ui::MainWindow *ui;
  QString settings_file;
  QTimer *timer;
  QGifImage gif;
  QPixmap screenshot;
  QVector<QImage> img_vector;
  int move_value = 1;
  int rotation_value = 10;  // в градусах
  double model_scale_value = 1.0;
  double giftime = 0;

  void free_ind_and_vert_arrays();
  void freeData();
  void moveObject(char axis);
  void rotateObject(char axis);
  void modelScale();
  void save_gif();

  void simpleDataFree();
};
#endif  // MAINWINDOW_H
