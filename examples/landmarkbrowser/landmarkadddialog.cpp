/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QListWidgetItem>
#include <qlandmarkcategory.h>
#include <QMessageBox>

#include <qlandmarkmanager.h>

#include "landmarkadddialog.h"

LandmarkAddDialog::LandmarkAddDialog(QWidget *parent, Qt::WindowFlags flags, const QLandmark &landmark)
    : QDialog(parent, flags)
{
    setupUi(this);
    if (landmark != QLandmark()) {
        setWindowTitle("Edit Landmark");

        lm = landmark;
        nameLineEdit->setText(landmark.name());
        latitudeLineEdit->setText(QString::number(landmark.coordinate().latitude()));
        longitudeLineEdit->setText(QString::number(landmark.coordinate().longitude()));
        streetLineEdit->setText(landmark.address().street());
        districtLineEdit->setText(landmark.address().district());
        cityLineEdit->setText(landmark.address().city());
        stateLineEdit->setText(landmark.address().state());
        countryLineEdit->setText(landmark.address().country());
        descriptionLineEdit->setText(landmark.description());
        iconUrlLineEdit->setText(landmark.iconUrl().toString());
        urlLineEdit->setText(landmark.url().toString());
        phoneLineEdit->setText(landmark.phoneNumber());
        radiusLineEdit->setText(QString::number(landmark.radius()));
    } else {
        setWindowTitle("Add Landmark");
    }

    QLandmarkManager manager;
    QList<QLandmarkCategory> categories = manager.categories();

    foreach( QLandmarkCategory category, categories) {
        QListWidgetItem  *categoryItem = new QListWidgetItem(categoryList,QListWidgetItem::UserType + 1);
        categoryItem->setData(Qt::DisplayRole,category.name());
        QVariant var;
        var.setValue(category.categoryId());
        categoryItem->setData(Qt::UserRole, var);
        categoryItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        if (landmark.categoryIds().contains(category.categoryId()))
            categoryItem->setCheckState(Qt::Checked);
        else
            categoryItem->setCheckState(Qt::Unchecked);
    }

    categoryList->setMinimumHeight(categories.count()
                                   * (categoryList->sizeHintForRow(0) + categoryList->spacing())
                                   + categoryList->frameWidth() *2);
}

LandmarkAddDialog::~LandmarkAddDialog()
{
    delete categoryList;
    categoryList =0;
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

    QGeoCoordinate coordinate;
    coordinate.setLatitude(latitude);
    coordinate.setLongitude(longitude);
    lm.setCoordinate(coordinate);
    lm.setName(nameLineEdit->text());
    QGeoAddress address = lm.address();
    address.setStreet(streetLineEdit->text());
    address.setDistrict(districtLineEdit->text());
    address.setCity(cityLineEdit->text());
    address.setState(stateLineEdit->text());
    address.setCountry(countryLineEdit->text());
    lm.setAddress(address);
    lm.setDescription(descriptionLineEdit->text());
    lm.setIconUrl(QUrl(iconUrlLineEdit->text()));
    lm.setUrl(QUrl(urlLineEdit->text()));
    lm.setPhoneNumber(phoneLineEdit->text());
    lm.setRadius(radiusLineEdit->text().toDouble());

    for (int i=0; i < categoryList->count(); ++i) {
        if (categoryList->item(i)->checkState() == Qt::Checked) {
            lm.addCategoryId(categoryList->item(i)->data(Qt::UserRole).value<QLandmarkCategoryId>());
        }
    }

    QDialog::accept();
}

void LandmarkAddDialog::reject()
{
    lm.clear();
    QDialog::reject();
}
