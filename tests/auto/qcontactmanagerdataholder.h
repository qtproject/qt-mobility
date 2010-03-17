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

        foreach(QString mgr, managerNames) {
            QMap<QString, QString> params;
            QString mgrUri = QContactManager::buildUri(mgr, params);
            QContactManager* cm = QContactManager::fromUri(mgrUri);
            if (cm) {
                QList<QContact> contacts;
                foreach (const QContactLocalId id,  cm->contactIds()) {
                    contacts.push_back(cm->contact(id));
                }
                savedContacts.insert(cm->managerName(),contacts);
                QList<QContactLocalId> ids = cm->contactIds();
                QMap<int, QContactManager::Error> errorMap;
                cm->removeContacts(ids, &errorMap);
                delete cm;
            }
        }
    }

    ~QContactManagerDataHolder()
    {
        QStringList managerNames = QContactManager::availableManagers();

        foreach(QString mgr, managerNames) {
            QMap<QString, QString> params;
            QString mgrUri = QContactManager::buildUri(mgr, params);
            QContactManager* cm = QContactManager::fromUri(mgrUri);
            if (cm) {
                QList<QContact> contacts = savedContacts.value(cm->managerName());
                foreach(QContact c, contacts) {
                    c.setId(QContactId());
                    cm->saveContact(&c);
                }
                delete cm;
            }
        }
    }
private:
    QMap<QString, QList<QContact> > savedContacts;
};

QTM_END_NAMESPACE

#endif

