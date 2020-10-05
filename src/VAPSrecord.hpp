#ifndef VAPSRECORD_HPP
#define VAPSRECORD_HPP

#include <QString>

class VAPSrecord {
public:
    QChar record_id;
    QString line_name;
    QString point_number;
    int point_index;
    int fleet_number;
    int vib_number;
    int vib_drive_level;
    int avg_phase;
    int peak_phase;
    int avg_distortion;
    int peak_distortion;
    int avg_force;
    int peak_force;
    int avg_ground_stiffness;
    int avg_ground_viscosity;
    double vib_pos_easting;
    double vib_pos_northing;
    double vib_pos_elevation;
    long long shot_nb;
    int acquisition_nb;
    int two_digits_fleet_number;
    int vib_status_code;
    QChar mass_1_warning;
    QChar mass_2_warning;
    QChar mass_3_warning;
    QChar plate_1_warning;
    QChar plate_2_warning;
    QChar plate_3_warning;
    QChar plate_4_warning;
    QChar plate_5_warning;
    QChar plate_6_warning;
    QChar force_overload;
    QChar pressure_overload;
    QChar mass_overload;
    QChar valve_overload;
    QChar excitation_overload;
    int stacking_fold;
    QChar computation_domain;
    QString ve432_version;
    int day_of_year;
    int hour;
    int minute;
    int second;
    double hdop;
    QString tb_date;
    QString gpgga;
};

#endif // VAPSRECORD_HPP
