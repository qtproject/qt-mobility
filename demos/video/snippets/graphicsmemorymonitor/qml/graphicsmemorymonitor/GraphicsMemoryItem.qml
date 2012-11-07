/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

import QtQuick 1.0
import GraphicsMemoryMonitor 1.0

Rectangle {
    id: root
    property bool logging: true
    property bool displayed: true
    property bool enabled: logging || displayed
    property int updateInterval: 500
    property color textColor: "yellow"
    property int textSize: 20

    border.width: 1
    border.color: "yellow"
    width: 5.6 * root.textSize
    height: 4.3 * root.textSize
    color: "black"
    opacity: 0.5
    radius: 10
    visible: displayed && monitor.active

    // This should ensure that the monitor is on top of all other content
    z: 999

    GraphicsMemoryMonitor {
        id: monitor
        updateInterval: root.enabled ? root.updateInterval : 0
        onChanged: if (root.logging) trace()
    }

    Text {
        anchors {
            right: parent.right
            top: parent.top
            margins: 10
        }
        color: root.textColor
        font.pixelSize: root.textSize
        text: monitor.usedMemoryHumanReadable
    }

    Text {
        anchors {
            right: parent.right
            verticalCenter: parent.verticalCenter
            margins: 10
        }
        color: root.textColor
        font.pixelSize: root.textSize
        text: monitor.currentProcessUsageHumanReadable
    }

    Text {
        anchors {
            right: parent.right
            bottom: parent.bottom
            margins: 10
        }
        color: root.textColor
        font.pixelSize: root.textSize
        text: monitor.totalMemoryHumanReadable
    }
}

