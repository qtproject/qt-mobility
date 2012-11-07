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
    void batchRemoveOperation();
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
    
    QContactLocalId addedContactId;
    QContactLocalId addedOrigContactId;
    QContactLocalId changedContactId;
    QContactLocalId changedOrigContactId;
    QList<QContactLocalId> removedContactId;
    QList<QContactLocalId> removedOrigContactId;
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
