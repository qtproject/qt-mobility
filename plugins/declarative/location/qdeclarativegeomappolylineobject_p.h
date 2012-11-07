/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEGEOMAPPOLYLINEOBJECT_H
#define QDECLARATIVEGEOMAPPOLYLINEOBJECT_H

#include "qdeclarativegeomapobject_p.h"
#include "qdeclarativecoordinate_p.h"
#include "qdeclarativegeomapobjectborder_p.h"
#include "qgeomappolylineobject.h"

#include <QDeclarativeListProperty>

class QColor;
class QBrush;

QTM_BEGIN_NAMESPACE

class QDeclarativeGeoMapPolylineObject : public QDeclarativeGeoMapObject
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeCoordinate> path READ declarativePath NOTIFY pathChanged)
    Q_PROPERTY(QDeclarativeGeoMapObjectBorder* border READ border)

public:
    QDeclarativeGeoMapPolylineObject(QDeclarativeItem *parent = 0);
    ~QDeclarativeGeoMapPolylineObject();

    Q_INVOKABLE void addCoordinate(QDeclarativeCoordinate* coordinate);
    Q_INVOKABLE void removeCoordinate(QDeclarativeCoordinate* coordinate);

    // From QDeclarativeParserStatus
    virtual void classBegin() {}
    virtual void componentComplete();

    QDeclarativeListProperty<QDeclarativeCoordinate> declarativePath();
    QDeclarativeGeoMapObjectBorder* border();

Q_SIGNALS:
    void pathChanged();

private Q_SLOTS:
    void borderColorChanged(const QColor &color);
    void borderWidthChanged(int width);

private:
    static void path_append(QDeclarativeListProperty<QDeclarativeCoordinate> *prop, QDeclarativeCoordinate *coordinate);
    static int path_count(QDeclarativeListProperty<QDeclarativeCoordinate> *prop);
    static QDeclarativeCoordinate* path_at(QDeclarativeListProperty<QDeclarativeCoordinate> *prop, int index);
    static void path_clear(QDeclarativeListProperty<QDeclarativeCoordinate> *prop);
    void pathPropertyChanged();

    QGeoMapPolylineObject* polyline_;
    QList<QDeclarativeCoordinate*> path_;
    QDeclarativeGeoMapObjectBorder border_;
    bool componentCompleted_;
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGeoMapPolylineObject));

#endif
