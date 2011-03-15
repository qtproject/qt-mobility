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
    signal deleted(int id)
    property bool showDetailed: false

    ListView {
        id: normalView
        focus: true
        keyNavigationWraps: true
        width: parent.width
        anchors.top: parent.top
        anchors.bottom: toolBar.top
        opacity: 1
        transform: Scale {
            id: normalViewScale;
            xScale: 1
            origin.x: normalView.width/2
        }

        model: VisualItemModel {
            Text {
                width: normalView.width - 6;
                height: 30
                text: "Name"
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                color: "white";
                font.weight: Font.Bold
            }
            FieldRow {
                id: customLabelRow
                width: normalView.width
                label: "Display"
                value: contact ? contact.name.customLabel : ""
            }
            FieldRow {
                id: firstNameRow
                width: normalView.width
                label: "First name"
                value: contact ? contact.name.firstName : ""
            }
            FieldRow {
                id: lastNameRow
                width: normalView.width
                label: "Last name"
                value: contact ? contact.name.lastName : ""
            }

            Text {
                width: normalView.width - 6;
                height: 30
                text: "Phone Numbers"
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                color: "white";
                font.weight: Font.Bold
            }
            Column {
                Repeater {
                    model: contact ? contact.phoneNumbers : []
                    delegate:
                        FieldRow {
                            width: normalView.width
                            label: modelData.contexts.toString()
                            value: modelData.number
                            onBlur: {
                                        modelData.setValue("PhoneNumber", newValue);
                                }
                        }
                }
            }
            MediaButton {
                text: "Add Phone";
                onClicked: {
                        var detail = Qt.createQmlObject(
                            "import QtMobility.contacts 1.1;" +
                            "PhoneNumber {number: ''}", contact);
                        contact.addDetail(detail);
                    }
            }

            Text {
                width: normalView.width - 6;
                height: 30
                text: "Email Addresses"
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                color: "white";
                font.weight: Font.Bold
            }
            Column {
                Repeater {
                    model: contact ? contact.emails : []
                    delegate:
                        FieldRow {
                            width: normalView.width
                            label: modelData.contexts.toString()
                            value: modelData.emailAddress
                            onBlur: {
                                        modelData.setValue("EmailAddress", newValue);
                                }
                        }
                }
            }
            MediaButton {
                text: "Add Email";
                onClicked: {
                        var detail = Qt.createQmlObject(
                            "import QtMobility.contacts 1.1;" +
                            "EmailAddress {emailAddress: ''}", contact);
                        contact.addDetail(detail);
                    }
            }
        }
    }

    ListView {
        id: detailView
        focus: true
        keyNavigationWraps: true
        width: parent.width
        anchors.top: parent.top
        anchors.bottom: toolBar.top
        opacity: 1
        transform: Scale {
            id: detailViewScale;
            xScale: 0
            origin.x: detailView.width/2
        }

        model: contact ? contact.details : null

        delegate: Column {
                width: parent.width
                Text {
                    width: detailView.width - 6;
                    height: 30
                    horizontalAlignment: Text.AlignHCenter
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
                            width: detailView.width
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
                            Text {
                                id: textRect
                                anchors.left: fieldName.right
                                anchors.right: parent.right
                                anchors.rightMargin: 3
                                height: 30
                                color: "white"
                                text: fieldView.detail.value(modelData).toString()
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
        labels: ["Save", "Cancel", "Delete", "Details"]
        onButtonClicked: {
                // force the focus away from any TextInputs, to ensure they save
                toolBar.focus = true
                contact.name.customLabel = customLabelRow.newValue
                contact.name.firstName = firstNameRow.newValue
                contact.name.lastName = lastNameRow.newValue
                switch (index) {
                    case 0:
                        contact.save();
                        break;
                    case 2:
                        deleted(contact.contactId)
                        break;
                    case 3:
                        showDetailed = !showDetailed;
                        break;
                }
                if (index != 3)
                    dismissed();
            }
    }

    states: [
        State {
            name: "Detailed"
            when: showDetailed
            PropertyChanges {
                target: normalViewScale
                xScale: 0
            }
            PropertyChanges {
                target: detailViewScale
                xScale: 1
            }
        }
        ]
    transitions:  [
            Transition {
                from: ""
                to: "Detailed"
                reversible: true
                SequentialAnimation {
                    NumberAnimation { duration: 100; target: normalViewScale; properties: "xScale" }
                    NumberAnimation { duration: 100; target: detailViewScale; properties: "xScale" }
                }
            }
        ]
}
