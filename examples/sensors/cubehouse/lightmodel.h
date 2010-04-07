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

#ifndef LIGHTMODEL_H
#define LIGHTMODEL_H

#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>
#include <QtGui/qcolor.h>

class LightModelPrivate;

class LightModel : public QObject
{
    Q_OBJECT
    Q_ENUMS(Model)
    Q_ENUMS(ColorControl)
    Q_ENUMS(ViewerPosition)
    Q_PROPERTY(Model model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(ColorControl colorControl READ colorControl WRITE setColorControl NOTIFY colorControlChanged)
    Q_PROPERTY(ViewerPosition viewerPosition READ viewerPosition WRITE setViewerPosition NOTIFY viewerPositionChanged)
    Q_PROPERTY(QColor ambientSceneColor READ ambientSceneColor WRITE setAmbientSceneColor NOTIFY ambientSceneColorChanged)
public:
    explicit LightModel(QObject *parent = 0);
    ~LightModel();

    enum Model
    {
        OneSided,
        TwoSided
    };

    enum ColorControl
    {
        SingleColor,
        SeparateSpecularColor
    };

    enum ViewerPosition
    {
        ViewerAtInfinity,
        LocalViewer
    };

    LightModel::Model model() const;
    void setModel(LightModel::Model value);

    LightModel::ColorControl colorControl() const;
    void setColorControl(LightModel::ColorControl value);

    LightModel::ViewerPosition viewerPosition() const;
    void setViewerPosition(LightModel::ViewerPosition value);

    QColor ambientSceneColor() const;
    void setAmbientSceneColor(const QColor& value);

Q_SIGNALS:
    void modelChanged();
    void colorControlChanged();
    void viewerPositionChanged();
    void ambientSceneColorChanged();
    void lightModelChanged();

private:
    QScopedPointer<LightModelPrivate> d_ptr;

    Q_DECLARE_PRIVATE(LightModel)
    Q_DISABLE_COPY(LightModel)
};

#endif
