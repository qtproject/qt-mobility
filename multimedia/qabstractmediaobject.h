#ifndef QABSTRACTMEDIAOBJECT_H
#define QABSTRACTMEDIAOBJECT_H

#include <QtCore/qobject.h>

class QAbstractMediaService;


class QAbstractMediaObjectPrivate;

class QAbstractMediaObject : public QObject
{
    Q_OBJECT

public:
    virtual QAbstractMediaService* service() const = 0;

protected:
    QAbstractMediaObject(QObject *parent);
};


#endif  // QABSTRACTMEDIAOBJECT_H
