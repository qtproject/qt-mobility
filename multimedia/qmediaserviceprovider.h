#ifndef QMEDIASERVICEPROVIDER_H
#define QMEDIASERVICEPROVIDER_H

#include <QtCore/qobject.h>

class QMediaServiceProvider : public QObject
{
    Q_OBJECT
public:
    virtual QObject* createObject(const char *interface) const = 0;
};

extern QMediaServiceProvider *defaultServiceProvider(const char *type);

#endif  // QMEDIASERVICEPROVIDER_H
