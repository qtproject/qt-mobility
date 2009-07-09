#ifndef QGSTREAMERMETADATAPROVIDER_H
#define QGSTREAMERMETADATAPROVIDER_H

#include "qmetadataprovider.h"
#include <phonon>

class QPhononPlayerSession;

class QPhononMetadataProvider : public QMetadataProvider
{
    Q_OBJECT
public:
    QPhononMetadataProvider( Phonon::MediaObject *session, QObject *parent );
    virtual ~QPhononMetadataProvider();

    bool metadataAvailable() const;
    bool isReadOnly() const;
    void setReadOnly(bool readonly);

    QList<QString> availableMetadata() const;
    QVariant metadata(QString const &name) const;
    void setMetadata(QString const &name, QVariant const &value);

Q_SIGNALS:
    void metadataChanged();
    void readOnlyChanged(bool readOnly);

private slots:
    void updateTags();

private:
    Phonon::MediaObject *m_session;
};

#endif // QGSTREAMERMETADATAPROVIDER_H
