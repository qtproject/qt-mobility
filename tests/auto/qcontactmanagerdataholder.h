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


#ifndef QCONTACTMANAGERDATAHOLDER_H
#define QCONTACTMANAGERDATAHOLDER_H

#include <QMap>
#include <QMultiMap>
#include <QList>
#include <QString>
#include <QDebug>

#include "qcontactmanager.h"

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
class QContact;
class QContactManagerDataHolder
{
public:
    QContactManagerDataHolder()
    {
        QStringList managerNames = QContactManager::availableManagers();

        foreach(const QString& mgr, managerNames) {
            // Don't bother with these
            if (mgr == "memory" || mgr == "invalid" || mgr == "testdummy" || mgr == "maliciousplugin")
                continue;
            QMap<QString, QString> params;
            QString mgrUri = QContactManager::buildUri(mgr, params);
            QContactManager* cm = QContactManager::fromUri(mgrUri);
            if (cm) {
                qDebug() << "Saving contacts for" << mgrUri;
                QList<QContact> contacts = cm->contacts();
                savedContacts.insert(cm->managerName(),contacts);
                QList<QContactLocalId> ids;
                foreach(const QContact& contact, contacts)
                    ids.append(contact.localId());
                cm->removeContacts(ids, 0);
                delete cm;
            }
        }
    }

    ~QContactManagerDataHolder()
    {
        foreach(const QString& mgrUri, savedContacts.keys()) {
            QContactManager* cm = QContactManager::fromUri(mgrUri);
            if (cm) {
                qDebug() << "Restoring contacts for" << mgrUri;
                QList<QContact> contacts = savedContacts.value(mgrUri);
                cm->saveContacts(&contacts, 0);
                // XXX this doesn't restore relationships..
                delete cm;
            }
        }
    }
private:
    QMap<QString, QList<QContact> > savedContacts;
};

QTM_END_NAMESPACE

#endif

