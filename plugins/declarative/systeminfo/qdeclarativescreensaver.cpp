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


#include "qdeclarativescreensaver_p.h"
#include <QDebug>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass ScreenSaver QDeclarativeScreenSaver

    \brief The ScreenSaver element allows you to temporarily suppress and delay the screensaver
        from turning on or blanking the screen.

    \inherits QObject

    \ingroup qml-systeminfo
    \since Mobility 1.1

    This element is part of the \bold{QtMobility.systeminfo 1.1} module.
    It is a convience class to make QML usage easier.

*/

QDeclarativeScreenSaver::QDeclarativeScreenSaver(QObject *parent) :
    QObject(parent),screensaverInfo(0),screenSaverDelay(0)
{
}

/*!
  Destroys the QDeclarativeScreenSaver object.
  \since Mobility 1.1
  */

QDeclarativeScreenSaver::~QDeclarativeScreenSaver()
{
    if(screenSaverDelay) {
        delete screensaverInfo;
        screenSaverDelay = false;
    }
}

/*!
    \qmlproperty bool ScreenSaver::screenSaverDelayed
    \since Mobility 1.1

    Delays the screensaver if true, otherwise continues the screensaver.
*/

void QDeclarativeScreenSaver::setScreenSaverDelayed(bool on)
{
    if(on && !screenSaverDelay) {
        screensaverInfo = new QSystemScreenSaver(this);
        screensaverInfo->setScreenSaverInhibit();
        screenSaverDelay = screensaverInfo->screenSaverInhibited();
    } else if(screenSaverDelay) {
        delete screensaverInfo;
        screenSaverDelay = false;
    }
}

bool QDeclarativeScreenSaver::screenSaverDelayed()
{
    return screenSaverDelay;
}

/*!
   \qmlmethod bool ScreenSaver::setScreenSaverInhibit()

   \since Mobility 1.1

   Set the screensaver to be inhibited.

   Temporarily inhibits the screensaver.

   The screensaver will be set to a non inhibited state only when this QSystemScreenSaver object gets destroyed.

   This is a non blocking function that will return true if the inhibit procedure was successful, otherwise false.


    On platforms that support it, if screensaver is secure by policy, the policy will be honored
    and this will fail.
*/
bool QDeclarativeScreenSaver::setScreenSaverInhibit()
{
    setScreenSaverDelayed(true);
    return screenSaverDelay;
}


/*!
    \qmlproperty bool ScreenSaver::screenSaverInhibited
    \since Mobility 1.1

    Inhibits the screensaver if true, otherwise continues the screensaver.
*/

bool  QDeclarativeScreenSaver::screenSaverInhibited()
{
    return screenSaverDelay;
}

void QDeclarativeScreenSaver::setScreenSaverInhibited(bool on)
{
    setScreenSaverDelayed(on);
}
