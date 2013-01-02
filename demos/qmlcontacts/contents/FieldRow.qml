/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

import Qt 4.7

Item {
    property string label
    property string value
    property alias newValue: textEdit.text

    signal blur

    height: childrenRect.height
    Text {
        id: fieldName
        width: parent.width * 0.4;
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
        color: "white"
        border.color: "#00000000";
        border.width: 0
        radius: 2
        opacity: 0.5

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

    TextInput {
        id: textEdit
        anchors.fill: textRect
        anchors.margins: 3
        text: value
        color: "black"
        onActiveFocusChanged: { if (!activeFocus) { blur(); } }
    }
}
