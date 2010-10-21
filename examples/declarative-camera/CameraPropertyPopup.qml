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

Rectangle {
    id: propertyPopup

    property alias model : view.model
    property variant currentValue
    property variant currentItem : model.get(view.currentIndex)

    property int itemWidth : 100
    property int itemHeight : 70
    property int columns : 2

    width: columns*itemWidth + view.anchors.margins*2
    height: Math.ceil(model.count/columns)*itemHeight + view.anchors.margins*2 + 25

    radius: 5
    border.color: "#000000"
    border.width: 2
    smooth: true
    color: "#5e5e5e"

    signal selected

    function indexForValue(value) {
        for (var i = 0; i < view.count; i++) {
            if (model.get(i).value == value) {
                return i;
            }
        }

        return 0;
    }

    GridView {
        id: view
        anchors.fill: parent
        anchors.margins: 5
        cellWidth: propertyPopup.itemWidth
        cellHeight: propertyPopup.itemHeight
        snapMode: ListView.SnapOneItem
        highlightFollowsCurrentItem: true
        highlight: Rectangle { color: "gray"; radius: 5 }
        currentIndex: indexForValue(propertyPopup.currentValue)

        onCurrentIndexChanged: {
            propertyPopup.currentValue = model.get(view.currentIndex).value
        }

        delegate: Item {
            width: propertyPopup.itemWidth
            height: 70

            Image {
                anchors.centerIn: parent
                source: icon
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    propertyPopup.currentValue = value
                    propertyPopup.selected(value)
                }
            }
        }
    }

    Text {
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 8
        anchors.left: parent.left
        anchors.leftMargin: 16

        color: "#ffffff"
        font.bold: true
        style: Text.Raised;
        styleColor: "black"
        font.pixelSize: 14

        text: view.model.get(view.currentIndex).text
    }
}
