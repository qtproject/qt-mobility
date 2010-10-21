/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#ifndef QDECLARATIVEOPENMETAOBJECT_H
#define QDECLARATIVEOPENMETAOBJECT_H

#include <QtCore/QMetaObject>
#include <QtCore/QObject>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

// Copied from qobject_p.h
struct QAbstractDynamicMetaObject : public QMetaObject
{
    virtual ~QAbstractDynamicMetaObject() {}
    virtual int metaCall(QMetaObject::Call, int _id, void **) { return _id; }
    virtual int createProperty(const char *, const char *) { return -1; }
};


class QDeclarativeOpenMetaObjectPrivate;
class QDeclarativeOpenMetaObject : public QAbstractDynamicMetaObject
{
public:
    QDeclarativeOpenMetaObject(QObject *);

    ~QDeclarativeOpenMetaObject();

    virtual void getValue(int id, void **a);
    virtual void setValue(int id, void **a);

    virtual int createProperty(const char *,  const char *);

    QObject *object() const;


protected:
    virtual int metaCall(QMetaObject::Call _c, int _id, void **_a);

    virtual void propertyRead(int);
    virtual void propertyWrite(int);
    virtual void propertyWritten(int);

    QAbstractDynamicMetaObject *parent() const;

private:

    QDeclarativeOpenMetaObjectPrivate *d;
    friend class QDeclarativeOpenMetaObjectType;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QDECLARATIVEOPENMETAOBJECT_H
