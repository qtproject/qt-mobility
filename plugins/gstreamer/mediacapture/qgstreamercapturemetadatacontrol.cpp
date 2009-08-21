#include "qgstreamercapturemetadatacontrol.h"

QGstreamerCaptureMetadataControl::QGstreamerCaptureMetadataControl(QObject *parent)
    :QMetadataProviderControl(parent)
{
}

QList<QString> QGstreamerCaptureMetadataControl::availableMetadata() const
{
    return m_values.keys();
}

void QGstreamerCaptureMetadataControl::setMetadata(QString const &name, QVariant const &value)
{
    m_values.insert(name, value);
    emit metadataChanged();
    emit metadataChanged(m_values);
}
