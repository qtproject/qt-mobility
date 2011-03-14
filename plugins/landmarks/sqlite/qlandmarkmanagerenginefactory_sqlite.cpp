/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qlandmarkmanagerenginefactory_sqlite_p.h"
#include "qlandmarkmanagerengine_sqlite_p.h"
#include <QtPlugin>

QLandmarkManagerEngineFactorySqlite::QLandmarkManagerEngineFactorySqlite() {}

QLandmarkManagerEngineFactorySqlite::~QLandmarkManagerEngineFactorySqlite() {}

QList<int> QLandmarkManagerEngineFactorySqlite::supportedImplementationVersions() const
{
    QList<int> versions;
    versions << 1;
    return versions;
}

QLandmarkManagerEngine* QLandmarkManagerEngineFactorySqlite::engine(const QMap<QString, QString> &parameters,
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

    QLandmarkManagerEngine * enginePtr = new QLandmarkManagerEngineSqlite(filename, error, errorString);
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

QString QLandmarkManagerEngineFactorySqlite::managerName() const
{
    return "com.nokia.qt.landmarks.engines.sqlite";
}

Q_EXPORT_PLUGIN2(qtlandmarks_sqlite, QLandmarkManagerEngineFactorySqlite)

