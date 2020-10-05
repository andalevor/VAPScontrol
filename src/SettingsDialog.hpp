#ifndef SETTINGSDIALOG_HPP
#define SETTINGSDIALOG_HPP

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget* parent = nullptr);
    ~SettingsDialog();
    int avg_phase_min();
    int avg_phase_max();
    int peak_phase_min();
    int peak_phase_max();
    int avg_distortion_min();
    int avg_distortion_max();
    int peak_distortion_min();
    int peak_distortion_max();
    int avg_force_min();
    int avg_force_max();
    int peak_force_min();
    int peak_force_max();
    void set_avg_phase_min(int value);
    void set_avg_phase_max(int value);
    void set_peak_phase_min(int value);
    void set_peak_phase_max(int value);
    void set_avg_distortion_min(int value);
    void set_avg_distortion_max(int value);
    void set_peak_distortion_min(int value);
    void set_peak_distortion_max(int value);
    void set_avg_force_min(int value);
    void set_avg_force_max(int value);
    void set_peak_force_min(int value);
    void set_peak_force_max(int value);

private:
    Ui::SettingsDialog* ui;
};

#endif // SETTINGSDIALOG_HPP
