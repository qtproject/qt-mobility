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
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

 import Qt 4.7

 Component {
     id: photoDelegate
     Item {
         id: wrapper; width: 79; height: 79

         function photoClicked() {
             imageDetails.photoTitle = title;
             imageDetails.photoDate = datetaken;
             imageDetails.photoUrl = "http://farm" + farm + ".static.flickr.com/" + server + "/" + id + "_" + secret + ".jpg";
             console.log(imageDetails.photoUrl);
             scaleMe.state = "Details";
         }

         Item {
             anchors.centerIn: parent
             scale: 0.0
             Behavior on scale { NumberAnimation { easing.type: Easing.InOutQuad} }
             id: scaleMe

             Rectangle { height: 79; width: 79; id: blackRect;  anchors.centerIn: parent; color: "black"; smooth: true }
             Rectangle {
                 id: whiteRect; width: 76; height: 76; anchors.centerIn: parent; color: "#dddddd"; smooth: true
                 Image { id: thumb;
                         // source: imagePath;
                         source: imageDetails.photoUrl = "http://farm" + farm + ".static.flickr.com/" + server + "/" + id + "_" + secret + "_t.jpg"
                         width: parent.width; height: parent.height
                         x: 1; y: 1; smooth: true}
                 Image { source: "images/gloss.png" }
             }

             Connections {
                 target: toolBar
                 onButton2Clicked: if (scaleMe.state == 'Details' ) scaleMe.state = 'Show'
             }

             states: [
                 State {
                     name: "Show"; when: thumb.status == Image.Ready
                     PropertyChanges { target: scaleMe; scale: 1 }
                 },
                 State {
                     name: "Details"
                     PropertyChanges { target: scaleMe; scale: 1 }
                     ParentChange { target: wrapper; parent: imageDetails.frontContainer }
                     PropertyChanges { target: wrapper; x: 20; y: 60; z: 1000 }
                     PropertyChanges { target: background; state: "DetailedView" }
                 }
             ]
             transitions: [
                 Transition {
                     from: "Show"; to: "Details"
                     ParentAnimation {
                         NumberAnimation { properties: "x,y"; duration: 500; easing.type: Easing.InOutQuad }
                     }
                 },
                 Transition {
                     from: "Details"; to: "Show"
                     SequentialAnimation {
                         ParentAnimation {
                            NumberAnimation { properties: "x,y"; duration: 500; easing.type: Easing.InOutQuad }
                         }
                         PropertyAction { targets: wrapper; properties: "z" }
                     }
                 }
             ]
         }
         MouseArea { anchors.fill: wrapper; onClicked: { photoClicked() } }
     }
 }
