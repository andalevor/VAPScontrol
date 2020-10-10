#ifndef INFODIALOG_HPP
#define INFODIALOG_HPP

#include "VAPSrecord.hpp"
#include <QDialog>

namespace Ui {
class InfoDialog;
}

class InfoDialog : public QDialog {
    Q_OBJECT

public:
    explicit InfoDialog(QWidget* parent = nullptr);
    ~InfoDialog();
    void add_row(VAPSrecord r);

private:
    Ui::InfoDialog* ui;
};

#endif // INFODIALOG_HPP
