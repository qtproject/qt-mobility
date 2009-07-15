#include "qphononmetadataprovider.h"

#include <QtCore/qstringlist.h>

#include <QDebug>

QPhononMetadataProvider::QPhononMetadataProvider(Phonon::MediaObject *session, QObject *parent)
    :QMetadataProvider(parent), m_session(session), m_metadataAvailable(false)
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

QList<QString> QPhononMetadataProvider::availableMetadata() const
{
    return m_session->metaData().keys();
}

QVariant QPhononMetadataProvider::metadata(QString const &name) const
{
    return m_session->metaData(name.toUpper());
}

void QPhononMetadataProvider::setMetadata(QString const &name, QVariant const &value)
{
    Q_UNUSED(name);
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
