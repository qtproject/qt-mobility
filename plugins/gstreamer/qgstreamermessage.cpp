#include <gst/gst.h>

#include "qgstreamermessage.h"


static int wuchi = qRegisterMetaType<QGstreamerMessage>();


/*!
    \class gstreamer::QGstreamerMessage
    \internal
*/

QGstreamerMessage::QGstreamerMessage():
    m_message(0)
{
}

QGstreamerMessage::QGstreamerMessage(GstMessage* message):
    m_message(message)
{
    gst_message_ref(m_message);
}

QGstreamerMessage::QGstreamerMessage(QGstreamerMessage const& m):
    m_message(m.m_message)
{
    gst_message_ref(m_message);
}


QGstreamerMessage::~QGstreamerMessage()
{
    if (m_message != 0)
        gst_message_unref(m_message);
}

GstMessage* QGstreamerMessage::rawMessage() const
{
    return m_message;
}

QGstreamerMessage& QGstreamerMessage::operator=(QGstreamerMessage const& rhs)
{
    if (m_message != 0)
        gst_message_unref(m_message);

    if ((m_message = rhs.m_message) != 0)
        gst_message_ref(m_message);

    return *this;
}
