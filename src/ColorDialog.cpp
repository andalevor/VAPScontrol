#include "ColorDialog.hpp"
#include "ui_ColorDialog.h"
#include <QColorDialog>

ColorDialog::ColorDialog(QColor bg, QColor vg, QColor vb, QColor s, QWidget* parent)
    : QDialog(parent)
    , background(bg)
    , vib_good(vg)
    , vib_bad(vb)
    , sps(s)
    , ui(new Ui::ColorDialog)
{
    ui->setupUi(this);
    QPalette pal = ui->backColorBut->palette();
    pal.setColor(QPalette::Button, background);
    ui->backColorBut->setAutoFillBackground(true);
    ui->backColorBut->setPalette(pal);
    ui->backColorBut->update();
    pal.setColor(QPalette::Button, vib_good);
    ui->vibGoodBut->setAutoFillBackground(true);
    ui->vibGoodBut->setPalette(pal);
    ui->vibGoodBut->update();
    pal.setColor(QPalette::Button, vib_bad);
    ui->vibBadBut->setAutoFillBackground(true);
    ui->vibBadBut->setPalette(pal);
    ui->vibBadBut->update();
    pal.setColor(QPalette::Button, sps);
    ui->spsColorBut->setAutoFillBackground(true);
    ui->spsColorBut->setPalette(pal);
    ui->spsColorBut->update();
    connect(ui->backColorBut, SIGNAL(clicked()), this, SLOT(background_color_changed()));
    connect(ui->vibGoodBut, SIGNAL(clicked()), this, SLOT(vib_good_color_changed()));
    connect(ui->vibBadBut, SIGNAL(clicked()), this, SLOT(vib_bad_color_changed()));
    connect(ui->spsColorBut, SIGNAL(clicked()), this, SLOT(sps_color_changed()));
}

ColorDialog::~ColorDialog()
{
    delete ui;
}

void ColorDialog::background_color_changed()
{
    QColor c = QColorDialog::getColor();
    if (!c.isValid())
        return;
    background = c;
    QPalette pal = ui->backColorBut->palette();
    pal.setColor(QPalette::Button, c);
    ui->backColorBut->setAutoFillBackground(true);
    ui->backColorBut->setPalette(pal);
    ui->backColorBut->update();
}

void ColorDialog::vib_good_color_changed()
{
    QColor c = QColorDialog::getColor();
    if (!c.isValid())
        return;
    vib_good = c;
    QPalette pal = ui->vibGoodBut->palette();
    pal.setColor(QPalette::Button, c);
    ui->vibGoodBut->setAutoFillBackground(true);
    ui->vibGoodBut->setPalette(pal);
    ui->vibGoodBut->update();
}

void ColorDialog::vib_bad_color_changed()
{
    QColor c = QColorDialog::getColor();
    if (!c.isValid())
        return;
    vib_bad = c;
    QPalette pal = ui->vibBadBut->palette();
    pal.setColor(QPalette::Button, c);
    ui->vibBadBut->setAutoFillBackground(true);
    ui->vibBadBut->setPalette(pal);
    ui->vibBadBut->update();
}

void ColorDialog::sps_color_changed()
{
    QColor c = QColorDialog::getColor();
    if (!c.isValid())
        return;
    sps = c;
    QPalette pal = ui->spsColorBut->palette();
    pal.setColor(QPalette::Button, c);
    ui->spsColorBut->setAutoFillBackground(true);
    ui->spsColorBut->setPalette(pal);
    ui->spsColorBut->update();
}

int ColorDialog::vib_ellipse_size()
{
    return ui->vibSizeSpin->value();
}

int ColorDialog::sps_ellipse_size()
{
    return ui->spsEllSizeSpin->value();
}

int ColorDialog::sps_cross_size()
{
    return ui->spsCrossSizeSpin->value();
}

void ColorDialog::set_vib_ellipse_size(int s)
{
    ui->vibSizeSpin->setValue(s);
}

void ColorDialog::set_sps_ellipse_size(int s)
{
    ui->spsEllSizeSpin->setValue(s);
}

void ColorDialog::set_sps_cross_size(int s)
{
    ui->spsCrossSizeSpin->setValue(s);
}
