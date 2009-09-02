/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qphononmetadataprovider.h"

#include <QtCore/qstringlist.h>

#include <QDebug>

QPhononMetadataProvider::QPhononMetadataProvider(Phonon::MediaObject *session, QObject *parent)
    :QMetadataProviderControl(parent), m_session(session), m_metadataAvailable(false)
{
    connect(m_session, SIGNAL(metaDataChanged()), SLOT(updateTags()));
}

QPhononMetadataProvider::~QPhononMetadataProvider()
{
}

bool QPhononMetadataProvider::metadataAvailable() const
{
    return !m_session->metaData().isEmpty();
}

bool QPhononMetadataProvider::isReadOnly() const
{
    return true;
}

void QPhononMetadataProvider::setReadOnly(bool readonly)
{
    Q_UNUSED(readonly);
}

QVariant QPhononMetadataProvider::metadata(QMediaMetadata::Key key) const
{
    switch (key) {
    case QMediaMetadata::ContributingArtist:
        return m_session->metaData(Phonon::ArtistMetaData);
    case QMediaMetadata::AlbumTitle:
        return m_session->metaData(Phonon::AlbumMetaData);
    case QMediaMetadata::Title:
        return m_session->metaData(Phonon::TitleMetaData);
    case QMediaMetadata::Date:
        return m_session->metaData(Phonon::DateMetaData);
    case QMediaMetadata::TrackNumber:
        return m_session->metaData(Phonon::TracknumberMetaData);
    case QMediaMetadata::Description:
        return m_session->metaData(Phonon::DescriptionMetaData);
    default:
        return QVariant();
    }
}

void QPhononMetadataProvider::setMetadata(QMediaMetadata::Key key, QVariant const &value)
{
    Q_UNUSED(key);
    Q_UNUSED(value);
}

QVariant QPhononMetadataProvider::extendedMetadata(const QString &key) const
{
    Q_UNUSED(key);

    return QVariant();
}

void QPhononMetadataProvider::setExtendedMetadata(const QString &key, QVariant const &value)
{
    Q_UNUSED(key);
    Q_UNUSED(value);
}

void QPhononMetadataProvider::updateTags()
{
    emit metadataChanged();
    if (metadataAvailable() != m_metadataAvailable) {
        m_metadataAvailable = !m_metadataAvailable;
        emit metadataAvailabilityChanged(m_metadataAvailable);
    }
}
