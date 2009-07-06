#ifndef QGSTREAMERMETADATAPROVIDER_H
#define QGSTREAMERMETADATAPROVIDER_H

#include "qmetadataprovider.h"

class QGstreamerPlayerSession;

class QGstreamerMetadataProvider : public QMetadataProvider
{
    Q_OBJECT
public:
    QGstreamerMetadataProvider( QGstreamerPlayerSession *session, QObject *parent );
    virtual ~QGstreamerMetadataProvider();

    bool metadataAvailable() const;
    bool isReadOnly() const;
    void setReadOnly(bool readonly);

    QList<QString> availableMetadata() const;
    QVariant metadata(QString const &name) const;
    void setMetadata(QString const &name, QVariant const &value);

Q_SIGNALS:
    void metadataAvailabilityChanged(bool metadataAvailable);
    void readOnlyChanged(bool readOnly);

private slots:
    void updateTags();

private:
    QGstreamerPlayerSession *m_session;
};

#endif // QGSTREAMERMETADATAPROVIDER_H
