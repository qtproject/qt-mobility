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

#ifndef MEEGOIRPROXIMITYSENSOR_H
#define MEEGOIRPROXIMITYSENSOR_H

#include "meegosensorbase.h"
#include <qirproximitysensor.h>
#include <proximitysensor_i.h>


QTM_USE_NAMESPACE

class meegoirproximitysensor : public meegosensorbase
{
    Q_OBJECT

public:
    static char const * const id;

    meegoirproximitysensor(QSensor *sensor);
protected:
    virtual bool doConnect();
    virtual QString sensorName() const;

private:
    QIRProximityReading m_reading;
    static bool m_initDone;
    int rangeMax;

private slots:
#ifdef Q_WS_MAEMO_6
    void slotDataAvailable(const Proximity& proximity);
#endif
};

#endif
