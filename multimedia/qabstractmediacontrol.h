#ifndef QABSTRACTMEDIACONTROL_H
#define QABSTRACTMEDIACONTROL_H

#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtCore/qvariant.h>


class QAbstractMediaControlPrivate;

class QAbstractMediaControl : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int notifyInterval READ notifyInterval WRITE setNotifyInterval NOTIFY notifyIntervalChanged)
    Q_DECLARE_PRIVATE(QAbstractMediaControl)
public:
    ~QAbstractMediaControl();

    virtual void setNotifyObject(QObject* userObject);

    int notifyInterval() const;
    void setNotifyInterval(int milliSeconds);

    void addPropertyWatch(QString const& name);
    void removePropertyWatch(QString const& name);

Q_SIGNALS:
    void notifyIntervalChanged(int milliSeconds);

protected:
    QAbstractMediaControl(QObject *parent);

    virtual void changePropertyValue(const char *name, QVariant const &value);
    virtual bool propertyValueChanged(const char *name, QVariant const &value);
    virtual void notifyPropertyValueChanged(const char* name, QVariant const &value);

private Q_SLOTS:
    void notifyCheck();
};


#endif  // QABSTRACTMEDIACONTROL_H
