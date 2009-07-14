/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/


#include "qcontactmanager.h"
#include "qcontactmanager_p.h"

#include "qcontactdetaildefinition.h"
#include "qcontactdetails.h"

#include "qcontact_p.h"
#include "qcontactgroup_p.h"

#include "qcontactfilter.h"
#include "qcontactsortorder.h"

#include "qcontactabstractaction.h"

#include <QSharedData>
#include <QPair>
#include <QtPlugin>
#include <QPluginLoader>

#include <QDebug>
#include <QDir>

#include <QApplication>

#include "memory/qcontactmemorybackend_p.h"
#include "invalid/qcontactinvalidbackend_p.h"


/* Shared QContactManager stuff here, default engine stuff below */
QMultiMap<QString, QContactAbstractAction*> QContactManagerData::m_actionImplementations;
QMap<QString, QContactManagerEngineFactory*> QContactManagerData::m_engines;
bool QContactManagerData::m_discovered;

static void qContactsCleanEngines()
{
    QContactManagerData::m_discovered = false;
    QList<QContactManagerEngineFactory*> factories = QContactManagerData::m_engines.values();
    for (int i=0; i < factories.count(); i++) {
        delete factories.at(i);
    }
    QContactManagerData::m_engines.clear();
}

void QContactManagerData::createEngine(const QString& managerName, const QMap<QString, QString>& parameters)
{
    m_managerName = managerName.isEmpty() ? QContactManager::availableManagers().value(0) : managerName;

    if (m_managerName == QLatin1String("memory"))
        m_engine = QContactMemoryEngine::createMemoryEngine(parameters);
    else {
        /* Look for a factory */
        loadFactories();
        QContactManagerEngineFactory *factory = m_engines.value(managerName);
        m_error = QContactManager::NoError;
        if (factory)
            m_engine = factory->engine(parameters, m_error);
        else
            m_engine = 0;

        if (!m_engine) {
            m_managerName = QLatin1String("invalid");
            if (m_error == QContactManager::NoError)
                m_error = QContactManager::DoesNotExistError;
            m_engine = new QContactInvalidEngine(); // XXX share
        }
    }
    m_params = parameters;
    m_uri = QContactManager::buildUri(m_managerName, parameters);
}

/* Plugin loader */
void QContactManagerData::loadFactories()
{
    if (!m_discovered) {
        m_discovered = true;

        /* Clean stuff up at the end */
        qAddPostRoutine(qContactsCleanEngines);

        /* Discover a bunch o plugins */
        QStringList plugins;

        /* First the static ones */
        QObjectList staticPlugins = QPluginLoader::staticInstances();
        for (int i=0; i < staticPlugins.count(); i++ ){
            QContactManagerEngineFactory *f = qobject_cast<QContactManagerEngineFactory*>(staticPlugins.at(i));
            QContactAbstractAction *g = qobject_cast<QContactAbstractAction*>(staticPlugins.at(i));
            if (f) {
                QString name = f->managerName();
                qDebug() << "Static: found an engine plugin" << f << "with name" << name;
                if (name != QLatin1String("memory") && name != QLatin1String("invalid") && !name.isEmpty()) {
                    if(!m_engines.contains(name)) {
                        m_engines.insert(name, f);
                    } else {
                        qWarning() << "Duplicate Contacts static plugin with id" << name;
                    }
                } else {
                    qWarning() << "Static contacts plugin with reserved name" << name << "ignored";
                }
            } else if (g) {
                QString name = g->actionName();
                qDebug() << "Static: found an action implementation" << g << "with name" << name;
                m_actionImplementations.insert(name, g);
            } else {
                qDebug() << "Static: plugin found is of unknown type!";
            }
        }

        QStringList paths;

        paths << QApplication::applicationDirPath() << QApplication::libraryPaths();

        /* XXX a bit of a hack */
#if defined(Q_OS_UNIX)
        if (qgetenv("LD_LIBRARY_PATH").size() > 0)
            paths << QString::fromLocal8Bit(qgetenv("LD_LIBRARY_PATH"));
#elif defined(Q_OS_WIN)
        if (qgetenv("PATH").size() > 0)
            paths << QString::fromLocal8Bit(qgetenv("PATH")).split(';');
#endif

        /* Code from docs.. */
        for (int i=0; i < paths.count(); i++) {
            QDir pluginsDir(paths.at(i));
#if defined(Q_OS_WIN)
             if (pluginsDir.dirName().toLower() == QLatin1String("debug") || pluginsDir.dirName().toLower() == QLatin1String("release"))
                 pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
             if (pluginsDir.dirName() == QLatin1String("MacOS")) {
                 pluginsDir.cdUp();
                 pluginsDir.cdUp();
                 pluginsDir.cdUp();
             }
#endif
            if (pluginsDir.cd(QLatin1String("plugins")) || (pluginsDir.cdUp() && pluginsDir.cd(QLatin1String("plugins")))) {
                const QStringList& files = pluginsDir.entryList(QDir::Files);
                qDebug() << "Looking for plugins in" << pluginsDir.path() << files;
                for (int j=0; j < files.count(); j++) {
                    plugins << pluginsDir.absoluteFilePath(files.at(j));
                }
            }
        }

        /* Now discover some other plugins */
        for (int i=0; i < plugins.count(); i++) {
            QPluginLoader qpl(plugins.at(i));
            QContactManagerEngineFactory *f = qobject_cast<QContactManagerEngineFactory*>(qpl.instance());
            QContactAbstractAction *g = qobject_cast<QContactAbstractAction*>(qpl.instance());
            if (f) {
                QString name = f->managerName();
                qDebug() << "Dynamic: found an engine plugin" << f << "with name" << name;

                if (name != QLatin1String("memory") && name != QLatin1String("invalid") && !name.isEmpty()) {
                    if(!m_engines.contains(name)) {
                        m_engines.insert(name, f);
                    } else {
                        qWarning() << "Duplicate Contacts plugin with id" << name;
                    }
                } else {
                    qWarning() << "Contacts plugin" << plugins.at(i) << "with reserved name" << name << "ignored";
                }
            } else if (g) {
                QString name = g->actionName();
                qDebug() << "Dynamic: found an action implementation" << g << "with name" << name;
                m_actionImplementations.insert(name, g);
            } else {
                qDebug() << "Dynamic: plugin found is of unknown type!";
                qDebug() << "    qpl.instance() =" << qpl.instance();
            }
        }

        qDebug() << "Found engines:" << m_engines.keys();
        qDebug() << "Found actions:" << m_actionImplementations.keys();
    }
}

QList<QContactAbstractAction*> QContactManagerData::actionImplementations(const QString& actionName)
{
    if (actionName.isEmpty()) {
        // return the entire list of implementations.
        return m_actionImplementations.values();
    }

    return m_actionImplementations.values(actionName);
}


/*!
 * \class QContactManagerEngineFactory
 * \preliminary
 * \brief This class is the interface for plugins that implement
 * QContactManagerEngine functionality.
 *
 * This class provides a simple interface for the creation of
 * manager engine instances.  Each factory has a specific id
 * associated with it, which forms the \c managerName parameter
 * when creating \l QContactManager objects.
 *
 * More information on writing a contacts engine plugin is TODO.
 *
 * \sa QContactManager, QContactManagerEngine
 */

/*!
 * \fn QContactManagerEngineFactory::~QContactManagerEngineFactory()
 *
 * A default, empty destructor.
 */

/*!
 * \fn QContactManagerEngineFactory::engine(const QMap<QString, QString>& parameters, QContactManager::Error &error)
 *
 * This function is called by the QContactManager implementation to
 * create an instance of the engine provided by this factory.
 *
 * The \a parameters supplied can be ignored or interpreted as desired.
 *
 * If a supplied parameter results in an unfulfillable request, or some other error
 * occurs, this function may return a null pointer, and the client developer will get an
 * invalid QContactManager in return.  Any error should be stored in the supplied \a error
 * reference.
 */

/*!
 * \fn QContactManagerEngineFactory::managerName() const
 *
 * This function should return a unique string that identifies
 * the engines provided by this factory.
 *
 * Typically this would be of the form "com.nokia.qt.contacts.engines.memory", with
 * the appropriate domain and engine name substituted.
 */


/* Dummy, default implementation of the backend functions */


/*!
 * \class QContactManagerEngine
 * \preliminary
 * \brief This class is the interface for all implementations of
 * the contact manager backend functionality.
 *
 * Instances of this class are usually provided by a
 * \l QContactManagerEngineFactory, which is loaded from a plugin.
 *
 * The default implementation of this interface provides a basic
 * level of functionality for some functions so that specific engines
 * can simply implement the functionality that is supported by
 * the specific contacts engine that is being adapted.
 *
 * More information on writing a contacts engine plugin is TODO.
 *
 * \sa QContactManager, QContactManagerEngineFactory
 */

/*!
 * \fn QContactManagerEngine::QContactManagerEngine()
 *
 * A default, empty constructor.
 */

/*!
 * \fn QContactManagerEngine::deref()
 *
 * Notifies the engine that it is no longer required.  If this
 * engine can not be shared between managers, it is safe for the
 * engine to delete itself in this function.
 *
 * If the engine implementation can be shared, this function can use a
 * reference count and track lifetime that way.  The factory that
 * returns an instance of this engine should increment the reference
 * count in this case.
 */


/*!
 * \fn QContactManagerEngine::contactsAdded(const QList<QUniqueId>& contactIds);
 *
 * This signal is emitted some time after a set of contacts has been added to
 * this engine.  As it is possible that other processes (or other devices) may
 * have added the contacts, the timing cannot be determined.
 *
 * The list of ids added is given by \a contactIds.  There may be one or more
 * ids in the list.
 */

/*!
 * \fn QContactManagerEngine::contactsChanged(const QList<QUniqueId>& contactIds);
 *
 * This signal is emitted some time after a set of contacts has been modified in
 * this engine.  As it is possible that other processes (or other devices) may
 * have modified the contacts, the timing cannot be determined.
 *
 * The list of ids added is given by \a contactIds.  There may be one or more
 * ids in the list.
 */

/*!
 * \fn QContactManagerEngine::contactsRemoved(const QList<QUniqueId>& contactIds);
 *
 * This signal is emitted some time after a set of contacts has been removed from
 * this engine.  As it is possible that other processes (or other devices) may
 * have removed the contacts, the timing cannot be determined.
 *
 * The list of ids added is given by \a contactIds.  There may be one or more
 * ids in the list.
 */


/*!
 * \fn QContactManagerEngine::groupsAdded(const QList<QUniqueId>& groupIds);
 *
 * This signal is emitted some time after a set of contact groups has been added to
 * this engine.  As it is possible that other processes (or other devices) may
 * have added the groups, the timing cannot be determined.
 *
 * The list of ids added is given by \a groupIds.  There may be one or more
 * ids in the list.
 */

/*!
 * \fn QContactManagerEngine::groupsChanged(const QList<QUniqueId>& groupIds);
 *
 * This signal is emitted some time after a set of contact groups has been modified in
 * this engine.  As it is possible that other processes (or other devices) may
 * have modified the groups, the timing cannot be determined.
 *
 * The list of ids added is given by \a groupIds.  There may be one or more
 * ids in the list.
 */

/*!
 * \fn QContactManagerEngine::groupsRemoved(const QList<QUniqueId>& groupIds);
 *
 * This signal is emitted some time after a set of contact groups has been removed from
 * this engine.  As it is possible that other processes (or other devices) may
 * have removed the groups, the timing cannot be determined.
 *
 * The list of ids added is given by \a groupIds.  There may be one or more
 * ids in the list.
 */

/*!
 * Return the list of contact ids present in this engine, sorted according to the given \a sortOrder.
 *
 * Any errors encountered should be stored to \a error.
 */
QList<QUniqueId> QContactManagerEngine::contacts(const QContactSortOrder& sortOrder, QContactManager::Error& error) const
{
    Q_UNUSED(sortOrder);
    error = QContactManager::NotSupportedError;
    return QList<QUniqueId>();
}

/*!
 * Returns a list of the ids of contacts that match the supplied \a filter, sorted according to the given \a sortOrder.
 * Any error that occurs will be stored in \a error.
 *
 * The default implementation will retrieve all contacts and test them with testFilter.
 */
QList<QUniqueId> QContactManagerEngine::contacts(const QContactFilter& filter, const QContactSortOrder& sortOrder, QContactManager::Error& error) const
{
    /* Slow way */
    QList<QUniqueId> ret;

    /* Try to do some early out, just in case */
    if (filter.type() != QContactFilter::Invalid) {
        /* Retrieve each contact.. . . */
        const QList<QUniqueId>& all = contacts(sortOrder, error);
        if (error != QContactManager::NoError)
            return ret;
        for (int j = 0; j < all.count(); j++) {
            if (testFilter(filter, contact(all.at(j), error)))
                ret << all.at(j);
        }
    }

    return ret;
}

/*!
 * Returns the contact in the database identified by \a contactId
 *
 * Any errors encountered should be stored to \a error.
 */
QContact QContactManagerEngine::contact(const QUniqueId& contactId, QContactManager::Error& error) const
{
    Q_UNUSED(contactId);
    error = QContactManager::NotSupportedError;
    return QContact();
}

/*!
 * Synthesises the display label of the given \a contact in a platform specific manner.
 * Any error that occurs will be stored in \a error.
 * Returns the synthesised display label.
 */
QString QContactManagerEngine::synthesiseDisplayLabel(const QContact& contact, QContactManager::Error& error) const
{
    // synthesise the display name from the name of the contact, or, failing that, the organisation of the contact.
    error = QContactManager::NoError;
    QList<QContactDetail> allOrgs = contact.details(QLatin1String(QContactOrganisation::DefinitionName));
    QList<QContactDetail> allNames = contact.details(QLatin1String(QContactName::DefinitionName));

    // first, check to see whether or not there is a name or org to synthesise from.
    if (allNames.isEmpty()) {
        if (allOrgs.isEmpty()) {
            error = QContactManager::UnspecifiedError;
            return QString();
        }

        foreach (const QContactOrganisation& org, allOrgs) {
            if (!org.displayLabel().isEmpty()) {
                return org.displayLabel();
            }
        }

        error = QContactManager::UnspecifiedError;
        return QString();
    }

    const QLatin1String space(" ");

    // synthesise the display label from the name.
    foreach (const QContactName& name, allNames) {
        QString result;
        if (!name.value(QLatin1String(QContactName::FieldPrefix)).trimmed().isEmpty()) {
           result += name.value(QLatin1String(QContactName::FieldPrefix));
        }

        if (!name.value(QLatin1String(QContactName::FieldFirst)).trimmed().isEmpty()) {
            if (!result.isEmpty())
                result += space;
            result += name.value(QLatin1String(QContactName::FieldFirst));
        }

        if (!name.value(QLatin1String(QContactName::FieldMiddle)).trimmed().isEmpty()) {
            if (!result.isEmpty())
                result += space;
            result += name.value(QLatin1String(QContactName::FieldMiddle));
        }

        if (!name.value(QLatin1String(QContactName::FieldLast)).trimmed().isEmpty()) {
            if (!result.isEmpty())
                result += space;
            result += name.value(QLatin1String(QContactName::FieldLast));
        }

        if (!name.value(QLatin1String(QContactName::FieldSuffix)).trimmed().isEmpty()) {
            if (!result.isEmpty())
                result += space;
            result += name.value(QLatin1String(QContactName::FieldSuffix));
        }

        if (!result.isEmpty()) {
            return result;
        }
    }

    error = QContactManager::UnspecifiedError;
    return QString();
}

/*!
 * Returns true if the given \a feature is supported by this engine
 */
bool QContactManagerEngine::hasFeature(QContactManagerInfo::ManagerFeature feature) const
{
    Q_UNUSED(feature);
    return false;
}

/*!
 * Returns a whether the supplied \a filter can be implemented
 * natively by this engine.  If not, the base class implementation
 * will emulate the functionality.
 */
bool QContactManagerEngine::filterSupported(const QContactFilter& filter) const
{
    Q_UNUSED(filter);
    return false;
}

/*!
 * Returns the list of data types supported by this engine.
 */
QList<QVariant::Type> QContactManagerEngine::supportedDataTypes() const
{
    return QList<QVariant::Type>();
}

/*! Returns the base schema definitions */
QMap<QString, QContactDetailDefinition> QContactManagerEngine::schemaDefinitions()
{
    // the map we will eventually return
    QMap<QString, QContactDetailDefinition> retn;

    // local variables for reuse
    QMap<QString, QContactDetailDefinition::Field> fields;
    QContactDetailDefinition::Field f;
    QContactDetailDefinition d;

    // sync target
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setId(QLatin1String(QContactSyncTarget::DefinitionName));
    fields.insert(QLatin1String(QContactSyncTarget::FieldSyncTarget), f);
    d.setFields(fields);
    d.setUnique(true);
    d.setAccessConstraint(QContactDetailDefinition::CreateOnly);
    retn.insert(d.id(), d);

    // guid
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setId(QLatin1String(QContactGuid::DefinitionName));
    fields.insert(QLatin1String(QContactGuid::FieldGuid), f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::CreateOnly);
    retn.insert(d.id(), d);

    // display label
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setId(QLatin1String(QContactDisplayLabel::DefinitionName));
    fields.insert(QLatin1String(QContactDisplayLabel::FieldLabel), f);
    f.dataType = QVariant::Bool;
    f.allowableValues = QVariantList();
    fields.insert(QLatin1String(QContactDisplayLabel::FieldSynthesised), f);
    d.setFields(fields);
    d.setUnique(true);
    d.setAccessConstraint(QContactDetailDefinition::Any);
    retn.insert(d.id(), d);

    // email address
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setId(QLatin1String(QContactEmailAddress::DefinitionName));
    fields.insert(QLatin1String(QContactEmailAddress::FieldEmailAddress), f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::Any);
    retn.insert(d.id(), d);

    // organisation
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setId(QLatin1String(QContactOrganisation::DefinitionName));
    fields.insert(QLatin1String(QContactOrganisation::FieldLogo), f);
    fields.insert(QLatin1String(QContactOrganisation::FieldDisplayLabel), f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::Any);
    retn.insert(d.id(), d);

    // phone number
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setId(QLatin1String(QContactPhoneNumber::DefinitionName));
    fields.insert(QLatin1String(QContactPhoneNumber::FieldNumber), f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::Any);
    retn.insert(d.id(), d);

    // anniversary
    fields.clear();
    f.dataType = QVariant::Date;
    f.allowableValues = QVariantList();
    d.setId(QLatin1String(QContactAnniversary::DefinitionName));
    fields.insert(QLatin1String(QContactAnniversary::FieldOriginalDate), f);
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    fields.insert(QLatin1String(QContactAnniversary::FieldCalendarId), f);
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    fields.insert(QLatin1String(QContactAnniversary::FieldEvent), f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::Any);
    retn.insert(d.id(), d);

    // birthday
    fields.clear();
    f.dataType = QVariant::Date;
    f.allowableValues = QVariantList();
    d.setId(QLatin1String(QContactBirthday::DefinitionName));
    fields.insert(QLatin1String(QContactBirthday::FieldBirthday), f);
    d.setFields(fields);
    d.setUnique(true);
    d.setAccessConstraint(QContactDetailDefinition::Any);
    retn.insert(d.id(), d);

#if 0 // leaf class not yet accepted into master
    // meeting
    fields.clear();
    f.dataType = QVariant::DateTime;
    f.allowableValues = QVariantList();
    d.setId(QContactMeeting::DefinitionName);
    fields.insert(QContactMeeting::FieldMeeting, f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::Any);
    retn.insert(d.id(), d);
#endif

#if 0 // leaf class not yet accepted into master
    // tag
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setId(QContactTag::DefinitionName);
    fields.insert(QContactTag::FieldTag, f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::Any);
    retn.insert(d.id(), d);
#endif

    // url
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setId(QLatin1String(QContactUrl::DefinitionName));
    fields.insert(QLatin1String(QContactUrl::FieldUrl), f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::Any);
    retn.insert(d.id(), d);

    // gender
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList() << QString(QLatin1String("Male")) << QString(QLatin1String("Female")) << QString(QLatin1String("Unspecified"));
    d.setId(QLatin1String(QContactGender::DefinitionName));
    fields.insert(QLatin1String(QContactGender::FieldGender), f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::Any);
    retn.insert(d.id(), d);

    // avatar
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setId(QLatin1String(QContactAvatar::DefinitionName));
    fields.insert(QLatin1String(QContactAvatar::FieldAvatar), f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::Any);
    retn.insert(d.id(), d);

#if 0 // leaf class not yet accepted into master
    // geolocation
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setId(QContactGeolocation::DefinitionName);
    fields.insert(QContactGeolocation::FieldLatitude, f);
    fields.insert(QContactGeolocation::FieldLongitude, f);
    fields.insert(QContactGeolocation::FieldAccuracy, f);
    fields.insert(QContactGeolocation::FieldAltitude, f);
    fields.insert(QContactGeolocation::FieldAltitudeAccuracy, f);
    fields.insert(QContactGeolocation::FieldSpeed, f);
    fields.insert(QContactGeolocation::FieldHeading, f);
    f.dataType = QVariant::QDateTime;
    fields.insert(QContactGeolocation::FieldUpdateTimestamp, f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::Any);
    retn.insert(d.id(), d);
#endif

    // street address
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setId(QLatin1String(QContactAddress::DefinitionName));
    fields.insert(QLatin1String(QContactAddress::FieldStreet), f);
    fields.insert(QLatin1String(QContactAddress::FieldLocality), f);
    fields.insert(QLatin1String(QContactAddress::FieldRegion), f);
    fields.insert(QLatin1String(QContactAddress::FieldPostcode), f);
    fields.insert(QLatin1String(QContactAddress::FieldCountry), f);
    fields.insert(QLatin1String(QContactAddress::FieldDisplayLabel), f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::Any);
    retn.insert(d.id(), d);

    // name
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setId(QLatin1String(QContactName::DefinitionName));
    fields.insert(QLatin1String(QContactName::FieldPrefix), f);
    fields.insert(QLatin1String(QContactName::FieldFirst), f);
    fields.insert(QLatin1String(QContactName::FieldMiddle), f);
    fields.insert(QLatin1String(QContactName::FieldLast), f);
    fields.insert(QLatin1String(QContactName::FieldSuffix), f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::Any);
    retn.insert(d.id(), d);

    return retn;
}


/*!
 * Add the given \a contact to the database if it is a new contact,
 * or updates the existing contact. Returns false on failure, or true on
 * success.  If successful and the contact was a new contact, its UID will
 * be set to a new, valid id.
 *
 * If the \a contact contains one or more details whose definitions have
 * not yet been saved with the manager, the operation will fail and the
 * manager will return \c QContactManager::UnsupportedError.
 *
 * The default implementation of QContactManagerEngine::saveContacts will
 * call this function with \a batch set to \c true.  All other calls to
 * this function will pass \c false for \a batch.
 *
 * If \a batch is \c false, the backend must (at some point) emit either
 * the \c QContactManagerEngine::contactsAdded signal or the
 * \c QContactManagerEngine::contactsChanged signal if the operation was
 * successful.  If the operation fails or \a batch is \c true, neither
 * signal may be emitted as a direct result of the operation.
 *
 * The base implementation of QContactManagerEngine::saveContacts
 * will emit a single add or single change signal once all the saving is
 * complete.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
bool QContactManagerEngine::saveContact(QContact* contact, bool batch, QContactManager::Error& error)
{
    Q_UNUSED(contact);
    Q_UNUSED(batch);
    error = QContactManager::NotSupportedError;
    return false;
}

/*!
 * Checks that the given contact \a contact does not have details which
 * don't conform to a valid definition, violate uniqueness constraints,
 * or contain values for nonexistent fields, and that the values contained are
 * of the correct type for each field, and are allowable values for that field.
 *
 * Note that this function is unable to ensure that the access constraints
 * (such as CreateOnly and ReadOnly) are observed; backend specific code
 * must be written if you wish to enforce these constraints.
 *
 * Returns true if the \a contact is valid according to the definitions for
 * its details, otherwise returns false.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
bool QContactManagerEngine::validateContact(const QContact& contact, QContactManager::Error& error) const
{
    QList<QString> uniqueDefinitionIds;

    // check that each detail conforms to its definition as supported by this manager.
    for (int i=0; i < contact.details().count(); i++) {
        const QContactDetail& d = contact.details().at(i);
        QVariantMap values = d.values();
        QContactDetailDefinition def = detailDefinition(d.definitionName(), error);

        // check that the definition is supported
        if (error != QContactManager::NoError) {
            return false; // this definition is not supported.
        }

        // check uniqueness
        if (def.isUnique()) {
            if (uniqueDefinitionIds.contains(def.id()))
                return false; // can't have two of a unique detail.
            uniqueDefinitionIds.append(def.id());
        }

        QList<QString> keys = values.keys();
        for (int i=0; i < keys.count(); i++) {
            const QString& key = keys.at(i);
            // check that no values exist for nonexistent fields.
            if (!def.fields().contains(key)) {
                return false; // value for nonexistent field.
            }

            QContactDetailDefinition::Field field = def.fields().value(key);
            // check that the type of each value corresponds to the allowable field type
            if (field.dataType != values.value(key).type()) {
                return false; // type doesn't match.
            }
            // check that the value is allowable
            // if the allowable values is an empty list, any are allowed.
            if (!field.allowableValues.isEmpty()
                && !field.allowableValues.contains(values.value(key))) {
                return false; // value not allowed.
            }
        }
    }

    return true;
}


/*!
 * Checks that the given detail definition \a definition seems valid,
 * with a correct id, defined fields, and any specified value types
 * are supported by this engine.  This function is called before
 * trying to save a definition.
 *
 * Note that a ReadOnly definition will fail validation, since
 * they are intended to be provided by an engine directly.
 *
 * Returns true if the \a definition seems valid, otherwise returns
 * false.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
bool QContactManagerEngine::validateDefinition(const QContactDetailDefinition& definition, QContactManager::Error& error) const
{
    if (definition.id().isEmpty()) {
        error = QContactManager::BadArgumentError;
        return false;
    }

    if (definition.fields().count() == 0) {
        error = QContactManager::BadArgumentError;
        return false;
    }

    if (definition.accessConstraint() == QContactDetailDefinition::ReadOnly) {
        error = QContactManager::BadArgumentError;
        return false;
    }

    // Check each field now
    QList<QVariant::Type> types = supportedDataTypes();
    QMapIterator<QString, QContactDetailDefinition::Field> it(definition.fields());
    while(it.hasNext()) {
        it.next();
        if (it.key().isEmpty()) {
            error = QContactManager::BadArgumentError;
            return false;
        }

        if (!types.contains(it.value().dataType)) {
            error = QContactManager::BadArgumentError;
            return false;
        }

        // Check that each allowed value is the same type
        for (int i=0; i < it.value().allowableValues.count(); i++) {
            if (it.value().allowableValues.at(i).type() != it.value().dataType) {
                error = QContactManager::BadArgumentError;
                return false;
            }
        }
    }
    error = QContactManager::NoError;
    return true;
}

/*!
 * Remove the contact identified by \a contactId from the database.
 * Returns true if the contact was removed successfully, otherwise
 * returns false.
 *
 * The default implementation of QContactManagerEngine::removeContacts will
 * call this function with \a batch set to \c true.  All other calls to
 * this function will pass \c false for \a batch.
 *
 * If \a batch is \c false, the backend must (at some point) emit
 * the \c QContactManagerEngine::contactsRemoved signal if the operation was
 * successful.  If the operation fails or \a batch is \c true, no such
 * signal may be emitted as a direct result of the operation.
 *
 * The base implementation of QContactManagerEngine::removeContacts
 * will emit a single contactsRemoved signal once all the removing is
 * complete.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
bool QContactManagerEngine::removeContact(const QUniqueId& contactId, bool batch, QContactManager::Error& error)
{
    Q_UNUSED(contactId);
    Q_UNUSED(batch);
    error = QContactManager::NotSupportedError;
    return false;
}

/*!
 * Return the list of group ids present in this manager.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
QList<QUniqueId> QContactManagerEngine::groups(QContactManager::Error& error) const
{
    error = QContactManager::NotSupportedError;
    return QList<QUniqueId>();
}

/*!
 * Returns the group which is identified by the given \a groupId,
 * or a default-constructed group if no such group exists
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
QContactGroup QContactManagerEngine::group(const QUniqueId& groupId, QContactManager::Error& error) const
{
    Q_UNUSED(groupId);
    error = QContactManager::NotSupportedError;
    return QContactGroup();
}

/*!
 * Saves the group \a group in the database.  The id of the group is
 * used to determine the group to update.  If the group has no name set,
 * this function should fail.  If the group does not already
 * exist, it should be added to the database.
 *
 * Returns true on success, or false on failure.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
bool QContactManagerEngine::saveGroup(QContactGroup* group, QContactManager::Error& error)
{
    Q_UNUSED(group);
    error = QContactManager::NotSupportedError;
    return false;
}

/*!
 * Remove the group with the given id \a groupId from the database.
 *
 * Returns false if no group with that id exists, or the operation otherwise failed.
 * Returns true if the group was successfully deleted.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
bool QContactManagerEngine::removeGroup(const QUniqueId& groupId, QContactManager::Error& error)
{
    Q_UNUSED(groupId);
    error = QContactManager::NotSupportedError;
    return false;
}

/*!
 * Returns the registered detail definitions which are valid for contacts in this engine.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
QMap<QString, QContactDetailDefinition> QContactManagerEngine::detailDefinitions(QContactManager::Error& error) const
{
    error = QContactManager::NotSupportedError;
    return QMap<QString, QContactDetailDefinition>();
}

/*!
 * Returns the definition identified by the given \a definitionName that
 * is valid for contacts in this store, or a default-constructed QContactDetailDefinition
 * if no such definition exists
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
QContactDetailDefinition QContactManagerEngine::detailDefinition(const QString& definitionName, QContactManager::Error& error) const
{
    Q_UNUSED(definitionName);

    QMap<QString, QContactDetailDefinition> definitions = detailDefinitions(error);
    if (definitions.contains(definitionName))  {
        error = QContactManager::NoError;
        return definitions.value(definitionName);
    } else {
        error = QContactManager::DoesNotExistError;
        return QContactDetailDefinition();
    }
}

/*!
 * Persists the given definition \a def in the database.
 *
 * Returns true if the definition was saved successfully, and otherwise returns false
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
bool QContactManagerEngine::saveDetailDefinition(const QContactDetailDefinition& def, QContactManager::Error& error)
{
    Q_UNUSED(def);
    error = QContactManager::NotSupportedError;
    return false;
}

/*!
 * Removes the definition identified by the given \a definitionName from the database.
 *
 * Returns true if the definition was removed successfully, otherwise returns false.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
bool QContactManagerEngine::removeDetailDefinition(const QString& definitionName, QContactManager::Error& error)
{
    Q_UNUSED(definitionName);
    error = QContactManager::NotSupportedError;
    return false;
}

/*!
 * Returns a list of ids of contacts that have been added to this manager
 * since the given \a timestamp.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
QList<QUniqueId> QContactManagerEngine::contactsAddedSince(const QDateTime& timestamp, QContactManager::Error& error) const
{
    Q_UNUSED(timestamp);
    error = QContactManager::NotSupportedError;
    return QList<QUniqueId>();
}

/*!
 * Returns a list of contact ids that have been modified in this manager
 * since the given \a timestamp.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
QList<QUniqueId> QContactManagerEngine::contactsModifiedSince(const QDateTime& timestamp, QContactManager::Error& error) const
{
    Q_UNUSED(timestamp);
    error = QContactManager::NotSupportedError;
    return QList<QUniqueId>();
}

/*!
 * Returns a list of contact ids that have been removed in this manager
 * since the given \a timestamp.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
QList<QUniqueId> QContactManagerEngine::contactsRemovedSince(const QDateTime& timestamp, QContactManager::Error& error) const
{
    Q_UNUSED(timestamp);
    error = QContactManager::NotSupportedError;
    return QList<QUniqueId>();
}

/*!
 * Returns a list of group ids that have been added in this manager
 * since the given \a timestamp.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
QList<QUniqueId> QContactManagerEngine::groupsAddedSince(const QDateTime& timestamp, QContactManager::Error& error) const
{
    Q_UNUSED(timestamp);
    error = QContactManager::NotSupportedError;
    return QList<QUniqueId>();
}

/*!
 * Returns a list of group ids that have been modified in this manager
 * since the given \a timestamp.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
QList<QUniqueId> QContactManagerEngine::groupsModifiedSince(const QDateTime& timestamp, QContactManager::Error& error) const
{
    Q_UNUSED(timestamp);
    error = QContactManager::NotSupportedError;
    return QList<QUniqueId>();
}

/*!
 * Returns a list of group ids that have been removed in this manager
 * since the given \a timestamp.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
QList<QUniqueId> QContactManagerEngine::groupsRemovedSince(const QDateTime& timestamp, QContactManager::Error& error) const
{
    Q_UNUSED(timestamp);
    error = QContactManager::NotSupportedError;
    return QList<QUniqueId>();
}

/*!
 * Adds the list of contacts given by \a contacts to the database.
 * Returns a list of the error codes corresponding to the contacts in
 * the \a contacts.  The \l QContactManager::error() function will
 * only return \c QContactManager::NoError if all contacts were saved
 * successfully.
 *
 * For each newly saved contact that was successful, the uid of the contact
 * in the list will be updated with the new value.  If a failure occurs
 * when saving a new contact, the id will be cleared.  If a failure occurs
 * when updating a contact that already exists, then TODO.
 *
 * At some stage after completion of the operation, the backend must emit
 * either the \c QContactManagerEngine::contactsAdded or
 * \c QContactManagerEngine::contactsChanged signal, or both, if any
 * contacts were successfully saved.  If the operation fails, neither
 * signal may be emitted as a direct result of the operation.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 *
 * \sa QContactManager::saveContact()
 */
QList<QContactManager::Error> QContactManagerEngine::saveContacts(QList<QContact>* contacts, QContactManager::Error& error)
{
    QList<QContactManager::Error> ret;
    if (!contacts) {
        error = QContactManager::BadArgumentError;
        return ret;
    } else {
        QList<QUniqueId> changedList;
        QList<QUniqueId> addedList;
        QContactManager::Error functionError = QContactManager::NoError;
        for (int i = 0; i < contacts->count(); i++) {
            QContact current = contacts->at(i);
            QUniqueId oldId = current.id();
            if (!saveContact(&current, true, error)) {
                (*contacts)[i].setId(0);
                functionError = error;
                ret.append(functionError);
            } else {
                (*contacts)[i] = current;
                ret.append(QContactManager::NoError);

                // add the contact's id to the appropriate signal list
                if (oldId != current.id()) {
                    // new contact
                    addedList.append(current.id());
                } else {
                    // old contact updated
                    changedList.append(current.id());
                }
            }
        }

        error = functionError;
        // emit the required signals.
        if (!addedList.isEmpty())
            emit contactsAdded(addedList);
        if (!changedList.isEmpty())
            emit contactsChanged(changedList);
        return ret;
    }
}

/*!
 * Remove the list of contacts identified in \a contactIds.
 * Returns a list of the error codes corresponding to the contact ids in
 * the \a contactIds.  The \l QContactManager::error() function will
 * only return \c QContactManager::NoError if all contacts were removed
 * successfully.
 *
 * For each contact that was removed succesfully, the corresponding
 * id in the list will be retained but set to zero.  The id of contacts
 * that were not successfully removed will be left alone.
 *
 * At some stage after completion of the operation, the backend must emit
 * the \c QContactManagerEngine::contactsRemoved signal if any
 * contacts were successfully removed.  If the operation fails, no such
 * signal may be emitted as a direct result of the operation.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 *
 * \sa QContactManager::removeContact()
 */
QList<QContactManager::Error> QContactManagerEngine::removeContacts(QList<QUniqueId>* contactIds, QContactManager::Error& error)
{
    QList<QContactManager::Error> ret;
    if (!contactIds) {
        error = QContactManager::BadArgumentError;
        return ret;
    } else {
        QList<QUniqueId> removedList;
        QContactManager::Error functionError = QContactManager::NoError;
        for (int i = 0; i < contactIds->count(); i++) {
            QUniqueId current = contactIds->at(i);
            if (!removeContact(current, true, error)) {
                functionError = error;
                ret.append(functionError);
            } else {
                (*contactIds)[i] = 0;
                ret.append(QContactManager::NoError);
                removedList.append(current);
            }
        }

        error = functionError;
        if (!removedList.isEmpty())
            emit contactsRemoved(removedList);
        return ret;
    }
}

/*!
 * Returns true if the supplied \a filter matches the supplied \a contact.
 *
 * The default implementation of this function will test each condition in
 * the filter, possibly recursing.
 */
bool QContactManagerEngine::testFilter(const QContactFilter &filter, const QContact &contact) const
{
    switch(filter.type()) {
        case QContactFilter::Invalid:
            return false;
            break;

        case QContactFilter::ContactDetail:
            {
                const QContactDetailFilter cdf(filter);
                if (cdf.detailDefinitionName().isEmpty())
                    return false;

                /* See if this contact has one of these details in it */
                const QList<QContactDetail>& details = contact.details(cdf.detailDefinitionName());

                if (details.count() == 0)
                    return false; /* can't match */

                /* See if we need to check the values */
                if (cdf.detailFieldName().isEmpty())
                    return true;                    /* just testing for the presence of one of these details */

                /* Now figure out what tests we are doing */
                const bool valueTest = cdf.value().isValid();
                const bool presenceTest = !valueTest;

                /* See if we need to test any values at all */
                if (presenceTest) {
                    for(int j=0; j < details.count(); j++) {
                        const QContactDetail& detail = details.at(j);
                        if (detail.values().contains(cdf.detailFieldName()))
                            return true;
                    }
                    return false;
                }

                /* Case sensitivity, for those parts that use it */
                Qt::CaseSensitivity cs = (cdf.matchFlags() & Qt::MatchCaseSensitive) ? Qt::CaseSensitive : Qt::CaseInsensitive;

                /* See what flags are requested, since we're looking at a value */
                if (cdf.matchFlags() & (Qt::MatchEndsWith | Qt::MatchStartsWith | Qt::MatchContains | Qt::MatchFixedString)) {
                    /* We're strictly doing string comparisons here */
                    bool matchStarts = (cdf.matchFlags() & 7) == Qt::MatchStartsWith;
                    bool matchEnds = (cdf.matchFlags() & 7) == Qt::MatchEndsWith;
                    bool matchContains = (cdf.matchFlags() & 7) == Qt::MatchContains;

                    /* Value equality test */
                    for(int j=0; j < details.count(); j++) {
                        const QContactDetail& detail = details.at(j);
                        const QString& var = detail.value(cdf.detailFieldName());
                        const QString& needle = cdf.value().toString();
                        if (matchStarts && var.startsWith(needle, cs))
                            return true;
                        if (matchEnds && var.endsWith(needle, cs))
                            return true;
                        if (matchContains && var.contains(needle, cs))
                            return true;
                        if (QString::compare(var, needle, cs) == 0)
                            return true;
                    }
                    return false;
                } else {
                    /* Nope, testing the values as a variant */
                    /* Value equality test */
                    // XXX value test should use cs
                    for(int j=0; j < details.count(); j++) {
                        const QContactDetail& detail = details.at(j);
                        if (detail.variantValue(cdf.detailFieldName()) == cdf.value())
                            return true;
                    }
                }
            }
            break;

        case QContactFilter::ContactDetailRange:
            {
                const QContactDetailRangeFilter cdf(filter);
                if (cdf.detailDefinitionName().isEmpty() || cdf.detailFieldName().isEmpty())
                    return false; /* we do not know which field to check */

                /* Now figure out what tests we are doing */
                if (!cdf.minValue().isValid() && !cdf.maxValue().isValid())
                    return false; // invalid range limits.

                /* See if this contact has one of these details in it */
                const QList<QContactDetail>& details = contact.details(cdf.detailDefinitionName());

                if (details.count() == 0)
                    return false; /* can't match */

                /* open or closed interval testing support */
                // relies on QString::compare returning an int, and '<= 0' is equivalent to '< 1'
                // int testing is also ok.
                // double testing is not ok - have to use conditional comparison.
                const int minComp = cdf.rangeFlags() & QContactDetailRangeFilter::ExcludeLower ? 1 : 0;
                const int maxComp = cdf.rangeFlags() & QContactDetailRangeFilter::IncludeUpper ? 1 : 0;

                /* Case sensitivity, for those parts that use it */
                Qt::CaseSensitivity cs = (cdf.matchFlags() & Qt::MatchCaseSensitive) ? Qt::CaseSensitive : Qt::CaseInsensitive;

                /* See what flags are requested, since we're looking at a value */
                if (cdf.matchFlags() & (Qt::MatchEndsWith | Qt::MatchStartsWith | Qt::MatchContains | Qt::MatchFixedString)) {
                    /* We're strictly doing string comparisons here */
                    //bool matchStarts = (cdf.matchFlags() & 7) == Qt::MatchStartsWith;
                    bool matchEnds = (cdf.matchFlags() & 7) == Qt::MatchEndsWith;
                    bool matchContains = (cdf.matchFlags() & 7) == Qt::MatchContains;

                    /* Min/Max and contains do not make sense */
                    if (matchContains)
                        return false;

                    bool testMin = cdf.minValue().isValid();
                    bool testMax = cdf.maxValue().isValid();

                    QString minVal = cdf.minValue().toString();
                    QString maxVal = cdf.maxValue().toString();

                    /* Starts with is the normal compare case, endsWith is a bit trickier */
                    for(int j=0; j < details.count(); j++) {
                        const QContactDetail& detail = details.at(j);
                        const QString& var = detail.value(cdf.detailFieldName());
                        if (!matchEnds) {
                            // MatchStarts, or MatchFixedString
                            if ((!testMin || QString::compare(var, minVal, cs) >= minComp)
                                && (!testMax || QString::compare(var, maxVal, cs) < maxComp))
                                return true;
                        } else {
                            /* Have to test the length of min & max */
                            if ((!testMin || QString::compare(var.right(minVal.length()), minVal, cs) >= minComp)
                                && (!testMax || QString::compare(var.right(maxVal.length()), maxVal, cs) < maxComp))
                                return true;
                        }
                    }

                    return false;
                } else {
                    /* Nope, testing the values as a variant */
                    bool testMin = cdf.minValue().isValid();
                    bool testMax = cdf.maxValue().isValid();

                    for(int j=0; j < details.count(); j++) {
                        const QContactDetail& detail = details.at(j);
                        const QVariant var = detail.variantValue(cdf.detailFieldName());

                        /* uggh.. we only handle some types of min/max */
                        switch(var.type()) {
                            case QVariant::Int:
                            {
                                if ((!testMin || (var.toInt() - cdf.minValue().toInt()) >= minComp)
                                 && (!testMax || (var.toInt() - cdf.maxValue().toInt()) < maxComp))
                                    return true;
                                break;
                            }

                            case QVariant::LongLong:
                            {
                                if ((!testMin || (var.toLongLong() - cdf.minValue().toLongLong()) >= minComp)
                                 && (!testMax || (var.toLongLong() - cdf.maxValue().toLongLong()) < maxComp))
                                    return true;
                                break;
                            }

                            case QVariant::Bool:
                            case QVariant::Char:
                            case QVariant::UInt:
                            {
                                if ((!testMin || long(var.toUInt() - cdf.minValue().toUInt()) >= minComp)
                                 && (!testMax || long(var.toUInt() - cdf.maxValue().toUInt()) < maxComp))
                                    return true;
                                break;
                            }

                            case QVariant::ULongLong:
                            {
                                if ((!testMin || ((long long)(var.toULongLong() - cdf.minValue().toULongLong())) >= minComp)
                                 && (!testMax || ((long long)(var.toULongLong() - cdf.maxValue().toULongLong())) < maxComp))
                                    return true;
                                break;
                            }

                            case QVariant::DateTime:
                            {
                                const QDateTime& dtvar = var.toDateTime();
                                if ((!testMin || (minComp ? dtvar > cdf.minValue().toDateTime() : dtvar >= cdf.minValue().toDateTime()))
                                    && (!testMax || (maxComp ? dtvar <= cdf.maxValue().toDateTime() : dtvar < cdf.maxValue().toDateTime())))
                                    return true;
                                break;
                            }

                            case QVariant::Date:
                            {
                                const QDate& dtvar = var.toDate();
                                if ((!testMin || (minComp ? dtvar > cdf.minValue().toDate() : dtvar >= cdf.minValue().toDate()))
                                    && (!testMax || (maxComp ? dtvar <= cdf.maxValue().toDate() : dtvar < cdf.maxValue().toDate())))
                                    return true;
                                break;
                            }

                            case QVariant::Double:
                            {
                                const double dtvar = var.toDouble();
                                if ((!testMin || (minComp ? dtvar > cdf.minValue().toDouble() : dtvar >= cdf.minValue().toDouble()))
                                    && (!testMax || (maxComp ? dtvar <= cdf.maxValue().toDouble() : dtvar < cdf.maxValue().toDouble())))
                                    return true;
                                break;
                            }

                            case QVariant::Time:
                            {
                                const QTime& dtvar = var.toTime();
                                if ((!testMin || (minComp ? dtvar > cdf.minValue().toTime() : dtvar >= cdf.minValue().toTime()))
                                    && (!testMax || (maxComp ? dtvar <= cdf.maxValue().toTime() : dtvar < cdf.maxValue().toTime())))
                                    return true;
                                break;
                            }

                            case QVariant::String:
                            {
                                if ((!testMin || QString::compare(var.toString(), cdf.minValue().toString(), cs) >= minComp)
                                    && (!testMax || QString::compare(var.toString(), cdf.maxValue().toString(), cs) < maxComp))
                                    return true;
                                break;
                            }

                            default:
                                break;
                        }
                    }
                }
            }
            break;

        case QContactFilter::GroupMembership:
            {

            }
            break;

        case QContactFilter::ChangeLog:
            {

            }
            break;

        case QContactFilter::Action:
            {

            }
            break;

        case QContactFilter::Intersection:
            {
                /* XXX In theory we could reorder the terms to put the native tests first */
                const QContactIntersectionFilter bf(filter);
                const QList<QContactFilter>& terms = bf.filters();
                if (terms.count() > 0) {
                    for(int j = 0; j < terms.count(); j++) {
                        if (!testFilter(terms.at(j), contact)) {
                            return false;
                        }
                    }
                    return true;
                }
            }
            break;

        case QContactFilter::Union:
            {
                /* XXX In theory we could reorder the terms to put the native tests first */
                const QContactUnionFilter bf(filter);
                const QList<QContactFilter>& terms = bf.filters();
                if (terms.count() > 0) {
                    for(int j = 0; j < terms.count(); j++) {
                        if (testFilter(terms.at(j), contact)) {
                            return true;
                        }
                    }
                    return false;
                }
            }
            break;
    }
    return false;
}

/*!
 * Performs insertion sort of the contact \a toAdd into the \a sorted list, according to the provided \a sortOrder
 */
void QContactManagerEngine::addSorted(QList<QContact>* sorted, const QContact& toAdd, const QContactSortOrder& sortOrder)
{
    if (!sortOrder.isValid()) {
        sorted->append(toAdd);
    } else {
        const QString& newValue = toAdd.detail(sortOrder.detailDefinitionName()).value(sortOrder.detailFieldName());

        if (newValue.isEmpty()) {
            if (sortOrder.blankPolicy() == QContactSortOrder::BlanksFirst)
                sorted->prepend(toAdd);
            else
                sorted->append(toAdd);
            return;
        }

        for (int i = 0; i < sorted->size(); i++) {
            const QContact& curr = sorted->at(i);

            // XXX this uses string compare, and is messy.
            // also, multiple details of the same type are broken.
            const QString& currValue = curr.detail(sortOrder.detailDefinitionName()).value(sortOrder.detailFieldName());
            if (sortOrder.direction() == Qt::AscendingOrder) {
                if (newValue < currValue) {
                    sorted->insert(i, toAdd);
                    return;
                }
            } else {
                if (newValue > currValue) {
                    sorted->insert(i, toAdd);
                    return;
                }
            }
        }
    }
    // couldn't find anywhere for it to go.  put it at the end.
    sorted->append(toAdd);
}


