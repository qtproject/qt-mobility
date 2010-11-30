/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qsystemdisplayinfo.h"
#include "qsysteminfocommon_p.h"
#include <QDesktopWidget>

QTM_BEGIN_NAMESPACE
        Q_GLOBAL_STATIC(QSystemDisplayInfoPrivate, displayInfoPrivate)

#ifdef QT_SIMULATOR
QSystemDisplayInfoPrivate *getSystemDisplayInfoPrivate() { return displayInfoPrivate(); }
#endif

// display
/*!
    \enum QSystemDisplayInfo::DisplayOrientation
    This enum describes the orientation of the default window.

    \value Unknown                  Unknown orientation or error.
    \value Landscape                Landscape is wider than high.
    \value Portrait                 Portrait is higher than wide.
    \value InvertedLandscape        Landscape that is inverted.
    \value InvertedPortrait         Portrait that is inverted.
  */

/*!
  \enum QSystemDisplayInfo::BacklightState
  This enum describes the state of the Backlight.

  \value BacklightUnknown          Error, no, or unknown Backlight state.
  \value BacklightOff              Backlight is turned off.
  \value BacklightStateDimmed      Backlight has been dimmed.
  \value BacklightStateFull        Backlight is on full.
  */
 /*!
   \class QSystemDisplayInfo
   \ingroup systeminfo
   \inmodule QtSystemInfo

    \brief The QSystemDisplayInfo class provides access to display information from the system.
*/

/*!
   Constructs a QSystemDisplayInfo object with the given \a parent.
 */
QSystemDisplayInfo::QSystemDisplayInfo(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<QSystemDisplayInfo::DisplayOrientation>("QSystemDisplayInfo::DisplayOrientation");
    qRegisterMetaType<QSystemDisplayInfo::BacklightState>("QSystemDisplayInfo::BacklightState");
}

/*!
  Destroys the QSystemDisplayInfo object.
 */
QSystemDisplayInfo::~QSystemDisplayInfo()
{
}

/*!
    Returns the display brightness of the screen with index \a screenNumber in %, 1 - 100 scale.

    Depending on platform, displayBrightness may not be available due to
    differing hardware, software or driver implementation. In which case this
    will return -1.

    \sa QDesktopWidget::screenCount()
*/
int QSystemDisplayInfo::displayBrightness(int screenNumber)
{
    QDesktopWidget wid;
    if(wid.screenCount() < 1 || wid.screenCount() - 1 < screenNumber) {
        return -1;
    }
    return displayInfoPrivate()->displayBrightness(screenNumber);
}

/*!
    Returns the color depth of the screen with the index \a screenNumber, in bits per pixel. Will return -1 in
    the event of an error.

    \sa QDesktopWidget::screenCount()
*/
int QSystemDisplayInfo::colorDepth(int screenNumber)
{
    QDesktopWidget wid;
    if(wid.screenCount() < 1 || wid.screenCount() - 1 < screenNumber) {
        return -1;
    }
    return displayInfoPrivate()->colorDepth(screenNumber);
}

/*!
    Returns the orientation of the \a screen.

    \sa QDesktopWidget::screenCount()
*/
QSystemDisplayInfo::DisplayOrientation QSystemDisplayInfo::getOrientation(int screen)
{
    return displayInfoPrivate()->getOrientation(screen);
}


/*!
    Returns the current contrast of the screen \a screen, from 0 to 1.

    \sa QDesktopWidget::screenCount()
*/
float QSystemDisplayInfo::contrast(int screen)
{
        return displayInfoPrivate()->contrast(screen);
}

/*!
    Returns the current dots per inch (DPI) for the width of \a screen.

    \sa QDesktopWidget::screenCount()
*/
int QSystemDisplayInfo::getDPIWidth(int screen)
{
        return displayInfoPrivate()->getDPIWidth(screen);
}

/*!
    Returns the dpi (Dot Per Inch) of the height os \a screen.

    \sa QDesktopWidget::screenCount()
*/
int QSystemDisplayInfo::getDPIHeight(int screen)
{
        return displayInfoPrivate()->getDPIHeight(screen);
}

/*!
    Returns the physical height of the \a screen in millimeters.
    \sa QDesktopWidget::screenCount()
*/
int QSystemDisplayInfo::physicalHeight(int screen)
{
        return displayInfoPrivate()->physicalHeight(screen);
}

/*!
    Returns the physical width of \a screen in millimeters.
    \sa QDesktopWidget::screenCount()
*/
int QSystemDisplayInfo::physicalWidth(int screen)
{
        return displayInfoPrivate()->physicalWidth(screen);
}

/*!
    Returns whether the QSystemDisplayInfo::BacklightState for the screen \a screen.
*/
QSystemDisplayInfo::BacklightState QSystemDisplayInfo::backlightStatus(int screen)
{
    return displayInfoPrivate()->backlightStatus(screen);
}



#include "moc_qsystemdisplayinfo.cpp"

QTM_END_NAMESPACE
