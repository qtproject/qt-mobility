/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "maemo6als.h"

const char *maemo6als::id("maemo6.als");
bool maemo6als::m_initDone = false;

maemo6als::maemo6als(QSensor *sensor)
    : maemo6sensorbase(sensor)
{
    setReading<QAmbientLightReading>(&m_reading);
    if (!m_initDone) {
        initSensor<ALSSensorChannelInterface>("alssensor");

        if (m_sensorInterface)
            QObject::connect(static_cast<const ALSSensorChannelInterface*>(m_sensorInterface), SIGNAL(ALSChanged(const int&)), this, SLOT(slotDataAvailable(const int&)));
        else
            qWarning() << "Unable to initialize ambient light sensor.";

        // metadata
        addOutputRange(0, 5, 1);
        setDescription(QLatin1String("Ambient light intensity given as 5 pre-defined levels"));

        m_initDone = true;
    }
}

void maemo6als::slotDataAvailable(const int& data)
{
    // Convert from integer to fixed levels
    // TODO: verify levels
    QAmbientLightReading::LightLevel level;
    if (data < 0) {
        level = QAmbientLightReading::Undefined;
    } else if (data < 10) {
        level = QAmbientLightReading::Dark;
    } else if (data < 50) {
        level = QAmbientLightReading::Twilight;
    } else if (data < 100) {
        level = QAmbientLightReading::Light;
    } else if (data < 150) {
        level = QAmbientLightReading::Bright;
    } else {
        level = QAmbientLightReading::Sunny;
    }

    if (level != m_reading.lightLevel()) {
        m_reading.setLightLevel(level);
        //m_reading.setTimestamp(data.timestamp());
        m_reading.setTimestamp(createTimestamp()); //TODO: use correct timestamp
        newReadingAvailable();
    }
}
