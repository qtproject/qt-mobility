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

#include "qsystemscreensaver.h"
#include "qsysteminfocommon_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QSystemScreenSaver
    \ingroup systeminfo
    \inmodule QtSystemInfo
    \brief The QSystemScreenSaver class provides access to screen saver and blanking.
    \since 1.0

    On platforms where there is no one default screensaver mechanism, such as Linux, this class
    may not be available.

    \reentrant

    \note All functions in this class are reentrant.

    \warning On Symbian this class does not support QObject::moveToThread().
*/

/*!
    Constructs a QSystemScreenSaver object with the given \a parent.
 */
QSystemScreenSaver::QSystemScreenSaver(QObject *parent)
    : QObject(parent)
{
    d = new QSystemScreenSaverPrivate();
}

/*!
    Destroys the QSystemScreenSaver object, and sets the screen saver to a non-inhibited state.
*/
QSystemScreenSaver::~QSystemScreenSaver()
{
    delete d;
}

/*!
    Set the screensaver to be inhibited.
*/
bool QSystemScreenSaver::setScreenSaverInhibit()
{
    return d->setScreenSaverInhibit();
}

/*!
    \property QSystemScreenSaver::screenSaverInhibited
    \brief Screensaver inhibited.

    Returns true if the screensaver is inhibited, otherwise false.
*/
bool QSystemScreenSaver::screenSaverInhibited()
{
    return d->screenSaverInhibited();
}

/*!
    Set the inhibition of the screensaver to be \a on.
    \since 1.2
*/
void QSystemScreenSaver::setScreenSaverInhibited(bool on)
{
    d->setScreenSaverInhibited(on);
}

#include "moc_qsystemscreensaver.cpp"

QTM_END_NAMESPACE
