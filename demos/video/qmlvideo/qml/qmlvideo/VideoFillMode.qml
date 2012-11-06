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

import QtQuick 1.0
import QtMultimediaKit 1.1

Scene {
    id: root

    Content {
        id: content
        anchors.centerIn: parent
        width: parent.contentWidth
        contentType: "video"
        source: parent.source1
        volume: parent.volume
        onVideoFramePainted: root.videoFramePainted()
    }

    Button {
        id: button
        anchors {
            right: parent.right
            verticalCenter: parent.verticalCenter
            margins: parent.margins
        }
        width: 150
        height: root.buttonHeight
        text: "PreserveAspectFit"
        onClicked: {
            if (!content.dummy) {
                var video = content.contentItem()
                if (video.fillMode == Video.Stretch) {
                    video.fillMode = Video.PreserveAspectFit
                    text = "PreserveAspectFit"
                } else if (video.fillMode == Video.PreserveAspectFit) {
                    video.fillMode = Video.PreserveAspectCrop
                    text = "PreserveAspectCrop"
                } else {
                    video.fillMode = Video.Stretch
                    text = "Stretch"
                }
            }
        }
    }

    Component.onCompleted: root.content = content
}
