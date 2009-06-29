
#include <private/qwidget_p.h>

#include "qmediaimageviewer.h"


/*!
    \class QMediaImageViewer
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

class QMediaImageViewerPrivate : public QWidgetPrivate
{
public:
};


QMediaImageViewer::QMediaImageViewer(QWidget* parent):
    QWidget(*new QMediaImageViewerPrivate, parent, 0)
{
}


QMediaImageViewer::~QMediaImageViewer()
{
}


int QMediaImageViewer::interval() const
{
    return 0;
}

QMediaImageViewer::State QMediaImageViewer::state() const
{
    return QMediaImageViewer::StoppedState;
}

QMediaSource QMediaImageViewer::mediaSource() const
{
    return QMediaSource();
}

//public Q_SLOTS:
void QMediaImageViewer::setMediaSource(QMediaSource mediaSource)
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

