/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "s60videooutpututils.h"
#include <QtCore/QDebug>

#ifdef QSYMBIANGRAPHICSSYSTEMHELPER_AVAILABLE
#   include <QtGui/qgraphicssystemhelper_symbian.h>
#else
#   ifdef PRIVATE_QTGUI_HEADERS_AVAILABLE
#       if QT_VERSION >= 0x040601 && !defined(__WINSCW__)
#           include <QtGui/private/qt_s60_p.h>
#           include <QtGui/private/qwidget_p.h>
#           define USE_PRIVATE_QTGUI_APIS
#       endif
#   else
#       define QTGUI_SUPPORT_UNAVAILABLE
#   endif
#endif

namespace S60VideoOutputUtils
{

// Helper function
void warnQtGuiSupportUnavailable()
{
    qWarning() << "QtMobility was compiled against Qt which lacked some extension functions";
    qWarning() << "Video and viewfinder may not render correctly";
}

#ifdef QSYMBIANGRAPHICSSYSTEMHELPER_AVAILABLE
QSymbianGraphicsSystemHelper::NativePaintMode helperNativePaintMode(NativePaintMode mode)
{
    QSymbianGraphicsSystemHelper::NativePaintMode result =
        QSymbianGraphicsSystemHelper::NativePaintModeDefault;
    switch (mode) {
    case Default:
        break;
    case ZeroFill:
        result = QSymbianGraphicsSystemHelper::NativePaintModeZeroFill;
        break;
    case BlitWriteAlpha:
        result = QSymbianGraphicsSystemHelper::NativePaintModeWriteAlpha;
        break;
    case Disable:
        result = QSymbianGraphicsSystemHelper::NativePaintModeDisable;
        break;
    }
    return result;
}
#endif

void setIgnoreFocusChanged(QWidget *widget)
{
#ifdef QSYMBIANGRAPHICSSYSTEMHELPER_AVAILABLE
    QSymbianGraphicsSystemHelper::setIgnoreFocusChanged(widget, true);
#endif
#ifdef USE_PRIVATE_QTGUI_APIS
    // Warning: if this flag is not set, the application may crash due to
    // CGraphicsContext being called from within the context of
    // QGraphicsVideoItem::paint(), when the video widget is shown.
    static_cast<QSymbianControl *>(widget->winId())->setIgnoreFocusChanged(true);
#endif
#ifdef QTGUI_SUPPORT_UNAVAILABLE
    Q_UNUSED(widget)
    warnQtGuiSupportUnavailable();
#endif
}

void setNativePaintMode(QWidget *widget, NativePaintMode mode)
{
#ifdef QSYMBIANGRAPHICSSYSTEMHELPER_AVAILABLE
    QSymbianGraphicsSystemHelper::setNativePaintMode(widget, helperNativePaintMode(mode));
#endif
#ifdef USE_PRIVATE_QTGUI_APIS
    QWidgetPrivate *widgetPrivate = qt_widget_private(widget->window());
    widgetPrivate->createExtra();
    QWExtra::NativePaintMode widgetMode = QWExtra::Default;
    switch (mode) {
    case Default:
        break;
    case ZeroFill:
        widgetMode = QWExtra::ZeroFill;
        break;
    case BlitWriteAlpha:
#   if QT_VERSION >= 0x040704
        widgetMode = QWExtra::BlitWriteAlpha;
#   endif
        break;
    case Disable:
        widgetMode = QWExtra::Disable;
        break;
    }
    widgetPrivate->extraData()->nativePaintMode = widgetMode;
#endif
#ifdef QTGUI_SUPPORT_UNAVAILABLE
    Q_UNUSED(widget)
    Q_UNUSED(mode)
    warnQtGuiSupportUnavailable();
#endif
}

void setNativePaintMode(WId wid, NativePaintMode mode)
{
#ifdef QSYMBIANGRAPHICSSYSTEMHELPER_AVAILABLE
    QSymbianGraphicsSystemHelper::setNativePaintMode(wid, helperNativePaintMode(mode));
#endif
#ifdef USE_PRIVATE_QTGUI_APIS
    QWidget *window = static_cast<QSymbianControl *>(wid)->widget()->window();
    setNativePaintMode(window, mode);
#endif
#ifdef QTGUI_SUPPORT_UNAVAILABLE
    Q_UNUSED(wid)
    Q_UNUSED(mode)
    warnQtGuiSupportUnavailable();
#endif
}

void setReceiveNativePaintEvents(QWidget *widget, bool enabled)
{
#ifdef QSYMBIANGRAPHICSSYSTEMHELPER_AVAILABLE
    QSymbianGraphicsSystemHelper::setReceiveNativePaintEvents(widget, enabled);
#endif
#ifdef USE_PRIVATE_QTGUI_APIS
    QWidgetPrivate *widgetPrivate = qt_widget_private(widget);
    widgetPrivate->createExtra();
    widgetPrivate->extraData()->receiveNativePaintEvents = enabled;
#endif
#ifdef QTGUI_SUPPORT_UNAVAILABLE
    Q_UNUSED(widget)
    Q_UNUSED(enabled)
    warnQtGuiSupportUnavailable();
#endif
}

} // namespace S60VideoOutputUtils

