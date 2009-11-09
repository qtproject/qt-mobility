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

#include "qphononmetadataprovider.h"

#include <QtCore/qstringlist.h>

#include <QDebug>

QPhononMetaDataProvider::QPhononMetaDataProvider(Phonon::MediaObject *session, QObject *parent)
    :QMetaDataControl(parent), m_session(session), m_metaDataAvailable(false)
{
    connect(m_session, SIGNAL(metaDataChanged()), SLOT(updateTags()));

    m_keysMap["TITLE"] = QtMedia::Title;
    m_keysMap["ALBUM"] = QtMedia::AlbumTitle;
    m_keysMap["TRACKNUMBER"] = QtMedia::TrackNumber;
    m_keysMap["ARTIST"] = QtMedia::AlbumArtist;
    m_keysMap["PERFORMER"] = QtMedia::ContributingArtist;
    m_keysMap["COPYRIGHT"] = QtMedia::Copyright;
    m_keysMap["DESCRIPTION"] = QtMedia::Description;
    m_keysMap["GENRE"] = QtMedia::Genre;
    m_keysMap["DATE"] = QtMedia::Date;
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

QVariant QPhononMetaDataProvider::metaData(QtMedia::MetaData key) const
{
    switch (key) {
    case QtMedia::AlbumArtist:
        return m_session->metaData(Phonon::ArtistMetaData);
    case QtMedia::ContributingArtist:
        return m_session->metaData("PERFORMER");
    case QtMedia::AlbumTitle:
        return m_session->metaData(Phonon::AlbumMetaData);
    case QtMedia::Title:
        return m_session->metaData(Phonon::TitleMetaData);
    case QtMedia::Date:
        return m_session->metaData(Phonon::DateMetaData);
    case QtMedia::TrackNumber:
        return m_session->metaData(Phonon::TracknumberMetaData);
    case QtMedia::Description:
        return m_session->metaData(Phonon::DescriptionMetaData);
    default:
        return QVariant();
    }
}

void QPhononMetaDataProvider::setMetaData(QtMedia::MetaData key, QVariant const &value)
{
    Q_UNUSED(key);
    Q_UNUSED(value);
}

QList<QtMedia::MetaData> QPhononMetaDataProvider::availableMetaData() const
{
    QList<QtMedia::MetaData> res;
    QMap<QString,QString> metaData = m_session->metaData();
    QMapIterator<QString, QString> i(metaData);
     while (i.hasNext()) {
         i.next();
         QtMedia::MetaData tag = m_keysMap.value(i.key(), QtMedia::MetaData(-1));

         if (tag != -1)
             res.append(tag);
     }

     return res;
}

QVariant QPhononMetaDataProvider::extendedMetaData(const QString &key) const
{
    return m_session->metaData(key);
}

void QPhononMetaDataProvider::setExtendedMetaData(const QString &key, QVariant const &value)
{
    Q_UNUSED(key);
    Q_UNUSED(value);
}

QStringList QPhononMetaDataProvider::availableExtendedMetaData() const
{
    return m_session->metaData().keys();
}

void QPhononMetaDataProvider::updateTags()
{
    emit metaDataChanged();
    if (isMetaDataAvailable() != m_metaDataAvailable) {
        m_metaDataAvailable = !m_metaDataAvailable;
        emit metaDataAvailableChanged(m_metaDataAvailable);
    }
}
