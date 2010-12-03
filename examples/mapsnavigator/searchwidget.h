/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include "markerlist.h"

#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>

#include <qgeosearchmanager.h>
#include <qgeoplace.h>
#include <qgeoaddress.h>
#include <qgeocoordinate.h>

QTM_USE_NAMESPACE

class SearchGeocodeDialog: public QDialog
{
    Q_OBJECT
public:
    SearchGeocodeDialog(QString& address, QWidget *parent = 0);

private slots:
    void accept();

private:
    QLineEdit *m_search;
    QString& m_address;
};


class SearchRevGeocodeDialog: public QDialog
{
    Q_OBJECT
public:
    SearchRevGeocodeDialog(QGeoCoordinate& coord, QWidget *parent = 0);

private slots:
    void accept();

private:
    QLineEdit *m_longitude;
    QLineEdit *m_latitude;
    QGeoCoordinate& m_coord;
};


class SearchWidget : public QDialog
{
    Q_OBJECT
public:
    SearchWidget(QWidget *parent = 0);
    ~SearchWidget();

signals:
    void addMarker(const QGeoCoordinate &coord);
    void addMarker(const QString &coord);
    void deleteMarker(MarkerObject *marker);
    void centerOnMarker(MarkerObject *marker);
    void setWaypoint(MarkerObject *marker);

public slots:
    void initialize(QGeoSearchManager *manager, MarkerObject *myLocation);
    void setupWidget(MarkerObject *maker);
    void showGeocodeDialog();
    void showRevGeocodeDialog();

private slots:
    void clearWidget();
    void updateWidget();
    void errorWidget();
    void deleteMarker();
    void setWaypoint();

private:
    QGeoSearchManager *m_searchManager;
    QGeoSearchReply *m_searchReply;
    MarkerObject* m_marker;

    QLabel *m_name;
    QLabel *m_street;
    QLabel *m_city;
    QLabel *m_distance;
    QLabel *m_latitude;
    QLabel *m_longitude;

    QPushButton *deleteButton;
    QPushButton *waypointButton;
    QPushButton *okButton;

    QGeoCoordinate myLocation;
};

#endif // SEARCHWIDGET_H
