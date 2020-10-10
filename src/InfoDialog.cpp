#include "InfoDialog.hpp"
#include "ui_InfoDialog.h"
#include <QDebug>

InfoDialog::InfoDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::InfoDialog)
{
    ui->setupUi(this);
    ui->tableWidget->clear();
    QStringList headers = { tr("Line name"), tr("Point Nb"), tr("Vib Nb"),
        tr("Avg phase"), tr("Peak phase"), tr("Avg distortion"),
        tr("Peak distortion"), tr("Avg force"), tr("Peak force"),
        tr("Shot Nb"), tr("Acuisition Nb"), tr("Vib code"),
        tr("Day"), tr("Hour"), tr("Minute"), tr("Second") };
    ui->tableWidget->setColumnCount(headers.size());
    for (int i = 0; i < ui->tableWidget->columnCount(); ++i)
        ui->tableWidget->setColumnWidth(i, ui->tableWidget->fontMetrics().size(0, (headers[i])).width() + 10);
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

InfoDialog::~InfoDialog()
{
    delete ui;
}

void InfoDialog::add_row(VAPSrecord r)
{
    ui->tableWidget->insertRow(0);
    ui->tableWidget->setItem(0, 0, new QTableWidgetItem(QString::number(r.line_name.first)));
    ui->tableWidget->setItem(0, 1, new QTableWidgetItem(QString::number(r.point_number.first)));
    ui->tableWidget->setItem(0, 2, new QTableWidgetItem(QString::number(r.vib_number.first)));
    ui->tableWidget->setItem(0, 3, new QTableWidgetItem(QString::number(r.avg_phase.first)));
    if (!r.avg_phase.second)
        ui->tableWidget->item(0, 3)->setBackground(QBrush(Qt::red));
    ui->tableWidget->setItem(0, 4, new QTableWidgetItem(QString::number(r.peak_phase.first)));
    if (!r.peak_phase.second)
        ui->tableWidget->item(0, 4)->setBackground(QBrush(Qt::red));
    ui->tableWidget->setItem(0, 5, new QTableWidgetItem(QString::number(r.avg_distortion.first)));
    if (!r.avg_distortion.second)
        ui->tableWidget->item(0, 5)->setBackground(QBrush(Qt::red));
    ui->tableWidget->setItem(0, 6, new QTableWidgetItem(QString::number(r.peak_distortion.first)));
    if (!r.peak_distortion.second)
        ui->tableWidget->item(0, 6)->setBackground(QBrush(Qt::red));
    ui->tableWidget->setItem(0, 7, new QTableWidgetItem(QString::number(r.avg_force.first)));
    if (!r.avg_force.second)
        ui->tableWidget->item(0, 7)->setBackground(QBrush(Qt::red));
    ui->tableWidget->setItem(0, 8, new QTableWidgetItem(QString::number(r.peak_force.first)));
    if (!r.peak_force.second)
        ui->tableWidget->item(0, 8)->setBackground(QBrush(Qt::red));
    ui->tableWidget->setItem(0, 9, new QTableWidgetItem(QString::number(r.shot_nb.first)));
    ui->tableWidget->setItem(0, 10, new QTableWidgetItem(QString::number(r.acquisition_nb.first)));
    ui->tableWidget->setItem(0, 11, new QTableWidgetItem(QString::number(r.vib_status_code.first)));
    ui->tableWidget->setItem(0, 12, new QTableWidgetItem(QString::number(r.day_of_year.first)));
    ui->tableWidget->setItem(0, 13, new QTableWidgetItem(QString::number(r.hour.first)));
    ui->tableWidget->setItem(0, 14, new QTableWidgetItem(QString::number(r.minute.first)));
    ui->tableWidget->setItem(0, 15, new QTableWidgetItem(QString::number(r.second.first)));
}
