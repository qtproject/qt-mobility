#include "qimagemediaoutput.h"

#include <private/qobject_p.h>

#include <qsize.h>

/*!
    \class QImageMediaOutputInterface
    \preliminary
    \internal
    \brief The QImageMediaOutputInterface provides an interface for image model media outputs.
*/

/*!
    Destroys a an image model.
*/
QImageMediaOutputInterface::~QImageMediaOutputInterface()
{
}

class QImageMediaOutputPrivate : public QObjectPrivate
{
public:
    QImageMediaOutputPrivate()
        : maximumCount(1)
        , count(0)
        , currentIndex(0)
        , maximumImages(1)
    {
    }

    QImageMediaOutput::State state;
    QImage image;
    QSize maximumSize;
};

/*!
    \class QImageMediaOutput
    \preliminary
    \brief The QImageMediaOutput class provides an image from a media object.

    \note QImageMediaOutput must be created by a media object and cannot be instantiated
    directly.
*/

/*!
    \enum ImageState

    Describes the state of of an image in an media output.

    \value NoImage There is no image.
    \value LoadingImage The image is being loaded.
    \value PartialImage The image is being loaded progressively.  An incomplete version of the image
    is available.
    \value CompleteImage The image is completely loaded.
*/

/*!
    Constructs a new image media output.
*/
QImageMediaOutput::QImageMediaOutput(QObject *parent)
    : QObject(*new QImageMediaOutputPrivate, parent)
{
}

/*!
    Destroys an image media output.
*/
QImageMediaOutput::~QImageMediaOutput()
{
}

/*!
    \fn QImageMediaOutput::image() const

    Returns the image.
*/
QImage QImageMediaOutput::image() const
{
    return d_func()->image;
}

/*!
    \fn void QImageMediaOutput::imageChanged(const QImage &image)

    Signals that the \a image has changed.
*/

/*!
    Returns the state of the image.
*/
QImageMediaOutput::State QImageMediaOutput::state() const
{
    return d_func()->state;
}

/*!
    \fn QImageMediaOutput::stateChanged(QImageMediaOutput::State state)

    Signals that the \a state of the image has changed.
*/

bool QImageMediaOutput::hasImage() const
{
    return d_func()->state != NoImage;
}

/*!
    Sets an output's \a image and it's \a state.
*/
void QImageMediaOutput::setImage(const QImage &image, State state)
{
    Q_D(QImageMediaOutput);

    d->image = image;
    d->state = state;

    emit imageChanged(d->image);
    emit stateChanged(d->state);
}

/*!
    Returns the maximum size of a loaded image.
*/
QSize QImageMediaOutput::maximumSize() const
{
    return d_func()->maximumSize;
}

/*!
    Sets the maximum \a size of a loaded image.

    Images larger than the maximum size will be scaled down to fit the maximum size, or not loaded
    if scaling is not possible.
*/
void QImageMediaOutput::setMaximumSize(const QSize &size)
{
    Q_D(QImageMediaOutput);

    d->maximumSize = size;
}
