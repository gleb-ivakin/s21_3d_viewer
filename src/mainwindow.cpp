#include "mainwindow.h"

#include "glview.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  setWindowTitle("3DViewer_v1.0 by 3.50 crew");
  settings_file = QApplication::applicationDirPath() + "/set.ini";
}

MainWindow::~MainWindow() {
  simpleDataFree();
  free_ind_and_vert_arrays();
  delete ui;
  delete timer;
}

// выбор файла
void MainWindow::on_button_choose_file_clicked() {
  QString homePath =
      QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
  QString filename = QFileDialog::getOpenFileName(this, "Выберите файл",
                                                  homePath, tr(" (*.obj)"));
  if (filename != "") {
    // освобождаем память от ранее открытого объекта
    simpleDataFree();
    free_ind_and_vert_arrays();

    QByteArray temp = filename.toLocal8Bit();
    char *str_filename = temp.data();
    int error = ui->widget->open_file(str_filename);

    if (error) {
      ui->statusBar->showMessage("Не удалось открыть файл");
    } else {
      // читаем новый объект
      ui->widget->initializeObject();
      QString fileinfo =
          filename.right(filename.size() - filename.lastIndexOf("/") - 1);
      fileinfo += ", vertices: ";
      fileinfo += QString::number(ui->widget->v);
      fileinfo += ", edges: ";
      fileinfo += QString::number(ui->widget->v + ui->widget->f -
                                  2);  // V−E+F=2 => E=V+F-2
      ui->statusBar->showMessage(fileinfo);
    }
  }
}

// функция очищает память, выделенную под объект
void MainWindow::simpleDataFree() {
  if (ui->widget->data_arr) {
    unsigned g = ui->widget->g;

    for (unsigned i = 0; i < g; i++) {
      free_data(&ui->widget->data_arr[i]);
    }

    free(ui->widget->data_arr);
  }
}

// освобождаем память, выделенную под массивы индексов и вершин, которые
// применяются для отрисовки в OpenGL
void MainWindow::free_ind_and_vert_arrays() {
  if (ui->widget->indexes_arr) {
    for (unsigned i = 0; i < ui->widget->g; i++) {
      if (ui->widget->indexes_arr[i]) free(ui->widget->indexes_arr[i]);
    }
  }
  free(ui->widget->indexes_arr);

  if (ui->widget->vert_array) {
    for (unsigned i = 0; i < ui->widget->g; i++)
      if (ui->widget->vert_array[i]) free(ui->widget->vert_array[i]);
  }
  free(ui->widget->vert_array);
}

// перемещение по осям
void MainWindow::moveObject(char axis) {
  ui->widget->move.ox -= (double)ui->spinbox_move_x->value();
  ui->widget->move.oy -= (double)ui->spinbox_move_y->value();
  ui->widget->move.oz -= (double)ui->spinbox_move_z->value();

  if (ui->widget->move.ox != 0 || ui->widget->move.oy != 0 ||
      ui->widget->move.oz != 0) {
    for (unsigned i = 0; i < ui->widget->g; i++) {
      if (axis == 'x')
        move_object(&ui->widget->data_arr[i].matrix_3d, ui->widget->move.ox,
                    'x');
      else if (axis == 'y')
        move_object(&ui->widget->data_arr[i].matrix_3d, ui->widget->move.oy,
                    'y');
      else if (axis == 'z')
        move_object(&ui->widget->data_arr[i].matrix_3d, ui->widget->move.oz,
                    'z');

      make_vert_arr(ui->widget->data_arr[i].matrix_3d,
                    ui->widget->vert_array[i]);
    }
  }

  ui->widget->move.ox = (double)ui->spinbox_move_x->value();
  ui->widget->move.oy = (double)ui->spinbox_move_y->value();
  ui->widget->move.oz = (double)ui->spinbox_move_z->value();

  ui->widget->update();

  //    ui->move_x_spinBox->setValue(ui->move_x_spinBox->value() + move_value);
}

void MainWindow::on_slider_move_x_valueChanged(int value) {
  moveObject('x');
  ui->spinbox_move_x->setValue(value);
}

void MainWindow::on_slider_move_y_valueChanged(int value) {
  moveObject('y');
  ui->spinbox_move_y->setValue(value);
}

void MainWindow::on_slider_move_z_valueChanged(int value) {
  moveObject('z');
  ui->spinbox_move_z->setValue(value);
}

void MainWindow::on_spinbox_move_x_valueChanged(int value) {
  moveObject('x');
  ui->slider_move_x->setValue(value);
}

void MainWindow::on_spinbox_move_y_valueChanged(int value) {
  moveObject('y');
  ui->slider_move_y->setValue(value);
}

void MainWindow::on_spinbox_move_z_valueChanged(int value) {
  moveObject('z');
  ui->slider_move_z->setValue(value);
}

// вращение
void MainWindow::rotateObject(char axis) {
  ui->widget->rotate.ox -= (double)ui->spinbox_rotate_x->value();
  ui->widget->rotate.oy -= (double)ui->spinbox_rotate_y->value();
  ui->widget->rotate.oz -= (double)ui->spinbox_rotate_z->value();

  if (ui->widget->rotate.ox != 0 || ui->widget->rotate.oy != 0 ||
      ui->widget->rotate.oz != 0) {
    for (unsigned i = 0; i < ui->widget->g; i++) {
      if (axis == 'x')
        rotation_by_ox(&ui->widget->data_arr[i].matrix_3d,
                       ui->widget->rotate.ox);
      else if (axis == 'y')
        rotation_by_oy(&ui->widget->data_arr[i].matrix_3d,
                       ui->widget->rotate.oy);
      else if (axis == 'z')
        rotation_by_oz(&ui->widget->data_arr[i].matrix_3d,
                       ui->widget->rotate.oz);

      make_vert_arr(ui->widget->data_arr[i].matrix_3d,
                    ui->widget->vert_array[i]);
    }
  }

  ui->widget->rotate.ox = (double)ui->spinbox_rotate_x->value();
  ui->widget->rotate.oy = (double)ui->spinbox_rotate_y->value();
  ui->widget->rotate.oz = (double)ui->spinbox_rotate_z->value();

  ui->widget->update();
}

void MainWindow::on_dial_rotate_x_valueChanged(int value) {
  rotateObject('x');
  ui->spinbox_rotate_x->setValue(value);
}

void MainWindow::on_dial_rotate_y_valueChanged(int value) {
  rotateObject('y');
  ui->spinbox_rotate_y->setValue(value);
}

void MainWindow::on_dial_rotate_z_valueChanged(int value) {
  rotateObject('z');
  ui->spinbox_rotate_z->setValue(value);
}

void MainWindow::on_spinbox_rotate_x_valueChanged(int value) {
  rotateObject('x');
  ui->dial_rotate_x->setValue(value);
}

void MainWindow::on_spinbox_rotate_y_valueChanged(int value) {
  rotateObject('y');
  ui->dial_rotate_y->setValue(value);
}

void MainWindow::on_spinbox_rotate_z_valueChanged(int value) {
  rotateObject('z');
  ui->dial_rotate_z->setValue(value);
}

// масштабирование самой модели
void MainWindow::modelScale() {
  model_scale_value = ui->spinbox_model_scale->value() / model_scale_value;

  if (model_scale_value != 0) {
    for (unsigned i = 0; i < ui->widget->g; i++) {
      zoom(&ui->widget->data_arr[i].matrix_3d,
           model_scale_value * model_scale_value);
      make_vert_arr(ui->widget->data_arr[i].matrix_3d,
                    ui->widget->vert_array[i]);
    }
  }

  model_scale_value = ui->spinbox_model_scale->value();

  ui->widget->update();
}

void MainWindow::on_spinbox_model_scale_valueChanged(double value) {
  modelScale();
  ui->slider_model_scale->setValue(value * 10);
}

void MainWindow::on_slider_model_scale_valueChanged(int value) {
  modelScale();
  ui->spinbox_model_scale->setValue((double)value / 10);
}

// смена проекции
void MainWindow::on_radioButton_ortho_clicked() {
  ui->widget->projection = 0;
  ui->widget->update();
}

void MainWindow::on_radioButton_frustum_clicked() {
  ui->widget->projection = 1;
  ui->widget->update();
}

// цвет фона
void MainWindow::on_button_bg_color_clicked() {
  ui->widget->colorBackground = QColorDialog::getColor();
}

// настройка отображения точек
void MainWindow::on_rb_no_points_clicked() {
  ui->widget->point_type = ui->widget->NO_POINTS;
}

void MainWindow::on_rb_circle_points_clicked() {
  ui->widget->point_type = ui->widget->CIRCLE_POINTS;
}

void MainWindow::on_rb_square_points_clicked() {
  ui->widget->point_type = ui->widget->SQUARE_POINTS;
}

void MainWindow::on_button_point_color_clicked() {
  ui->widget->colorPoint = QColorDialog::getColor();
}

void MainWindow::on_spinbox_point_size_valueChanged(int value) {
  ui->widget->point_size = value;
  ui->slider_point_size->setValue(value);
  ui->widget->update();
}

void MainWindow::on_slider_point_size_valueChanged(int value) {
  ui->widget->point_size = value;
  ui->spinbox_point_size->setValue(value);
  ui->widget->update();
}

// настройка отображения линий
void MainWindow::on_rb_solid_lines_clicked() {
  ui->widget->line_type = ui->widget->SOLID_LINE;
}

void MainWindow::on_rb_dashed_lines_clicked() {
  ui->widget->line_type = ui->widget->DASHED_LINE;
}

void MainWindow::on_button_line_color_clicked() {
  ui->widget->colorLine = QColorDialog::getColor();
}

void MainWindow::on_spinbox_line_size_valueChanged(int value) {
  ui->widget->line_size = value;
  ui->slider_line_size->setValue(value);
  ui->widget->update();
}

void MainWindow::on_slider_line_size_valueChanged(int value) {
  ui->widget->line_size = value;
  ui->spinbox_line_size->setValue(value);
  ui->widget->update();
}

// сохранение настроек
void MainWindow::on_button_save_settings_clicked() {
  QSettings settings(settings_file, QSettings::NativeFormat);

  settings.setValue("radioButton_ortho", ui->radioButton_ortho->isChecked());
  settings.setValue("radioButton_frustum",
                    ui->radioButton_frustum->isChecked());

  settings.setValue("colorBackground", ui->widget->colorBackground);

  settings.setValue("point_size", ui->widget->point_size);
  settings.setValue("point_type", ui->widget->point_type);
  settings.setValue("colorPoint", ui->widget->colorPoint);
  settings.setValue("slider_point_size", ui->slider_point_size->value());
  settings.setValue("spinbox_point_size", ui->spinbox_point_size->value());
  settings.setValue("rb_no_points", ui->rb_no_points->isChecked());
  settings.setValue("rb_circle_points", ui->rb_circle_points->isChecked());
  settings.setValue("rb_square_points", ui->rb_square_points->isChecked());

  settings.setValue("line_size", ui->widget->line_size);
  settings.setValue("line_type", ui->widget->line_type);
  settings.setValue("colorLine", ui->widget->colorLine);
  settings.setValue("slider_line_size", ui->slider_line_size->value());
  settings.setValue("spinbox_line_size", ui->spinbox_line_size->value());
  settings.setValue("rb_solid_lines", ui->rb_solid_lines->isChecked());
  settings.setValue("rb_dashed_lines", ui->rb_dashed_lines->isChecked());
}

// загрузка настроек
void MainWindow::on_button_load_settings_clicked() {
  QSettings settings(settings_file, QSettings::NativeFormat);

  ui->radioButton_ortho->setChecked(
      settings.value("radioButton_ortho").toBool());
  ui->radioButton_frustum->setChecked(
      settings.value("radioButton_frustum").toBool());

  ui->widget->colorBackground =
      settings.value("colorBackground").value<QColor>();

  ui->widget->point_size = settings.value("point_size").toInt();
  ui->widget->point_type = settings.value("point_type").toInt();
  ui->widget->colorPoint = settings.value("point_type").value<QColor>();
  ui->slider_point_size->setValue(settings.value("slider_point_size").toInt());
  ui->spinbox_point_size->setValue(
      settings.value("spinbox_point_size").toInt());
  ui->rb_no_points->setChecked(settings.value("rb_no_points").toBool());
  ui->rb_circle_points->setChecked(settings.value("rb_circle_points").toBool());
  ui->rb_square_points->setChecked(settings.value("rb_square_points").toBool());

  ui->widget->line_size = settings.value("line_size").toInt();
  ui->widget->line_type = settings.value("line_type").toInt();
  ui->widget->colorLine = settings.value("colorLine").value<QColor>();
  ui->slider_line_size->setValue(settings.value("slider_line_size").toInt());
  ui->spinbox_line_size->setValue(settings.value("spinbox_line_size").toInt());
  ui->rb_solid_lines->setChecked(settings.value("rb_solid_lines").toBool());
  ui->rb_dashed_lines->setChecked(settings.value("rb_dashed_lines").toBool());
}

// скриншот
void MainWindow::on_button_screenshot_clicked() {
  QString desktopPath =
      QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

  QFileDialog dialog(this);
  QString screenshot_filename =
      dialog.getSaveFileName(this, "Save screenshot as...", desktopPath,
                             "BMP (*.bmp);; JPEG (*.jpeg)");

  ui->widget->setStyleSheet(
      "background-color: " + ui->widget->colorBackground.name() + ";");
  QPixmap screenshot = ui->widget->grab();

  screenshot.save(screenshot_filename);
}

// гифка
void MainWindow::record_gif() {
  if (giftime < 5.0) {
    ui->widget->setStyleSheet(
        "background-color: " + ui->widget->colorBackground.name() + ";");
    img_vector.push_back(ui->widget->grab().toImage());
    giftime += 0.1;
    if (giftime <= 5.0) ui->label_gif_timer->setText(QString::number(giftime));
  } else if (giftime >= 5.0) {
    save_gif();
    timer->stop();
  }
}

void MainWindow::save_gif() {
  QString desktopPath =
      QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

  QFileDialog dialog(this);
  QString gif_filename = dialog.getSaveFileName(this, "Save gif as...",
                                                desktopPath, "GIF (*.gif);;");

  gif.setDefaultDelay(100);

  for (QVector<QImage>::Iterator img = img_vector.begin();
       img != img_vector.end(); ++img) {
    QImage imgScaled = img->scaled(640, 480);
    gif.addFrame(imgScaled);
  }

  gif.save(gif_filename);
  img_vector.clear();
  ui->label_gif_timer->clear();
  giftime = 0.0;
}

void MainWindow::on_button_gif_clicked() {
  timer = new QTimer(0);
  timer->start(100);
  connect(timer, SIGNAL(timeout()), this, SLOT(record_gif()));
}
