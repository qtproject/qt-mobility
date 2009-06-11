#ifndef QABSTRACTMEDIAOBJECT_H
#define QABSTRACTMEDIAOBJECT_H

class QAbstractMediaObject : public QObject
{
public:
    virtual void setUserObject(QObject* userObject);

protected:
    virtual bool propertyValueChanged(QByteArray const& name, QVariant const& value) = 0;

    virtual bool event(QEvent* e);

    QObject* userObject;
};


#endif  // QABSTRACTMEDIAOBJECT_H
