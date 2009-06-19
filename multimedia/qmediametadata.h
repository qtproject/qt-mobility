#ifndef QMEDIAMETADATA_H
#define QMEDIAMETADATA_H

#include <QtCore/qobject.h>

class QAbstractMediaObject;
class QMediaMetadataPrivate;

class QMediaMetadata : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool metaDataAvailable READ metaDataAvailable NOTIFY metaDataAvailabilityChanged)

public:
    QMediaMetadata(QAbstractMediaObject *mediaObject);
    ~QMediaMetadata();

    bool metaDataAvailable() const;

    QList<QString> availableMetadata() const;
    QVariant valueFor(QString const &name) const;

Q_SIGNALS:
    void metaDataAvailabilityChanged(bool metaDataAvailable);

private:
    Q_DECLARE_PRIVATE(QMediaMetadata)
};


#endif  // QMEDIAMETADATA_H
