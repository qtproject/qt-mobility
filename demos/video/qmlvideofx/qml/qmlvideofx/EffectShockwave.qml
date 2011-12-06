/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** $QT_END_LICENSE$
**
****************************************************************************/

// Based on http://www.geeks3d.com/20091116/shader-library-2d-shockwave-post-processing-filter-glsl/

import QtQuick 1.0

Effect {
    id: root

    // Constant properties which must be supported by every effect
    property int numParameters: 1
    property bool supportsDivider: true

    property real param1Value: 0.5
    property real dividerValue: 0.5

    property real granularity: param1Value * 20

    property real centerX
    property real centerY
    property real time

    SequentialAnimation {
        running: true
        loops: Animation.Infinite
        ScriptAction {
            script: {
                centerX = Math.random()
                centerY = Math.random()
            }
        }
        NumberAnimation {
            target: root
            property: "time"
            from: 0
            to: 1
            duration: 1000
        }
    }

    property real weight: param1Value

    fragmentShader: fragmentShaderCommon + "
        uniform float centerX;
        uniform float centerY;
        uniform float dividerValue;
        uniform float granularity;
        uniform float time;
        uniform float weight;

        uniform sampler2D source;
        uniform lowp float qt_Opacity;
        varying vec2 qt_TexCoord0;

        void main()
        {
            vec2 uv = qt_TexCoord0.xy;
            vec2 tc = qt_TexCoord0;
            vec2 center = vec2(centerX, centerY);
            const vec3 shock = vec3(10.0, 1.5, 0.1);
            if (uv.x < dividerValue) {
                float distance = distance(uv, center);
                if ((distance <= (time + shock.z)) &&
                    (distance >= (time - shock.z))) {
                    float diff = (distance - time);
                    float powDiff = 1.0 - pow(abs(diff*shock.x), shock.y*weight);
                    float diffTime = diff  * powDiff;
                    vec2 diffUV = normalize(uv - center);
                    tc += (diffUV * diffTime);
                }
            }
            gl_FragColor = qt_Opacity * texture2D(source, tc);
        }"
}
