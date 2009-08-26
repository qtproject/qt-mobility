#ifndef QGSTREAMERMETADATAPROVIDER_H
#define QGSTREAMERMETADATAPROVIDER_H

#include "qmetadataprovidercontrol.h"

class QGstreamerPlayerSession;

class QGstreamerMetadataProvider : public QMetadataProviderControl
{
    Q_OBJECT
public:
    QGstreamerMetadataProvider( QGstreamerPlayerSession *session, QObject *parent );
    virtual ~QGstreamerMetadataProvider();

    bool metadataAvailable() const;
    bool isReadOnly() const;
    void setReadOnly(bool readonly);

    QVariant metadata(QMediaMetadata::Key key) const;
    QVariant metadata(const QString &key) const ;
    void setMetadata(QMediaMetadata::Key key, const QVariant &value);
    void setMetadata(const QString &key, const QVariant &value);

private slots:
    void updateTags();

private:
    QGstreamerPlayerSession *m_session;
};

#endif // QGSTREAMERMETADATAPROVIDER_H
