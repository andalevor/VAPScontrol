#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "VAPSrecord.hpp"
#include <QGraphicsScene>
#include <QMainWindow>
#include <QtSql/QSqlDatabase>
#include <QTranslator>

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
    void open_aps_file();
    void open_sps_file();
    void change_statusbar(QPointF p);
    void run_settings_dialog();
    void run_color_dialog();
    void peek_point(QPointF p);
    void show_about_author();
    void show_about_vapscontrol();

private:
    Ui::MainWindow* ui;
    QGraphicsScene scene;
    QSqlDatabase aps_db;
    QVector<QPair<double, double>> sps_points;
    double x_min, y_min;
    int vib_ellipse_size, search_radius, sps_ellipse_size, cross_size;
    QString settings_file_name;
    int avg_phase_min, avg_phase_max;
    int peak_phase_min, peak_phase_max;
    int avg_distortion_min, avg_distortion_max;
    int peak_distortion_min, peak_distortion_max;
    int avg_force_min, avg_force_max;
    int peak_force_min, peak_force_max;
    QColor vib_good_color, vib_bad_color, background_color, sps_color;
    QBrush vib_good_brush, vib_bad_brush;
    QPen vib_good_pen, vib_bad_pen;
    QString language;
    QGraphicsItem* selected;
    QTranslator translator;

    void load_settings();
    void process_points();
    void redraw();
    void draw_aps_points();
    void draw_sps_points();
};
#endif // MAINWINDOW_HPP
