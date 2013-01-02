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

#include "qlandmarkmanagerenginefactory_qsparql_p.h"
#include "qlandmarkmanagerengine_qsparql_p.h"
#include <QtPlugin>

QLandmarkManagerEngineFactoryQsparql::QLandmarkManagerEngineFactoryQsparql() {}

QLandmarkManagerEngineFactoryQsparql::~QLandmarkManagerEngineFactoryQsparql() {}

QList<int> QLandmarkManagerEngineFactoryQsparql::supportedImplementationVersions() const
{
    QList<int> versions;
    versions << 1;
    return versions;
}

QLandmarkManagerEngine* QLandmarkManagerEngineFactoryQsparql::engine(const QMap<QString, QString> &parameters,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    QString filename;

    QList<QString> keys = parameters.keys();
    for (int i = 0; i < keys.size(); ++i) {
        QString key = keys.at(i);
        if (key == "filename")
            filename = parameters.value(keys.at(i));
    }

    QLandmarkManagerEngine * enginePtr = new QLandmarkManagerEngineQsparql(filename, error, errorString);
    if(*error != QLandmarkManager::NoError)
    {
        delete enginePtr;
        enginePtr =0;
        return NULL;
    }
    else {
        return enginePtr;
    }
}

QString QLandmarkManagerEngineFactoryQsparql::managerName() const
{
    return "com.nokia.qt.landmarks.engines.qsparql";
}

Q_EXPORT_PLUGIN2(qtlandmarks_qsparql, QLandmarkManagerEngineFactoryQsparql)

