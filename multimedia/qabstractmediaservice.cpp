
#include "qabstractmediaservice.h"

#include <QtCore/private/qobject_p.h>
#include <QtCore/qtimer.h>

#ifndef QT_NO_VIDEOSURFACE
#include "qvideorendererendpoint.h"
#include "qvideorendererwidget_p.h"
#endif

class QAbstractMediaServicePrivate : public QObjectPrivate
{
public:
    QObject *audioOutput;
    QObject *videoOutput;
};

/*!
    \class QAbstractMediaService
*/

/*!
*/
QAbstractMediaService::QAbstractMediaService(QObject *parent):
    QObject(*new QAbstractMediaServicePrivate, parent)
{
}

/*!
    Returns the output that a media service renders audio to.
*/
QObject *QAbstractMediaService::audioOutput() const
{
    return d_func()->audioOutput;
}

/*!
    Sets the \a output that a media service renders audio to.

    The output must be a media end point constructed by the createEndpoint() function of the media
    service.

    \sa createEndpoint()
*/
void QAbstractMediaService::setAudioOutput(QObject *output)
{
    d_func()->audioOutput = output;
}

/*!
    Returns the output that a media service renders video to.
*/
QObject *QAbstractMediaService::videoOutput() const
{
    return d_func()->videoOutput;
}

/*!
    Sets the \a output that a media service renders video to.

    The output must be a media end point constructed by the createEndpoint() function of the media
    service.

    \sa createEndpoint()
*/
void QAbstractMediaService::setVideoOutput(QObject *output)
{
    d_func()->videoOutput = output;
}

/*!
    Returns a list of interfaces for media end point objects that can be created by a media service.

    \sa createEndpoint()
*/
QList<QByteArray> QAbstractMediaService::supportedEndpointInterfaces() const
{
    return QList<QByteArray>();
}

/*!
    Creates a new media end point object from the given \a interface.

    An end point object may be an input or output to a media service and can only be used with the
    service object that created it.

    A list of supported end point interfaces can be queried using the supportedEndpointInterfaces()
    function.

    \sa supportedEndpointInterface(), setAudioOutput(), setVideoOutput()
*/
QObject *QAbstractMediaService::createEndpoint(const char *interface)
{
#ifndef QT_NO_VIDEOSURFACE
    if (qstrcmp(interface, QMediaWidgetEndpointInterface_iid)) {
        QObject *object = createEndpoint(QVideoRendererEndpointInterface_iid);

        if (object) {
            QVideoRendererEndpoint *renderer = qobject_cast<QVideoRendererEndpoint *>(object);

            if (renderer)
                return new QVideoRendererWidget(renderer);

            delete object;
        }
    }
#endif
    return 0;
}
