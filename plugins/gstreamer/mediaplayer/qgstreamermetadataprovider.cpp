#include "qgstreamermetadataprovider.h"
#include "qgstreamerplayersession.h"
#include <QDebug>

struct QGstreamerMetadataKeyLookup
{
    QMediaMetadata::Key key;
    const char *token;
};

static const QGstreamerMetadataKeyLookup qt_gstreamerMetadataKeys[] =
{
    { QMediaMetadata::Title, GST_TAG_TITLE },
    //{ QMediaMetadata::SubTitle, 0 },
    //{ QMediaMetadata::Author, 0 },
    { QMediaMetadata::Comment, GST_TAG_COMMENT },
    { QMediaMetadata::Description, GST_TAG_DESCRIPTION },
    //{ QMediaMetadata::Category, 0 },
    { QMediaMetadata::Genre, GST_TAG_GENRE },
    //{ QMediaMetadata::Year, 0 },
    //{ QMediaMetadata::UserRating, 0 },

    { QMediaMetadata::Language, GST_TAG_LANGUAGE_CODE },

    { QMediaMetadata::Publisher, GST_TAG_ORGANIZATION },
    { QMediaMetadata::Copyright, GST_TAG_COPYRIGHT },
    //{ QMediaMetadata::ParentalRating, 0 },
    //{ QMediaMetadata::RatingOrganisation, 0 },

    // Media
    //{ QMediaMetadata::Size, 0 },
    //{ QMediaMetadata::MediaType, 0 },
    { QMediaMetadata::Duration, GST_TAG_DURATION },

    // Audio
    { QMediaMetadata::AudioBitrate, GST_TAG_BITRATE },
    { QMediaMetadata::AudioCodec, GST_TAG_AUDIO_CODEC },
    //{ QMediaMetadata::Channels, 0 },
    //{ QMediaMetadata::Frequency, 0 },

    // Music
    { QMediaMetadata::AlbumTitle, GST_TAG_ALBUM },
    { QMediaMetadata::AlbumArtist,  GST_TAG_ARTIST},
    { QMediaMetadata::ContributingArtist, GST_TAG_PERFORMER },
    { QMediaMetadata::Composer, GST_TAG_COMPOSER },
    //{ QMediaMetadata::Conductor, 0 },
    //{ QMediaMetadata::Lyrics, 0 },
    //{ QMediaMetadata::Mood, 0 },
    { QMediaMetadata::TrackNumber, GST_TAG_TRACK_NUMBER },

    //{ QMediaMetadata::CoverArtUriSmall, 0 },
    //{ QMediaMetadata::CoverArtUriLarge, 0 },

    // Image/Video
    //{ QMediaMetadata::Resolution, 0 },
    //{ QMediaMetadata::PixelAspectRatio, 0 },

    // Video
    //{ QMediaMetadata::FrameRate, 0 },
    //{ QMediaMetadata::VideoBitRate, 0 },
    { QMediaMetadata::VideoCodec, GST_TAG_VIDEO_CODEC },

    //{ QMediaMetadata::PosterUri, 0 },

    // Movie
    //{ QMediaMetadata::ChapterNumber, 0 },
    //{ QMediaMetadata::Director, 0 },
    { QMediaMetadata::LeadPerformer, GST_TAG_PERFORMER },
    //{ QMediaMetadata::Writer, 0 },

    // Photos
    //{ QMediaMetadata::CameraManufacturer, 0 },
    //{ QMediaMetadata::CameraModel, 0 },
    //{ QMediaMetadata::Event, 0 },
    //{ QMediaMetadata::Subject, 0 }
};

QGstreamerMetadataProvider::QGstreamerMetadataProvider(QGstreamerPlayerSession *session, QObject *parent)
    :QMetadataProviderControl(parent), m_session(session)
{
    connect(m_session, SIGNAL(tagsChanged()), SLOT(updateTags()));
}

QGstreamerMetadataProvider::~QGstreamerMetadataProvider()
{
}

bool QGstreamerMetadataProvider::metadataAvailable() const
{
    return !m_session->tags().isEmpty();
}

bool QGstreamerMetadataProvider::isReadOnly() const
{
    return true;
}

void QGstreamerMetadataProvider::setReadOnly(bool readonly)
{
    Q_UNUSED(readonly);
}

QVariant QGstreamerMetadataProvider::metadata(QMediaMetadata::Key key) const
{
    static const int count = sizeof(qt_gstreamerMetadataKeys) / sizeof(QGstreamerMetadataKeyLookup);

    for (int i = 0; i < count; ++i) {
        if (qt_gstreamerMetadataKeys[i].key == key) {
            return m_session->tags().value(QByteArray::fromRawData(
                    qt_gstreamerMetadataKeys[i].token, qstrlen(qt_gstreamerMetadataKeys[i].token)));
        }
    }
    return QVariant();
}

void QGstreamerMetadataProvider::setMetadata(const QString &key, QVariant const &value)
{
    Q_UNUSED(key);
    Q_UNUSED(value);
}

QVariant QGstreamerMetadataProvider::extendedMetadata(const QString &key) const
{
    return m_session->tags().value(key.toLatin1());
}

void QGstreamerMetadataProvider::setExtendedMetadata(QMediaMetadata::Key key, QVariant const &value)
{
    Q_UNUSED(key);
    Q_UNUSED(value);
}

void QGstreamerMetadataProvider::updateTags()
{
    emit metadataChanged();
}
