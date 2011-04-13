/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qdeclarativendefrecord.h"

#include <QtCore/QMap>
#include <QtCore/QRegExp>

QTM_BEGIN_NAMESPACE

static QMap<QString, const QMetaObject *> registeredNdefRecordTypes;

class QDeclarativeNdefRecordPrivate
{
public:
    QNdefRecord record;
};

static QString urnForRecordType(QNdefRecord::TypeNameFormat typeNameFormat, const QByteArray &type)
{
    switch (typeNameFormat) {
    case QNdefRecord::NfcRtd:
        return QLatin1String("urn:nfc:wkt:") + type;
    case QNdefRecord::ExternalRtd:
        return QLatin1String("urn:nfc:ext:") + type;
    case QNdefRecord::Mime:
        return QLatin1String("urn:nfc:mime:") + type;
    default:
        return QString();
    }
}

void qRegisterNdefRecordTypeHelper(const QMetaObject *metaObject,
                                   QNdefRecord::TypeNameFormat typeNameFormat,
                                   const QByteArray &type)
{
    registeredNdefRecordTypes.insert(urnForRecordType(typeNameFormat, type), metaObject);
}

QDeclarativeNdefRecord *qNewDeclarativeNdefRecordForNdefRecord(const QNdefRecord &record)
{
    const QString urn = urnForRecordType(record.typeNameFormat(), record.type());

    QMapIterator<QString, const QMetaObject *> i(registeredNdefRecordTypes);
    while (i.hasNext()) {
        i.next();

        QRegExp ex(i.key());
        if (!ex.exactMatch(urn))
            continue;

        const QMetaObject *metaObject = i.value();
        if (!metaObject)
            continue;

        return static_cast<QDeclarativeNdefRecord *>(metaObject->newInstance(
            Q_ARG(QNdefRecord, record), Q_ARG(QObject *, 0)));
    }

    return new QDeclarativeNdefRecord(record);
}

QDeclarativeNdefRecord::QDeclarativeNdefRecord(QObject *parent)
:   QObject(parent), d_ptr(new QDeclarativeNdefRecordPrivate)
{
}

QDeclarativeNdefRecord::QDeclarativeNdefRecord(const QNdefRecord &record, QObject *parent)
:   QObject(parent), d_ptr(new QDeclarativeNdefRecordPrivate)
{
    d_ptr->record = record;
}

QString QDeclarativeNdefRecord::recordType() const
{
    Q_D(const QDeclarativeNdefRecord);

    if (d->record.typeNameFormat() == QNdefRecord::Empty)
        return QString();

    return urnForRecordType(d->record.typeNameFormat(), d->record.type());
}

void QDeclarativeNdefRecord::setRecordType(const QString &type)
{
    if (type == recordType())
        return;

    Q_D(QDeclarativeNdefRecord);

    if (type.startsWith(QLatin1String("urn:nfc:wkt:"))) {
        d->record.setTypeNameFormat(QNdefRecord::NfcRtd);
        d->record.setType(type.mid(12).toUtf8());
    } else if (type.startsWith(QLatin1String("urn:nfc:ext:"))) {
        d->record.setTypeNameFormat(QNdefRecord::ExternalRtd);
        d->record.setType(type.mid(12).toUtf8());
    } else if (type.startsWith(QLatin1String("urn:nfc:mime:"))) {
        d->record.setTypeNameFormat(QNdefRecord::Mime);
        d->record.setType(type.mid(13).toUtf8());
    } else {
        qWarning("Don't know how to decode NDEF type %s\n", qPrintable(type));
    }

    emit recordTypeChanged();
}

QNdefRecord QDeclarativeNdefRecord::record() const
{
    Q_D(const QDeclarativeNdefRecord);

    return d->record;
}

void QDeclarativeNdefRecord::setRecord(const QNdefRecord &record)
{
    Q_D(QDeclarativeNdefRecord);

    d->record = record;
}

#include "moc_qdeclarativendefrecord.cpp"

QTM_END_NAMESPACE
