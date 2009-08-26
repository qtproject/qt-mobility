#ifndef QGSTREAMERMETADATAPROVIDER_H
#define QGSTREAMERMETADATAPROVIDER_H

#include "qmetadataprovidercontrol.h"

#include <Phonon/MediaObject>

class QPhononPlayerSession;

class QPhononMetadataProvider : public QMetadataProviderControl
{
    Q_OBJECT
public:
    QPhononMetadataProvider( Phonon::MediaObject *session, QObject *parent );
    virtual ~QPhononMetadataProvider();

    bool metadataAvailable() const;
    bool isReadOnly() const;
    void setReadOnly(bool readonly);

    QVariant metadata(QMediaMetadata::Key key) const;
    void setMetadata(QMediaMetadata::Key key, const QVariant &value);

    QVariant extendedMetadata(const QString &key) const;
    void setExtendedMetadata(const QString &key, const QVariant &value);

Q_SIGNALS:
    void metadataChanged();
    void readOnlyChanged(bool readOnly);

private slots:
    void updateTags();

private:
    Phonon::MediaObject *m_session;
    bool m_metadataAvailable;
};

#endif // QGSTREAMERMETADATAPROVIDER_H
