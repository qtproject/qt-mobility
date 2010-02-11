/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "explorer.h"
#include <QTimer>
#include <QDebug>
#include <qsensor.h>
#include <QMetaObject>
#include <QMetaProperty>

QTM_USE_NAMESPACE

Explorer::Explorer(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi(this);
    // Clear out example data from the .ui file
    m_sensors->clear();
    clearReading();
    // We need to know when this widget resizes
    // so we can resize the columns
    m_reading->installEventFilter(this);
}

Explorer::~Explorer()
{
}

void Explorer::loadSensors()
{
    qDebug() << "Explorer::loadSensors";

    // Clear out anything that's in there now
    m_sensors->clear();

    foreach (const QByteArray &type, QSensor::sensorTypes()) {
        qDebug() << "Found type" << type;
        // The type item. We don't add it yet because there may not be any sensors of this type
        // and we only show types that have sensors available.
        QTreeWidgetItem *typeItem = new QTreeWidgetItem(QStringList() << QString::fromLatin1(type));
        bool added = false;
        foreach (const QByteArray &identifier, QSensor::sensorsForType(type)) {
            qDebug() << "Found identifier" << identifier;
            // Don't put in sensors we can't connect to
            QSensor sensor;
            sensor.setType(type);
            sensor.setIdentifier(identifier);
            if (!sensor.connect()) {
                qDebug() << "Couldn't connect to" << identifier;
                continue;
            }

            // Since we're adding a sensor we can go ahead and add the type (unless it has
            // already been added).
            if (!added) {
                qDebug() << "Adding type" << type;
                m_sensors->addTopLevelItem(typeItem);
                added = true;
            }
            qDebug() << "Adding identifier" << identifier;
            (void)new QTreeWidgetItem(typeItem, QStringList() << QString::fromLatin1(identifier));
        }
        // Cleanup if we didn't add the type
        if (!added) {
            qDebug() << "Didn't add type" << type;
            delete typeItem;
        }
    }

    // Don't hide stuff
    m_sensors->expandAll();
}

void Explorer::clearReading()
{
    m_reading->setRowCount(0);
}

void Explorer::loadReading()
{
    qDebug() << "Explorer::loadReading";

    // Clear out anything that's in there now
    clearReading();

    // Check that we've selected an item
    QTreeWidgetItem *item = m_sensors->currentItem();
    if (!item) {
        qWarning() << "Didn't select an item!";
        return;
    }

    // Check that we've selected a sensor (which has a parent)
    QTreeWidgetItem *parent = item->parent();
    if (!parent) {
        qWarning() << "Didn't select a sensor!";
        return;
    }

    QByteArray type = parent->data(0, Qt::DisplayRole).toString().toLatin1();
    QByteArray identifier = item->data(0, Qt::DisplayRole).toString().toLatin1();

    // Connect to the sensor so we can probe it
    QSensor sensor;
    sensor.setType(type);
    sensor.setIdentifier(identifier);
    if (!sensor.connect()) {
        qWarning() << "Can't connect to the sensor!";
        return;
    }

    // Probe the reading using Qt's meta-object facilities
    QSensorReading *reading = sensor.reading();
    const QMetaObject *mo = reading->metaObject();
    int firstProperty = QSensorReading::staticMetaObject.propertyOffset();

    m_reading->setRowCount(mo->propertyCount() - firstProperty);

    for(int i = firstProperty; i < mo->propertyCount(); ++i) {
        int row = i - firstProperty;
        QTableWidgetItem *index;
        if (row == 0)
            // timestamp is not available via index
            index = new QTableWidgetItem(QLatin1String("N/A"));
        else
            index = new QTableWidgetItem(QVariant(row - 1).toString());
        QTableWidgetItem *prop = new QTableWidgetItem(mo->property(i).name());
        QTableWidgetItem *type = new QTableWidgetItem(mo->property(i).typeName());
        m_reading->setItem(row, 0, index);
        m_reading->setItem(row, 1, prop);
        m_reading->setItem(row, 2, type);
    }

    adjustReadingColumns();
}

void Explorer::showEvent(QShowEvent *event)
{
    // Once we're visible, load the sensors
    // (don't delay showing the UI while we load plugins, connect to backends, etc.)
    QTimer::singleShot(0, this, SLOT(loadSensors()));

    QMainWindow::showEvent(event);
}

// Resize columns to fit the space.
// This shouldn't be so hard!
void Explorer::adjustReadingColumns()
{
    // At least this is easy to do
    m_reading->resizeColumnsToContents();

    int indexWidth = m_reading->columnWidth(0);
    int propWidth = m_reading->columnWidth(1);
    int typeWidth = m_reading->columnWidth(2);

    int suggestedWidth = indexWidth + propWidth + typeWidth;
    int actualWidth = m_reading->size().width();
    //qDebug() << "suggestedWidth" << suggestedWidth << "actualWidth" << actualWidth;

    // We only scale the columns up, we don't scale down
    if (actualWidth <= suggestedWidth)
        return;

    qreal multiplier = actualWidth / static_cast<qreal>(suggestedWidth);
    indexWidth = multiplier * indexWidth;
    propWidth = multiplier * propWidth;
    typeWidth = multiplier * typeWidth;

    // It ends up too big due to cell decorations or something.
    // Make things smaller one pixel at a time in round robin fashion until we're good.
    int currentSpace = indexWidth + propWidth + typeWidth + 4;
    while (actualWidth < currentSpace) {
        if (actualWidth < currentSpace) {
            --indexWidth;
            --currentSpace;
        }
        if (actualWidth < currentSpace) {
            --propWidth;
            --currentSpace;
        }
        if (actualWidth < currentSpace) {
            --typeWidth;
            --currentSpace;
        }
    }

    m_reading->setColumnWidth(0, indexWidth);
    m_reading->setColumnWidth(1, propWidth);
    m_reading->setColumnWidth(2, typeWidth);
}

bool Explorer::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_reading && event->type() == QEvent::Resize) {
        // If the table resizes, adjust the column sizes automatically
        adjustReadingColumns();
    }

    return QMainWindow::eventFilter(obj, event);
}

