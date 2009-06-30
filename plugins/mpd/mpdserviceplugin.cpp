
#include <QtCore/qstring.h>

#include <qmediaserviceprovider.h>


#include "mpdserviceplugin.h"
#include "mpdplayerservice.h"


class MpdProvider : public QMediaServiceProvider
{
    Q_OBJECT

public:
    QObject* createObject(const char *interface) const
    {
        if (QLatin1String(interface) == QLatin1String("com.nokia.qt.MediaPlayer/1.0"))
            return new MpdPlayerService;

        return 0;
    }
};

QStringList MpdServicePlugin::keys() const
{
    return QStringList() << "mediaplayer";
}

QMediaServiceProvider* MpdServicePlugin::create(QString const& key)
{
    if (key == "mediaplayer")
        return new MpdProvider;

    return 0;
}

#include "mpdserviceplugin.moc"

Q_EXPORT_PLUGIN2(mpd_serviceplugin, MpdServicePlugin);


