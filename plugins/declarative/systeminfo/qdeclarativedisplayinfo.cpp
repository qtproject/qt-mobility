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


/*!
    \qmlclass NetworkInfo QDeclarativeDisplayInfo
    \brief The DisplayInfo element allows you to get information and
     receive notifications about the diplsay.
    \inherits QObject

    \ingroup qml-systeminfo

    This element is part of the \bold{QtMobility.systeminfo 1.2} module.
    It is a convience class to make QML usage easier.

    \endqml

\sa QSystemDisplayInfo
*/

/*!
    \qmlsignal DisplayInfo::orientationChanged()

    This handler is called when gui orientation has changed.
*/

/*!
  Constructs the DeviceInfo
  */
QDeclarativeDisplayInfo::QDeclarativeDisplayInfo(QObject *parent) :
    QSystemDisplayInfo(parent),currentScreen(0)
{
}

/*!
    \qmlproperty int DeviceInfo::screen

    Contains the display number of this screen.
*/
int QDeclarativeDisplayInfo::screen()
{
    return currentScreen;
}

/*!
   This function sets this DisplayInfo to use screen \a screen.

    If not set, the default is the default screen, or display 0.
*/
void QDeclarativeDisplayInfo::setCurrentScreen(int screen)
{
    currentScreen = screen;
}

/*!
    \qmlproperty int DisplayInfo::displayBrightness
   Returns the brightness of the screen.
  */
int QDeclarativeDisplayInfo::displayBrightness()
{
    return displayInfo()->displayBrightness(currentScreen);
}

/*!
    \qmlproperty int DisplayInfo::colorDepth
   Returns the color depth of the screen, in bits per pixel.
  */
int QDeclarativeDisplayInfo::colorDepth()
{
    return displayInfo()->colorDepth(currentScreen);
}

/*!
    \qmlproperty QSystemDisplayInfo::DisplayOrientation DisplayInfo::orientation
    Returns the QSystemDisplayInfo::DisplayOrientation of the UI QDesktopWidget for the screen.
  */
QSystemDisplayInfo::DisplayOrientation QDeclarativeDisplayInfo::orientation()
{
    return displayInfo()->orientation(currentScreen);
}

/*!
    \qmlproperty float DisplayInfo::contrast
   Returns the contrast of the screen, from 0 to 1.
  */
float QDeclarativeDisplayInfo::contrast()
{
    return displayInfo()->contrast(currentScreen);
}

/*!
    \qmlproperty int DisplayInfo::dpiWidth
   Returns the DPI width of the screen in millimeters.
  */
int QDeclarativeDisplayInfo::dpiWidth()
{
    return displayInfo()->getDPIWidth(currentScreen);
}

/*!
    \qmlproperty int DisplayInfo::dpiHeight
   Returns the DPI height of the screen in millimeters.
  */
int QDeclarativeDisplayInfo::dpiHeight()
{
    return displayInfo()->getDPIHeight(currentScreen);
}

/*!
    \qmlproperty int DisplayInfo::physicalHeight
   Returns the physical height of the screen in millimeters.
  */
int QDeclarativeDisplayInfo::physicalHeight()
{
    return displayInfo()->physicalHeight(currentScreen);
}

/*!
    \qmlproperty int DisplayInfo::physicalWidth
   Returns the physical width of the screen in millimeters.
  */
int QDeclarativeDisplayInfo::physicalWidth()
{
    return displayInfo()->physicalWidth(currentScreen);
}
