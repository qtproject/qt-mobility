/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
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
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEGEOMAPCIRCLEOBJECT_H
#define QDECLARATIVEGEOMAPCIRCLEOBJECT_H

#include "qdeclarativecoordinate_p.h"
#include "qdeclarativegeomapobjectborder_p.h"
#include "qgeomapcircleobject.h"

class QColor;
class QBrush;

QTM_BEGIN_NAMESPACE

class QDeclarativeGeoMapCircleObject : public QGeoMapCircleObject
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeCoordinate* center READ declarativeCenter WRITE setDeclarativeCenter NOTIFY declarativeCenterChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QDeclarativeGeoMapObjectBorder* border READ border)

public:
    QDeclarativeGeoMapCircleObject();
    ~QDeclarativeGeoMapCircleObject();

    QDeclarativeCoordinate* declarativeCenter();
    void setDeclarativeCenter(const QDeclarativeCoordinate *center);

    QColor color() const;
    void setColor(const QColor &color);

    QDeclarativeGeoMapObjectBorder* border();

Q_SIGNALS:
    void declarativeCenterChanged(const QDeclarativeCoordinate *center);
    void colorChanged(const QColor &color);

private Q_SLOTS:
    void borderColorChanged(const QColor &color);
    void borderWidthChanged(int width);
    void centerLatitudeChanged(double latitude);
    void centerLongitudeChanged(double longitude);
    void centerAltitudeChanged(double altitude);

private:
    QDeclarativeCoordinate* m_center;
    QColor m_color;
    QDeclarativeGeoMapObjectBorder m_border;
    Q_DISABLE_COPY(QDeclarativeGeoMapCircleObject)
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGeoMapCircleObject));

#endif
