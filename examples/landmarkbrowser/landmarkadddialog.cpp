#include <QMessageBox>

#include "landmarkadddialog.h"

LandmarkAddDialog::LandmarkAddDialog(QWidget *parent, Qt::WindowFlags flags)
    : QDialog(parent, flags)
{
    setupUi(this);
}

LandmarkAddDialog::~LandmarkAddDialog()
{
}

QLandmark LandmarkAddDialog::landmark()
{
    return lm;
}

void LandmarkAddDialog::accept()
{
    bool ok;
    double latitude = this->latitudeLineEdit->text().toDouble(&ok);
    if (!ok || latitude < -90.0 || latitude > 90.0) {
        QMessageBox::warning(this, "Invalid latitude", "Latitude must be between -90 and 90 degrees",
                                QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }

    double longitude = this->longitudeLineEdit->text().toDouble(&ok);
    if (!ok || longitude < -180.0 || longitude > 180.0) {
        QMessageBox::warning(this, "Invalid longitude", "Longitude must be between -180 and 180 degrees",
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }

    lm.clear();
    QGeoCoordinate coordinate;
    coordinate.setLatitude(latitude);
    coordinate.setLongitude(longitude);
    lm.setCoordinate(coordinate);
    lm.setName(nameLineEdit->text());
    QDialog::accept();
}

void LandmarkAddDialog::reject()
{
    lm.clear();
    QDialog::reject();
}
