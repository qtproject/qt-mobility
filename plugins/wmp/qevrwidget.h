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

#ifndef QEVRWIDGET_H
#define QEVRWIDGET_H

#ifdef QWMP_EVR

#include "qmediawidgetendpoint.h"

#include <QtCore/qmetatype.h>

#include <evr.h>

class QEvrWidgetActivate;

Q_DECLARE_METATYPE(IMFActivate*)

class QEvrWidget : public QMediaWidgetEndpoint
{
    Q_OBJECT
    Q_PROPERTY(IMFActivate* activate READ activate)
public:
    QEvrWidget(QWidget *parent = 0);
    ~QEvrWidget();

    IMFActivate *activate() const;

    QSize sizeHint() const;

public Q_SLOTS:
    void setFullscreen(bool fullscreen);

protected:
    void paintEvent(QPaintEvent *event);
    void moveEvent(QMoveEvent *event);
    void resizeEvent(QResizeEvent *event);
    bool event(QEvent *event);

private:
    void setDisplayControl(IMFVideoDisplayControl *control);

    IMFVideoDisplayControl *m_display;
    QEvrWidgetActivate *m_activate;

    friend class QEvrWidgetActivate;
};

#endif

#endif
