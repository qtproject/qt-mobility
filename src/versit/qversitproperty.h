/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#ifndef QVERSITPROPERTY_H
#define QVERSITPROPERTY_H

#include "qmobilityglobal.h"
#include "qversitdocument.h"

#include <QString>
#include <QStringList>
#include <QMultiHash>
#include <QByteArray>
#include <QSharedDataPointer>

QT_BEGIN_NAMESPACE
class QVariant;
QT_END_NAMESPACE

QTM_BEGIN_NAMESPACE
class QVersitPropertyPrivate;

class Q_VERSIT_EXPORT QVersitProperty
{
public:
    QVersitProperty();
    QVersitProperty(const QVersitProperty& other);
    ~QVersitProperty();

    QVersitProperty& operator=(const QVersitProperty& other);
    bool operator==(const QVersitProperty& other) const;
    bool operator!=(const QVersitProperty& other) const;

    void setGroups(const QStringList& groups);
    QStringList groups() const;

    void setName(const QString& name);
    QString name() const;

    void insertParameter(const QString& name, const QString& value);
    void removeParameter(const QString& name, const QString& value);
    void removeParameters(const QString& name);

    void setParameters(const QMultiHash<QString,QString>& parameters);
    QMultiHash<QString,QString> parameters() const;

    void setValue(const QVariant& value);
    QVariant variantValue() const;
    template <typename T> T value() const
    {
        return variantValue().value<T>();
    }
    QString value() const;

    bool isEmpty() const;
    void clear();

private:

    QSharedDataPointer<QVersitPropertyPrivate> d;
};

Q_VERSIT_EXPORT uint qHash(const QVersitProperty& key);
#ifndef QT_NO_DEBUG_STREAM
Q_VERSIT_EXPORT QDebug operator<<(QDebug dbg, const QVersitProperty& property);
#endif

QTM_END_NAMESPACE

#endif // QVERSITPROPERTY_H
