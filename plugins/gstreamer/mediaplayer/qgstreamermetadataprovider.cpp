#include "qgstreamermetadataprovider.h"
#include "qgstreamerplayersession.h"
#include <QDebug>

QGstreamerMetadataProvider::QGstreamerMetadataProvider(QGstreamerPlayerSession *session, QObject *parent)
    :QMetadataProviderControl(parent), m_session(session)
{
    connect(m_session, SIGNAL(tagsChanged()), SLOT(updateTags()));
}

QGstreamerMetadataProvider::~QGstreamerMetadataProvider()
{
}

bool QGstreamerMetadataProvider::metadataAvailable() const
{
    return !m_session->tags().isEmpty();
}

bool QGstreamerMetadataProvider::isReadOnly() const
{
    return true;
}

void QGstreamerMetadataProvider::setReadOnly(bool readonly)
{
    Q_UNUSED(readonly);
}

QList<QString> QGstreamerMetadataProvider::availableMetadata() const
{
    return m_session->tags().keys();
}

QVariant QGstreamerMetadataProvider::metadata(QString const &name) const
{
    return m_session->tags().value(name);
}

void QGstreamerMetadataProvider::setMetadata(QString const &name, QVariant const &value)
{
    Q_UNUSED(name);
    Q_UNUSED(value);
}

void QGstreamerMetadataProvider::updateTags()
{
    emit metadataChanged();
}
