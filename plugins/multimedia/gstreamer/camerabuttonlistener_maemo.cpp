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

#include "camerabuttonlistener_maemo.h"

#include <QtDBus/qdbusconnection.h>
#include <QtDBus/qdbusinterface.h>

#include <QtGui/qapplication.h>
#include <QtGui/qevent.h>
#include <QtGui/qwidget.h>


CameraButtonListener::CameraButtonListener(QObject *parent) :
    QObject(parent),
    m_focusPressed(false),
    m_shutterPressed(false)
{
    QDBusConnection::systemBus().connect(
                QString(),
                "/org/freedesktop/Hal/devices/platform_cam_launch",
                "org.freedesktop.Hal.Device",
                "PropertyModified",
                this,
                SLOT(updateShuterButtonState()));

    QDBusConnection::systemBus().connect(
                QString(),
                "/org/freedesktop/Hal/devices/platform_cam_focus",
                "org.freedesktop.Hal.Device",
                "PropertyModified",
                this,
                SLOT(updateFocusButtonState()));
}


CameraButtonListener::~CameraButtonListener()
{
}

void CameraButtonListener::updateFocusButtonState()
{
    QDBusInterface propertyInterface("org.freedesktop.Hal",
                                     "/org/freedesktop/Hal/devices/platform_cam_focus",
                                     "org.freedesktop.Hal.Device",
                                     QDBusConnection::systemBus());

    bool pressed = propertyInterface.call("GetProperty", "button.state.value").arguments().at(0).toBool();

    if (m_focusPressed != pressed) {
        m_focusPressed = pressed;
        QWidget *window = QApplication::focusWidget();

        if (window) {
            QApplication::postEvent(window,
                                    new QKeyEvent(pressed ? QEvent::KeyPress : QEvent::KeyRelease,
                                                  0x01100021, //Qt::Key_CameraFocus since Qt 4.7.0
                                                  Qt::NoModifier));
        }
    }
}

void CameraButtonListener::updateShuterButtonState()
{
    QDBusInterface propertyInterface("org.freedesktop.Hal",
                                     "/org/freedesktop/Hal/devices/platform_cam_launch",
                                     "org.freedesktop.Hal.Device",
                                     QDBusConnection::systemBus());

    bool pressed = propertyInterface.call("GetProperty", "button.state.value").arguments().at(0).toBool();

    if (m_shutterPressed != pressed) {
        m_shutterPressed = pressed;
        QWidget *window = QApplication::focusWidget();

        if (window) {
            QApplication::postEvent(window,
                                    new QKeyEvent(pressed ? QEvent::KeyPress : QEvent::KeyRelease,
                                                  0x01100020, //Qt::Key_Camera since Qt 4.7.0
                                                  Qt::NoModifier));
        }
    }
}
