/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt 4.7

Item {
    id: flickableList
    clip: true

    signal clicked

    property alias delegate : repeater.delegate

    property variant items: []
    property int index: 0
    property int itemWidth : flickableList.width

    function scrollTo(id) {
        var x = id*flickableList.itemWidth
        if (flickArea.contentX != x) {
            centeringAnimation.stop();
            flickArea.newX = id*flickableList.itemWidth
            centeringAnimation.start();
        }
    }

    onIndexChanged: scrollTo(index)
    onWidthChanged: scrollTo(index)

    Flickable {
        id: flickArea
        property int newX: 0

        MouseArea {
            anchors.fill: parent
            onClicked: {
                var x = mapToItem(flickableList, mouseX, mouseY).x

                if (x < flickableList.width/3) {
                    if (flickableList.index > 0)
                        flickableList.scrollTo(flickableList.index-1);
                } else if (x > flickableList.width*2/3) {
                    if (flickableList.index < flickableList.items.length-1)
                        flickableList.scrollTo(flickableList.index+1);
                } else {
                    flickableList.clicked()
                }

            }
        }

        PropertyAnimation {
            id: centeringAnimation
            target: flickArea
            properties: "contentX"
            easing.type: Easing.OutQuad
            from: flickArea.contentX
            to: flickArea.newX

            onCompleted: {
                flickableList.index = flickArea.newX / flickableList.itemWidth
            }
        }

        onMovementStarted: {
            centeringAnimation.stop();
        }

        onMovementEnded: {
            var modulo = flickArea.contentX % flickableList.itemWidth;
            var offset = flickableList.itemWidth / 2;
            flickArea.newX = modulo < offset ? flickArea.contentX - modulo : flickArea.contentX + (flickableList.itemWidth - modulo);
            centeringAnimation.start();
        }


        width: flickableList.width
        height: flickableList.height
        contentWidth: items.width
        contentHeight: items.height
        flickDeceleration: 4000

        Row {
            id: items
            Repeater {
                id: repeater
                model: flickableList.items.length
            }
        }
    }
}
