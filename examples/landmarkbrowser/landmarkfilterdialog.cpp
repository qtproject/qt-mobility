/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#include <QButtonGroup>
#include <QListWidgetItem>
#include <QMessageBox>

#include <qlandmarkmanager.h>
#include <qlandmarkintersectionfilter.h>
#include <qlandmarknamefilter.h>
#include <qlandmarkcategoryfilter.h>
#include <qlandmarkboxfilter.h>
#include <qlandmarkproximityfilter.h>
#include <QDebug>

#include "landmarkfilterdialog.h"

LandmarkFilterDialog::LandmarkFilterDialog(QLandmarkFetchRequest *fetchRequest, QLandmarkManager * manager, QWidget *parent, Qt::WindowFlags flags)
    : QDialog(parent, flags),
    manager(manager),
      fetchRequest(fetchRequest)

{
    setupUi(this);

    connect(filterAllCheckBox,SIGNAL(stateChanged(int)),
            this,SLOT(filterAllCheckBoxStateChanged(int)));
    connect(filterNameCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(otherFiltersCheckBoxStateChanged(int)));
    connect(filterCategoryCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(otherFiltersCheckBoxStateChanged(int)));
    connect(filterBoxCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(otherFiltersCheckBoxStateChanged(int)));
    connect(filterProximityCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(otherFiltersCheckBoxStateChanged(int)));

    connect(manager, SIGNAL(categoriesRemoved(QList<QLandmarkCategoryId>)),
            this, SLOT(categoryRemoved(QList<QLandmarkCategoryId>)));

    filterAllCheckBox->setCheckState(Qt::Checked);

    QButtonGroup * buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(startsWithRadioButton);
    buttonGroup->addButton(containsRadioButton);
}

LandmarkFilterDialog::~LandmarkFilterDialog()
{
}

void LandmarkFilterDialog::accept()
{
    if (!setupFetchRequest())
        return;
    QDialog::accept();
}

void LandmarkFilterDialog::reject()
{
    QDialog::reject();
}

void LandmarkFilterDialog::filterAllCheckBoxStateChanged(int state)
{
    if (state == Qt::Checked) {
        filterNameCheckBox->setChecked(false);
        filterCategoryCheckBox->setChecked(false);
        filterBoxCheckBox->setChecked(false);
        filterProximityCheckBox->setChecked(false);
    } else {
        if (!filterNameCheckBox->isChecked()
            && !filterCategoryCheckBox->isChecked()
            && !filterBoxCheckBox->isChecked()
            && !filterProximityCheckBox->isChecked()) {
            filterAllCheckBox->setCheckState(Qt::Checked);
        }
    }
}

void LandmarkFilterDialog::otherFiltersCheckBoxStateChanged(int state)
{
    if (state == Qt::Checked) {
        if (sender() == this->filterCategoryCheckBox) {
            if (this->categoryComboBox->count() == 0) {
                filterCategoryCheckBox->setCheckState(Qt::Unchecked);
                return;
            }
        }
        filterAllCheckBox->setCheckState(Qt::Unchecked);
    } else {
        if (!filterNameCheckBox->isChecked()
            && !filterCategoryCheckBox->isChecked()
            && !filterBoxCheckBox->isChecked()
            && !filterProximityCheckBox->isChecked()) {
            filterAllCheckBox->setCheckState(Qt::Checked);
        }
    }
}

void LandmarkFilterDialog::showEvent(QShowEvent *showEvent)
{

    QList<QLandmarkCategory> categories = manager->categories();
    categoryComboBox->clear();

    for (int i=0; i < categories.count(); ++i) {
        QLandmarkCategory category = categories.at(i);
        QVariant var;
        var.setValue(category.categoryId());
        categoryComboBox->addItem(category.name(),var);

        if (oldCategoryId.isValid() && oldCategoryId == category.categoryId()) {
            categoryComboBox->setCurrentIndex(i);
        }
    }

    QDialog::showEvent(showEvent);
}


void LandmarkFilterDialog::categoryRemoved(const QList<QLandmarkCategoryId> &categoryIds)
{
    if (categoryIds.contains(oldCategoryId)) {
        filterAllCheckBox->setCheckState(Qt::Checked);
        setupFetchRequest();
        emit doFetchAll();
    }
}

bool LandmarkFilterDialog::setupFetchRequest()
{
    if (filterAllCheckBox->checkState() == Qt::Checked) {
        fetchRequest->setFilter(QLandmarkFilter());
        fetchRequest->setSorting(QLandmarkNameSort());
    } else {
        QLandmarkIntersectionFilter filter;
        QLandmarkSortOrder sortOrder = QLandmarkNameSort();

        if (filterNameCheckBox->checkState() == Qt::Checked) {
            QLandmarkNameFilter nameFilter;
            nameFilter.setName(nameLineEdit->text());
            if (startsWithRadioButton->isChecked())
                nameFilter.setMatchFlags(QLandmarkFilter::MatchStartsWith);
            else
                nameFilter.setMatchFlags(QLandmarkFilter::MatchContains);
            filter.append(nameFilter);
        }

        if (filterCategoryCheckBox->checkState() == Qt::Checked) {
            QLandmarkCategoryFilter categoryFilter;
            if (categoryComboBox->count() > 0) {
                oldCategoryId = categoryComboBox->itemData(categoryComboBox->currentIndex())
                            .value<QLandmarkCategoryId>();
                categoryFilter.setCategoryId(oldCategoryId);
            }
            filter.append(categoryFilter);
        }

        if (filterBoxCheckBox->checkState() == Qt::Checked) {

            if (!isValidLatitude(boxTopLeftLatitudeLineEdit->text())) {
                QMessageBox::warning(this,"Warning", "Top left latitude is invalid.  Must be between -90 and 90 (not inclusive)", QMessageBox::Ok, QMessageBox::NoButton);
                return false;
            }
            else if (!isValidLongitude(boxTopLeftLongitudeLineEdit->text())) {
                QMessageBox::warning(this,"Warning", "Top left longitude is invalid. Must be between -180 and 180 (inclusive) ", QMessageBox::Ok, QMessageBox::NoButton);
                return false;
            }
            else if (!isValidLatitude(boxBottomRightLatitudeLineEdit->text())) {
                QMessageBox::warning(this,"Warning", "Bottom right latitude is invalid. (Must be between -90 and 90 (not inclusive)", QMessageBox::Ok, QMessageBox::NoButton);
                return false;
            }
            else if (!isValidLongitude(boxBottomRightLongitudeLineEdit->text())) {
                QMessageBox::warning(this,"Warning", "Bottom right longitude is invalid. (Must be between -180 and 180 (inclusive)", QMessageBox::Ok, QMessageBox::NoButton);
                return false;
            }

            QGeoCoordinate topLeft;
            topLeft.setLatitude(boxTopLeftLatitudeLineEdit->text().toDouble());
            topLeft.setLongitude(boxTopLeftLongitudeLineEdit->text().toDouble());
            QGeoCoordinate bottomRight;
            bottomRight.setLatitude(boxBottomRightLatitudeLineEdit->text().toDouble());
            bottomRight.setLongitude(boxBottomRightLongitudeLineEdit->text().toDouble());

            QGeoBoundingBox box;
            box.setTopLeft(topLeft);
            box.setBottomRight(bottomRight);
            QLandmarkBoxFilter boxFilter;
            boxFilter.setBoundingBox(box);
            filter.append(boxFilter);
        }

        if (filterProximityCheckBox->checkState() == Qt::Checked) {
            if (!isValidLatitude(proximityLatitudeLineEdit->text())) {
                QMessageBox::warning(this,"Warning", "Proximity latitude is invalid.  It must be between -90 and 90 (not inclusive)", QMessageBox::Ok, QMessageBox::NoButton);
                return false;
            } else if (!isValidLongitude(proximityLongitudeLineEdit->text())) {
                QMessageBox::warning(this,"Warning", "Proximity longitude is invalid. It must be between -180 and 180 (inclusive) ", QMessageBox::Ok, QMessageBox::NoButton);
                return false;
            }
            bool ok;
            qreal radius = radiusLineEdit->text().toDouble(&ok);
            if (!ok) {
                QMessageBox::warning(this,"Warning", "Proximity radius is invalid. It must be a number", QMessageBox::Ok, QMessageBox::NoButton);
                return false;
            }

            QLandmarkProximityFilter proximityFilter;
            QGeoCoordinate center;
            center.setLatitude(proximityLatitudeLineEdit->text().toDouble());
            center.setLongitude(proximityLongitudeLineEdit->text().toDouble());
            proximityFilter.setCenter(center);
            proximityFilter.setRadius(radiusLineEdit->text().toDouble());
            filter.append(proximityFilter);
            sortOrder = QLandmarkSortOrder();
        }

        fetchRequest->setFilter(filter);
        fetchRequest->setSorting(sortOrder);
    }
    return true;
}

bool LandmarkFilterDialog::isValidLatitude(const QString &latitude)
{
    bool ok;
    double lat = latitude.toDouble(&ok);
    if (!ok)
        return false;

    if (-90.0 < lat && lat < 90.0)
        return true;
    else
        return false;
}

bool LandmarkFilterDialog::isValidLongitude(const QString &longitude)
{
    bool ok;
    double lon = longitude.toDouble(&ok);
    if (!ok)
        return false;

    if (-180.0 <= lon && lon <= 180.0)
        return true;
    else
        return false;
}
