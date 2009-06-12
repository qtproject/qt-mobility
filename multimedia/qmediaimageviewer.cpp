
#include <private/qobject_p.h>

#include "qmediaimageviewer.h"


/*!
    \class QMediaImageViewer
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

class QMediaImageViewerPrivate : public QObjectPrivate
{
public:
};


QMediaImageViewer::QMediaImageViewer(QWidget* parent):
    QWidget(*new QMediaImageViewerPrivate, parent)
{
}


QMediaImageViewer::~QMediImageViewer()
{
}


int QMediaImageViewer::interval() const
{
}

QMediaImageViewer::State QMediaImageViewer::state() const
{
}

//public Q_SLOTS:
void QMediaImageViwewer::setMediaSource(QMediaSource mediaSource)
{
}

void QMediaImageViewer::play()
{
}

void QMediaImageViewer::pause()
{
}

void QMediaImageViewer::stop()
{
}

void QMediaImageViewer::skipForward()
{
}

void QMediaImageViewer::skipBackward()
{
}

