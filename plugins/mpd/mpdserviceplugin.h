
#ifndef MPDSERVICEPLUGIN_H
#define MPDSERVICEPLUGIN_H


#include <qmediaserviceproviderplugin.h>


class MpdServicePlugin : public QMediaServiceProviderPlugin
{
    Q_OBJECT

public:
    QStringList keys() const;
    QMediaServiceProvider* create(QString const& key);
};



#endif  // MPDSERVICEPLUGIN_H
