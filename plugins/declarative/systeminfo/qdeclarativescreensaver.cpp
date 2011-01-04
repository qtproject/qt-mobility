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
#include <QDebug>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass ScreenSaver QDeclarativeScreenSaver

    \brief The ScreenSaver element allows you to temporarily suppress and delay the screensaver
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

/*!
  Destroys the QDeclarativeScreenSaver object.
 */

QDeclarativeScreenSaver::~QDeclarativeScreenSaver()
{
    if(screenSaverDelay) {
        delete screensaverInfo;
        screenSaverDelay = false;
    }
}

/*!
    \qmlproperty void ScreenSaver::setScreenSaverDelayed(bool)

    Delays the screensaver if \a on is true, otherwise continues the screensaver.
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

/*!
    \qmlproperty bool ScreenSaver::screenSaverDelayed

    Returns whether the screensaver has been suppressed, or not.
*/

bool QDeclarativeScreenSaver::screenSaverDelayed()
{
    return screenSaverDelay;
}

/*!
  \brief Set the screensaver to be inhibited.

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
  \property QSystemScreenSaver::screenSaverInhibited
  \brief Screensaver inhibited.

   Returns true if the screensaver is inhibited, otherwise false.
*/

bool  QDeclarativeScreenSaver::screenSaverInhibited()
{
    return screenSaverDelay;
}
