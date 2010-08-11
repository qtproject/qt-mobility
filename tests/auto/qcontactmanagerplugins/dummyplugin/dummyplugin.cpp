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


/* We build multiple plugins with the same code, just different names */
#ifndef DUMMYPLUGINTARGET
#define DUMMYPLUGINTARGET contacts_testdummy
#endif

#define makestr(x) (#x)
#define makename(x) makestr(x)

#include "qcontactmanager.h"
#include "qcontactmanagerengine.h"
#include "qcontactmanagerenginefactory.h"

QTM_USE_NAMESPACE

class DummyEngine : public QContactManagerEngine
{
public:
    DummyEngine(const QMap<QString, QString>& parameters, QContactManager::Error* error);
    DummyEngine(const DummyEngine& other);
    ~DummyEngine();
    DummyEngine& operator=(const DummyEngine& other);
    QContactManagerEngine* clone();
    void deref();
    QString managerName() const;

    /* Contacts - Accessors and Mutators */
    QList<QContactLocalId> contacts(QContactManager::Error* error) const;
    QContact contact(const QContactLocalId& contactId, QContactManager::Error* error) const;
    QContact contact(const QContactLocalId& contactId, const QContactFetchHint& fetchHint, QContactManager::Error* error) const;
    bool saveContact(QContact* contact, bool batch, QContactManager::Error* error);
    bool removeContact(const QContactLocalId& contactId, bool batch, QContactManager::Error* error);

    /* Capabilities reporting */
    QStringList capabilities() const;
    QStringList fastFilterableDefinitions() const;
    QList<QVariant::Type> supportedDataTypes() const;

    QMap<QString, QString> managerParameters() const {return QMap<QString, QString>();}
    int managerVersion() const {return 0;}

    QList<QContactLocalId> contactIds(const QContactFilter&, const QList<QContactSortOrder>&, QContactManager::Error* error) const
    {
        *error = QContactManager::NotSupportedError;
        return QList<QContactLocalId>();
    }

    QList<QContact> contacts(const QContactFilter&, const QList<QContactSortOrder>&, const QContactFetchHint&, QContactManager::Error* error) const
    {
        *error = QContactManager::NotSupportedError;
        return QList<QContact>();
    }

    bool saveContacts(QList<QContact>*, QMap<int, QContactManager::Error>*, QContactManager::Error* error)
    {
        *error = QContactManager::NotSupportedError;
        return false;
    }

    bool removeContacts(const QList<QContactLocalId>&, QMap<int, QContactManager::Error>*, QContactManager::Error* error)
    {
        *error = QContactManager::NotSupportedError;
        return false;
    }

    QContact conformingContact(const QContact&, QContactManager::Error* error)
    {
        *error = QContactManager::NotSupportedError;
        return QContact();
    }

    /* Synthesize the display label of a contact */
    virtual QString synthesizedDisplayLabel(const QContact&, QContactManager::Error* error) const
    {
        *error = QContactManager::NotSupportedError;
        return QString();
    }

    /* "Self" contact id (MyCard) */
    virtual bool setSelfContactId(const QContactLocalId&, QContactManager::Error* error)
    {
        *error = QContactManager::NotSupportedError;
        return false;
    }

    virtual QContactLocalId selfContactId(QContactManager::Error* error) const
    {
        *error = QContactManager::NotSupportedError;
        return 0;
    }

    /* Relationships between contacts */
    virtual QList<QContactRelationship> relationships(const QString&, const QContactId&, QContactRelationship::Role, QContactManager::Error* error) const
    {
        *error = QContactManager::NotSupportedError;
        return QList<QContactRelationship>();
    }

    virtual bool saveRelationships(QList<QContactRelationship>*, QMap<int, QContactManager::Error>*, QContactManager::Error* error)
    {
        *error = QContactManager::NotSupportedError;
        return false;
    }

    virtual bool removeRelationships(const QList<QContactRelationship>&, QMap<int, QContactManager::Error>*, QContactManager::Error* error)
    {
        *error = QContactManager::NotSupportedError;
        return false;
    }

    /* Validation for saving */
    virtual QContact compatibleContact(const QContact&, QContactManager::Error* error) const
    {
        *error =  QContactManager::NotSupportedError;
        return QContact();
    }

    virtual bool validateContact(const QContact&, QContactManager::Error* error) const
    {
        *error = QContactManager::NotSupportedError;
        return false;
    }

    virtual bool validateDefinition(const QContactDetailDefinition&, QContactManager::Error* error) const
    {
        *error = QContactManager::NotSupportedError;
        return false;
    }

    /* Definitions - Accessors and Mutators */
    virtual QMap<QString, QContactDetailDefinition> detailDefinitions(const QString&, QContactManager::Error* error) const
    {
        *error = QContactManager::NotSupportedError;
        return QMap<QString, QContactDetailDefinition>();
    }

    virtual QContactDetailDefinition detailDefinition(const QString&, const QString&, QContactManager::Error* error) const
    {
        *error = QContactManager::NotSupportedError;
        return QContactDetailDefinition();
    }

    virtual bool saveDetailDefinition(const QContactDetailDefinition&, const QString&, QContactManager::Error* error)
    {
        *error = QContactManager::NotSupportedError;
        return false;
    }

    virtual bool removeDetailDefinition(const QString&, const QString&, QContactManager::Error* error)
    {
        *error = QContactManager::NotSupportedError;
        return false;
    }

    /* Asynchronous Request Support */
    virtual void requestDestroyed(QContactAbstractRequest*) {}
    virtual bool startRequest(QContactAbstractRequest*) {return false;}
    virtual bool cancelRequest(QContactAbstractRequest*) {return false;}
    virtual bool waitForRequestFinished(QContactAbstractRequest*, int) {return false;}

    /* Capabilities reporting */
    virtual bool hasFeature(QContactManager::ManagerFeature, const QString&) const
    {
        return false;
    }

    virtual bool isRelationshipTypeSupported(const QString&, const QString&) const
    {
        return false;
    }

    virtual bool isFilterSupported(const QContactFilter&) const
    {
        return false;
    }

    virtual QStringList supportedContactTypes() const
    {
        return QStringList();
    }

};

class DummyEngineFactory : public QObject, public QContactManagerEngineFactory
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QContactManagerEngineFactory)
    public:
        QContactManagerEngine* engine(const QMap<QString, QString>& parameters, QContactManager::Error* error);
        QString managerName() const;
};

QContactManagerEngine* DummyEngineFactory::engine(const QMap<QString, QString>& parameters, QContactManager::Error* error)
{
    return new DummyEngine(parameters, error);
}

QString DummyEngineFactory::managerName() const
{
#ifdef DUMMYPLUGINNAME
    return QString(makename(DUMMYPLUGINNAME));
#else
    return QString();
#endif
}
Q_EXPORT_PLUGIN2(DUMMYPLUGINTARGET, DummyEngineFactory);

DummyEngine::DummyEngine(const QMap<QString, QString>& parameters, QContactManager::Error* error)
{
    Q_UNUSED(parameters);
    *error = QContactManager::AlreadyExistsError; // Another random choice
}

DummyEngine::DummyEngine(const DummyEngine& other)
    : QContactManagerEngine()
{
    Q_UNUSED(other);
}

DummyEngine& DummyEngine::operator=(const DummyEngine& other)
{
    Q_UNUSED(other);
    return *this;
}

DummyEngine::~DummyEngine()
{
}

QContactManagerEngine* DummyEngine::clone()
{
    // Disallow sharing
    return new DummyEngine(*this);
}

void DummyEngine::deref()
{
    delete this;
}

QString DummyEngine::managerName() const
{
#ifdef DUMMYPLUGINNAME
    return QString(makename(DUMMYPLUGINNAME));
#else
    return QString();
#endif
}

QList<QContactLocalId> DummyEngine::contacts(QContactManager::Error* error) const
{
    QList<QContactLocalId> allCIds;

    if (allCIds.count() > 0 && *error == QContactManager::NoError)
        *error = QContactManager::DoesNotExistError;

    return allCIds;
}

QContact DummyEngine::contact(const QContactLocalId& contactId, QContactManager::Error* error) const
{
    Q_UNUSED(contactId);
    *error = QContactManager::DoesNotExistError;
    return QContact();
}

QContact DummyEngine::contact(const QContactLocalId& contactId, const QContactFetchHint& fetchHint, QContactManager::Error* error) const
{
    Q_UNUSED(contactId);
    Q_UNUSED(fetchHint);
    *error = QContactManager::DoesNotExistError;
    return QContact();
}

bool DummyEngine::saveContact(QContact* contact, bool batch, QContactManager::Error* error)
{
    // ensure that the contact's details conform to their definitions
    if (!validateContact(*contact, error)) {
        *error = QContactManager::InvalidDetailError;
        return false;
    }

    // success!
    QContactId newId;
    newId.setManagerUri(managerUri());
    newId.setLocalId(5);
    contact->setId(newId);
    *error = QContactManager::NoError;

    // if we need to emit signals (ie, this isn't part of a batch operation)
    // then emit the correct one.
    if (!batch) {
        QList<QContactLocalId> emitList;
        emitList.append(contact->id().localId());
        emit contactsAdded(emitList);
    }

    return true;
}

bool DummyEngine::removeContact(const QContactLocalId& contactId, bool batch, QContactManager::Error* error)
{
    if (contactId != 5) {
        *error = QContactManager::DoesNotExistError;
        return false;
    }

    *error = QContactManager::NoError;

    // if we need to emit signals (ie, this isn't part of a batch operation)
    // then emit the correct one.
    if (!batch) {
        QList<QContactLocalId> emitList;
        emitList.append(contactId);
        emit contactsRemoved(emitList);
    }

    return true;
}

/*! Returns the capabilities of the in-memory engine. */
QStringList DummyEngine::capabilities() const
{
    QStringList caplist;
    caplist << "Locking" << "Batch" << "ReadOnly" << "Filtering" << "Sorting" << "Preferences";
    // ie, doesn't support: Changelog, Volatile, Asynchronous.
    return caplist;
}

/*!
 * Returns a list of definition identifiers which are natively (fast) filterable
 * on the default backend store managed by the manager from which the capabilities object was accessed
 */
QStringList DummyEngine::fastFilterableDefinitions() const
{
    QStringList fastlist;
    fastlist << "Name::First" << "Name::Last" << "PhoneNumber::PhoneNumber" << "EmailAddress::EmailAddress";
    return fastlist;
}

/*!
 * Returns the list of data types supported by the vCard engine
 */
QList<QVariant::Type> DummyEngine::supportedDataTypes() const
{
    QList<QVariant::Type> st;
    st.append(QVariant::String);
    st.append(QVariant::Date);
    st.append(QVariant::DateTime);

    return st;
}

#include "dummyplugin.moc"
