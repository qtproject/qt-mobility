#ifndef QGSTREAMERCAPTUREMETADATACONTROL_H
#define QGSTREAMERCAPTUREMETADATACONTROL_H

#include <qmetadataprovidercontrol.h>

class QGstreamerCaptureMetadataControl : public QMetadataProviderControl
{
    Q_OBJECT
public:
    QGstreamerCaptureMetadataControl(QObject *parent);
    virtual ~QGstreamerCaptureMetadataControl() {};


    bool metadataAvailable() const { return true; }
    bool isReadOnly() const { return false; }

    QList<QString> availableMetadata() const;
    QVariant metadata(QString const &name) const { return m_values.value(name); }
    void setMetadata(QString const &name, QVariant const &value);

Q_SIGNALS:
    void metadataChanged();
    void metadataChanged(const QMap<QString, QVariant>&);
    void metadataAvailabilityChanged(bool available);
    void readOnlyChanged(bool readOnly);

private:
    QMap<QString, QVariant> m_values;
};

#endif // QGSTREAMERCAPTUREMETADATACONTROL_H
