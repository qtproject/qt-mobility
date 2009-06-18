#ifndef QABSTRACTMEDIAOBJECT_H
#define QABSTRACTMEDIAOBJECT_H

class QAbstractMediaObject : public QObject
{
    Q_OBJECT

public:
    virtual QAbstractMediaService* service() const = 0;

protected:
    QAbstractMediaObject(QAbstractMediaObjectPrivate &dd, QObject *parent);
};


#endif  // QABSTRACTMEDIAOBJECT_H
