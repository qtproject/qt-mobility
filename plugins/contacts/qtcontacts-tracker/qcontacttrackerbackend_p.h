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

#ifndef QCONTACTTRACKERBACKEND_P_H
#define QCONTACTTRACKERBACKEND_P_H

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

#include <QSharedData>
#include <QtTracker/Tracker>
#include <QtTracker/ontologies/nco.h>
#include <QtTracker/QLive>

#include <qmobilityglobal.h>
#include <qcontactmanagerengine.h>
#include <qcontactmanagerenginefactory.h>

using namespace SopranoLive;
#include "qtrackercontactasyncrequest.h"

QTM_BEGIN_NAMESPACE
class QContactAbstractRequest;
class QContactChangeSet;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class QContactTrackerEngineData : public QSharedData
{
public:
    QContactTrackerEngineData()
        : QSharedData(), m_refCount(QAtomicInt(1)),
        m_engineName(QString("tracker")),
        m_engineVersion(0)
    {
    }

    QContactTrackerEngineData(const QContactTrackerEngineData& other)
        : QSharedData(other), m_refCount(QAtomicInt(1)),
        m_lastUsedId(other.m_lastUsedId),
        m_definitions(other.m_definitions),
        m_engineName(other.m_engineName),
        m_engineVersion(other.m_engineVersion)
    {
    }

    void init();

    ~QContactTrackerEngineData() {}

    QAtomicInt m_refCount;
    mutable QContactLocalId m_lastUsedId;
    mutable QMap<QString, QContactDetailDefinition> m_definitions;
    mutable QMap<QContactAbstractRequest*, QTrackerContactAsyncRequest*> m_requests;
    QString m_engineName;
    int m_engineVersion;
};

class QContactTrackerEngine : public QContactManagerEngine
{
Q_OBJECT

public:
    QContactTrackerEngine(const QString& managerName, int managerVersion, const QMap<QString, QString>& parameters);
    QContactTrackerEngine(const QMap<QString, QString>& parameters);
    QContactTrackerEngine(const QContactTrackerEngine& other);
    ~QContactTrackerEngine();
    QContactTrackerEngine& operator=(const QContactTrackerEngine& other);
    QContactManagerEngine* clone();
    void deref();

    // sync methods, wrapping async methods & waitForFinished
    QList<QContactLocalId> contactIds(const QList<QContactSortOrder>& sortOrders, QContactManager::Error* error) const;
    QList<QContactLocalId> contactIds(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error* error) const;
    QList<QContact> contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, const QContactFetchHint& fetchHint, QContactManager::Error* error) const;
    QContact contact(const QContactLocalId& contactId, const QContactFetchHint& fetchHint, QContactManager::Error* error) const;

    /* Save contacts - single and in batch */
    bool saveContact( QContact* contact, QContactManager::Error* error);
    bool saveContacts(QList<QContact>* contacts, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error);

    bool removeContact(const QContactLocalId& contactId, QContactManager::Error* error);
    bool removeContacts(const QList<QContactLocalId>& contactIds, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error) ;

    /* Definitions - Accessors and Mutators */
    QMap<QString, QContactDetailDefinition> detailDefinitions(const QString& contactType, QContactManager::Error* error) const;

    QContactLocalId selfContactId(QContactManager::Error* error) const;

    /* Asynchronous Request Support */
    void requestDestroyed(QContactAbstractRequest* req);
    bool startRequest(QContactAbstractRequest* req);
    bool waitForRequestFinished(QContactAbstractRequest* req, int msecs);

    /* Capabilities reporting */
    bool hasFeature(QContactManager::ManagerFeature feature, const QString& contactType) const;

    bool isFilterSupported(const QContactFilter& filter) const;
    QList<QVariant::Type> supportedDataTypes() const;

    /* Version Reporting */
    QString managerName() const;
    int managerVersion() const;

    /* Synthesise the display label of a contact */
    QString synthesizedDisplayLabel(const QContact& contact, QContactManager::Error* error) const;



    /* XXX TODO: pure virtual unimplemented functions! */
    QMap<QString, QString> managerParameters() const {return QMap<QString,QString>();}
    bool setSelfContactId(const QContactLocalId&, QContactManager::Error* error) {*error = QContactManager::NotSupportedError; return false;}
    QList<QContactRelationship> relationships(const QString&, const QContactId&, QContactRelationship::Role, QContactManager::Error* error) const {*error = QContactManager::NotSupportedError; return QList<QContactRelationship>();}
    bool saveRelationships(QList<QContactRelationship>*, QMap<int, QContactManager::Error>*, QContactManager::Error* error) {*error = QContactManager::NotSupportedError; return false;}
    bool removeRelationships(const QList<QContactRelationship>&, QMap<int, QContactManager::Error>*, QContactManager::Error* error) {*error = QContactManager::NotSupportedError; return false;}
    bool validateContact(const QContact&, QContactManager::Error* error) const {*error = QContactManager::NotSupportedError; return false;}
    bool validateDefinition(const QContactDetailDefinition&, QContactManager::Error* error) const {*error = QContactManager::NotSupportedError; return false;}
    QContactDetailDefinition detailDefinition(const QString&, const QString&, QContactManager::Error* error) const {*error = QContactManager::NotSupportedError; return QContactDetailDefinition();}
    bool saveDetailDefinition(const QContactDetailDefinition&, const QString&, QContactManager::Error* error) {*error = QContactManager::NotSupportedError; return false;}
    bool removeDetailDefinition(const QString&, const QString&, QContactManager::Error* error) {*error = QContactManager::NotSupportedError; return false;}
    bool cancelRequest(QContactAbstractRequest*) {return false;}
    bool isRelationshipTypeSupported(const QString&, const QString&) const {return false;}
    QStringList supportedContactTypes() const {return (QStringList() << QContactType::TypeContact);}


private:
    //called from both constructors, connecting to all contact NodeList changes signals
    void connectToSignals();
    RDFVariable contactDetail2Rdf(const RDFVariable& rdfContact, const QString& definitionName, const QString& fieldName) const;
    QContact contact_impl(const QContactLocalId& contactId, const QContactFetchHint& fetchHint, QContactManager::Error* error) const;
private:
    QSharedDataPointer<QContactTrackerEngineData> d;
    const QString contactArchiveFile;
    const QString contactArchiveDir;
    friend class ut_qtcontacts_trackerplugin;
};

class Q_DECL_EXPORT ContactTrackerFactory : public QObject, public QtMobility::QContactManagerEngineFactory
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QContactManagerEngineFactory)
    public:
        QContactManagerEngine* engine(const QMap<QString, QString>& parameters, QContactManager::Error*);
        QString managerName() const;
};

#endif
