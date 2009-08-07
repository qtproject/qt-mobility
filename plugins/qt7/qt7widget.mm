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

#include <QMacCocoaViewContainer>
#include <QResizeEvent>

#include "qt7widget.h"

#import <CoreFoundation/CoreFoundation.h>
#import <QTKit/QTMovieView.h>

class Qt7WidgetPrivate
{
public:
    QTMovieView *movieView;
    QMacCocoaViewContainer* viewContainer;
};

Qt7Widget::Qt7Widget():
    d(new Qt7WidgetPrivate)
{
}

Qt7Widget::~Qt7Widget()
{
}

void Qt7Widget::setMovie(void *movie)
{
    QRect   f = rect();
    NSRect  frame = NSMakeRect(f.x(), f.y(), f.width(), f.height());
    d->movieView = [[QTMovieView alloc] initWithFrame:frame];
    d->viewContainer = new QMacCocoaViewContainer(d->movieView, this);

    [d->movieView setMovie:(QTMovie*)movie];
    [d->movieView setNeedsDisplay:TRUE];
}

void Qt7Widget::resizeEvent(QResizeEvent* event)
{
    const QSize& size = event->size();

    [d->movieView setFrameSize:NSMakeSize(size.width(), size.height())];
}

