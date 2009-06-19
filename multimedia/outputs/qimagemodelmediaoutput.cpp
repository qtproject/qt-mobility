#include "qimagemodelmediaoutput.h"

#include <private/qobject_p.h>

#include <qsize.h>

/*!
    \class QImageModelMediaOutputInterface
    \preliminary
    \internal
    \brief The QImageModelMediaOutputInterface provides an interface for image model media outputs.
*/

/*!
    Destroys a an image model.
*/
QImageModelMediaOutputInterface::~QImageModelMediaOutputInterface()
{
}

class QImageModelMediaOutputPrivate : public QObjectPrivate
{
public:
    QImageModelMediaOutputPrivate()
        : maximumCount(1)
        , count(0)
        , currentIndex(0)
        , maximumImages(1)
    {
    }

    int maximumCount;
    int count;
    int currentIndex;
    int maximumImages;
    QSize maximumSize;
};

/*!
    \class QImageModelMediaOutput
    \preliminary
    \brief The QImageModelMediaOutput class provides a list of images from a media object.

    \note QImageModelMediaOutput must be created by a media object and cannot be instantiated
    directly.
*/

/*!
    \enum ImageState

    Describes the state of of an image in an image model.

    \value NoImage There is no image.
    \value LoadingImage The image is being loaded.
    \value PartialImage The image is being loaded progressively.  An incomplete version of the image
    is available.
    \value CompleteImage The image is completely loaded.
*/

/*!
    Constructs a new image model media output.
*/
QImageModelMediaOutput::QImageModelMediaOutput(QObject *parent)
    : QObject(*new QImageModelMediaOutputPrivate, parent)
{
}

/*!
    Destroys an image model.
*/
QImageModelMediaOutput::~QImageModelMediaOutput()
{
}

/*!
    Returns the number of images in a model.
*/
int QImageModelMediaOutput::count() const
{
    return d_func()->count;
}

/*!
    \fn void QImageModelMediaOutput::countChanged(int images)

    Signals that the number of \a images in a model has changed.
*/

/*!
    Returns the index of the image currently selected by a media object.
*/
int QImageModelMediaOutput::currentIndex() const
{
    return d_func()->currentIndex;
}

/*!
    Sets the \a index of image currently selected by a media object.
*/
void QImageModelMediaOutput::setCurrentIndex(int index)
{
    Q_D(QImageModelMediaOutput);

    if (d->currentIndex != index)
        emit currentIndexChanged(d->currentIndex = index);
}

/*!
    \fn ImageModelMediaObject::currentIndexChanged(int index)

    Signals that the currently selected \a index has changed.
*/

/*!
    \fn QImageModelMediaOutput::image(int index) const

    Returns the image at \a index.
*/

/*!
    \fn void QImageModelMediaOutput::imageChanged(int index, const QImage &image)

    Signals that the \a image at \a index has changed.
*/

/*!
    \fn QImageModelMediaOutput::imageState(int index) const

    Returns the state of the image at \a index.
*/

/*!
    \fn QImageModelMediaOutput::imageStateChanged(int index, QImageModelMediaOutput::ImageState state)

    Signals that the \a state of the image at \a index has changed.
*/

/*!
    Inserts \a count images into a model starting at \a index.
*/
void QImageModelMediaOutput::insertImages(int index, int count)
{
    Q_D(QImageModelMediaOutput);

    d->count += count;

    emit imagesInserted(index, count);
}

/*!
    \fn QImageModelMediaOutput::imagesInserted(int index, int count);

    Signals that \a count images have been inserted in a model starting at \a index.
*/

/*!
    Removes \a count images from a model starting at \a index.
*/
void QImageModelMediaOutput::removeImages(int index, int count)
{
    Q_D(QImageModelMediaOutput);

    d->count -= count;

    emit imagesRemoved(index, count);
}

/*!
    \fn QImageModelMediaOutput::imagesRemoved(int index, int count)

    Signals that \a count images have been removed from a model starting \a index.
*/

/*!
    Returns the maximum size of images in a model.
*/
QSize QImageModelMediaOutput::maximumImageSize() const
{
    return d_func()->maximumSize;
}

/*!
    Sets the maximum \a size of images in a model.

    Images larger than the maximum size will be scaled down to fit the maximum size, or not loaded
    if scaling is not possible.
*/
void QImageModelMediaOutput::setMaximumImageSize(const QSize &size)
{
    Q_D(QImageModelMediaOutput);

    d->maximumSize = size;
}

/*!
    Returns the maximum number of images loaded by the model at one time.
*/
int QImageModelMediaOutput::maximumImagesLoaded() const
{
    return d_func()->maximumImages;
}

/*!
    Sets the \a maximum number of images loaded by the model at one time.
*/
void QImageModelMediaOutput::setMaximumImagesLoaded(int maximum)
{
    d_func()->maximumImages = maximum;
}
