/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEORGANIZERITEMDETAIL_H
#define QDECLARATIVEORGANIZERITEMDETAIL_H

#include <QtDeclarative>

#include <QVariant>
#include "qorganizeritemdetail.h"

QTM_USE_NAMESPACE;

class QDeclarativeOrganizerItemDetail : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString definitionName READ definitionName NOTIFY valueChanged)
    Q_PROPERTY(QStringList fields READ fields NOTIFY valueChanged)
    Q_PROPERTY(AccessConstraints accessConstraints READ accessConstraints NOTIFY valueChanged)

    Q_ENUMS(AccessConstraint)
    Q_FLAGS(AccessConstraints)

public:

    enum AccessConstraint {
        NoConstraint = QOrganizerItemDetail::NoConstraint,
        ReadOnly = QOrganizerItemDetail::ReadOnly,
        Irremovable = QOrganizerItemDetail::Irremovable
    };
    Q_DECLARE_FLAGS(AccessConstraints, AccessConstraint)

    explicit QDeclarativeOrganizerItemDetail(QObject* parent = 0);

    AccessConstraints accessConstraints() const;
    QString definitionName() const;

    QOrganizerItemDetail detail() const;
    void setDetail(const QOrganizerItemDetail& detail);

    Q_INVOKABLE QStringList fields() const;
    Q_INVOKABLE QString value(const QString& key) const;
    Q_INVOKABLE bool setValue(const QString& key, const QVariant& value);
    Q_INVOKABLE bool removeValue(const QString& key);

signals:
    void valueChanged();

private:
    QOrganizerItemDetail m_detail;
};

QML_DECLARE_TYPE(QDeclarativeOrganizerItemDetail)


//event time range detail
class QDeclarativeOrganizerEventTimeRange : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
public:
    QDeclarativeOrganizerEventTimeRange(QObject* parent)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }

signals:
    void valueChanged();
};

//audible reminder detail
//comment detail
//description detail
//display label detail
//email reminder detail
//guid detail
//instance origin detail
//location detail
//priority detail
//recurrence detail
//reminder detail
//timestamp detail
//type detail
//visual reminder detail
//journal time range detail
//todo progress detail
//todo time range detail
#endif // QDECLARATIVEORGANIZERITEMDETAIL_H



