/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
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

//#define QT_SFW_SERVICEDATABASE_DEBUG

#include <QDir>
#include "servicedatabase.h"
#include <qserviceinterfacedescriptor.h>
#include <qserviceinterfacedescriptor_p.h>
#include <QUuid>

//database name
#define RESOLVERDATABASE "services.db"

//database table names
#define SERVICE_TABLE "Service"
#define INTERFACE_TABLE "Interface"
#define DEFAULTS_TABLE "Defaults"
#define SERVICE_PROPERTY_TABLE "ServiceProperty"
#define INTERFACE_PROPERTY_TABLE "InterfaceProperty"

//default connection
#define RESOLVERDATABASE_DEFAULT_CONNECTION "qt_sql_default_connection"

//separator
#define RESOLVERDATABASE_PATH_SEPARATOR "//"

#ifdef QT_SFW_SERVICEDATABASE_DEBUG
#include <QDebug>
#endif

#define SERVICE_DESCRIPTION_KEY 0

QT_BEGIN_NAMESPACE

enum TBindIndexes
    {
        EBindIndex=0,
        EBindIndex1,
        EBindIndex2,
        EBindIndex3,
        EBindIndex4,
        EBindIndex5,
        EBindIndex6,
        EBindIndex7
    };

DBError::DBError()
{
    setError(NoError);
}

void DBError::setError(ErrorCode error, const QString &text)
{
    m_error = error;
    switch (error) {
        case (NoError):
            m_text = "No error";
            break;
        case(DatabaseNotOpen):
            m_text = "Database not open";
            break;
        case(InvalidDatabaseConnection):
            m_text = "Invalid database connection";
            break;
        case(CannotCloseDatabase):
            m_text= "Cannot close database";
            break;
        case(SqlError):
        case(NotFound):
        case(ComponentAlreadyRegistered):
        case(IfaceImplAlreadyRegistered):
        case(InvalidSearchCriteria):
            m_text = text;
            break;
        default:
            m_text= "Unknown error";
            m_error = UnknownError;
    }
}

/*!
    \class ServiceDatabase
    Utility class (used by service resolver) that offers support for resolver database management. \n
    A Sqlite database is used to store metadata for each registered service. \n
    The supported operations are: 
     - open/close database(create database tables)
     - register service(populate database tables)
     - query service metadata(retrieve data from database tables) 
*/

/*!
    Constructor
*/
ServiceDatabase::ServiceDatabase(void)
:iDatabaseOpen(false)
{
}

/*!
    Destructor
*/
ServiceDatabase::~ServiceDatabase()
{
    close();
}

/*!
    Opens the service resolver database \n
    The method creates or opens the resolver database and creates tables if they are not present
    @return 0 if operation was successfull(database and tables are available), error code otherwise
*/
bool ServiceDatabase::open()
{
    if (iDatabaseOpen)
        return true;

    QString path;

    //Create full path to database
    if(iDatabasePath.isEmpty ())
        iDatabasePath = databasePath();

    path = iDatabasePath;

    if (path.lastIndexOf(RESOLVERDATABASE_PATH_SEPARATOR) != path.length() -1) {
        path.append(RESOLVERDATABASE_PATH_SEPARATOR);
    }
    path.append(RESOLVERDATABASE);
    //Create and/or open database and create tables if neccessary
    QSqlDatabase  database;
    if(QSqlDatabase::contains(RESOLVERDATABASE_DEFAULT_CONNECTION)) {
        database = QSqlDatabase::database(RESOLVERDATABASE_DEFAULT_CONNECTION);
    } else {
        database = QSqlDatabase::addDatabase("QSQLITE");
        database.setDatabaseName(path);
    }

    if (!database.isValid()){
        m_lastError.setError(DBError::InvalidDatabaseConnection);
        close();
        return false;
    }

    //Create or open resolver database
    if (!database.isOpen()) {
        if(!database.open()) {
            m_lastError.setError(DBError::SqlError, database.lastError().text());
            qWarning() << "ServiceDatabase::open():-"
                        << "Problem:" << "Could not open database"
                        << "\nReason:" << m_lastError.text();
            close();
            return false;
        }
    }
    iDatabaseOpen = true;

    //Check database structure (tables) and recreate tables if neccessary
    //If one of tables is missing remove all tables and recreate them
    //This operation is required in order to avoid data coruption
    if (!checkTables()) {
        if(dropTables()) {
            if (createTables()) {
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
                qDebug() << "ServiceDatabase::open():-"
                    << "Database tables recreated";
#endif
            } else {
                //createTable() should've handled error message
                //and warning
                close();
                return false;
            }
        }
        else {
            //dropTables() should've handled error message
            //and warning
            close();
            return false;
        }
    }
    return true;
}

bool ServiceDatabase::registerService(ServiceMetaData &service)
{
    if(!checkConnection()) {
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::registerService():-"
                    << "Problem:" << qPrintable(m_lastError.text());
#endif
        return false;
    }

    QSqlDatabase database = QSqlDatabase::database();
    QSqlQuery query(database);

    //Begin Transaction
    if (!database.transaction()) {
        m_lastError.setSQLError(database.lastError().text());
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::registerService():-"
                    << "Unable to begin transaction"
                    << "\nReason:" << qPrintable(m_lastError.text());
#endif
        return false;
    }

    QString statement("SELECT Name from Service WHERE FilePath=? COLLATE NOCASE");
    QList<QVariant> bindValues;
    bindValues.append(service.location());
    if (!executeQuery(&query, statement, bindValues)) {
        databaseRollback(&query, &database);
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::registerService():-"
                    << qPrintable(m_lastError.text());
#endif
        return false;
    }

    if (query.next()) {
        QString alreadyRegisteredService = query.value(EBindIndex).toString();
        const QString errorText = "Cannot register service \"%1\". Service component \"%2\" already "
                    "registered to service \"%3\".  \"%3\" must first be deregistered "
                    "for new registration to take place.";

        m_lastError.setError(DBError::ComponentAlreadyRegistered,
                errorText.arg(service.name())
                        .arg(service.location())
                        .arg(alreadyRegisteredService));

        databaseRollback(&query, &database);
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::registerService():-"
                    << "Problem:" << qPrintable(m_lastError.text());
#endif
        return false;
    }

    statement = "INSERT INTO Service(ID,Name,FilePath) VALUES(?,?,?)";
    QString serviceID = QUuid::createUuid().toString();

    bindValues.clear();
    bindValues.append(serviceID);
    bindValues.append(service.name());
    bindValues.append(service.location());

    if (!executeQuery(&query, statement, bindValues)) {
        databaseRollback(&query, &database);
        qWarning() << "ServiceDatabase::registerService():-"
                    << qPrintable(m_lastError.text());
        return false;
    }

    statement = "INSERT INTO ServiceProperty(ServiceID,Key,Value) VALUES(?,?,?)";
    bindValues.clear();
    bindValues.append(serviceID);
    bindValues.append(SERVICE_DESCRIPTION_KEY);
    if (service.description().isNull())
        bindValues.append("");
    else
        bindValues.append(service.description());

    if (!executeQuery(&query, statement, bindValues)) {
        databaseRollback(&query, &database);
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::registerService():-"
                    << qPrintable(m_lastError.text());
#endif
        return false;
    }

    QList <QServiceInterfaceDescriptor> interfaces = service.getInterfaces();
    QString interfaceID;;
    bool ok = false;
    foreach (const QServiceInterfaceDescriptor &interface, interfaces) {
        interfaceID = getInterfaceID(&query, interface, &ok);
        if (!ok) {
            databaseRollback(&query, &database);
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
            qWarning() << "ServiceDatabase::registerService():-"
                        << "Unable to confirm if implementation version"
                        << (QString::number(interface.majorVersion()) + "."
                           + QString::number(interface.minorVersion())).toAscii()
                        << "for interface" << interface.interfaceName()
                        << "is already registered for service "
                        << interface.serviceName()
                        << qPrintable(QString("\n") + m_lastError.text());
#endif
            return false;
        }

        if (!interfaceID.isEmpty()) {
            QString errorText;
            errorText = "Cannot register service \"%1\". \"%1\" is already registered "
                        "and implements interface \"%2\", Version \"%3.%4.\"  \"%1\" must "
                        "first be deregistered for new registration to take place.";
            m_lastError.setError(DBError::IfaceImplAlreadyRegistered,
                                errorText.arg(interface.serviceName())
                                            .arg(interface.interfaceName())
                                            .arg(interface.majorVersion())
                                            .arg(interface.minorVersion()));

            databaseRollback(&query, &database);
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
            qWarning() << "ServiceDatabase::registerService():-"
                        << "Problem:" << qPrintable(m_lastError.text());
#endif
            return false;
        } else {
            //No interface implementation already exists for the service
            //so add it
            if(!insertInterfaceData(&query, interface, serviceID)) {
                databaseRollback(&query, &database);
                return false;
            }
        }
    }
    interfaces = service.latestInterfaces();
    QServiceInterfaceDescriptor defaultInterface;
    foreach(const QServiceInterfaceDescriptor &interface, interfaces) {
        defaultInterface = defaultServiceInterface(interface.interfaceName(), &ok);
        if (!ok) {
            databaseRollback(&query, &database);
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
            qWarning() << "ServiceDatabase::registerService()"
                        << "Problem: Unable to confirm if interface"
                        << interface.interfaceName()
                        << "already has a default implementation";
#endif
            return false;
        }

        if (defaultInterface.isValid())
            continue;

        //default does not already exist so create one
        interfaceID = getInterfaceID(&query, interface, &ok);
        if (!ok) {
            databaseRollback(&query, &database);
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
            qWarning() << "ServiceDatabase::registerService():-"
                        << "Unable to retrieve interfaceID for "
                           "interface" << interface.interfaceName()
                            <<  qPrintable(QString("\n") + m_lastError.text());
#endif
            return false;
        }

        statement = "INSERT INTO Defaults(InterfaceName, InterfaceID) VALUES(?,?)";
        bindValues.clear();
        bindValues.append(interface.interfaceName());
        bindValues.append(interfaceID);
        if (!executeQuery(&query, statement, bindValues)) {
            databaseRollback(&query, &database);
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
            qWarning() << "ServiceDatabase::registerService():-"
                        << qPrintable(m_lastError.text());
#endif
            return false;
        }
    }

    //End Transaction
    databaseCommit(&query, &database);
    return true;
}

QString ServiceDatabase::getInterfaceID(QSqlQuery *query, const QServiceInterfaceDescriptor &interface, bool *ok)
{
    QString statement = "SELECT Interface.ID "
                        "FROM Interface, Service "
                        "WHERE Service.ID = Interface.ServiceID "
                        "AND Service.Name = ? COLLATE NOCASE "
                        "AND Interface.Name = ? COLLATE NOCASE "
                        "AND Interface.VerMaj = ? AND Interface.VerMin = ?";
    QList<QVariant> bindValues;
    bindValues.append(interface.serviceName());
    bindValues.append(interface.interfaceName());
    bindValues.append(interface.majorVersion());
    bindValues.append(interface.minorVersion());

    if (!executeQuery(query, statement, bindValues)) {
        if (ok != NULL)
            *ok = false;
        return QString();
    }
    if (!query->next()) {
        if (ok != NULL)
            *ok = true;
        return QString();
    }
    if (ok != NULL)
        *ok = true;
    return query->value(EBindIndex).toString();
}

/*!
    Helper method that saves interface related data in the Interface table
    @param aQuery database query
    @param anInterface interface metadata
    @param aServiceID service identifier 
    @return 0 if operation was successfull, error code otherwise
*/
bool ServiceDatabase::insertInterfaceData(QSqlQuery *query,const QServiceInterfaceDescriptor &interface, const QString &serviceID)
{
    QString statement = "INSERT INTO Interface(ID, ServiceID,Name,VerMaj, VerMin) "
                        "VALUES(?,?,?,?,?)";
    QString interfaceID = QUuid::createUuid();

    QList<QVariant> bindValues;
    bindValues.append(interfaceID);
    bindValues.append(serviceID);
    bindValues.append(interface.interfaceName());
    bindValues.append(interface.majorVersion());
    bindValues.append(interface.minorVersion());

    if (!executeQuery(query, statement, bindValues)) {
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::insertInterfaceData():-"
                    << qPrintable(m_lastError.text());
#endif
        return false;
    }

    statement = "INSERT INTO InterfaceProperty(InterfaceID, Key, Value) VALUES(?,?,?)";
    QHash<QServiceInterfaceDescriptor::PropertyKey, QVariant>::const_iterator iter = interface.d->properties.constBegin();
    bool isValidProperty;
    QString capabilities;
    QString interfaceDescription;
    while (iter != interface.d->properties.constEnd()) {
        isValidProperty = true;

        bindValues.clear();
        bindValues.append(interfaceID);
        switch (iter.key()) {
            case (QServiceInterfaceDescriptor::Capabilities):
                bindValues.append(QServiceInterfaceDescriptor::Capabilities);
                capabilities = interface.property(QServiceInterfaceDescriptor::Capabilities).toStringList().join(",");
                if (capabilities.isNull())
                    capabilities = "";
                bindValues.append(capabilities);
                break;
            case(QServiceInterfaceDescriptor::Location):
                isValidProperty = false;
                break;
            case(QServiceInterfaceDescriptor::ServiceDescription):
                isValidProperty = false;
                break;
            case(QServiceInterfaceDescriptor::InterfaceDescription):
                bindValues.append(QServiceInterfaceDescriptor::InterfaceDescription);
                interfaceDescription = interface.property(QServiceInterfaceDescriptor::InterfaceDescription).toString();
                if (interfaceDescription.isNull())
                    interfaceDescription = "";
                bindValues.append(interfaceDescription);
                break;
            default:
                isValidProperty = false;
                break;
        }

        if (isValidProperty) {
              if (!executeQuery(query, statement, bindValues)) {
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
                  qWarning() << "ServiceDatabase::insertInterfaceData():-"
                                << qPrintable(m_lastError.text());
#endif
                  return false;
              }
        }
        ++iter;
    }
    return true;
}

bool ServiceDatabase::executeQuery(QSqlQuery *query, const QString &statement, const QList<QVariant> &bindValues)
{
    Q_ASSERT(query != NULL);
    if (!query->prepare(statement))
    {
        QString errorText;
        errorText = "Problem: Could not prepare statement: %1"
                    "\nReason: %2"
                    "\nParameters: %3\n";
        QString parameters;
        if (bindValues.count() > 0) {
            for(int i = 0; i < bindValues.count(); ++i) {
                parameters.append(QString("\n\t[") + QString::number(i) + "]: " + bindValues.at(i).toString());
            }
        } else {
            parameters = "None";
        }
        m_lastError.setSQLError(errorText.arg(statement)
                                        .arg(query->lastError().text())
                                        .arg(parameters));
        query->finish();
        query->clear();
        return false;
    }

    foreach(const QVariant &bindValue, bindValues)
        query->addBindValue(bindValue);

    if(!query->exec())
    {
        QString errorText;
        errorText = "Problem: Could not execute statement: %1"
                    "\nReason: %2"
                    "\nParameters: %3\n";
        QString parameters;
        if (bindValues.count() > 0) {
            for(int i = 0; i < bindValues.count(); ++i) {
                parameters.append(QString("\n\t[") + QString::number(i) + "]: " + bindValues.at(i).toString());

            }
        } else {
            parameters = "None";
        }
        m_lastError.setSQLError(errorText.arg(statement)
                                        .arg(query->lastError().text())
                                        .arg(parameters));
        query->finish();
        query->clear();
        return false;
    }
    return true;
}

QList<QServiceInterfaceDescriptor> ServiceDatabase::getInterfaces(const QServiceFilter &filter, bool *ok)
{
    QList<QServiceInterfaceDescriptor> interfaces;
    if (!checkConnection()) {
        if (ok != NULL)
            *ok =false;
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::getInterfaces():-"
                    << "Problem:" << qPrintable(m_lastError.text());
#endif
        return interfaces;
    }

    QSqlDatabase database = QSqlDatabase::database();
    QSqlQuery query(database);

    //Prepare search query, bind criteria values and execute search
    QString selectComponent = "SELECT Interface.Name, "
                                "Service.Name, Interface.VerMaj, "
                                "Interface.VerMin, "
                                "Service.FilePath, "
                                "Service.ID, "
                                "Interface.ID ";
    QString fromComponent = "FROM Interface, Service ";
    QString whereComponent = "WHERE Service.ID = Interface.ServiceID ";
    QList<QVariant> bindValues;

    if (filter.service.isEmpty() && filter.interface.isEmpty()) {
        //do nothing, (don't add any extra constraints to the query
    } else {

        if (!filter.service.isEmpty()) {
            whereComponent.append("AND Service.Name = ?").append(" COLLATE NOCASE ");
            bindValues.append(filter.service);
        }
        if (!filter.interface.isEmpty()) {
            whereComponent.append("AND Interface.Name = ?").append(" COLLATE NOCASE ");
            bindValues.append(filter.interface);
            if (filter.majorVersion >=0 && filter.minorVersion >=0) {
                if (filter.matchingRule == QServiceFilter::ExactVersionMatch) {
                    whereComponent.append("AND Interface.VerMaj = ?").append(" AND Interface.VerMin = ? ");
                    bindValues.append(QString::number(filter.majorVersion));
                    bindValues.append(QString::number(filter.minorVersion));
                }
                else if (filter.matchingRule == QServiceFilter::MinimumVersionMatch) {
                    whereComponent.append("AND ((Interface.VerMaj > ?")
                        .append(") OR Interface.VerMaj = ?").append(" AND Interface.VerMin >= ?").append(")");
                    bindValues.append(QString::number(filter.majorVersion));
                    bindValues.append(QString::number(filter.majorVersion));
                    bindValues.append(QString::number(filter.minorVersion));
                }
            }
        }
    }

    if (!executeQuery(&query, selectComponent + fromComponent + whereComponent, bindValues)) {
        if (ok != NULL)
            *ok = false;
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::getInterfaces():-"
                    << "Problem:" << qPrintable(m_lastError.text());
#endif
        return interfaces;
    }

    QServiceInterfaceDescriptor interface;
    interface.d = new QServiceInterfaceDescriptorPrivate;
    QStringList capabilities;
    QString serviceID;
    QString interfaceID;
    while(query.next()){
        interface.d->interfaceName =query.value(EBindIndex).toString();
        interface.d->serviceName = query.value(EBindIndex1).toString();
        interface.d->major = query.value(EBindIndex2).toInt();
        interface.d->minor = query.value(EBindIndex3).toInt();

        interface.d->properties[QServiceInterfaceDescriptor::Location]
            = query.value(EBindIndex4).toString();

        serviceID = query.value(EBindIndex5).toString();
        if (!populateServiceProperties(&interface, serviceID)) {
            if (ok != NULL)
                *ok = false;
            //populateServiceProperties should already give a warning message
            //and set the last error
            interfaces.clear();
            return interfaces;
        }

        interfaceID = query.value(EBindIndex6).toString();
        if (!populateInterfaceProperties(&interface, interfaceID)) {
            if (ok != NULL)
                *ok = false;
            //populateInterfaceProperties should already give a warning message
            //and set the last error
            interfaces.clear();
            return interfaces;
        }
        interfaces.append(interface);
    }

    query.finish();
    query.clear();
    if (ok != NULL)
        *ok = true;
    return interfaces;
}

QStringList ServiceDatabase::getServiceNames(const QString &interfaceName, bool *ok)
{
    QStringList services;
    if (!checkConnection()) {
        if (ok != NULL) {
            *ok = false;
        }
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::getServiceNames():-"
                    << "Problem:" << qPrintable(m_lastError.text());
#endif
        return services;
    }
    QSqlDatabase database = QSqlDatabase::database();
    QSqlQuery query(database);
    QString selectComponent("SELECT DISTINCT Service.Name COLLATE NOCASE ");
    QString fromComponent;
    QString whereComponent;
    QList<QVariant> bindValues;
    if (interfaceName.isEmpty()) {
        fromComponent = "FROM Service ";
    } else {
        fromComponent = "FROM Interface,Service ";
        whereComponent = "WHERE Service.ID = Interface.ServiceID AND Interface.Name = ? COLLATE NOCASE ";
        bindValues.append(interfaceName);
    }

    if (!executeQuery(&query, selectComponent + fromComponent + whereComponent, bindValues)) {
        if (ok != NULL)
            *ok = false;
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::getServiceNames():-"
                    << qPrintable(m_lastError.text());
#endif
        return services;
    }

    while( query.next()) {
        services.append(query.value(EBindIndex).toString());
    }
    if (ok != NULL)
        *ok = true;
    query.finish();
    query.clear();
    return services;
}

QServiceInterfaceDescriptor ServiceDatabase::defaultServiceInterface(const QString &interfaceName, bool *ok)
{
    QServiceInterfaceDescriptor interface;
    if (!checkConnection())
    {
        if (ok != NULL)
            *ok = false;
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::defaultServiceInterface():-"
                    << "Problem:" << qPrintable(m_lastError.text());
#endif
        return interface;
    }

    QSqlDatabase database = QSqlDatabase::database();
    QSqlQuery query(database);
    QString statement("SELECT InterfaceID FROM Defaults WHERE InterfaceName = ? COLLATE NOCASE");
    QList<QVariant> bindValues;
    bindValues.append(interfaceName);
    if(!executeQuery(&query, statement, bindValues)) {
        if (ok != NULL)
            *ok = false;
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::defaultServiceInterface():-"
                    << qPrintable(m_lastError.text());
#endif
        return interface;
    }

    QString interfaceID;
    if (!query.next())
    {
        if ( ok != NULL )
            *ok = true;
        return interface;
    }
    else
        interfaceID = query.value(EBindIndex).toString();
    Q_ASSERT(!interfaceID.isEmpty());

    statement = "SELECT Interface.Name, "
                        "Service.Name, Interface.VerMaj, "
                        "Interface.VerMin, "
                        "Service.FilePath, "
                        "Service.ID "
                    "FROM Service, Interface "
                    "WHERE Service.ID = Interface.ServiceID AND Interface.ID = ?";
    bindValues.clear();
    bindValues.append(interfaceID);
    if(!executeQuery(&query, statement, bindValues))
    {
        if (ok != NULL)
            *ok = false;
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::defaultServiceInterface():-"
                    << qPrintable(m_lastError.text());
#endif
        return interface;
    }

    if(!query.next()) {
        QString errorText;
        errorText = "Database integrity has been corrupted, InterfaceID: %1, does not "
                    "exist in the Interface table for interface: %2";
        m_lastError.setSQLError(errorText.arg(interfaceID).arg(interfaceName));

        if (ok != NULL)
            *ok = false;

#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::defaultServiceInterface():-"
                    << "Problem:" << qPrintable(m_lastError.text());
#endif
        return interface;
    }

    interface.d = new QServiceInterfaceDescriptorPrivate;
    interface.d->interfaceName =query.value(EBindIndex).toString();
    interface.d->serviceName = query.value(EBindIndex1).toString();
    interface.d->major = query.value(EBindIndex2).toInt();
    interface.d->minor = query.value(EBindIndex3).toInt();

    interface.d->properties[QServiceInterfaceDescriptor::Location]
        = query.value(EBindIndex4).toString();

    QString serviceID = query.value(EBindIndex5).toString();
    if (!populateServiceProperties(&interface, serviceID)) {
        if (ok != NULL)
            *ok = false;
        //populateServiceProperties should already give a warning
        //and set the last error
        return QServiceInterfaceDescriptor();
    }

    if (!populateInterfaceProperties(&interface, interfaceID)) {
        if (ok != NULL)
            *ok = false;
        //populateInterfaceProperties should already give a warning
        //and set the last error
        return QServiceInterfaceDescriptor();
    }

    if (ok != NULL)
        *ok = true;
    return interface;
}

bool ServiceDatabase::setDefaultService(const QString &serviceName, const QString &interfaceName)
{
    if(!checkConnection()) {
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::setDefaultService(QString,QString):-"
                    << "Problem:" << qPrintable(m_lastError.text());
#endif
        return false;
    }

    QSqlDatabase database = QSqlDatabase::database();
    QSqlQuery query(database);

    //Begin Transaction
    if (!database.transaction()) {
        m_lastError.setSQLError(database.lastError().text());
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::setDefaultService(QString,Qtring):-"
                    << "Problem: Unable to begin transaction"
                    << "\nReason:" << qPrintable(m_lastError.text());
#endif
        return false;
    }

    //Get the interface id to be placed in the Defaults table
    QString statement("SELECT Interface.ID from Interface, Service "
                        "WHERE Service.ID = Interface.ServiceID "
                            "AND Service.Name = ? COLLATE NOCASE "
                            "AND Interface.Name = ? COLLATE NOCASE "
                        "ORDER BY Interface.VerMaj DESC, Interface.VerMin DESC");
    QList<QVariant> bindValues;
    bindValues.append(serviceName);
    bindValues.append(interfaceName);

    if(!executeQuery(&query, statement, bindValues)) {
        databaseRollback(&query, &database);
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::setDefaultService(QString,QString):-"
                    << qPrintable(m_lastError.text());
#endif
        return false;
    }

    if (!query.next()) {
        QString errorText;
        errorText = "No implementation for interface \"%1\" found for service \"%2\"";
        m_lastError.setNotFoundError(errorText.arg(interfaceName).arg(serviceName));

        databaseRollback(&query, &database);
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::setDefaultService(QString,QString):-"
                    << "Problem: Unable to set default service"
                    << "\nReason:" << qPrintable(m_lastError.text());
#endif
        return false;
    }

    QString interfaceID = query.value(EBindIndex).toString();
    Q_ASSERT(!interfaceID.isEmpty());

    statement = "UPDATE Defaults "
                "SET InterfaceID = ? "
                "WHERE InterfaceName = ? COLLATE NOCASE";

    bindValues.clear();
    bindValues.append(interfaceID);
    bindValues.append(interfaceName);
    if(!executeQuery(&query, statement, bindValues)) {
        databaseRollback(&query, &database);
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::setDefaultService(QString,QString):-"
                    << qPrintable(m_lastError.text());
#endif
        return false;
    }

    //End Transaction
    databaseCommit(&query, &database);
    return true;
}

bool ServiceDatabase::setDefaultService(const QServiceInterfaceDescriptor &interface)
{
    if(!checkConnection()) {
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::setDefaultService(QServiceInterfaceDescriptor):-"
            << "Problem:" << qPrintable(m_lastError.text());
#endif
        return false;
    }

    QSqlDatabase database = QSqlDatabase::database();
    QSqlQuery query(database);

    //Begin Transaction
    if(!database.transaction()) {
        m_lastError.setSQLError(database.lastError().text());
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::setDefaultService(QServiceInterfaceDescriptor):-"
            << "Problem: Unable to begin transaction"
            << "\nReason:" << qPrintable(m_lastError.text());
#endif
        return false;
    }

    QString statement("SELECT Interface.ID from Interface, Service "
            "WHERE Service.ID = Interface.ServiceID "
            "AND Service.Name = ? COLLATE NOCASE "
            "AND Interface.Name = ? COLLATE NOCASE "
            "AND Interface.VerMaj = ? "
            "AND Interface.VerMin = ? ");
    QList<QVariant> bindValues;
    bindValues.append(interface.serviceName());
    bindValues.append(interface.interfaceName());
    bindValues.append(interface.majorVersion());
    bindValues.append(interface.minorVersion());

    if (!executeQuery(&query, statement, bindValues)) {
        databaseRollback(&query, &database);
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::setDefaultService(QServiceInterfaceDescriptor):-"
                    << qPrintable(m_lastError.text());
#endif
        return false;
    }

    if(!query.next()) {
        QString errorText;
        errorText = "No implementation for interface: %1, Version: %2.%3 found "
            "for service: %4";
        m_lastError.setNotFoundError(errorText.arg(interface.interfaceName())
                .arg(interface.majorVersion())
                .arg(interface.minorVersion())
                .arg(interface.serviceName()));

        databaseRollback(&query, &database);
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatbase::setDefaultService(QServiceInterfaceDescriptor):-"
            << "Problem: Unable to set default service"
            << "\nReason:" << qPrintable(m_lastError.text());
#endif
        return false;
    }

    QString interfaceID = query.value(EBindIndex).toString();
    Q_ASSERT(!interfaceID.isEmpty());

    statement = "UPDATE Defaults "
        "SET InterfaceID = ? "
        "WHERE InterfaceName = ? COLLATE NOCASE";
    bindValues.clear();
    bindValues.append(interfaceID);
    bindValues.append(interface.interfaceName());

    if (!executeQuery(&query, statement, bindValues)) {
        databaseRollback(&query, &database);
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::setDefaultService(QServiceInterfaceDescriptor):-"
                    << qPrintable(m_lastError.text());
#endif
        return false;
    }

    //End Transaction
    databaseCommit(&query, &database);
    return true;
}

bool ServiceDatabase::unregisterService(const QString &serviceName)
{
    if (!checkConnection()) {
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::unregisterService():-"
                    << "Problem:" << qPrintable(m_lastError.text());
#endif
        return false;
    }

    QSqlDatabase database = QSqlDatabase::database();
    QSqlQuery query(database);

    //Begin Transaction
    if(!database.transaction()) {
        m_lastError.setSQLError(database.lastError().text());
        qWarning() << "ServiceDatabase::unregisterService():-"
                    << "Problem: Unable to begin transaction"
                    << "\nReason:" << qPrintable(m_lastError.text());
        return false;
    }

    QString statement("SELECT Service.ID from Service WHERE Service.Name = ? COLLATE NOCASE");
    QList<QVariant> bindValues;
    bindValues.append(serviceName);
    if(!executeQuery(&query, statement, bindValues)) {
        databaseRollback(&query, &database);
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::unregisterService():-"
                    << qPrintable(m_lastError.text());
#endif
        return false;
    }

    QStringList serviceIDs;
    while(query.next()) {
        serviceIDs << query.value(EBindIndex).toString();
    }

    statement = "SELECT Interface.ID from Interface, Service "
                "WHERE Interface.ServiceID = Service.ID "
                    "AND Service.Name =? COLLATE NOCASE";
    bindValues.clear();
    bindValues.append(serviceName);
    if (!executeQuery(&query, statement, bindValues)) {
        databaseRollback(&query, &database);
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::unregisterService():-"
                    << qPrintable(m_lastError.text());
#endif
        return false;
    }

    QStringList interfaceIDs;
    while (query.next()) {
        interfaceIDs << query.value(EBindIndex).toString();
    }

    if (serviceIDs.count() == 0) {
        QString errorText("Service not found: \"%1\"");
        m_lastError.setError(DBError::NotFound, errorText.arg(serviceName));
        databaseRollback(&query, &database);
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::unregisterService():-"
                    << "Problem: Unable to unregister service"
                    << "\nReason:" << qPrintable(m_lastError.text());
#endif
        return false;
    }

    statement = "SELECT Defaults.InterfaceName "
                "FROM Defaults, Interface, Service "
                "WHERE Defaults.InterfaceID = Interface.ID "
                    "AND Interface.ServiceID = Service.ID "
                    "AND Service.Name = ? COLLATE NOCASE";
    bindValues.clear();
    bindValues.append(serviceName);
    if (!executeQuery(&query, statement, bindValues)) {
        databaseRollback(&query, &database);
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase:unregisterService():-"
                    << qPrintable(m_lastError.text());
#endif
        return false;
    }

    QStringList serviceDefaultInterfaces;
    while(query.next()) {
        serviceDefaultInterfaces << query.value(EBindIndex).toString();
    }

    statement = "DELETE FROM Service WHERE Service.Name = ? COLLATE NOCASE";
    bindValues.clear();
    bindValues.append(serviceName);
    if (!executeQuery(&query, statement, bindValues)) {
        databaseRollback(&query, &database);
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::unregisterService():-"
                    << qPrintable(m_lastError.text());
#endif
        return false;
    }

    statement = "DELETE FROM Interface WHERE Interface.ServiceID = ?";
    foreach(const QString &serviceID, serviceIDs) {
        bindValues.clear();
        bindValues.append(serviceID);
        if (!executeQuery(&query, statement, bindValues)) {
            databaseRollback(&query, &database);
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
            qWarning() << "ServiceDatabase::unregisterService():-"
                        << qPrintable(m_lastError.text());
#endif
            return false;
        }
    }

    statement = "DELETE FROM ServiceProperty WHERE ServiceID = ?";
    foreach(const QString &serviceID, serviceIDs) {
        bindValues.clear();
        bindValues.append(serviceID);
        if (!executeQuery(&query, statement, bindValues)) {
            databaseRollback(&query, &database);
            qWarning() << "ServiceDatabase::unregisterService():-"
                        << qPrintable(m_lastError.text());
            return false;
        }
    }

    statement = "DELETE FROM InterfaceProperty WHERE InterfaceID = ?";
    foreach(const QString &interfaceID,  interfaceIDs) {
        bindValues.clear();
        bindValues.append(interfaceID);
        if (!executeQuery(&query, statement, bindValues)) {
            databaseRollback(&query, &database);
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
            qWarning() << "ServiceDatabase::unregisterService():-"
                        << qPrintable(m_lastError.text());
#endif
            return false;
        }
    }

    foreach(const QString &interfaceName, serviceDefaultInterfaces) {
        statement = "SELECT ID FROM Interface WHERE Interface.Name = ? COLLATE NOCASE "
                    "ORDER BY Interface.VerMaj DESC, Interface.VerMin DESC";
        bindValues.clear();
        bindValues.append(interfaceName);
        if (!executeQuery(&query, statement, bindValues)) {
            databaseRollback(&query, &database);
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
            qWarning() << "ServiceDatabase::unregisterService():-"
                        << qPrintable(m_lastError.text());
#endif
            return false;
        }

        if(query.next()) {
            QString newDefaultID = query.value(EBindIndex).toString();
            statement = "UPDATE Defaults SET InterfaceID = ? WHERE InterfaceName = ? COLLATE NOCASE ";
            bindValues.clear();
            bindValues.append(newDefaultID);
            bindValues.append(interfaceName);
            if (!executeQuery(&query, statement, bindValues)) {
                databaseRollback(&query, &database);
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
                qWarning() << "ServiceDatabase::unregisterService():-"
                            << qPrintable(m_lastError.text());
#endif
                return false;
            }
        } else {
            statement = "DELETE FROM Defaults WHERE InterfaceName = ? COLLATE NOCASE ";
            bindValues.clear();
            bindValues.append(interfaceName);
            if (!executeQuery(&query, statement, bindValues)) {
                databaseRollback(&query, &database);
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
                qWarning() << "ServiceDatabase::unregisterService():-"
                            << qPrintable(m_lastError.text());
#endif
                return false;
            }
        }
    }

    //databaseCommit
    databaseCommit(&query, &database);
    return true;
}

bool ServiceDatabase::close()
{
    if(iDatabaseOpen) {
        QSqlDatabase database = QSqlDatabase::database();
        if (database.isValid()){
            if(database.isOpen()) {
                database.close();
                iDatabaseOpen = database.isOpen();
                if (iDatabaseOpen) {
                    m_lastError.setError(DBError::CannotCloseDatabase);
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
                    qWarning() << "ServiceDatabase::close():-"
                                << "Problem:" << qPrintable(m_lastError.text());
#endif
                    return false;
                } else {
                    return true;
                }
            }
        } else {
            m_lastError.setError(DBError::InvalidDatabaseConnection);
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
            qWarning() << "ServiceDatabase::close():-"
                        << "Problem: " << qPrintable(m_lastError.text());
#endif
            return false;
        }
    }
    return true;
}

/*!
    Sets the path of the service database
    @return void
*/
void ServiceDatabase::setDatabasePath(const QString &aDatabasePath)
{
    iDatabasePath = aDatabasePath; 
}

/*!
    returns the path of the service database
    @return QString
*/
QString ServiceDatabase::databasePath() const
{
    QString path;
    if(iDatabasePath.isEmpty()) {
        QSettings settings(QSettings::SystemScope, "Nokia", "Services");
        path = settings.value("ServicesDB/Path").toString();
        if (path.isEmpty()) {
            path = QDir::currentPath();
        }
    } else {
        path = iDatabasePath;
    }

    return path;
}

/*!
    Creates the database tables: Service, Interface, Defaults, Properties
    Sets the last error when error condition occurs.
*/
bool ServiceDatabase::createTables()
{
    QSqlDatabase database = QSqlDatabase::database();
    QSqlQuery query(database);

    //Begin Transaction
    if (!database.transaction()) {
        m_lastError.setError(DBError::SqlError, database.lastError().text());
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::createTables():-"
                    << "Unable to begin transaction"
                    << "\nReason:" << qPrintable(m_lastError.text());
#endif
        return false;
    }

    QString statement("CREATE TABLE Service("
                        "ID TEXT NOT NULL PRIMARY KEY UNIQUE,"
                        "Name TEXT NOT NULL, "
                        "FilePath TEXT NOT NULL)");
    if (!executeQuery(&query, statement)) {
        databaseRollback(&query, &database);
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::createTables():-"
                    << qPrintable(m_lastError.text());
#endif
        return false;
    }

    statement = "CREATE TABLE Interface("
                "ID TEXT NOT NULL PRIMARY KEY UNIQUE,"
                "ServiceID TEXT NOT NULL, "
                "Name TEXT NOT NULL, "
                "VerMaj INTEGER NOT NULL, "
                "VerMin INTEGER NOT NULL)";
    if (!executeQuery(&query, statement)) {
        databaseRollback(&query, &database);
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::createTables():-"
                    << qPrintable(m_lastError.text());
#endif
        return false;
    }

    statement = "CREATE TABLE Defaults("
                "InterfaceName TEXT PRIMARY KEY UNIQUE NOT NULL,"
                "InterfaceID TEXT NOT NULL)";
    if (!executeQuery(&query, statement)) {
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::createTables():-"
                    << qPrintable(m_lastError.text());
#endif
        return false;
    }

    statement = "CREATE TABLE ServiceProperty("
                "ServiceID TEXT NOT NULL,"
                "Key INTEGER NOT NULL,"
                "Value TEXT NOT NULL)";
    if (!executeQuery(&query, statement)) {
        qWarning() << "ServiceDatabase::createTables():-"
                    << qPrintable(m_lastError.text());
        return false;
    }

    statement = "CREATE TABLE InterfaceProperty("
                "InterfaceID TEXT NOT NULL,"
                "Key INTEGER NOT NULL,"
                "Value TEXT NOT NULL)";

    if (!executeQuery(&query, statement)) {
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::createTables():-"
                    << qPrintable(m_lastError.text());
#endif
        return false;
    }

    //End Transaction
    databaseCommit(&query, &database);
    return true;
}

/*!
    Checks if the service related tables exist in the database
    @return true if tables are present, false otherwise
*/
bool ServiceDatabase::checkTables()
{
    bool bTables(false);
    QStringList tables = QSqlDatabase::database().tables();
    if (tables.contains(SERVICE_TABLE)
        && tables.contains(INTERFACE_TABLE)
        && tables.contains(DEFAULTS_TABLE)
        && tables.contains(SERVICE_PROPERTY_TABLE)
        && tables.contains(INTERFACE_PROPERTY_TABLE)){
            bTables = true;
    }
    return bTables;
}

/*!
    Removes service related tables from database
    @return 0 if was sucessful, error code otherwise
*/
bool ServiceDatabase::dropTables()
{
    //Execute transaction for deleting the database tables
    QSqlDatabase database = QSqlDatabase::database();
    QStringList expectedTables;
    expectedTables << SERVICE_TABLE
                << INTERFACE_TABLE
                << DEFAULTS_TABLE
                << SERVICE_PROPERTY_TABLE
                << INTERFACE_PROPERTY_TABLE;

    if (database.tables().count() > 0) {
        if (!database.transaction()) {
            m_lastError.setError(DBError::SqlError, database.lastError().text());
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
            qWarning() << "ServiceDatabase::dropTables():-"
                        << "Unable to begin transaction"
                        << "\nReason:" << qPrintable(m_lastError.text());
#endif
            return false;
        }
        QStringList actualTables = database.tables();
        QSqlQuery query(database);


        foreach(QString expectedTable, expectedTables) {
            if ((actualTables.contains(expectedTable))
                && (!executeQuery(&query, QString("DROP TABLE ") + expectedTable))) {
                databaseRollback(&query, &database);
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
                qWarning() << "ServiceDatabase::dropTables():-"
                           << qPrintable(m_lastError.text());
#endif
                return false;
            }
        }
        databaseCommit(&query, &database);
    }
    return true;
}

/*!
    Checks if the resolver database is open
    @return true if database is open, false otherwise
*/
bool ServiceDatabase::isOpen() const
{
  return iDatabaseOpen;
}

bool ServiceDatabase::checkConnection()
{
    if(!iDatabaseOpen)
    {
        m_lastError.setError(DBError::DatabaseNotOpen);
        return false;
    }

    if (!QSqlDatabase::database().isValid())
    {
        m_lastError.setError(DBError::InvalidDatabaseConnection);
        return false;
    }

    return true;
}

void ServiceDatabase::databaseCommit(QSqlQuery *query, QSqlDatabase *database)
{
    Q_ASSERT(query != NULL);
    Q_ASSERT(database != NULL);
    query->finish();
    query->clear();
    database->commit();
}

void ServiceDatabase::databaseRollback(QSqlQuery *query, QSqlDatabase *database)
{
    Q_ASSERT(query !=NULL);
    Q_ASSERT(database != NULL);
    query->finish();
    query->clear();
    database->rollback();
}

bool ServiceDatabase::populateInterfaceProperties(QServiceInterfaceDescriptor *interface, const QString &interfaceID)
{
    QSqlQuery query(QSqlDatabase::database());
    QString statement("SELECT Key, Value FROM InterfaceProperty WHERE InterfaceID = ?");
    QList<QVariant> bindValues;
    bindValues.append(interfaceID);
    if (!executeQuery(&query, statement, bindValues)) {
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::populateInterfaceProperties():-"
                    << qPrintable(m_lastError.text());
#endif
        return false;
    }

    bool isFound = false;
    int propertyKey;
    while (query.next()) {
        isFound = true;
        propertyKey = query.value(EBindIndex).toInt();
        switch (propertyKey) {
            case(QServiceInterfaceDescriptor::Capabilities): {
                    QStringList capabilities = query.value(EBindIndex1).toString().split(",");

                    if (capabilities.count() == 1 && capabilities[0].isEmpty()) {
                        interface->d->properties[QServiceInterfaceDescriptor::Capabilities]
                            = QStringList();
                    } else {
                        interface->d->properties[QServiceInterfaceDescriptor::Capabilities]
                        = capabilities;
                    }
                    break;
            }
            case(QServiceInterfaceDescriptor::Location):
                break; //should not be possible for this to be in the Property table
            case(QServiceInterfaceDescriptor::ServiceDescription):
                    break;//should not be possible for this to be in the InterfaceProperty table
            case(QServiceInterfaceDescriptor::InterfaceDescription):
                   interface->d->properties[QServiceInterfaceDescriptor::InterfaceDescription]
                       = query.value(EBindIndex1).toString();
        }
    }

    if (!isFound) {
        QString errorText("Database integrity corrupted, Properties for InterfaceID: %1 does not exist in the InterfaceProperty table for interface \"%2\"");
        m_lastError.setError(DBError::SqlError, errorText.arg(interfaceID).arg(interface->interfaceName()));
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::populateInterfaceProperties():-"
                    << "Problem:" << qPrintable(m_lastError.text());
#endif
        return false;
    }

    return true;
}

bool ServiceDatabase::populateServiceProperties(QServiceInterfaceDescriptor *interface, const QString &serviceID)
{
    QSqlQuery query(QSqlDatabase::database());
    QString statement("SELECT Key, Value FROM ServiceProperty WHERE ServiceID = ?");
    QList<QVariant> bindValues;
    bindValues.append(serviceID);
    if (!executeQuery(&query, statement, bindValues)) {
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::populateServiceProperties():-"
                    << qPrintable(m_lastError.text());
#endif
        return false;
    }

    bool isFound = false;
    int propertyKey;
    while (query.next()) {
        isFound = true;
        propertyKey = query.value(EBindIndex).toInt();
        switch(propertyKey) {
            case(SERVICE_DESCRIPTION_KEY):
                interface->d->properties[QServiceInterfaceDescriptor::ServiceDescription]
                    = query.value(EBindIndex1).toString();
                break;
            default:
                break;
        }
    }

    if (!isFound) {
        QString errorText("Database integrity corrupted, Service Properties for ServiceID: \"%1\" does not exist in the ServiceProperty table for service \"%2\"");
        m_lastError.setError(DBError::SqlError, errorText.arg(serviceID).arg(interface->serviceName()));
#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "ServiceDatabase::populateServiceProperties():-"
                    << "Problem:" << qPrintable(m_lastError.text());
#endif
        return false;
    }
    return true;
}

QT_END_NAMESPACE
