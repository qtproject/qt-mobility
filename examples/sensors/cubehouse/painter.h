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

#ifndef PAINTER_H
#define PAINTER_H

#include <QtOpenGL/qgl.h>
#include <QtGui/qmatrix4x4.h>

class Light;
class LightModel;
class Material;
class QGLShaderProgram;

class Painter
{
public:
    Painter() : light(0), lightModel(0) {}
    virtual ~Painter() {}

    virtual void setMatrices(const QMatrix4x4 &mv, const QMatrix4x4 &proj) = 0;
    virtual void selectMaterial(Material *material) = 0;
    virtual void selectTexturedMaterial(Material *material) = 0;
    virtual void setVertices(const float *array, int stride) = 0;
    virtual void setTexCoords(const float *array, int stride) = 0;
    virtual void setNormals(const float *array, int stride) = 0;

    void drawQuad(const QVector3D &c1, const QVector3D &c2,
                  const QVector3D &c3, const QVector3D &c4,
                  const QVector3D &normal);

    void setLight(Light *value) { light = value; }
    void setLightModel(LightModel *value) { lightModel = value; }

protected:
    Light *light;
    LightModel *lightModel;
};

class FixedFunctionPainter : public Painter
{
public:
    FixedFunctionPainter();

    void setMatrices(const QMatrix4x4 &mv, const QMatrix4x4 &proj);
    void selectMaterial(Material *material);
    void selectTexturedMaterial(Material *material);
    void setVertices(const float *array, int stride);
    void setTexCoords(const float *array, int stride);
    void setNormals(const float *array, int stride);
};

class ShaderPainter : public Painter
{
public:
    ShaderPainter();
    ~ShaderPainter();

    void setMatrices(const QMatrix4x4 &mv, const QMatrix4x4 &proj);
    void selectMaterial(Material *material);
    void selectTexturedMaterial(Material *material);
    void setVertices(const float *array, int stride);
    void setTexCoords(const float *array, int stride);
    void setNormals(const float *array, int stride);

private:
    QGLShaderProgram *materialProgram;
    QGLShaderProgram *textureProgram;
    QMatrix4x4 combinedMatrix;
    QMatrix4x4 modelViewMatrix;
    QMatrix3x3 normalMatrix;

    void updateMaterials(QGLShaderProgram *program, Material *material);
};

#endif
