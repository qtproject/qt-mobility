#ifndef QWIDGETMEDIAOUTPUT_P_H
#define QWIDGETMEDIAOUTPUT_P_H

#include "qwidgetmediaoutput.h"

#include <QtGui/private/qwidget_p.h>

class QWidgetMediaOutputPrivate : public QWidgetPrivate
{
public:
    QWidgetMediaOutputPrivate()
        : fullscreen(false)
    {
    }

    bool fullscreen;
};

#endif
