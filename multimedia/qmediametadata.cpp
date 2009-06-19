
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


QMediaMetadata::QMediaMetaData(QAbstractMediaObject *mediaObject):
    QObject(*new QMediaMetadataPrivate, mediaObject)
{
}

QMediaMetadata::~QMediaMetaData()
{
}

bool QMediaMetadata::metaDataAvailable() const
{
}

QList<QString> QMediaMetadata::availableMetaData() const
{
}

QVariant QMediaMetadata::valueFor(QString const &name) const
{
}
