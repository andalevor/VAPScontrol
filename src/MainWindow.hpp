#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "VAPSrecord.hpp"
#include <QGraphicsScene>
#include <QMainWindow>
#include <QtSql/QSqlDatabase>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

public slots:
    void open_file();
    void draw_points();
    void change_statusbar(QPointF p);
    void run_settings_dialog();

private:
    Ui::MainWindow* ui;
    QGraphicsScene scene;
    QSqlDatabase db;
    double x_min, y_min, ellipse_size;
    QString settings_file_name;
    int avg_phase_min, avg_phase_max;
    int peak_phase_min, peak_phase_max;
    int avg_distortion_min, avg_distortion_max;
    int peak_distortion_min, peak_distortion_max;
    int avg_force_min, avg_force_max;
    int peak_force_min, peak_force_max;
    QPen good_pen;
    QBrush good_brush;
    QPen bad_pen;
    QBrush bad_brush;

    void load_settings();
    void process_points();
};
#endif // MAINWINDOW_HPP
