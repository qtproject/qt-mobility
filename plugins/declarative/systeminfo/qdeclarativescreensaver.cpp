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


#include "qdeclarativescreensaver_p.h"

QT_BEGIN_NAMESPACE

/*!
    \qmlclass ScreenSaver QDeclarativeScreenSaver

    \brief The ScreenSaver element allows you to temporarily suppress the screensaver
        from turning on or blanking the screen.

    \inherits QObject

    \ingroup qml-systeminfo

    This element is part of the \bold{QtMobility.systeminfo 1.1} module.
    It is a convience class to make QML usage easier.

*/

QDeclarativeScreenSaver::QDeclarativeScreenSaver(QObject *parent) :
    QObject(parent),screensaverInfo(0),screenSaverDelay(0)
{
}

QDeclarativeScreenSaver::~QDeclarativeScreenSaver()
{
}

void QDeclarativeScreenSaver::setScreenSaverDelayed(bool on)
{
    if(on && !screenSaverDelay) {
        screensaverInfo = new QSystemScreenSaver(this);
        screenSaverDelay = true;
    } else if(screenSaverDelay) {
        delete screensaverInfo;
        screenSaverDelay = false;
    }
}

/*!
    \qmlproperty string ScreenSaver::screenSaverDelayed

    Returns whether the screensaver has been supressed, or not.
*/

bool QDeclarativeScreenSaver::screenSaverDelayed()
{
    return screenSaverDelay;
}

