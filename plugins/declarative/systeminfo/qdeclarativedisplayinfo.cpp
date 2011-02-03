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
#include "qdeclarativedisplayinfo_p.h"
#include "qsystemdisplayinfo.h"

QT_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QSystemDisplayInfo, displayInfo)

QDeclarativeDisplayInfo::QDeclarativeDisplayInfo(QObject *parent) :
    QSystemDisplayInfo(parent)
{
}

int QDeclarativeDisplayInfo::screen()
{
    return currentScreen;
}

void QDeclarativeDisplayInfo::setCurrentScreen(int screen)
{
    currentScreen = screen;
}

int QDeclarativeDisplayInfo::displayBrightness()
{
    return displayInfo()->displayBrightness(currentScreen);
}

int QDeclarativeDisplayInfo::colorDepth()
{
    return displayInfo()->colorDepth(currentScreen);
}

QSystemDisplayInfo::DisplayOrientation QDeclarativeDisplayInfo::orientation()
{
    return displayInfo()->orientation(currentScreen);
}

float QDeclarativeDisplayInfo::contrast()
{
    return displayInfo()->contrast(currentScreen);
}

int QDeclarativeDisplayInfo::dpiWidth()
{
    return displayInfo()->getDPIWidth(currentScreen);
}

int QDeclarativeDisplayInfo::dpiHeight()
{
    return displayInfo()->getDPIHeight(currentScreen);
}

int QDeclarativeDisplayInfo::physicalHeight()
{
    return displayInfo()->physicalHeight(currentScreen);
}

int QDeclarativeDisplayInfo::physicalWidth()
{
    return displayInfo()->physicalWidth(currentScreen);
}
