/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef S60VIDEOOUTPUTFACTORY_H
#define S60VIDEOOUTPUTFACTORY_H

#include <qmediacontrol.h>
#include <QtCore/QVector>

QT_USE_NAMESPACE

class S60EglExtensions;

class S60VideoOutputFactory : public QObject
{
public:
    S60VideoOutputFactory(QObject *parent = 0);
    ~S60VideoOutputFactory();

    QMediaControl *requestControl(const char *name);
    void releaseControl(QMediaControl *control);

    static const char *eglRenderingAllowedPropertyName();

private:
    bool isEglRenderingAllowed() const;

private:
    struct ControlData
    {
        QString name;
        QMediaControl *control;
        int refCount;
    };
    QVector<ControlData> m_data;
    S60EglExtensions *m_eglExtensions;
};

#endif // S60VIDEOOUTPUTFACTORY_H

