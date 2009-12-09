/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgstreamermediacontainercontrol.h"
#include <gst/gst.h>

QGstreamerMediaContainerControl::QGstreamerMediaContainerControl(QObject *parent)
    :QMediaContainerControl(parent)
{
    QList<QByteArray> formatCandidates;
    formatCandidates << "matroska" << "ogg" << "quicktime" << "mp4" << "avi" << "3gpp";
    formatCandidates << "flv" << "wav" << "amr" << "asf" << "dv" << "gif";
    formatCandidates << "mpeg" << "vob" << "mpegts" << "3g2" << "3gp";

    m_elementNames["matroska"] = "matroskamux";
    m_elementNames["ogg"] = "oggmux";
    m_elementNames["quicktime"] = "qtmux";
    m_elementNames["mp4"] = "mp4mux";
    m_elementNames["avi"] = "avimux";
    m_elementNames["3gpp"] = "gppmux";
    m_elementNames["flv"] = "flvmux";
    m_elementNames["wav"] = "wavenc";
    m_elementNames["amr"] = "ffmux_amr";
    m_elementNames["asf"] = "ffmux_asf";
    m_elementNames["dv"] = "ffmux_dv";
    m_elementNames["gif"] = "ffmux_gif";
    m_elementNames["mpeg"] = "ffmux_mpeg";
    m_elementNames["vob"] = "ffmux_vob";
    m_elementNames["mpegts"] = "ffmux_mpegts";
    m_elementNames["3g2"] = "ffmux_3g2";
    m_elementNames["3gp"] = "ffmux_3gp";


    foreach( const QByteArray& formatName, formatCandidates ) {
        QByteArray elementName = m_elementNames[formatName];
        GstElementFactory *factory = gst_element_factory_find(elementName.constData());
        if (factory) {
            m_supportedContainers.append(formatName);
            const gchar *descr = gst_element_factory_get_description(factory);
            m_containerDescriptions.insert(formatName, QString::fromUtf8(descr));

            gst_object_unref(GST_OBJECT(factory));
        }
    }

    if (!m_supportedContainers.isEmpty())
        setFormat(m_supportedContainers[0]);
}


