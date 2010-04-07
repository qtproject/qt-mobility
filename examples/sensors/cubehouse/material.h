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

#ifndef MATERIAL_H
#define MATERIAL_H

#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>
#include <QtGui/qcolor.h>

class MaterialPrivate;

class Material : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QColor basicColor READ basicColor WRITE setBasicColor NOTIFY basicColorChanged)
    Q_PROPERTY(QColor ambientColor READ ambientColor WRITE setAmbientColor NOTIFY ambientColorChanged)
    Q_PROPERTY(QColor diffuseColor READ diffuseColor WRITE setDiffuseColor NOTIFY diffuseColorChanged)
    Q_PROPERTY(QColor specularColor READ specularColor WRITE setSpecularColor NOTIFY specularColorChanged)
    Q_PROPERTY(QColor emittedLight READ emittedLight WRITE setEmittedLight NOTIFY emittedLightChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY basicColorChanged)
    Q_PROPERTY(qreal shininess READ shininess WRITE setShininess NOTIFY shininessChanged)
public:
    explicit Material(QObject *parent = 0);
    ~Material();

    QColor basicColor() const;
    void setBasicColor(const QColor& value);

    QColor ambientColor() const;
    void setAmbientColor(const QColor& value);

    QColor diffuseColor() const;
    void setDiffuseColor(const QColor& value);

    QColor specularColor() const;
    void setSpecularColor(const QColor& value);

    QColor emittedLight() const;
    void setEmittedLight(const QColor& value);

    QColor color() const;
    void setColor(const QColor& value);

    qreal shininess() const;
    void setShininess(qreal value);

Q_SIGNALS:
    void basicColorChanged();
    void ambientColorChanged();
    void diffuseColorChanged();
    void specularColorChanged();
    void emittedLightChanged();
    void shininessChanged();
    void materialChanged();

private:
    QScopedPointer<MaterialPrivate> d_ptr;

    Q_DECLARE_PRIVATE(Material)
    Q_DISABLE_COPY(Material)
};

#endif
