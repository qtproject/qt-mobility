#ifndef QMEDIAWIDGETENDPOINT_P_H
#define QMEDIAWIDGETENDPOINT_P_H

#include "qmediawidgetendpoint.h"

#include <QtGui/private/qwidget_p.h>

class QMediaWidgetEndpointPrivate : public QWidgetPrivate
{
public:
    QMediaWidgetEndpointPrivate()
        : fullscreen(false)
    {
    }

    bool fullscreen;
};

#endif
