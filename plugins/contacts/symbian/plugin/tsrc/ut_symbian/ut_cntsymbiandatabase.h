/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef CNTSYMBIANDATABASE_UNIT_TEST
#define CNTSYMBIANDATABASE_UNIT_TEST

#include <QObject>
#include <qcontactid.h>
class CntSymbianEngine;
class CntSymbianDatabase;

QTM_USE_NAMESPACE
QTM_BEGIN_NAMESPACE
QTM_END_NAMESPACE

class TestCntSymbianDatabase : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();    
    void cleanupTestCase();
    
    void init();
    void cleanup();
        
    void contactDatabase();
    void createChangeRemoveContactEvents();
    void ownCardEvents();
    void relationshipEvents();

private slots:
    void handleAddedOrig(const QList<QContactLocalId>& contactIds);
    void handleChangedOrig(const QList<QContactLocalId>& contactIds);
    void handleRemovedOrig(const QList<QContactLocalId>& contactIds);
    void handleMyCardChangedOrig(const QContactLocalId& oldId, const QContactLocalId& newId);
    void handleRelationshipsAddedOrig(QList<QContactLocalId> contactIds);
    void handleRelationshipsRemovedOrig(QList<QContactLocalId> contactIds);
    
    void handleAdded(const QList<QContactLocalId>& contactIds);
    void handleChanged(const QList<QContactLocalId>& contactIds);
    void handleRemoved(const QList<QContactLocalId>& contactIds);
    void handleMyCardChanged(const QContactLocalId& oldId, const QContactLocalId& newId);
    void handleRelationshipsAdded(QList<QContactLocalId> contactIds);
    void handleRelationshipsRemoved(QList<QContactLocalId> contactIds);

private:
    void removeAllContacts();
    
private:
    CntSymbianEngine *m_engine;
    CntSymbianDatabase *m_db;
    
    QContactLocalId addedContactId;
    QContactLocalId addedOrigContactId;
    QContactLocalId changedContactId;
    QContactLocalId changedOrigContactId;
    QContactLocalId removedContactId;
    QContactLocalId removedOrigContactId;
    QContactLocalId ownCardId;
    QContactLocalId ownCardOrigId;
    QContactLocalId addedRelId1;
    QContactLocalId addedRelId2;
    QContactLocalId addedRelOrigId1;
    QContactLocalId addedRelOrigId2;
    QContactLocalId removedRelId1;
    QContactLocalId removedRelId2;
    QContactLocalId removedRelOrigId1;
    QContactLocalId removedRelOrigId2;
    
};


#endif
