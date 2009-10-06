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

QPhononMetaDataProvider::QPhononMetaDataProvider(Phonon::MediaObject *session, QObject *parent)
    :QMetaDataControl(parent), m_session(session), m_metaDataAvailable(false)
{
    connect(m_session, SIGNAL(metaDataChanged()), SLOT(updateTags()));
}

QPhononMetaDataProvider::~QPhononMetaDataProvider()
{
}

bool QPhononMetaDataProvider::isMetaDataAvailable() const
{
    return !m_session->metaData().isEmpty();
}

bool QPhononMetaDataProvider::isWritable() const
{
    return false;
}

QVariant QPhononMetaDataProvider::metaData(QMediaObject::MetaData key) const
{
    switch (key) {
    case QMediaObject::ContributingArtist:
        return m_session->metaData(Phonon::ArtistMetaData);
    case QMediaObject::AlbumTitle:
        return m_session->metaData(Phonon::AlbumMetaData);
    case QMediaObject::Title:
        return m_session->metaData(Phonon::TitleMetaData);
    case QMediaObject::Date:
        return m_session->metaData(Phonon::DateMetaData);
    case QMediaObject::TrackNumber:
        return m_session->metaData(Phonon::TracknumberMetaData);
    case QMediaObject::Description:
        return m_session->metaData(Phonon::DescriptionMetaData);
    default:
        return QVariant();
    }
}

void QPhononMetaDataProvider::setMetaData(QMediaObject::MetaData key, QVariant const &value)
{
    Q_UNUSED(key);
    Q_UNUSED(value);
}

QVariant QPhononMetaDataProvider::extendedMetaData(const QString &key) const
{
    Q_UNUSED(key);

    return QVariant();
}

void QPhononMetaDataProvider::setExtendedMetaData(const QString &key, QVariant const &value)
{
    Q_UNUSED(key);
    Q_UNUSED(value);
}

void QPhononMetaDataProvider::updateTags()
{
    emit metaDataChanged();
    if (isMetaDataAvailable() != m_metaDataAvailable) {
        m_metaDataAvailable = !m_metaDataAvailable;
        emit metaDataAvailableChanged(m_metaDataAvailable);
    }
}
