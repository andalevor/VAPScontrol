#ifndef COLORDIALOG_HPP
#define COLORDIALOG_HPP

#include <QDialog>

namespace Ui {
class ColorDialog;
}

class ColorDialog : public QDialog {
    Q_OBJECT

public:
    ColorDialog(QColor bg, QColor vg, QColor vb, QColor s, QWidget* parent = nullptr);
    ~ColorDialog();

    QColor background, vib_good, vib_bad, sps;
    int vib_ellipse_size();
    int sps_ellipse_size();
    int sps_cross_size();
    void set_vib_ellipse_size(int s);
    void set_sps_ellipse_size(int s);
    void set_sps_cross_size(int s);

public slots:
    void background_color_changed();
    void vib_good_color_changed();
    void vib_bad_color_changed();
    void sps_color_changed();

private:
    Ui::ColorDialog* ui;
};

#endif // COLORDIALOG_HPP
