#ifndef QMEDIAPROVIDERFACTORY_H
#define QMEDIAPROVIDERFACTORY_H

class QMediaServiceProvider;

class QMediaProviderFactory
{
public:
    QMediaServiceProvider* defaultServiceProvider(QString const &key);
};

#endif  // QMEDIAPROVIDERFACTORY_H
