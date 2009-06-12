
#include "qmediametadata.h"

/*!
    \class QMediaMetaData
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/


QMediaMetaData::QMediaMetaData(QMediaPlayer* mediaPlayer):
    QObject(mediaPlayer)
{
}

QMediaMetaData::~QMediaMetaData()
{
}

bool QmediMetaData::metaDataAvailable() const
{
}

QList<QString> QMediaMetaData::availableMetaData() const
{
}

QVariant QMediaMetaData::valueFor(QString const& name) const
{
}
