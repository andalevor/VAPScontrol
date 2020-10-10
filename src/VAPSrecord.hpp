#ifndef VAPSRECORD_HPP
#define VAPSRECORD_HPP

#include <QPair>
#include <QString>

class VAPSrecord {
public:
    QPair<char, bool> record_id;
    QPair<int, bool> line_name;
    QPair<int, bool> point_number;
    QPair<int, bool> point_index;
    QPair<int, bool> fleet_number;
    QPair<int, bool> vib_number;
    QPair<int, bool> vib_drive_level;
    QPair<int, bool> avg_phase;
    QPair<int, bool> peak_phase;
    QPair<int, bool> avg_distortion;
    QPair<int, bool> peak_distortion;
    QPair<int, bool> avg_force;
    QPair<int, bool> peak_force;
    QPair<int, bool> avg_ground_stiffness;
    QPair<int, bool> avg_ground_viscosity;
    QPair<double, bool> vib_pos_easting;
    QPair<double, bool> vib_pos_northing;
    QPair<double, bool> vib_pos_elevation;
    QPair<long long, bool> shot_nb;
    QPair<int, bool> acquisition_nb;
    QPair<int, bool> two_digits_fleet_number;
    QPair<int, bool> vib_status_code;
    QPair<char, bool> mass_1_warning;
    QPair<char, bool> mass_2_warning;
    QPair<char, bool> mass_3_warning;
    QPair<char, bool> plate_1_warning;
    QPair<char, bool> plate_2_warning;
    QPair<char, bool> plate_3_warning;
    QPair<char, bool> plate_4_warning;
    QPair<char, bool> plate_5_warning;
    QPair<char, bool> plate_6_warning;
    QPair<char, bool> force_overload;
    QPair<char, bool> pressure_overload;
    QPair<char, bool> mass_overload;
    QPair<char, bool> valve_overload;
    QPair<char, bool> excitation_overload;
    QPair<int, bool> stacking_fold;
    QPair<char, bool> computation_domain;
    QPair<QString, bool> ve432_version;
    QPair<int, bool> day_of_year;
    QPair<int, bool> hour;
    QPair<int, bool> minute;
    QPair<int, bool> second;
    QPair<double, bool> hdop;
    QPair<QString, bool> tb_date;
    QPair<QString, bool> gpgga;
};

#endif // VAPSRECORD_HPP
