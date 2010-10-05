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
import QtMultimediaKit 1.1

FocusScope {
    property Camera camera
    property bool previewAvailable : false

    property alias whiteBalance : wbModesButton.value
    property alias flashMode : flashModesButton.value
    property alias exposureCompensation : exposureCompensationButton.value

    signal previewSelected
    id : captureControls

    Column {
        spacing : 8
        anchors.right : parent.right
        anchors.rightMargin: 8
        anchors.top : parent.top
        anchors.topMargin: 8

        FocusButton {
            camera: captureControls.camera
            visible: false
        }

        CameraButton {
            text: "Capture"
            onClicked: camera.captureImage()
        }

        CameraPropertyButton {
            id : flashModesButton
            value: Camera.FlashOff
            model: ListModel {
                ListElement {
                    icon: "images/camera_flash_auto.png"
                    value: Camera.FlashAuto
                    text: "Auto"
                }
                ListElement {
                    icon: "images/camera_flash_off.png"
                    value: Camera.FlashOff
                    text: "Off"
                }
                ListElement {
                    icon: "images/camera_flash_fill.png"
                    value: Camera.FlashOn
                    text: "On"
                }
                ListElement {
                    icon: "images/camera_flash_redeye.png"
                    value: Camera.FlashRedEyeReduction
                    text: "Red Eye Reduction"
                }
            }
        }

        CameraPropertyButton {
            id : wbModesButton
            value: Camera.WhiteBalanceAuto
            model: ListModel {
                ListElement {
                    icon: "images/camera_auto_mode.png"
                    value: Camera.WhiteBalanceAuto
                    text: "Auto"
                }
                ListElement {
                    icon: "images/camera_white_balance_sunny.png"
                    value: Camera.WhiteBalanceSunlight
                    text: "Sunlight"
                }
                ListElement {
                    icon: "images/camera_white_balance_cloudy.png"
                    value: Camera.WhiteBalanceCloudy
                    text: "Cloudy"
                }
                ListElement {
                    icon: "images/camera_white_balance_incandescent.png"
                    value: Camera.WhiteBalanceIncandescent
                    text: "Incandescent"
                }
                ListElement {
                    icon: "images/camera_white_balance_flourescent.png"
                    value: Camera.WhiteBalanceFluorescent
                    text: "Fluorescent"
                }
            }
        }

        ExposureCompensationButton {
            id : exposureCompensationButton
        }

        CameraButton {
            text: "View"
            onClicked: captureControls.previewSelected()
            visible: captureControls.previewAvailable
        }
    }

    CameraButton {
        id: quitButton
        anchors.right : parent.right
        anchors.rightMargin: 8
        anchors.bottom : parent.bottom
        anchors.bottomMargin: 8
        text: "Quit"
        onClicked: Qt.quit()
    }

    Item {
        id: exposureDetails
        anchors.bottom : parent.bottom
        anchors.left : parent.left
        anchors.bottomMargin: 16
        anchors.leftMargin: 16
        height: childrenRect.height
        width: childrenRect.width

        visible : camera.lockStatus == Camera.Locked

        Rectangle {
            opacity: 0.4
            color: "black"
            anchors.fill: parent
        }

        Row {
            spacing : 16

            Text {
                text: "Av: "+camera.aperture.toFixed(1)
                font.pixelSize: 18
                color: "white"
                visible: camera.aperture > 0
            }

            Text {
                font.pixelSize: 18
                color: "white"
                visible: camera.shutterSpped > 0
                text: "Tv: "+printableExposureTime(camera.shutterSpeed)

                function printableExposureTime(t) {
                    if (t > 3.9)
                        return "Tv: "+t.toFixed() + "\"";

                    if (t > 0.24 )
                        return "Tv: "+t.toFixed(1) + "\"";

                    if (t > 0)
                        return "Tv: 1/"+(1/t).toFixed();

                    return "";
                }
            }

            Text {
                text: "ISO: "+camera.iso.toFixed()
                font.pixelSize: 18
                color: "white"
                visible: camera.iso > 0
            }
        }
    }

    ZoomControl {
        x : 0
        y : 0
        width : 100
        height: parent.height

        currentZoom: camera.digitalZoom
        maximumZoom: Math.min(4.0, camera.maximumDigitalZoom)
        onZoomTo: camera.setDigitalZoom(value)
    }
}
