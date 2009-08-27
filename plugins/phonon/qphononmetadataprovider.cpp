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
