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


#ifndef QCONTACTMANAGER_P_H
#define QCONTACTMANAGER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QMap>
#include <QMultiMap>
#include <QList>
#include <QString>

#include "qorganizermanager.h"
#include "qorganizermanagerengine.h"

QTM_BEGIN_NAMESPACE

class QOrganizerManagerEngineFactory;

/* Data and stuff that is shared amongst all backends */
class QOrganizerManagerData
{
public:
    QOrganizerManagerData()
        : m_engine(0),
        m_error(QOrganizerManager::NoError)
    {
    }

    ~QOrganizerManagerData()
    {
        delete m_engine;
    }

    void createEngine(const QString& managerName, const QMap<QString, QString>& parameters);
    static QOrganizerManagerEngine* engine(const QOrganizerManager* manager);
    static QOrganizerItemEngineId* createEngineItemId(const QString& managerName, const QMap<QString, QString>& parameters, const QString& engineIdString);
    static QOrganizerCollectionEngineId* createEngineCollectionId(const QString& managerName, const QMap<QString, QString>& parameters, const QString& engineIdString);

    QOrganizerManagerEngine* m_engine;
    QOrganizerManager::Error m_error;
    QMap<int, QOrganizerManager::Error> m_errorMap;

    /* Manager plugins */
    static QHash<QString, QOrganizerManagerEngineFactory*> m_engines;
    static bool m_discovered;
    static bool m_discoveredStatic;
    static QStringList m_pluginPaths;
    static void loadFactories();
    static void loadStaticFactories();

private:
    Q_DISABLE_COPY(QOrganizerManagerData)
};


QTM_END_NAMESPACE

#endif
