#ifndef QMEDIAPROVIDERFACTORY_H
#define QMEDIAPROVIDERFACTORY_H

class QMediaServiceProvider;
class QString;

class QMediaProviderFactory
{
public:
    static QMediaServiceProvider* defaultServiceProvider(QString const &key);
};

#endif  // QMEDIAPROVIDERFACTORY_H
