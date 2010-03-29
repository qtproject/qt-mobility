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

#include "view.h"
#include "teapot.h"
#include "cube.h"
#include <QtCore/qdebug.h>
#include <QtCore/qtimer.h>
#include <stdio.h>
#include "qaccelerometer.h"

#if defined(QT_OPENGL_ES)
#define USE_BUFFERS 1
#endif

View::View(QWidget *parent)
    : QGLWidget(parent),
      sensitivity(0.1f),
      painter(0),
      showFrameRate(false)
{
    mainCamera = new Camera(this);

    roomCamera = new Camera(this);
    roomCamera->setAdjustForAspectRatio(false);

    sensor = new QAccelerometer(this);
    connect(sensor, SIGNAL(readingChanged()), this, SLOT(accelerometerTimeout()));
    sensor->start();

    time.start();

    vertexBuffer = 0;
    indexBuffer = 0;
}

View::~View()
{
    delete painter;
}

void View::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
}

void View::initializeGL()
{
#if defined(QT_OPENGL_ES_2)
    painter = new ShaderPainter();
#else
    painter = new FixedFunctionPainter();
#endif

    glEnable(GL_DEPTH_TEST);

    roomMaterialBack = new Material();
    roomMaterialBack->setDiffuseColor(QColor(128, 100, 0));

    roomMaterialLeftRight = new Material();
    roomMaterialLeftRight->setDiffuseColor(Qt::cyan);

    roomMaterialTopBottom = new Material();
    roomMaterialTopBottom->setDiffuseColor(Qt::yellow);

    cubeMaterial = new Material();
    cubeMaterial->setColor(QColor(170, 202, 0));

    teapotMaterial = new Material();
    teapotMaterial->setAmbientColor(QColor(192, 150, 128));
    teapotMaterial->setSpecularColor(QColor(60, 60, 60));
    teapotMaterial->setShininess(128);

    roomModel = new LightModel(this);
    roomModel->setAmbientSceneColor(Qt::white);
    roomModel->setViewerPosition(LightModel::LocalViewer);

    normalModel = new LightModel(this);

    Light *light = new Light(this);
    light->setPosition(QVector3D(0.0f, 0.0f, 3.0f));
    painter->setLight(light);

    texture = bindTexture(QImage(QLatin1String(":/qtlogo.png")));

#ifdef USE_BUFFERS
    // Upload the teapot data into GL buffers for quicker rendering.
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 teapotVertexCount * teapotVertexStride * sizeof(float),
                 teapotVertexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 teapotTriangleCount * 3 * sizeof(ushort),
                 teapotTriangleData, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#endif
}

void View::paintGL()
{
    if (showFrameRate)
        qWarning("time since last frame: %d ms", time.restart());

    qreal aspectRatio = qreal(width()) / qreal(height());
    QMatrix4x4 mv, mv2;
    QMatrix4x4 proj;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mv = roomCamera->modelViewMatrix();
    proj = roomCamera->projectionMatrix(aspectRatio);
    painter->setLightModel(roomModel);
    painter->setMatrices(mv, proj);

    painter->selectMaterial(roomMaterialBack);
    painter->drawQuad(QVector3D(-3.0f, -3.0f, -15.0f),
                      QVector3D( 3.0f, -3.0f, -15.0f),
                      QVector3D( 3.0f,  3.0f, -15.0f),
                      QVector3D(-3.0f,  3.0f, -15.0f),
                      QVector3D(0.0f, 0.0f, 1.0f));

    painter->selectMaterial(roomMaterialLeftRight);
    painter->drawQuad(QVector3D(-3.0f, -3.0f, -15.0f),
                      QVector3D(-3.0f,  3.0f, -15.0f),
                      QVector3D(-3.0f,  3.0f, 0.0f),
                      QVector3D(-3.0f, -3.0f, 0.0f),
                      QVector3D(1.0f, 0.0f, 0.0f));
    painter->drawQuad(QVector3D(3.0f,  3.0f, -15.0f),
                      QVector3D(3.0f, -3.0f, -15.0f),
                      QVector3D(3.0f, -3.0f, 0.0f),
                      QVector3D(3.0f,  3.0f, 0.0f),
                      QVector3D(-1.0f, 0.0f, 0.0f));

    painter->selectMaterial(roomMaterialTopBottom);
    painter->drawQuad(QVector3D(-3.0f,  3.0f, -15.0f),
                      QVector3D( 3.0f,  3.0f, -15.0f),
                      QVector3D( 3.0f,  3.0f, 0.0f),
                      QVector3D(-3.0f,  3.0f, 0.0f),
                      QVector3D(0.0f, -1.0f, 0.0f));
    painter->drawQuad(QVector3D(-3.0f, -3.0f, -15.0f),
                      QVector3D(-3.0f, -3.0f, 0.0f),
                      QVector3D( 3.0f, -3.0f, 0.0f),
                      QVector3D( 3.0f, -3.0f, -15.0f),
                      QVector3D(0.0f, 1.0f, 0.0f));

    mv = mv2 = mainCamera->modelViewMatrix();
    proj = mainCamera->projectionMatrix(aspectRatio);
    mv.translate(1.0f, -0.5f, 0.0f);
    mv.rotate(45.0f, 1.0f, 1.0f, 1.0f);
    painter->setMatrices(mv, proj);
    painter->setLightModel(normalModel);
    painter->selectTexturedMaterial(cubeMaterial);
    painter->setVertices(cubeVertices, 8);
    painter->setNormals(cubeVertices + 3, 8);
    painter->setTexCoords(cubeVertices + 6, 8);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    mv2.translate(-0.8f, -1.5f, -3.0f);
    painter->setMatrices(mv2, proj);
    painter->selectMaterial(teapotMaterial);
#ifdef USE_BUFFERS
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    painter->setVertices(0, 8);
    painter->setNormals(reinterpret_cast<float *>(3 * sizeof(float)), 8);
    painter->setTexCoords(reinterpret_cast<float *>(6 * sizeof(float)), 8);
    glDrawElements(GL_TRIANGLES, teapotTriangleCount * 3,
                   GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#else
    painter->setVertices(teapotVertexData, 8);
    painter->setNormals(teapotVertexData + 3, 8);
    painter->setTexCoords(teapotVertexData + 6, 8);
    glDrawElements(GL_TRIANGLES, teapotTriangleCount * 3,
                   GL_UNSIGNED_SHORT, teapotTriangleData);
#endif
}

void View::accelerometerTimeout()
{
    QVector3D g = gravity();
    mainCamera->setMotionAdjustment(g);
    roomCamera->setMotionAdjustment(g);
    update();
}

#define ACCEL_TO_G(v) (v / 9.80665)

QVector3D View::gravity() const
{
    qreal x = ACCEL_TO_G(sensor->reading()->x()) * sensitivity;
    qreal y = ACCEL_TO_G(sensor->reading()->y()) * sensitivity;
    qreal z = ACCEL_TO_G(sensor->reading()->z());

    return QVector3D(x, y, z);
}
