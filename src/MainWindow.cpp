#include "MainWindow.hpp"
#include "ColorDialog.hpp"
#include "InfoDialog.hpp"
#include "SettingsDialog.hpp"
#include "VAPSrecord.hpp"
#include "ui_MainWindow.h"
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QGraphicsItem>
#include <QMessageBox>
#include <QSettings>
#include <QSqlError>
#include <QSqlQuery>
#include <QTextStream>
#include <cmath>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , aps_db(QSqlDatabase::addDatabase("QSQLITE"))
    , vib_ellipse_size { 5 }
    , search_radius { 25 }
    , sps_ellipse_size { 25 }
    , cross_size { 5 }
    , selected { nullptr }
{
    aps_db.setDatabaseName(":memory:");
    settings_file_name = QApplication::applicationDirPath() + "/settings.ini";
    load_settings();
    ui->setupUi(this);
    ui->graphicsView->setScene(&scene);
    connect(ui->actionExit, SIGNAL(triggered()), QApplication::instance(), SLOT(quit()));
    connect(ui->actionOpen_APS, SIGNAL(triggered()), this, SLOT(open_aps_file()));
    connect(ui->actionOpen_SPS, SIGNAL(triggered()), this, SLOT(open_sps_file()));
    connect(ui->graphicsView, SIGNAL(mousePosChanged(QPointF)), this, SLOT(change_statusbar(QPointF)));
    connect(ui->graphicsView, SIGNAL(mouseClicked(QPointF)), this, SLOT(peek_point(QPointF)));
    connect(ui->actionLimits, SIGNAL(triggered()), this, SLOT(run_settings_dialog()));
    connect(ui->actionColor_Size, SIGNAL(triggered()), this, SLOT(run_color_dialog()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::open_aps_file()
{
    if (aps_db.isOpen()) {
        aps_db.close();
    }
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open VAPS file"));
    if (file_name.isEmpty())
        return;
    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(nullptr, "Error", "Can not read from " + file_name);
        return;
    }
    QTextStream in(&file);
    if (!aps_db.open()) {
        QMessageBox::critical(nullptr, "Error", "Cannot create in memory database");
        exit(1);
    }
    QSqlQuery query;
    query.exec("CREATE TABLE data ("
               "id INTEGER primary key, record_id TEXT, "
               "line_name INTEGER, point_number INTEGER, "
               "point_index INTEGER, fleet_number INTEGER, "
               "vib_number INTEGER, vib_drive_level INTEGER, "
               "avg_phase INTEGER, peak_phase INTEGER, "
               "avg_distortion INTEGER, peak_distortion INTEGER, "
               "avg_force INTEGER, peak_force INTEGER, "
               "avg_ground_stiffness INTEGER, avg_ground_viscosity INTEGER, "
               "vib_pos_easting REAL, vib_pos_northing REAL, "
               "vib_pos_elevation REAL, shot_nb INTEGER, "
               "acquisition_nb INTEGER, two_digits_fleet_number INTEGER, "
               "vib_status_code INTEGER, mass_1_warning TEXT, "
               "mass_2_warning TEXT, mass_3_warning TEXT, "
               "plate_1_warning TEXT, plate_2_warning TEXT, "
               "plate_3_warning TEXT, plate_4_warning TEXT, "
               "plate_5_warning TEXT, plate_6_warning TEXT, "
               "force_overload TEXT, pressure_overload TEXT, "
               "mass_overload TEXT, valve_overload TEXT, "
               "excitation_overload TEXT, stacking_fold TEXT, "
               "computation_domain TEXT, ve432_version TEXT, "
               "day_of_year INTEGER, hour INTEGER, minute INTEGER, "
               "second INTEGER, hdop REAL, tb_date TEXT, gpgga TEXT, "
               "good INTEGER DEFAULT 1, draw INTEGER DEFAULT 1)");
    unsigned long long counter = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        ++counter;
        if (line.startsWith("H26") || line == "/n")
            continue;
        VAPSrecord r;
        bool ok = true;
        query.prepare("INSERT INTO data ("
                      "id, record_id, line_name, point_number, "
                      "point_index, fleet_number, vib_number, "
                      "vib_drive_level, avg_phase, peak_phase, "
                      "avg_distortion, peak_distortion, avg_force, "
                      "peak_force, avg_ground_stiffness, avg_ground_viscosity, "
                      "vib_pos_easting, vib_pos_northing, vib_pos_elevation, "
                      "shot_nb, acquisition_nb, two_digits_fleet_number, "
                      "vib_status_code, mass_1_warning, mass_2_warning, "
                      "mass_3_warning, plate_1_warning, plate_2_warning, "
                      "plate_3_warning, plate_4_warning, plate_5_warning, "
                      "plate_6_warning, force_overload, pressure_overload, "
                      "mass_overload, valve_overload, excitation_overload, "
                      "stacking_fold, computation_domain, ve432_version, "
                      "day_of_year, hour, minute, second, hdop, tb_date, gpgga)"
                      "VALUES (NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?,"
                      "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?,"
                      "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        query.addBindValue(line.mid(0, 1)); // record_id
        query.addBindValue(static_cast<int>(line.mid(1, 16).replace(',', '.').toDouble(&ok))); // line_name
        if (!ok) {
            QMessageBox::critical(nullptr, "Error", "On line " + QString::number(counter) + ".<br>Can not read line name (2, 17):<br>" + line.mid(1, 16));
            exit(1);
        }
        query.addBindValue(static_cast<int>(line.mid(17, 8).replace(',', '.').toDouble(&ok))); // point_number
        if (!ok) {
            QMessageBox::critical(nullptr, "Error", "On line " + QString::number(counter) + ".<br>Can not read point number (18, 25):<br>" + line.mid(17, 8));
            exit(1);
        }
        query.addBindValue(line.mid(25, 1).toInt(&ok)); // point_index
        if (!ok) {
            QMessageBox::critical(nullptr, "Error", "On line " + QString::number(counter) + ".<br>Can not read point index (26, 26):<br>" + line.mid(25, 1));
            exit(1);
        }
        query.addBindValue(line.mid(26, 1).toInt(&ok)); // fleet_number
        if (!ok) {
            QMessageBox::critical(nullptr, "Error", "On line " + QString::number(counter) + ".<br>Can not read fleet number (27, 27):<br>" + line.mid(26, 1));
            exit(1);
        }
        query.addBindValue(line.mid(27, 2).toInt(&ok)); // vib_number
        if (!ok) {
            QMessageBox::critical(nullptr, "Error", "On line " + QString::number(counter) + ".<br>Can not read vibrator number (28, 29):<br>" + line.mid(27, 2));
            exit(1);
        }
        query.addBindValue(line.mid(29, 3).toInt(&ok)); // vib_drive_level
        if (!ok) {
            QMessageBox::critical(nullptr, "Error", "On line " + QString::number(counter) + ".<br>Can not read vibrator drive level (30, 32):<br>" + line.mid(29, 3));
            exit(1);
        }
        int avg_phase = line.mid(32, 4).toInt(&ok); // avg_phase
        query.addBindValue(avg_phase);
        if (!ok) {
            QMessageBox::critical(nullptr, "Error", "On line " + QString::number(counter) + ".<br>Can not read average phase (33, 36):<br>" + line.mid(32, 4));
            exit(1);
        }
        int peak_phase = line.mid(36, 4).toInt(&ok); // peak_phase
        query.addBindValue(peak_phase);
        if (!ok) {
            QMessageBox::critical(nullptr, "Error", "On line " + QString::number(counter) + ".<br>Can not read peak phase (37, 40):<br>" + line.mid(36, 4));
            exit(1);
        }
        int avg_distortion = line.mid(40, 2).toInt(&ok); // avg_distortion
        query.addBindValue(avg_distortion);
        if (!ok) {
            QMessageBox::critical(nullptr, "Error", "On line " + QString::number(counter) + ".<br>Can not read average distortion (41, 42):<br>" + line.mid(40, 2));
            exit(1);
        }
        int peak_distortion = line.mid(42, 2).toInt(&ok); // peak_distortion
        query.addBindValue(peak_distortion);
        if (!ok) {
            QMessageBox::critical(nullptr, "Error", "On line " + QString::number(counter) + ".<br>Can not read peak distortion (43, 44):<br>" + line.mid(42, 2));
            exit(1);
        }
        int avg_force = line.mid(44, 2).toInt(&ok); // avg_force
        query.addBindValue(avg_force);
        if (!ok) {
            QMessageBox::critical(nullptr, "Error", "On line " + QString::number(counter) + ".<br>Can not read average force (45, 46):<br>" + line.mid(44, 2));
            exit(1);
        }
        int peak_force = line.mid(46, 3).toInt(&ok); // peak_force
        query.addBindValue(peak_force);
        if (!ok) {
            QMessageBox::critical(nullptr, "Error", "On line " + QString::number(counter) + ".<br>Can not read peak force (47, 49):<br>" + line.mid(46, 3));
            exit(1);
        }
        query.addBindValue(line.mid(49, 3).toInt(&ok)); // avg_ground_stiffness
        if (!ok) {
            QMessageBox::critical(nullptr, "Error", "On line " + QString::number(counter) + ".<br>Can not read average ground stiffness (50, 52):<br>" + line.mid(49, 3));
            exit(1);
        }
        query.addBindValue(line.mid(52, 3).toInt(&ok)); // avg_ground_viscosity
        if (!ok) {
            QMessageBox::critical(nullptr, "Error", "On line " + QString::number(counter) + ".<br>Can not read average ground viscosity (53, 55):<br>" + line.mid(52, 3));
            exit(1);
        }
        query.addBindValue(line.mid(55, 9).replace(',', '.').toDouble(&ok)); // vib_pos_easting
        if (!ok) {
            QMessageBox::critical(nullptr, "Error", "On line " + QString::number(counter) + ".<br>Can not read vib. position easting (56, 64):<br>" + line.mid(55, 9));
            exit(1);
        }
        query.addBindValue(line.mid(64, 10).replace(',', '.').toDouble(&ok)); // vib_pos_northing
        if (!ok) {
            QMessageBox::critical(nullptr, "Error", "On line " + QString::number(counter) + ".<br>Can not read vib. position northing (65, 74):<br>" + line.mid(64, 10));
            exit(1);
        }
        query.addBindValue(line.mid(74, 6).replace(',', '.').toDouble(&ok)); // vib_pos_elevation
        if (!ok) {
            QMessageBox::critical(nullptr, "Error", "On line " + QString::number(counter) + ".<br>Can not read vib. position elevation (75, 80):<br>" + line.mid(74, 6));
            exit(1);
        }
        query.addBindValue(line.mid(81, 5).toLongLong(&ok)); // shot_np
        if (!ok) {
            QMessageBox::critical(nullptr, "Error", "On line " + QString::number(counter) + ".<br>Can not read shot nb (82, 86):<br>" + line.mid(81, 5));
            exit(1);
        }
        query.addBindValue(line.mid(86, 2).toInt(&ok)); // acquisition_nb
        if (!ok) {
            QMessageBox::critical(nullptr, "Error", "On line " + QString::number(counter) + ".<br>Can not read aquisition nb (87, 88):<br>" + line.mid(86, 2));
            exit(1);
        }
        query.addBindValue(line.mid(88, 2).toInt(&ok)); // two_digits_fleet_number
        if (!ok) {
            QMessageBox::critical(nullptr, "Error", "On line " + QString::number(counter) + ".<br>Can not read 2 digits fleet number (89, 90):<br>" + line.mid(88, 2));
            exit(1);
        }
        query.addBindValue(line.mid(90, 2).toInt(&ok)); // int_status_vib
        if (!ok) {
            QMessageBox::critical(nullptr, "Error", "On line " + QString::number(counter) + ".<br>Can not read vib status code (91, 92):<br>" + line.mid(90, 2));
            exit(1);
        }
        query.addBindValue(line.mid(93, 1)); // mass_1_warning
        query.addBindValue(line.mid(94, 1)); // mass_2_warning
        query.addBindValue(line.mid(95, 1)); // mass_3_warning
        query.addBindValue(line.mid(99, 1)); // plate_1_warning
        query.addBindValue(line.mid(100, 1)); // plate_2_warning
        query.addBindValue(line.mid(101, 1)); // plate_3_warning
        query.addBindValue(line.mid(102, 1)); // plate_4_warning
        query.addBindValue(line.mid(103, 1)); // plate_5_warning
        query.addBindValue(line.mid(104, 1)); // plate_6_warning
        query.addBindValue(line.mid(105, 1)); // force_overload
        query.addBindValue(line.mid(106, 1)); // pressure_overload
        query.addBindValue(line.mid(107, 1)); // mass_overload
        query.addBindValue(line.mid(108, 1)); // valve_overload
        query.addBindValue(line.mid(109, 1)); // excitation_overload
        query.addBindValue(line.mid(110, 2).toInt(&ok)); // stacking_fold
        if (!ok) {
            QMessageBox::critical(nullptr, "Error", "On line " + QString::number(counter) + ".<br>Can not read stacking fold (111, 112):<br>" + line.mid(110, 2));
            exit(1);
        }
        query.addBindValue(line.mid(112, 1)); // computation_domain
        query.addBindValue(line.mid(113, 4)); // ve432_version
        query.addBindValue(line.mid(117, 3).toInt(&ok)); // day_of_year
        if (!ok) {
            QMessageBox::critical(nullptr, "Error", "On line " + QString::number(counter) + ".<br>Can not read day of year (118, 120):<br>" + line.mid(117, 3));
            exit(1);
        }
        QString h = line.mid(120, 2);
        if (h == "  ")
            query.addBindValue(0); // hour
        else
            query.addBindValue(h.toInt(&ok)); // hour
        if (!ok) {
            QMessageBox::critical(nullptr, "Error", "On line " + QString::number(counter) + ".<br>Can not read hour (121, 122):<br>" + line.mid(120, 2));
            exit(1);
        }
        QString m = line.mid(122, 2);
        if (m == "  ")
            query.addBindValue(0); // minute
        else
            query.addBindValue(m.toInt(&ok)); // minute
        if (!ok) {
            QMessageBox::critical(nullptr, "Error", "On line " + QString::number(counter) + ".<br>Can not read minute (123, 124):<br>" + line.mid(122, 2));
            exit(1);
        }
        QString s = line.mid(124, 2);
        if (s == "  ")
            query.addBindValue(0); // second
        else
            query.addBindValue(s.toInt(&ok)); // second
        if (!ok) {
            QMessageBox::critical(nullptr, "Error", "On line " + QString::number(counter) + ".<br>Can not read second (125, 126):<br>" + line.mid(124, 2));
            exit(1);
        }
        query.addBindValue(line.mid(126, 4).replace(',', '.').toDouble(&ok)); // hdop
        if (!ok) {
            QMessageBox::critical(nullptr, "Error", "On line " + QString::number(counter) + ".<br>Can not read HDOP (127, 130):<br>" + line.mid(126, 4));
            exit(1);
        }
        query.addBindValue(line.mid(130, 20)); // tb_date
        query.addBindValue(line.mid(150, 199)); // gpgga
        query.exec();
    }
    redraw();
}

void MainWindow::draw_aps_points()
{
    if (!aps_db.isOpen())
        return;
    process_points();
    scene.setBackgroundBrush(QBrush(background_color));
    QPen pen;
    QBrush brush;
    QSqlQuery query("SELECT vib_pos_easting, vib_pos_northing, good, draw FROM data", aps_db);
    while (query.next()) {
        double vib_pos_easting = query.value(0).toDouble();
        double vib_pos_northing = query.value(1).toDouble();
        int good = query.value(2).toInt();
        int draw = query.value(3).toInt();
        if (good) {
            pen = vib_good_pen;
            brush = vib_good_brush;
        } else {
            pen = vib_bad_pen;
            brush = vib_bad_brush;
        }
        if (vib_pos_easting && vib_pos_northing && draw)
            scene.addEllipse(vib_pos_easting - vib_ellipse_size / 2, -(vib_pos_northing + vib_ellipse_size / 2), vib_ellipse_size, vib_ellipse_size, pen, brush);
    }
}

void MainWindow::change_statusbar(QPointF p)
{
    QString s;
    QTextStream out(&s);
    out.setRealNumberPrecision(1);
    out.setRealNumberNotation(QTextStream::FixedNotation);
    out << "X: ";
    out.setFieldWidth(10);
    out << p.x() << " Y: ";
    out.setFieldWidth(10);
    out << -p.y();
    statusBar()->showMessage(s);
}

void MainWindow::run_settings_dialog()
{
    SettingsDialog dialog;
    dialog.set_avg_phase_min(avg_phase_min);
    dialog.set_avg_phase_max(avg_phase_max);
    dialog.set_peak_phase_min(peak_phase_min);
    dialog.set_peak_phase_max(peak_phase_max);
    dialog.set_avg_distortion_min(avg_distortion_min);
    dialog.set_avg_distortion_max(avg_distortion_max);
    dialog.set_peak_distortion_min(peak_distortion_min);
    dialog.set_peak_distortion_max(peak_distortion_max);
    dialog.set_avg_force_min(avg_force_min);
    dialog.set_avg_force_max(avg_force_max);
    dialog.set_peak_force_min(peak_force_min);
    dialog.set_peak_force_max(peak_force_max);
    int code = dialog.exec();
    if (!code)
        return;
    QSettings settings(settings_file_name, QSettings::IniFormat);
    settings.beginGroup("Limits");
    avg_phase_min = dialog.avg_phase_min();
    settings.setValue("avg_phase_min", avg_phase_min);
    avg_phase_max = dialog.avg_phase_max();
    settings.setValue("avg_phase_max", avg_phase_max);
    peak_phase_min = dialog.peak_phase_min();
    settings.setValue("peak_phase_min", peak_phase_min);
    peak_phase_max = dialog.peak_phase_max();
    settings.setValue("peak_phase_max", peak_phase_max);
    avg_distortion_min = dialog.avg_distortion_min();
    settings.setValue("avg_distortion_min", avg_distortion_min);
    avg_distortion_max = dialog.avg_distortion_max();
    settings.setValue("avg_distortion_max", avg_distortion_max);
    peak_distortion_min = dialog.peak_distortion_min();
    settings.setValue("peak_distortion_min", peak_distortion_min);
    peak_distortion_max = dialog.peak_distortion_max();
    settings.setValue("peak_distortion_max", peak_distortion_max);
    avg_force_min = dialog.avg_force_min();
    settings.setValue("avg_force_min", avg_force_min);
    avg_force_max = dialog.avg_force_max();
    settings.setValue("avg_force_max", avg_force_max);
    peak_force_min = dialog.peak_force_min();
    settings.setValue("peak_force_min", peak_force_min);
    peak_force_max = dialog.peak_force_max();
    settings.setValue("peak_force_max", peak_force_max);
    settings.endGroup();
    while (avg_phase_min >= avg_phase_max || peak_phase_min >= peak_phase_max
        || avg_distortion_min >= avg_distortion_max || peak_distortion_min >= peak_distortion_max
        || avg_force_min >= avg_force_max || peak_force_min >= peak_force_max) {
        QMessageBox::critical(nullptr, "Error", "Minimum value of one of limits is more or equal to maximum value");
        run_settings_dialog();
    }
    redraw();
}

void MainWindow::run_color_dialog()
{
    ColorDialog dialog(background_color, vib_good_color, vib_bad_color, sps_color);
    dialog.set_sps_cross_size(cross_size);
    dialog.set_sps_ellipse_size(sps_ellipse_size);
    dialog.set_vib_ellipse_size(vib_ellipse_size);
    int code = dialog.exec();
    if (!code)
        return;
    QSettings settings(settings_file_name, QSettings::IniFormat);
    settings.beginGroup("Color_and_Size");
    background_color = dialog.background;
    settings.setValue("background", background_color);
    vib_bad_color = dialog.vib_bad;
    settings.setValue("vib_point_bad_color", vib_bad_color);
    vib_good_color = dialog.vib_good;
    settings.setValue("vib_point_good_color", vib_good_color);
    sps_color = dialog.sps;
    settings.setValue("sps_point_color", sps_color);
    vib_ellipse_size = dialog.vib_ellipse_size();
    settings.setValue("vib_ellipse_size", vib_ellipse_size);
    sps_ellipse_size = dialog.sps_ellipse_size();
    settings.setValue("sps_ellipse_size", sps_ellipse_size);
    cross_size = dialog.sps_cross_size();
    settings.setValue("sps_cross_size", cross_size);
    settings.endGroup();
    vib_bad_brush = QBrush(vib_bad_color);
    vib_good_brush = QBrush(vib_good_color);
    vib_bad_pen = QPen(vib_bad_color);
    vib_good_pen = QPen(vib_good_color);
    redraw();
}

void MainWindow::load_settings()
{
    QSettings settings(settings_file_name, QSettings::IniFormat);
    settings.beginGroup("Limits");
    avg_phase_min = settings.value("avg_phase_min", 0).toInt();
    avg_phase_max = settings.value("avg_phase_max", 5).toInt();
    peak_phase_min = settings.value("peak_phase_min", 0).toInt();
    peak_phase_max = settings.value("peak_phase_max", 15).toInt();
    avg_distortion_min = settings.value("avg_distortion_min", 0).toInt();
    avg_distortion_max = settings.value("avg_distortion_max", 25).toInt();
    peak_distortion_min = settings.value("peak_distortion_min", 0).toInt();
    peak_distortion_max = settings.value("peak_distortion_max", 35).toInt();
    avg_force_min = settings.value("avg_force_min", 0).toInt();
    avg_force_max = settings.value("avg_force_max", 99).toInt();
    peak_force_min = settings.value("peak_force_min", 0).toInt();
    peak_force_max = settings.value("peak_force_max", 99).toInt();
    settings.endGroup();
    settings.beginGroup("Color_and_Size");
    background_color = settings.value("background", QColor(Qt::black)).value<QColor>();
    vib_bad_color = settings.value("vib_point_bad_color", QColor(Qt::red)).value<QColor>();
    vib_good_color = settings.value("vib_point_good_color", QColor(Qt::green)).value<QColor>();
    sps_color = settings.value("sps_point_color", QColor(Qt::yellow)).value<QColor>();
    vib_ellipse_size = settings.value("vib_ellipse_size", 5).toInt();
    sps_ellipse_size = settings.value("sps_ellipse_size", 25).toInt();
    cross_size = settings.value("sps_cross_size", 5).toInt();
    settings.endGroup();
    vib_bad_brush = QBrush(vib_bad_color);
    vib_good_brush = QBrush(vib_good_color);
    vib_bad_pen = QPen(vib_bad_color);
    vib_good_pen = QPen(vib_good_color);
}

void MainWindow::process_points()
{
    // take unique names to process all acuisitions from this shot point at once
    QSqlQuery names_query("SELECT DISTINCT line_name, point_number FROM data", aps_db);
    names_query.exec();
    while (names_query.next()) {
        int line_name = names_query.value(0).toInt();
        int point_number = names_query.value(1).toInt();
        QSqlQuery sp_query(aps_db);
        sp_query.prepare("SELECT acquisition_nb, vib_number, day_of_year, hour, minute, second, "
                         "avg_phase, peak_phase, avg_distortion, peak_distortion, avg_force, peak_force, id "
                         "FROM data WHERE line_name = ? AND point_number = ? "
                         "ORDER BY acquisition_nb, vib_number, day_of_year, hour, minute, second");
        sp_query.addBindValue(line_name);
        sp_query.addBindValue(point_number);
        sp_query.exec();
        bool first = true;
        int prev_acuisition_nb, prev_vib_number, prev_id, curr_acuisition_nb, curr_vib_number, curr_day_of_year,
            curr_hour, curr_minute, curr_second, curr_avg_phase, curr_peak_phase, curr_avg_distortion,
            curr_peak_distortion, curr_avg_force, curr_peak_force, curr_id;
        while (sp_query.next()) {
            curr_acuisition_nb = sp_query.value(0).toInt();
            curr_vib_number = sp_query.value(1).toInt();
            curr_day_of_year = sp_query.value(2).toInt();
            curr_hour = sp_query.value(3).toInt();
            curr_minute = sp_query.value(4).toInt();
            curr_second = sp_query.value(5).toInt();
            curr_avg_phase = sp_query.value(6).toInt();
            curr_peak_phase = sp_query.value(7).toInt();
            curr_avg_distortion = sp_query.value(8).toInt();
            curr_peak_distortion = sp_query.value(9).toInt();
            curr_avg_force = sp_query.value(10).toInt();
            curr_peak_force = sp_query.value(11).toInt();
            curr_id = sp_query.value(12).toInt();
            // check if some vib from this acuisition was out of limits
            bool good = true;
            QSqlQuery check_query(aps_db);
            check_query.prepare("SELECT good FROM data WHERE line_name = ? AND point_number = ? "
                                "AND day_of_year = ? AND hour = ? AND minute = ? AND second = ?");
            check_query.addBindValue(line_name);
            check_query.addBindValue(point_number);
            check_query.addBindValue(curr_day_of_year);
            check_query.addBindValue(curr_hour);
            check_query.addBindValue(curr_minute);
            check_query.addBindValue(curr_second);
            check_query.exec();
            while (check_query.next())
                if (!check_query.value(0).toInt())
                    good = false;
            QSqlQuery mark_query(aps_db);
            mark_query.prepare("UPDATE data SET good = ?, draw = 1 WHERE id = ?");
            // if one of vib was out of limits than mark all other vibs as bad also
            if (good) {
                mark_query.addBindValue(1);
            } else {
                mark_query.addBindValue(0);
            }
            mark_query.addBindValue(curr_id);
            mark_query.exec();
            // if this vib is out of limits than mark whole acuisition as bad
            if (curr_avg_phase < avg_phase_min || curr_avg_phase > avg_phase_max
                || curr_peak_phase < peak_phase_min || curr_peak_phase > peak_phase_max
                || curr_avg_distortion < avg_distortion_min || curr_avg_distortion > avg_distortion_max
                || curr_peak_distortion < peak_distortion_min || curr_peak_distortion > peak_distortion_max
                || curr_avg_force < avg_force_min || curr_avg_force > avg_force_max
                || curr_peak_force < peak_force_min || curr_peak_force > peak_force_max) {
                mark_query.prepare("UPDATE data SET good = 0 WHERE id = ?");
                mark_query.addBindValue(curr_id);
                mark_query.exec();
            }
            if (first) {
                first = false;
                prev_acuisition_nb = curr_acuisition_nb;
                prev_vib_number = curr_vib_number;
                prev_id = curr_id;
                continue;
            }
            // if this acuisition was restarted than hide this point
            if (curr_acuisition_nb == prev_acuisition_nb && curr_vib_number == prev_vib_number) {
                mark_query.prepare("UPDATE data SET draw = 0 WHERE id = ?");
                mark_query.addBindValue(prev_id);
                mark_query.exec();
            }
            prev_acuisition_nb = curr_acuisition_nb;
            prev_vib_number = curr_vib_number;
            prev_id = curr_id;
        }
    }
}

void MainWindow::peek_point(QPointF p)
{
    if (!aps_db.isOpen())
        return;
    QSqlQuery query(aps_db);
    p = QPointF(p.x(), -p.y());
    query.prepare("SELECT line_name, point_number, avg_phase, peak_phase, "
                  "avg_distortion, peak_distortion, avg_force, peak_force, "
                  "vib_pos_easting, vib_pos_northing, id FROM data "
                  "WHERE vib_pos_easting > ? AND vib_pos_easting < ? "
                  "AND vib_pos_northing > ? AND vib_pos_northing < ?");
    query.addBindValue(p.x() - search_radius);
    query.addBindValue(p.x() + search_radius);
    query.addBindValue(p.y() - search_radius);
    query.addBindValue(p.y() + search_radius);
    query.exec();
    bool first = true;
    int line_name, point_number;
    double vib_x, vib_y;
    double dist;
    while (query.next()) {
        double curr_x = query.value(8).toDouble();
        double curr_y = query.value(9).toDouble();
        if (first) {
            first = false;
            line_name = query.value(0).toInt();
            point_number = query.value(1).toInt();
            vib_x = curr_x;
            vib_y = curr_y;
            dist = sqrt(pow(p.x() - curr_x, 2) + pow(p.y() - curr_y, 2));
            continue;
        }
        double curr_dist = sqrt(pow(p.x() - curr_x, 2) + pow(p.y() - curr_y, 2));
        if (dist > curr_dist) {
            dist = curr_dist;
            line_name = query.value(0).toInt();
            point_number = query.value(1).toInt();
            vib_x = curr_x;
            vib_y = curr_y;
        }
    }
    QPen pen((QColor(Qt::cyan)));
    selected = scene.addEllipse(vib_x - vib_ellipse_size / 2, vib_y - vib_ellipse_size / 2, vib_ellipse_size, vib_ellipse_size, pen);
    query.prepare("SELECT line_name, point_number, vib_number, avg_phase, peak_phase, "
                  "avg_distortion, peak_distortion, avg_force, peak_force, shot_nb, "
                  "acquisition_nb, vib_status_code, day_of_year, hour, minute, second "
                  "FROM data WHERE line_name = ? AND point_number = ? "
                  "ORDER BY day_of_year DESC, hour DESC, minute DESC, second DESC, acquisition_nb DESC, vib_number DESC");
    query.addBindValue(line_name);
    query.addBindValue(point_number);
    query.exec();
    InfoDialog dialog;
    while (query.next()) {
        VAPSrecord r;
        r.line_name = QPair<int, bool>(query.value(0).toInt(), true);
        r.point_number = QPair<int, bool>(query.value(1).toInt(), true);
        r.vib_number = QPair<int, bool>(query.value(2).toInt(), true);
        int avg_phase = query.value(3).toInt();
        bool good = true;
        if (avg_phase < avg_phase_min || avg_phase > avg_phase_max)
            good = false;
        r.avg_phase = QPair<int, bool>(avg_phase, good);
        int peak_phase = query.value(4).toInt();
        good = true;
        if (peak_phase < peak_phase_min || peak_phase > peak_phase_max)
            good = false;
        r.peak_phase = QPair<int, bool>(peak_phase, good);
        int avg_distortion = query.value(5).toInt();
        good = true;
        if (avg_distortion < avg_distortion_min || avg_distortion > avg_distortion_max)
            good = false;
        r.avg_distortion = QPair<int, bool>(avg_distortion, good);
        int peak_distortion = query.value(6).toInt();
        good = true;
        if (peak_distortion < peak_distortion_min || peak_distortion > peak_distortion_max)
            good = false;
        r.peak_distortion = QPair<int, bool>(peak_distortion, good);
        int avg_force = query.value(7).toInt();
        good = true;
        if (avg_force < avg_force_min || avg_force > avg_force_max)
            good = false;
        r.avg_force = QPair<int, bool>(avg_force, good);
        int peak_force = query.value(8).toInt();
        good = true;
        if (peak_force < peak_force_min || peak_force > peak_force_max)
            good = false;
        r.peak_force = QPair<int, bool>(peak_force, good);
        r.shot_nb = QPair<int, bool>(query.value(9).toInt(), true);
        r.acquisition_nb = QPair<int, bool>(query.value(10).toInt(), true);
        r.vib_status_code = QPair<int, bool>(query.value(11).toInt(), true);
        r.day_of_year = QPair<int, bool>(query.value(12).toInt(), true);
        r.hour = QPair<int, bool>(query.value(13).toInt(), true);
        r.minute = QPair<int, bool>(query.value(14).toInt(), true);
        r.second = QPair<int, bool>(query.value(15).toInt(), true);
        dialog.add_row(r);
    }
    dialog.exec();
    if (selected) {
        scene.removeItem(selected);
        delete selected;
        selected = nullptr;
    }
}

void MainWindow::open_sps_file()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open Source SPS file"));
    if (file_name.isEmpty())
        return;
    QFile s_sps_file(file_name);
    if (!s_sps_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(nullptr, "Error", "Can not read S SPS file");
        QApplication::quit();
    }
    QTextStream in(&s_sps_file);
    while (!in.atEnd()) {
        QString str = in.readLine();
        if (str.startsWith('S')) {
            bool ok = true;
            double x = str.mid(46, 9).toDouble(&ok);
            if (!ok) {
                QMessageBox::critical(nullptr, "Error", "Can not read X coordinate (47, 55) from S SPS file: " + str);
                exit(1);
            }
            double y = str.mid(55, 10).toDouble(&ok);
            if (!ok) {
                QMessageBox::critical(nullptr, "Error", "Can not read Y coordinate (56, 65) from S SPS file: " + str);
                exit(1);
            }
            sps_points.push_back(QPair<double, double>(x, y));
        }
    }
    redraw();
}

void MainWindow::draw_sps_points()
{
    if (sps_points.isEmpty())
        return;
    QPen el_pen(sps_color);
    QPen cross_pen(sps_color);
    for (QPair<double, double>& p : sps_points) {
        scene.addEllipse(p.first - sps_ellipse_size / 2, -(p.second + sps_ellipse_size / 2), sps_ellipse_size, sps_ellipse_size, el_pen);
        scene.addLine(p.first - cross_size / 2, -p.second, p.first + cross_size / 2, -p.second, cross_pen);
        scene.addLine(p.first, -(p.second - cross_size / 2), p.first, -(p.second + cross_size / 2), cross_pen);
    }
}

void MainWindow::redraw()
{
    scene.clear();
    draw_aps_points();
    draw_sps_points();
}
