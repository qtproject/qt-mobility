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

    QVariant metadata(QMediaMetadata::Key key) const;
    void setMetadata(QMediaMetadata::Key key, const QVariant &value);

    QVariant extendedMetadata(QString const &name) const;
    void setExtendedMetadata(QString const &name, QVariant const &value);

Q_SIGNALS:
    void metadataChanged(const QMap<QByteArray, QVariant>&);

private:
    QMap<QByteArray, QVariant> m_values;
};

#endif // QGSTREAMERCAPTUREMETADATACONTROL_H
