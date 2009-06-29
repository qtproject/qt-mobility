
#include <QtCore/private/qobject_p.h>

#include "qmediametadata.h"
#include "qabstractmediaobject.h"


/*!
    \class QMediaMetadata
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

class QMediaMetadataPrivate : public QObjectPrivate
{
public:
};


QMediaMetadata::QMediaMetadata(QAbstractMediaObject *mediaObject)
        : QObject(*new QMediaMetadataPrivate, mediaObject)
{
}

QMediaMetadata::~QMediaMetadata()
{
}

bool QMediaMetadata::metaDataAvailable() const
{
    return false;
}

QList<QString> QMediaMetadata::availableMetadata() const
{
    return QList<QString>();
}

QVariant QMediaMetadata::valueFor(QString const &name) const
{
    return QVariant();
}
