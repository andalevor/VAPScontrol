#ifndef COLORDIALOG_HPP
#define COLORDIALOG_HPP

#include <QDialog>
#include <QTranslator>

namespace Ui {
class ColorDialog;
}

class ColorDialog : public QDialog {
    Q_OBJECT

public:
    ColorDialog(QColor bg, QColor vg, QColor vb, QColor s, QString l, QWidget* parent = nullptr);
    ~ColorDialog();

    QColor background, vib_good, vib_bad, sps;
    QString language;
    int vib_ellipse_size();
    int sps_ellipse_size();
    int sps_cross_size();
    void set_vib_ellipse_size(int s);
    void set_sps_ellipse_size(int s);
    void set_sps_cross_size(int s);

private slots:
    void background_color_changed();
    void vib_good_color_changed();
    void vib_bad_color_changed();
    void sps_color_changed();
    void lang_selection_changed(int i);

private:
    Ui::ColorDialog* ui;
    QTranslator translator;
};

#endif // COLORDIALOG_HPP
