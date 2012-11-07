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


#ifndef QORGANIZERITEMMANAGERDATAHOLDER_H
#define QORGANIZERITEMMANAGERDATAHOLDER_H

#include <QMap>
#include <QMultiMap>
#include <QList>
#include <QString>
#include <QDebug>

#include "qorganizermanager.h"

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

QTM_BEGIN_NAMESPACE
class QOrganizerItem;
class QOrganizerManagerDataHolder
{
public:
    QOrganizerManagerDataHolder()
    {
        QStringList managerNames = QOrganizerManager::availableManagers();

        foreach(const QString& mgr, managerNames) {
            // Don't bother with these
            if (mgr == "memory" || mgr == "invalid" || mgr == "testdummy" || mgr == "maliciousplugin" || mgr == "skeleton")
                continue;
            QMap<QString, QString> params;
            QString mgrUri = QOrganizerManager::buildUri(mgr, params);
            QOrganizerManager* cm = QOrganizerManager::fromUri(mgrUri);
            if (cm) {
                qDebug() << "Saving items for" << mgrUri;
                QList<QOrganizerItem> items = cm->itemsForExport(QDateTime(), QDateTime(), QOrganizerItemFilter());
                savedItems.insert(cm->managerName(),items);
                QList<QOrganizerItemId> ids;
                foreach(const QOrganizerItem& item, items)
                    ids.append(item.id());
                cm->removeItems(ids);
                delete cm;
            }
        }
    }

    ~QOrganizerManagerDataHolder()
    {
        foreach(const QString& mgrUri, savedItems.keys()) {
            QOrganizerManager* cm = QOrganizerManager::fromUri(mgrUri);
            if (cm) {
                qDebug() << "Restoring items for" << mgrUri;
                QList<QOrganizerItem> items = savedItems.value(mgrUri);
                cm->saveItems(&items);
                // XXX this doesn't restore relationships..
                delete cm;
            }
        }
    }
private:
    QMap<QString, QList<QOrganizerItem> > savedItems;
};

QTM_END_NAMESPACE

#endif

