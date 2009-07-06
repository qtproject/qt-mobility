/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/private/qobject_p.h>
#include <QtCore/QDebug>

#include "qmediametadata.h"
#include "qmetadataprovider.h"
#include "qabstractmediaobject.h"
#include "qabstractmediaservice.h"


/*!
    \class QMediaMetadata
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

class QMediaMetadataPrivate : public QObjectPrivate
{
public:
    QAbstractMediaService   *service;
    QMetadataProvider       *provider;
};


QMediaMetadata::QMediaMetadata(QAbstractMediaObject *mediaObject):
    QObject(*new QMediaMetadataPrivate, mediaObject)
{
    Q_D(QMediaMetadata);

    d->service = mediaObject->service();
    d->provider = qobject_cast<QMetadataProvider*>(d->service->control("com.nokia.qt.MetadataControl"));
    if (d->provider) {
        connect(d->provider, SIGNAL(metadataAvailabilityChanged(bool)), SIGNAL(metadataAvailabilityChanged(bool)));
        connect(d->provider, SIGNAL(readOnlyChanged(bool)), SIGNAL(readOnlyChanged(bool)));
    } else {
        qWarning() << "No provider for media metadata";
    }
}

QMediaMetadata::~QMediaMetadata()
{
}

bool QMediaMetadata::metadataAvailable() const
{
    Q_D(const QMediaMetadata);

    if (d->provider == 0)
        return false;

    return d->provider->metadataAvailable();
}

bool QMediaMetadata::isReadOnly() const
{
    Q_D(const QMediaMetadata);

    if (d->provider == 0)
        return true;

    return d->provider->isReadOnly();
}

QList<QString> QMediaMetadata::availableMetadata() const
{
    Q_D(const QMediaMetadata);

    if (d->provider == 0)
        return QList<QString>();

    return d->provider->availableMetadata();
}

QVariant QMediaMetadata::metadata(QString const &name) const
{
    Q_D(const QMediaMetadata);

    if (d->provider == 0)
        return QVariant();

    return d->provider->metadata(name);
}

void QMediaMetadata::setMetadata(QString const &name, QVariant const &value)
{
    Q_D(QMediaMetadata);

    if (d->provider != 0 && !d->provider->isReadOnly())
        d->provider->setMetadata(name, value);
}

