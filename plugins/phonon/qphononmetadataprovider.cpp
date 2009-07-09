#include "qphononmetadataprovider.h"
#include <QDebug>

QPhononMetadataProvider::QPhononMetadataProvider(Phonon::MediaObject *session, QObject *parent)
    :QMetadataProvider(parent), m_session(session)
{
    connect(m_session, SIGNAL(tagsChanged()), SLOT(updateTags()));
}

QPhononMetadataProvider::~QPhononMetadataProvider()
{
}

bool QPhononMetadataProvider::metadataAvailable() const
{
    return false;
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
    return QList<QString>();
}

QVariant QPhononMetadataProvider::metadata(QString const &name) const
{
    return QVariant();
}

void QPhononMetadataProvider::setMetadata(QString const &name, QVariant const &value)
{
    Q_UNUSED(name);
    Q_UNUSED(value);
}

void QPhononMetadataProvider::updateTags()
{    
    emit metadataChanged();
}
