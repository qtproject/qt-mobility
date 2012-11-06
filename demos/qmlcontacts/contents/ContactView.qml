/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

import Qt 4.7
import QtMobility.contacts 1.1

Item {
    property variant contact
    signal dismissed
    signal deleted(int id)
    property bool showDetailed: false

    // list<string> type properties do not update automatically as model, assigning them again triggers an update
    function update() {
        phoneNumberRepeater.model = contact.phoneNumbers
        emailRepeater.model = contact.emails
    }

    ListView {
        id: normalView
        focus: true
        keyNavigationWraps: true
        width: parent.width
        anchors.top: parent.top
        anchors.bottom: toolBar.top
        opacity: 1
        clip: true
        spacing: 2
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
                spacing: 2
                Repeater {
                    id: phoneNumberRepeater
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
                        update();
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
                spacing: 2
                Repeater {
                    id: emailRepeater
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
                        update();
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
        clip: true
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
                                width: parent.width * 0.4;
                                height: 20;
                                anchors.margins: 3
                                anchors.left: parent.left
                                text: modelData
                                color: "white"
                                elide: Text.ElideRight
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
                        showDetailed = false;
                        break;
                    case 1:
                        showDetailed = false;
                        break;
                    case 2:
                        deleted(contact.contactId)
                        showDetailed = false;
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
