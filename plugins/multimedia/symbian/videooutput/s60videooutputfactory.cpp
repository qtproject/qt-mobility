/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
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

#include "s60mmtrace.h"
#include "s60videooutputfactory.h"
#include "s60videowidgetcontrol.h"
#include "s60videowindowcontrol.h"
#ifdef VIDEOOUTPUT_GRAPHICS_SURFACES
#include "s60eglextensions.h"
#include "s60videoeglrenderercontrol.h"
#endif
#include <QtCore/QVariant>

S60VideoOutputFactory::S60VideoOutputFactory(QObject *parent)
    : QObject(parent)
    , m_eglExtensions(0)
{
    TRACE("S60VideoOutputFactory::S60VideoOutputFactory" << qtThisPtr());
#ifdef VIDEOOUTPUT_GRAPHICS_SURFACES
    m_eglExtensions = S60EglExtensions::create(this);
#endif
}

S60VideoOutputFactory::~S60VideoOutputFactory()
{
    TRACE("S60VideoOutputFactory::~S60VideoOutputFactory" << qtThisPtr());
    foreach (ControlData d, m_data)
        delete d.control;
}

QMediaControl *S60VideoOutputFactory::requestControl(const char *name)
{
    TRACE("S60VideoOutputFactory::requestControl" << qtThisPtr() << "name" << name);
    QMediaControl *control = 0;
    for (int i=0; i<m_data.count(); ++i) {
        ControlData &d = m_data[i];
        if (d.name == name) {
            control = d.control;
            // TODO: protect against multithreaded usage, if QMediaService is
            // required to be thread-safe
            ++d.refCount;
        }
    }
    if (!control) {
#ifdef VIDEOOUTPUT_GRAPHICS_SURFACES
        if (qstrcmp(name, QVideoRendererControl_iid) == 0) {
            if (m_eglExtensions && isEglRenderingAllowed())
                control = new S60VideoEglRendererControl(m_eglExtensions, this);
        } else
#endif
        if (qstrcmp(name, QVideoWidgetControl_iid) == 0) {
            control = new S60VideoWidgetControl(this);
        } else if (qstrcmp(name, QVideoWindowControl_iid) == 0) {
            control = new S60VideoWindowControl(this);
        }
        if (control) {
            ControlData d;
            d.control = control;
            d.name = name;
            d.refCount = 1;
            m_data += d;
        }
    }
    return control;
}

void S60VideoOutputFactory::releaseControl(QMediaControl *control)
{
    TRACE("S60VideoOutputFactory::requestControl" << qtThisPtr() << "control" << control);
    int index = -1;
    for (int i=0; index == -1 && i<m_data.count(); ++i)
        if (m_data.at(i).control == control)
            index = i;
    if (index != -1 && --m_data[index].refCount == 0) {
        delete control;
        m_data.remove(index);
    }
}

const char *S60VideoOutputFactory::eglRenderingAllowedPropertyName()
{
    return "_q_eglRenderingAllowed";
}

bool S60VideoOutputFactory::isEglRenderingAllowed() const
{
    bool allowed = true;
    const QObject *obj = this;
    const char *propertyName = eglRenderingAllowedPropertyName();
    while (obj) {
        QVariant v = obj->property(propertyName);
        if (v.isValid()) {
            allowed = v.toBool();
            break;
        }
        obj = obj->parent();
    }
    return allowed;
}
