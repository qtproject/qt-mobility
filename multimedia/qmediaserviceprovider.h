#ifndef QMEDIASERVICEPROVIDER_H
#define QMEDIASERVICEPROVIDER_H

#include <QtCore/qobject.h>


class QMediaServiceProvider : public QObject
{
    Q_OBJECT

public
    QObject* createObject(const char *interface) const = 0;
};

extern QmediaServiceProvider* defaultServiceProvider(const char *type);


#endif  // QMEDIASERVICEPROVIDER_H
