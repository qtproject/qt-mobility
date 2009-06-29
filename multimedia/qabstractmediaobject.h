#ifndef QABSTRACTMEDIAOBJECT_H
#define QABSTRACTMEDIAOBJECT_H

#include <QtCore/qobject.h>

class QAbstractMediaService;


class QAbstractMediaObjectPrivate;

class QAbstractMediaObject : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QAbstractMediaObject)
public:
    virtual QAbstractMediaService* service() const = 0;

protected:
    QAbstractMediaObject(QObject *parent);
    QAbstractMediaObject(QAbstractMediaObjectPrivate &dd, QObject *parent);
};


#endif  // QABSTRACTMEDIAOBJECT_H
