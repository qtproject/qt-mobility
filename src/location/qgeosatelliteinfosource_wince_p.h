/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGEOSATELLITEINFOSOURCE_WINCE_P_H
#define QGEOSATELLITEINFOSOURCE_WINCE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qgeosatelliteinfosource.h>

#include "qgeoinfothread_wince_p.h"

QTM_BEGIN_NAMESPACE

class QGeoSatelliteInfoValidator : public QGeoInfoValidator
{
public:
    QGeoSatelliteInfoValidator();
    ~QGeoSatelliteInfoValidator();

    bool valid(const GPS_POSITION &data) const;
};

class QGeoSatelliteInfoSourceWinCE : public QGeoSatelliteInfoSource
{
    Q_OBJECT

public:
    enum {
        // The interval at which the periodic updates will occur.
        DefaultUpdateInterval = 5000
    };

    explicit QGeoSatelliteInfoSourceWinCE(QObject *parent = 0);
    ~QGeoSatelliteInfoSourceWinCE();

public slots:
    void startUpdates();
    void stopUpdates();
    void requestUpdate(int timeout = 0);

private slots:
    void dataUpdated(GPS_POSITION data);

private:
    QGeoInfoThreadWinCE *infoThread;
};

QTM_END_NAMESPACE

#endif //#ifndef QGEOSATELLITEINFOSOURCE_WINCE_P_H
