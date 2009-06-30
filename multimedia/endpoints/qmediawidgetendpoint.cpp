#include "qmediawidgetendpoint_p.h"

/*!
    \class QMediaWidgetEndpointInterface
    \preliminary
    \internal
    \brief The QMediaWidgetEndpointInterface class provides an interface for widget media outputs.
*/

/*!
*/
QMediaWidgetEndpointInterface::~QMediaWidgetEndpointInterface()
{
}

/*!
    \class QMediaWidgetEndpoint
    \preliminary
    \brief The QMediaWidgetEndpoint class provides a QWidget media output.

    \note QMediaWidgetEndpoint must be created by a media service and cannot be instantiated
    directly.

    \sa QAbstractMediaService::createEndpoint()
*/

/*!
    Constructs a new media output widget with the given \a parent.
*/
QMediaWidgetEndpoint::QMediaWidgetEndpoint(QWidget *parent)
    : QWidget(*new QMediaWidgetEndpointPrivate, parent, 0)
{
}

/*!
    \internal
*/
QMediaWidgetEndpoint::QMediaWidgetEndpoint(QMediaWidgetEndpointPrivate &dd, QWidget *parent)
    : QWidget(dd, parent, 0)
{
}

/*!
    Destroys a media output widget.
*/
QMediaWidgetEndpoint::~QMediaWidgetEndpoint()
{
}

/*!
    Identifies if a media output widget is in full screen mode.

    Returns true if the widget is full screen, and false otherwise.
*/
bool QMediaWidgetEndpoint::isFullscreen() const
{
    return d_func()->fullscreen;
}

/*!
    Sets the \a fullscreen mode of a media output widget.
*/
void QMediaWidgetEndpoint::setFullscreen(bool fullscreen)
{
    d_func()->fullscreen = fullscreen;
}
