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
#ifndef QDECLARATIVEDISPLAYINFO_P_H
#define QDECLARATIVEDISPLAYINFO_P_H

#include "qsystemdisplayinfo.h"

QT_BEGIN_HEADER
QTM_USE_NAMESPACE

class QDeclarativeDisplayInfo : public QSystemDisplayInfo
{
    Q_OBJECT
    Q_PROPERTY(int screen READ screen WRITE setCurrentScreen)
    Q_PROPERTY(int displayBrightness READ displayBrightness)
    Q_PROPERTY(int colorDepth READ colorDepth)
    Q_PROPERTY(QSystemDisplayInfo::DisplayOrientation orientation READ orientation)
    Q_PROPERTY(float contrast READ contrast)
    Q_PROPERTY(int dpiWidth READ dpiWidth)
    Q_PROPERTY(int dpiHeight READ dpiHeight)
    Q_PROPERTY(int physicalWidth READ physicalWidth)
    Q_PROPERTY(int physicalHeight READ physicalHeight)
    Q_PROPERTY(QSystemDisplayInfo::BacklightState backlightStatus READ backlightStatus)

public:
    explicit QDeclarativeDisplayInfo(QObject *parent = 0);
    int screen();
    void setCurrentScreen(int screen);

    int displayBrightness();
    int colorDepth();

    QSystemDisplayInfo::DisplayOrientation orientation();
    float contrast();
    int dpiWidth();
    int dpiHeight();
    int physicalHeight();
    int physicalWidth();

    QSystemDisplayInfo::BacklightState backlightStatus();

signals:
    void orientationChanged();

public slots:
private:
    int currentScreen;
};
QT_END_NAMESPACE
QT_END_HEADER
#endif // QDECLARATIVEDISPLAYINFO_P_H
