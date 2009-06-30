#ifndef QGSTREAMERMESSAGE_H
#define QGSTREAMERMESSAGE_H

#include <QMetaType>

#include <gst/gst.h>


class QGstreamerMessage
{
public:
    QGstreamerMessage();
    QGstreamerMessage(GstMessage* message);
    QGstreamerMessage(QGstreamerMessage const& m);
    ~QGstreamerMessage();

    GstMessage* rawMessage() const;

    QGstreamerMessage& operator=(QGstreamerMessage const& rhs);

private:
    GstMessage* m_message;
};

Q_DECLARE_METATYPE(QGstreamerMessage);

#endif
