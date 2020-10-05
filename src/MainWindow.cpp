#include "MainWindow.hpp"
#include "SettingsDialog.hpp"
#include "VAPSrecord.hpp"
#include "ui_MainWindow.h"
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QSettings>
#include <QSqlError>
#include <QSqlQuery>
#include <QTextStream>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , db(QSqlDatabase::addDatabase("QSQLITE"))
    , ellipse_size { 5 }
    , good_pen((QColor(Qt::GlobalColor::green)))
    , good_brush(Qt::GlobalColor::green)
    , bad_pen((QColor(Qt::GlobalColor::red)))
    , bad_brush(Qt::GlobalColor::red)
{
    db.setDatabaseName(":memory:");
    settings_file_name = QApplication::applicationDirPath() + "/settings.ini";
    load_settings();
    ui->setupUi(this);
    ui->graphicsView->setScene(&scene);
    connect(ui->actionExit, SIGNAL(triggered()), QApplication::instance(), SLOT(quit()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(open_file()));
    connect(ui->graphicsView, SIGNAL(mousePosChanged(QPointF)), this, SLOT(change_statusbar(QPointF)));
    connect(ui->actionLimits, SIGNAL(triggered()), this, SLOT(run_settings_dialog()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::open_file()
{
    if (db.isOpen()) {
        db.close();
    }
    QString file_name = "/home/andalevor/Work/vaps/24092020apsver";
    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(nullptr, "Error", "Can not read from " + file_name);
        return;
    }
    QTextStream in(&file);
    if (!db.open()) {
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
               "int_status_vib INTEGER, mass_1_warning TEXT, "
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
                      "int_status_vib, mass_1_warning, mass_2_warning, "
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
    draw_points();
}

void MainWindow::draw_points()
{
    if (!db.isOpen())
        return;
    scene.clear();
    process_points();
    scene.setBackgroundBrush(QBrush(Qt::black));
    QPen pen;
    QBrush brush;
    QSqlQuery query("SELECT vib_pos_easting, vib_pos_northing, good, draw FROM data", db);
    while (query.next()) {
        double vib_pos_easting = query.value(0).toDouble();
        double vib_pos_northing = query.value(1).toDouble();
        int good = query.value(2).toInt();
        int draw = query.value(3).toInt();
        if (good) {
            pen = good_pen;
            brush = good_brush;
        } else {
            pen = bad_pen;
            brush = bad_brush;
        }
        if (vib_pos_easting && vib_pos_northing && draw)
            scene.addEllipse(vib_pos_easting - ellipse_size / 2, vib_pos_northing - ellipse_size / 2, ellipse_size, ellipse_size, pen, brush);
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
    out << p.y();
    statusBar()->showMessage(s);
}

void MainWindow::run_settings_dialog()
{
    QSettings settings(settings_file_name, QSettings::IniFormat);
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
    draw_points();
}

void MainWindow::load_settings()
{
    QSettings settings(settings_file_name, QSettings::IniFormat);
    settings.beginGroup("Limits");
    avg_phase_min = settings.value("avg_phase_min").toInt();
    avg_phase_max = settings.value("avg_phase_max").toInt();
    peak_phase_min = settings.value("peak_phase_min").toInt();
    peak_phase_max = settings.value("peak_phase_max").toInt();
    avg_distortion_min = settings.value("avg_distortion_min").toInt();
    avg_distortion_max = settings.value("avg_distortion_max").toInt();
    peak_distortion_min = settings.value("peak_distortion_min").toInt();
    peak_distortion_max = settings.value("peak_distortion_max").toInt();
    avg_force_min = settings.value("avg_force_min").toInt();
    avg_force_max = settings.value("avg_force_max").toInt();
    peak_force_min = settings.value("peak_force_min").toInt();
    peak_force_max = settings.value("peak_force_max").toInt();
    settings.endGroup();
}

void MainWindow::process_points()
{
    // take unique names to process all acuisitions from this shot point at once
    QSqlQuery names_query("SELECT DISTINCT line_name, point_number FROM data", db);
    names_query.exec();
    while (names_query.next()) {
        int line_name = names_query.value(0).toInt();
        int point_number = names_query.value(1).toInt();
        QSqlQuery sp_query(db);
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
            QSqlQuery check_query(db);
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
            QSqlQuery mark_query(db);
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
