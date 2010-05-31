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

#ifndef MAPTILETAB_H_
#define MAPTILETAB_H_

#include <QWidget>
#include <qgeomappingmanager.h>

class QLabel;
class QLineEdit;

QTM_USE_NAMESPACE

class MapTileTab: public QWidget
{
    Q_OBJECT

public:
    MapTileTab(QWidget *parent = 0);
    ~MapTileTab();

public slots:
    void initialize(QGeoMappingManager *mapManager);

private slots:
    void on_btnRequest_clicked();
    void replyFinished(QGeoMapReply* reply);
    void resultsError(QGeoMapReply* reply, QGeoMapReply::Error error, QString errorString);

private:
    QGeoMappingManager *m_mapManager;
    QGeoMapViewport *m_viewport;
    QLineEdit *m_tileLong;
    QLineEdit *m_tileLat;
    QLineEdit *m_tileZoomLevel;
    QLabel *m_result;
};
#endif /* ROUTETAB_H_ */
