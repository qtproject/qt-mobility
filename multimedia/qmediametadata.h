#ifndef QMEDIAMETADATA_H
#define QMEDIAMETADATA_H

#include <QObject>

// Needs to change to be attached to any media object
// Potentially needs to set data as well

class QMediaMetaData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool metaDataAvailable READ metaDataAvailable NOTIFY metaDataAvailabilityChanged)

public:
    QMediaMetaData(QMediaPlayer* mediaPlayer);
    ~QMediaMetaData();

    bool metaDataAvailable() const;

    QList<QString> availableMetaData() const;
    QVariant valueFor(QString const& name) const;

Q_SIGNALS:
    void metaDataAvailabilityChanged(bool metaDataAvailable);

private:
    Q_DECLARE_PRIVATE(QMediaMetaData)
};


#endif  // QMEDIAMETADATA_H
