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

#ifndef QVIDEOWIDGET_H
#define QVIDEOWIDGET_H

#include <QtGui/qwidget.h>

#include "qmultimediaglobal.h"

class QAbstractMediaService;

class QVideoWidgetPrivate;

class Q_MEDIA_EXPORT QVideoWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool fullscreen READ isFullscreen WRITE setFullscreen NOTIFY fullscreenChanged)
    Q_DECLARE_PRIVATE(QVideoWidget)
public:
    QVideoWidget(QAbstractMediaService *service, QWidget *parent = 0);
    ~QVideoWidget();

    bool isFullscreen() const;

    QSize sizeHint() const;

public Q_SLOTS:
    void setFullscreen(bool fullscreen);

Q_SIGNALS:
    void fullscreenChanged(bool fullscreen);

protected:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);
    void moveEvent(QMoveEvent *event);
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    Q_PRIVATE_SLOT(d_func(), void _q_overlayFullscreenChanged(bool));
    Q_PRIVATE_SLOT(d_func(), void _q_dimensionsChanged());
    Q_PRIVATE_SLOT(d_func(), void _q_fullscreenWindowDone());
};

#endif
