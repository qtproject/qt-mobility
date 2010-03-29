/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt3D module of the Qt Toolkit.
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

#include "painter.h"
#include <QtCore/qvarlengtharray.h>

void Painter::drawQuad(const QVector3D &c1, const QVector3D &c2,
                       const QVector3D &c3, const QVector3D &c4,
                       const QVector3D &normal)
{
    QVarLengthArray<float> vertices;
    QVarLengthArray<float> normals;

    vertices.append(c1.x());
    vertices.append(c1.y());
    vertices.append(c1.z());
    normals.append(normal.x());
    normals.append(normal.y());
    normals.append(normal.z());

    vertices.append(c2.x());
    vertices.append(c2.y());
    vertices.append(c2.z());
    normals.append(normal.x());
    normals.append(normal.y());
    normals.append(normal.z());

    vertices.append(c3.x());
    vertices.append(c3.y());
    vertices.append(c3.z());
    normals.append(normal.x());
    normals.append(normal.y());
    normals.append(normal.z());

    vertices.append(c4.x());
    vertices.append(c4.y());
    vertices.append(c4.z());
    normals.append(normal.x());
    normals.append(normal.y());
    normals.append(normal.z());

    setVertices(vertices.constData(), 3);
    setNormals(normals.constData(), 3);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
