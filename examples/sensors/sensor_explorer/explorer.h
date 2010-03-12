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

#ifndef EXPLORER_H
#define EXPLORER_H

#include <QMainWindow>
#include <ui_explorer.h>
#include <qsensor.h>

QTM_USE_NAMESPACE

class Explorer : public QMainWindow, public QSensorFilter
{
    Q_OBJECT
public:
    Explorer(QWidget *parent = 0);
    ~Explorer();

    bool filter(QSensorReading *reading);

private slots:
    void loadSensors();
    void on_sensors_currentItemChanged();
    void on_sensorprops_itemChanged(QTableWidgetItem *item);
    void on_start_clicked();
    void on_stop_clicked();
    void on_poll_clicked();
    void sensor_changed();
    void adjustSizes();
    void loadSensorProperties();

private:
    void showEvent(QShowEvent *event);
    void resizeEvent(QResizeEvent *event);

    void clearReading();
    void loadReading();
    void clearSensorProperties();
    void adjustTableColumns(QTableWidget *table);
    void resizeSensors();

    Ui::Explorer ui;
    QSensor *m_sensor;
    bool ignoreItemChanged;
};

#endif

