#include "qwidgetmediaoutput_p.h"

/*!
    \class QWidgetMediaOutputInterface
    \preliminary
    \internal
    \brief The QWidgetMediaOutputInterface class provides an interface for widget media outputs.
*/

/*!
*/
QWidgetMediaOutputInterface::~QWidgetMediaOutputInterface()
{
}

/*!
    \class QWidgetMediaOutput
    \preliminary
    \brief The QWidgetMediaOutput class provides a QWidget media output.

    \note QWidgetMediaOutput must be created by a media object and cannot be instantiated
    directly.
*/

/*!
    Constructs a new media output widget with the given \a parent.
*/
QWidgetMediaOutput::QWidgetMediaOutput(QWidget *parent)
    : QWidget(*new QWidgetMediaOutputPrivate, parent, 0)
{
}

/*!
    \internal
*/
QWidgetMediaOutput::QWidgetMediaOutput(QWidgetMediaOutputPrivate &dd, QWidget *parent)
    : QWidget(dd, parent, 0)
{
}

/*!
    Destroys a media output widget.
*/
QWidgetMediaOutput::~QWidgetMediaOutput()
{
}

/*!
    Identifies if a media output widget is in full screen mode.

    Returns true if the widget is full screen, and false otherwise.
*/
bool QWidgetMediaOutput::isFullscreen() const
{
    return d_func()->fullscreen;
}

/*!
    Sets the \a fullscreen mode of a media output widget.
*/
void QWidgetMediaOutput::setFullscreen(bool fullscreen)
{
    d_func()->fullscreen = fullscreen;
}
