/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qdeclarativedisplayinfo_p.h"
#include "qsystemdisplayinfo.h"

QT_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QSystemDisplayInfo, displayInfo)


/*!
    \qmlclass DisplayInfo QDeclarativeDisplayInfo
    \brief The DisplayInfo element allows you to get information and
     receive notifications about the diplsay.
    \inherits QObject

    \ingroup qml-systeminfo
    \since Mobility 1.2

    This element is part of the \bold{QtMobility.systeminfo 1.2} module.
    It is a convience class to make QML usage easier.

    This class is currently only implemented on Meego.

\sa QSystemDisplayInfo
*/

/*!
    \qmlsignal DisplayInfo::orientationChanged()
    \since Mobility 1.2

    This handler is called when gui orientation has changed.
*/

/*!
  Constructs the DisplayInfo
  \since Mobility 1.2
  */
QDeclarativeDisplayInfo::QDeclarativeDisplayInfo(QObject *parent) :
    QSystemDisplayInfo(parent),currentScreen(0)
{
}

/*!
    \qmlproperty int DisplayInfo::screen
    \since Mobility 1.2

    Contains the display number of this screen.
*/
int QDeclarativeDisplayInfo::screen()
{
    return currentScreen;
}

/*!
   This function sets this DisplayInfo to use screen \a screen.

    If not set, the default is the default screen, or display 0.
    \since Mobility 1.2
*/
void QDeclarativeDisplayInfo::setCurrentScreen(int screen)
{
    currentScreen = screen;
}

/*!
    \qmlproperty int DisplayInfo::displayBrightness
   Returns the brightness of the screen.
   \since Mobility 1.2
*/
int QDeclarativeDisplayInfo::displayBrightness()
{
    return displayInfo()->displayBrightness(currentScreen);
}

/*!
    \qmlproperty int DisplayInfo::colorDepth
   Returns the color depth of the screen, in bits per pixel.
   \since Mobility 1.2
*/
int QDeclarativeDisplayInfo::colorDepth()
{
    return displayInfo()->colorDepth(currentScreen);
}

/*!
    \qmlproperty QSystemDisplayInfo::DisplayOrientation DisplayInfo::orientation
    Returns the QSystemDisplayInfo::DisplayOrientation of the UI QDesktopWidget for the screen.
    \since Mobility 1.2
*/
QSystemDisplayInfo::DisplayOrientation QDeclarativeDisplayInfo::orientation()
{
    return displayInfo()->orientation(currentScreen);
}

/*!
    \qmlproperty float DisplayInfo::contrast
   Returns the contrast of the screen, from 0 to 1.
   \since Mobility 1.2
*/
float QDeclarativeDisplayInfo::contrast()
{
    return displayInfo()->contrast(currentScreen);
}

/*!
    \qmlproperty int DisplayInfo::dpiWidth
    Returns the current dots per inch (DPI) for the width.
    \since Mobility 1.2

  */
int QDeclarativeDisplayInfo::dpiWidth()
{
    return displayInfo()->getDPIWidth(currentScreen);
}

/*!
    \qmlproperty int DisplayInfo::dpiHeight
    Returns the current dots per inch (DPI) for the width.
    \since Mobility 1.2

  */
int QDeclarativeDisplayInfo::dpiHeight()
{
    return displayInfo()->getDPIHeight(currentScreen);
}

/*!
    \qmlproperty int DisplayInfo::physicalHeight
   Returns the physical height of the screen in millimeters.
   \since Mobility 1.2
   */
int QDeclarativeDisplayInfo::physicalHeight()
{
    return displayInfo()->physicalHeight(currentScreen);
}

/*!
    \qmlproperty int DisplayInfo::physicalWidth
   Returns the physical width of the screen in millimeters.
   \since Mobility 1.2
   */
int QDeclarativeDisplayInfo::physicalWidth()
{
    return displayInfo()->physicalWidth(currentScreen);
}

/*!
    \qmlproperty QSystemDisplayInfo::BacklightState  DisplayInfo::backlightStatus
    Returns whether the QSystemDisplayInfo::BacklightState for the screen
    \since Mobility 1.2
    */
QSystemDisplayInfo::BacklightState QDeclarativeDisplayInfo::backlightStatus()
{
    return displayInfo()->backlightStatus(currentScreen);
}

