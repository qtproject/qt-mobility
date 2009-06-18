#ifndef QABSTRACTMEDIACONTROL_H
#define QABSTRACTMEDIACONTROL_H


class QAbstractMediaControl : public QObject
{
    Q_OBJECT
    Q_PROPERTIES(int notifyInterval READ notifyInterval WRITE setNotifyInterval NOTIFY notifyIntervalChanged)

public:
    ~QAbstractMediaControl();

    virtual void setUserObject(QObject* userObject);
    int notifyInterval() const;
    void setNotifyInterval(int milliSeconds);

    void addPropertyWatch(QString const& name);

Q_SIGNALS:
    void notifyIntervalChanged(int milliSeconds);

protected:
    QAbstractMediaControl(QAbstractMediaControlPrivate &dd, QObject *parent);

    virtual bool propertyValueChanged(QByteArray const& name, QVariant const& value) = 0;

    virtual bool event(QEvent* e);

    QObject* userObject;
};


#endif  // QABSTRACTMEDIACONTROL_H
