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
import QtMobility.contacts 1.1

Item {
    property variant contact
    signal dismissed

    ListView {
        id: detailView
        focus: true
        keyNavigationWraps: true
        width: parent.width
        anchors.top: parent.top
        anchors.bottom: toolBar.top

        model: contact ? contact.details : null

        delegate: Column {
                width: parent.width
                Text {
                    width: detailView.width - 6;
                    height: 30
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: modelData.definitionName;
                    verticalAlignment: Text.AlignVCenter
                    color: "white";
                    font.weight: Font.Bold
                }
                Repeater {
                    id: fieldView

                    property variant detail: modelData
                    model: modelData.fieldNames

                    delegate:
                        Item {
                            width: parent.width
                            height: childrenRect.height
                            Text {
                                id: fieldName
                                width: parent.width * 0.5;
                                height: 20;
                                anchors.margins: 3
                                anchors.left: parent.left
                                text: modelData
                                color: "white"
                            }
                            TextEdit {
                                id: textEdit
                                width: parent.width * 0.5;
                                height: 20;
                                anchors.margins: 3
                                anchors.left: fieldName.right
                                text: fieldView.detail.value(modelData).toString();
                                color: "#ffffaa";
//                                    onActiveFocusChanged: {
//                                                              if (!activeFocus) {
//                                                                  fieldView.detail.setValue(modelData, text);
//                                                              }
//                                                          }
                            }
                        }
                }
            }
    }

    ToolBar {
        id: toolBar
        height: 40;
        anchors.bottom: parent.bottom;
        width: parent.width;
        opacity: 0.9
        labels: ["Done"]
        onButtonClicked: dismissed()
    }
}
