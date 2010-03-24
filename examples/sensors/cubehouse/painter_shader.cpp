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
#include "light.h"
#include "lightmodel.h"
#include "material.h"
#include <QtOpenGL/qglshaderprogram.h>

ShaderPainter::ShaderPainter()
{
    materialProgram = new QGLShaderProgram();
    materialProgram->addShaderFromSourceFile
        (QGLShader::Vertex, QLatin1String(":/lighting.vsh"));
    materialProgram->addShaderFromSourceFile
        (QGLShader::Fragment, QLatin1String(":/material.fsh"));
    materialProgram->bindAttributeLocation("vertex", 0);
    materialProgram->bindAttributeLocation("normal", 1);
    materialProgram->bindAttributeLocation("texcoord", 2);
    materialProgram->link();

    textureProgram = new QGLShaderProgram();
    textureProgram->addShaderFromSourceFile
        (QGLShader::Vertex, QLatin1String(":/lighting.vsh"));
    textureProgram->addShaderFromSourceFile
        (QGLShader::Fragment, QLatin1String(":/texture.fsh"));
    textureProgram->bindAttributeLocation("vertex", 0);
    textureProgram->bindAttributeLocation("normal", 1);
    textureProgram->bindAttributeLocation("texcoord", 2);
    textureProgram->link();

    currentProgram = 0;
    matricesChanged = false;
}

ShaderPainter::~ShaderPainter()
{
    delete materialProgram;
    delete textureProgram;
}

void ShaderPainter::setMatrices(const QMatrix4x4 &mv, const QMatrix4x4 &proj)
{
    combinedMatrix = proj * mv;
    modelViewMatrix = mv;
    normalMatrix = mv.normalMatrix();
    matricesChanged = true;
}

void ShaderPainter::selectMaterial(Material *material)
{
    if (currentProgram != materialProgram) {
        materialProgram->bind();
        materialProgram->enableAttributeArray(0);
        materialProgram->enableAttributeArray(1);
        materialProgram->disableAttributeArray(2);
        currentProgram = materialProgram;
        matricesChanged = true;
    }
    if (matricesChanged) {
        materialProgram->setUniformValue("matrix", combinedMatrix);
        materialProgram->setUniformValue("modelView", modelViewMatrix);
        materialProgram->setUniformValue("normalMatrix", normalMatrix);
        matricesChanged = false;
    }
    updateMaterials(materialProgram, material);
}

void ShaderPainter::selectTexturedMaterial(Material *material)
{
    if (currentProgram != textureProgram) {
        textureProgram->bind();
        textureProgram->enableAttributeArray(0);
        textureProgram->enableAttributeArray(1);
        textureProgram->enableAttributeArray(2);
        textureProgram->setUniformValue("tex", 0);
        currentProgram = textureProgram;
        matricesChanged = true;
    }
    if (matricesChanged) {
        textureProgram->setUniformValue("matrix", combinedMatrix);
        textureProgram->setUniformValue("modelView", modelViewMatrix);
        textureProgram->setUniformValue("normalMatrix", normalMatrix);
        matricesChanged = false;
    }
    updateMaterials(textureProgram, material);
}

void ShaderPainter::setVertices(const float *array, int stride)
{
    // Doesn't matter which program we use - they have the same locations.
    materialProgram->setAttributeArray(0, array, 3, stride * sizeof(float));
}

void ShaderPainter::setTexCoords(const float *array, int stride)
{
    materialProgram->setAttributeArray(2, array, 2, stride * sizeof(float));
}

void ShaderPainter::setNormals(const float *array, int stride)
{
    materialProgram->setAttributeArray(1, array, 3, stride * sizeof(float));
}

void ShaderPainter::updateMaterials
    (QGLShaderProgram *program, Material *material)
{
    // Set the uniform variables for the light.
    QVector4D pli = light->eyePosition(QMatrix4x4());
    program->setUniformValue("pli", QVector3D(pli.x(), pli.y(), pli.z()));
    program->setUniformValue("pliw", GLfloat(pli.w()));

    // Set the uniform variables for the light model.
    program->setUniformValue("viewerAtInfinity", (int)(lightModel->viewerPosition() == LightModel::ViewerAtInfinity));
    program->setUniformValue("acs", lightModel->ambientSceneColor());

    // Set the uniform variables for the material.
    program->setUniformValue("acm", material->ambientColor());
    program->setUniformValue("dcm", material->diffuseColor());
    program->setUniformValue("scm", material->specularColor());
    program->setUniformValue("srm", (float)(material->shininess()));
}
