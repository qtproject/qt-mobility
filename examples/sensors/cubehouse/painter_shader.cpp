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
}

void ShaderPainter::selectMaterial(Material *material)
{
    materialProgram->bind();
    materialProgram->enableAttributeArray(0);
    materialProgram->enableAttributeArray(1);
    materialProgram->disableAttributeArray(2);
    materialProgram->setUniformValue("matrix", combinedMatrix);
    materialProgram->setUniformValue("modelView", modelViewMatrix);
    materialProgram->setUniformValue("normalMatrix", normalMatrix);
    updateMaterials(materialProgram, material);
}

void ShaderPainter::selectTexturedMaterial(Material *material)
{
    textureProgram->bind();
    textureProgram->enableAttributeArray(0);
    textureProgram->enableAttributeArray(1);
    textureProgram->enableAttributeArray(2);
    textureProgram->setUniformValue("matrix", combinedMatrix);
    textureProgram->setUniformValue("modelView", modelViewMatrix);
    textureProgram->setUniformValue("normalMatrix", normalMatrix);
    textureProgram->setUniformValue("tex", 0);
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

static inline QVector4D colorToVector4(const QColor& color)
{
    return QVector4D(color.redF(), color.greenF(),
                     color.blueF(), color.alphaF());
}

void ShaderPainter::updateMaterials
    (QGLShaderProgram *program, Material *material)
{
    // Set the uniform variables for the light.
    program->setUniformValue("acli", light->ambientColor());
    program->setUniformValue("dcli", light->diffuseColor());
    program->setUniformValue("scli", light->specularColor());
    program->setUniformValue
        ("sdli", light->eyeSpotDirection(QMatrix4x4()));
    QVector4D pli = light->eyePosition(QMatrix4x4());
    program->setUniformValue("pli", QVector3D(pli.x(), pli.y(), pli.z()));
    program->setUniformValue("pliw", GLfloat(pli.w()));
    program->setUniformValue("srli", GLfloat(light->spotExponent()));
    program->setUniformValue("crli", GLfloat(light->spotAngle()));
    program->setUniformValue("ccrli", GLfloat(light->spotCosAngle()));
    program->setUniformValue("k0", GLfloat(light->constantAttenuation()));
    program->setUniformValue("k1", GLfloat(light->linearAttenuation()));
    program->setUniformValue("k2", GLfloat(light->quadraticAttenuation()));

    // Set the uniform variables for the light model.
    program->setUniformValue("twoSided", (int)(lightModel->model() == LightModel::TwoSided));
    program->setUniformValue("viewerAtInfinity", (int)(lightModel->viewerPosition() == LightModel::ViewerAtInfinity));
    program->setUniformValue("acs", lightModel->ambientSceneColor());

    static const int MaxMaterials = 2;
    QVector4D acm[MaxMaterials];
    QVector4D dcm[MaxMaterials];
    QVector4D scm[MaxMaterials];
    QVector4D ecm[MaxMaterials];
    float srm[MaxMaterials];
    acm[0] = colorToVector4(material->ambientColor());
    dcm[0] = colorToVector4(material->diffuseColor());
    scm[0] = colorToVector4(material->specularColor());
    ecm[0] = colorToVector4(material->emittedLight());
    srm[0] = (float)(material->shininess());
    acm[1] = colorToVector4(material->ambientColor());
    dcm[1] = colorToVector4(material->diffuseColor());
    scm[1] = colorToVector4(material->specularColor());
    ecm[1] = colorToVector4(material->emittedLight());
    srm[1] = (float)(material->shininess());
    program->setUniformValueArray("acm", (const GLfloat *)acm, MaxMaterials, 4);
    program->setUniformValueArray("dcm", (const GLfloat *)dcm, MaxMaterials, 4);
    program->setUniformValueArray("scm", (const GLfloat *)scm, MaxMaterials, 4);
    program->setUniformValueArray("ecm", (const GLfloat *)ecm, MaxMaterials, 4);
    program->setUniformValueArray("srm", srm, MaxMaterials, 1);
}
