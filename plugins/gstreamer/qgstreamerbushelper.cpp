#include <QMap>
#include <QTimer>

#include "qgstreamerbushelper.h"


#ifndef QT_NO_GLIB
class QGstreamerBusHelperPrivate : public QObject
{
    Q_OBJECT

public:
    void addWatch(GstBus* bus, QGstreamerBusHelper* helper)
    {
        setParent(helper);
        m_tag = gst_bus_add_watch_full(bus, 0, busCallback, this, NULL);
        m_helper = helper;
    }

    void removeWatch(QGstreamerBusHelper* helper)
    {
        Q_UNUSED(helper);
        g_source_remove(m_tag);
    }

    static QGstreamerBusHelperPrivate* instance()
    {
        return new QGstreamerBusHelperPrivate;
    }

private:
    void processMessage(GstBus* bus, GstMessage* message)
    {
        Q_UNUSED(bus);
        emit m_helper->message(message);
    }

    static gboolean busCallback(GstBus *bus, GstMessage *message, gpointer data)
    {
        reinterpret_cast<QGstreamerBusHelperPrivate*>(data)->processMessage(bus, message);
        return TRUE;
    }

    guint       m_tag;
    QGstreamerBusHelper*  m_helper;
};

#else

class QGstreamerBusHelperPrivate : public QObject
{
    Q_OBJECT
    typedef QMap<QGstreamerBusHelper*, GstBus*>   HelperMap;

public:
    void addWatch(GstBus* bus, QGstreamerBusHelper* helper)
    {
        m_helperMap.insert(helper, bus);

        if (m_helperMap.size() == 1)
            m_intervalTimer->start();
    }

    void removeWatch(QGstreamerBusHelper* helper)
    {
        m_helperMap.remove(helper);

        if (m_helperMap.size() == 0)
            m_intervalTimer->stop();
    }

    static QGstreamerBusHelperPrivate* instance()
    {
        static QGstreamerBusHelperPrivate self;

        return &self;
    }

private slots:
    void interval()
    {
        for (HelperMap::iterator it = m_helperMap.begin(); it != m_helperMap.end(); ++it) {
            GstMessage* message;

            while ((message = gst_bus_poll(it.value(), GST_MESSAGE_ANY, 0)) != 0) {
                emit it.key()->message(message);
                gst_message_unref(message);
            }

            emit it.key()->message(QGstreamerMessage());
        }
    }

private:
    QGstreamerBusHelperPrivate()
    {
        m_intervalTimer = new QTimer(this);
        m_intervalTimer->setInterval(250);

        connect(m_intervalTimer, SIGNAL(timeout()), SLOT(interval()));
    }

    HelperMap   m_helperMap;
    QTimer*     m_intervalTimer;
};
#endif


/*!
    \class gstreamer::QGstreamerBusHelper
    \internal
*/

QGstreamerBusHelper::QGstreamerBusHelper(GstBus* bus, QObject* parent):
    QObject(parent),
    d(QGstreamerBusHelperPrivate::instance())
{
    d->addWatch(bus, this);
}

QGstreamerBusHelper::~QGstreamerBusHelper()
{
    d->removeWatch(this);
}

#include "qgstreamerbushelper.moc"
