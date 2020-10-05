#include "SettingsDialog.hpp"
#include "ui_SettingsDialog.h"

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

int SettingsDialog::avg_phase_min() { return ui->avgPhaseMin->value(); }
int SettingsDialog::avg_phase_max() { return ui->avgPhaseMax->value(); }
int SettingsDialog::peak_phase_min() { return ui->peakPhaseMin->value(); }
int SettingsDialog::peak_phase_max() { return ui->peakPhaseMax->value(); }
int SettingsDialog::avg_distortion_min() { return ui->avgDistortionMin->value(); }
int SettingsDialog::avg_distortion_max() { return ui->avgDistortionMax->value(); }
int SettingsDialog::peak_distortion_min() { return ui->peakDistortionMin->value(); }
int SettingsDialog::peak_distortion_max() { return ui->peakDistortionMax->value(); }
int SettingsDialog::avg_force_min() { return ui->avgForceMin->value(); }
int SettingsDialog::avg_force_max() { return ui->avgForceMax->value(); }
int SettingsDialog::peak_force_min() { return ui->peakForceMin->value(); }
int SettingsDialog::peak_force_max() { return ui->peakForceMax->value(); }
void SettingsDialog::set_avg_phase_min(int value) { ui->avgPhaseMin->setValue(value); }
void SettingsDialog::set_avg_phase_max(int value) { ui->avgPhaseMax->setValue(value); }
void SettingsDialog::set_peak_phase_min(int value) { ui->peakPhaseMin->setValue(value); }
void SettingsDialog::set_peak_phase_max(int value) { ui->peakPhaseMax->setValue(value); }
void SettingsDialog::set_avg_distortion_min(int value) { ui->avgDistortionMin->setValue(value); }
void SettingsDialog::set_avg_distortion_max(int value) { ui->avgDistortionMax->setValue(value); }
void SettingsDialog::set_peak_distortion_min(int value) { ui->peakDistortionMin->setValue(value); }
void SettingsDialog::set_peak_distortion_max(int value) { ui->peakDistortionMax->setValue(value); }
void SettingsDialog::set_avg_force_min(int value) { ui->avgForceMin->setValue(value); }
void SettingsDialog::set_avg_force_max(int value) { ui->avgForceMax->setValue(value); }
void SettingsDialog::set_peak_force_min(int value) { ui->peakForceMin->setValue(value); }
void SettingsDialog::set_peak_force_max(int value) { ui->peakForceMax->setValue(value); }
