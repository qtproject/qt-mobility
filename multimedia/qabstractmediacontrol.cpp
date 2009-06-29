

#include <private/qobject_p.h>
#include <qmetaobject.h>
#include <qtimer.h>

#include "qabstractmediacontrol.h"



class QAbstractMediaControlPrivate : public QObjectPrivate
{
public:
    int         notifyInterval;
    QTimer*     notifyTimer;
    QList<QString>  notifyProperties;
    QObject*    notifyObject;
};

QAbstractMediaControl::~QAbstractMediaControl()
{
}

void QAbstractMediaControl::setNotifyObject(QObject* notifyObject)
{
    d_func()->notifyObject = notifyObject;
}

int QAbstractMediaControl::notifyInterval() const
{
    return d_func()->notifyInterval;
}

void QAbstractMediaControl::setNotifyInterval(int milliSeconds)
{
    Q_D(QAbstractMediaControl);

    if (d->notifyInterval != milliSeconds) {
        d->notifyInterval = milliSeconds;
        if (d->notifyTimer->isActive())
            d->notifyTimer->start(d->notifyInterval);
        emit notifyIntervalChanged(d->notifyInterval);
    }
}

void QAbstractMediaControl::addPropertyWatch(QString const& name)
{
    Q_D(QAbstractMediaControl);

    const bool beginNotify = d->notifyProperties.isEmpty();

    d->notifyProperties << name;

    if (beginNotify)
        d->notifyTimer->start(notifyInterval());
}

void QAbstractMediaControl::removePropertyWatch(QString const& name)
{
    Q_D(QAbstractMediaControl);

    d->notifyProperties.removeAll(name);

    if (d->notifyProperties.isEmpty())
        d->notifyTimer->stop();
}

QAbstractMediaControl::QAbstractMediaControl(QObject *parent):
    QObject(*new QAbstractMediaControlPrivate, parent)
{
    Q_D(QAbstractMediaControl);

    d->notifyTimer = new QTimer(this);
    connect(d->notifyTimer, SIGNAL(timeout()), SLOT(notifyCheck()));
}

void QAbstractMediaControl::changePropertyValue(const char *name, QVariant const& value)
{
    if (property(name) != value) {
        if (propertyValueChanged(name, value)) {
            setProperty(name, value);
            notifyPropertyValueChanged(name, value);
        }
    }
}

bool QAbstractMediaControl::propertyValueChanged(const char *name, QVariant const &value)
{
    Q_UNUSED(name)
    Q_UNUSED(value)

    return true;
}

void QAbstractMediaControl::notifyPropertyValueChanged(const char *name, QVariant const &value)
{
    Q_D(QAbstractMediaControl);

    const QMetaObject* m = d->notifyObject->metaObject();

    int pi = m->indexOfProperty(name);
    if (pi == -1)
        return;

    QMetaProperty p = m->property(pi);
    if (p.hasNotifySignal()) {
        p.notifySignal().invoke(
                d->notifyObject, QGenericArgument(QMetaType::typeName(p.userType()), value.data()));
    }
}

void QAbstractMediaControl::notifyCheck()
{
    Q_D(QAbstractMediaControl);

    const int len = d->notifyProperties.length();

    for (int i = 0; i < len; ++i) {
        const char *name = d->notifyProperties.at(i).toAscii().constData();
        notifyPropertyValueChanged(name, property(name));
    }
}
