/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qimagemediaoutput.h"

#include <private/qobject_p.h>

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
        : state(QImageMediaOutput::NoImage)
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
