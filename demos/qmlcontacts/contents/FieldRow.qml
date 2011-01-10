/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

import Qt 4.7

Item {
    property string label
    property variant value
    property alias newValue: textEdit.text

    signal blur

    height: childrenRect.height
    Text {
        id: fieldName
        width: parent.width * 0.5;
        height: 20;
        anchors.margins: 3
        anchors.left: parent.left
        text: label
        color: "white"
    }
    Rectangle {
        id: textRect
        anchors.left: fieldName.right
        anchors.right: parent.right
        anchors.rightMargin: 3
        height: 30;
        color: "#00000000";
        border.color: "#00000000";
        border.width: 0
        TextInput {
            id: textEdit
            anchors.fill: parent
            anchors.margins: 3
            text: value.toString();
            color: activeFocus? "black" : "#ffffaa";
            onActiveFocusChanged: { if (!activeFocus) { blur(); } }
        }
        states: [
                State {
                    name: "focused"
                    when: textEdit.activeFocus
                    PropertyChanges {
                        target: textRect
                        color: "#aaffffff"
                        radius: 2
                        border.width: 1
                        border.color: "black"
                    }
                }
            ]
    }
}
