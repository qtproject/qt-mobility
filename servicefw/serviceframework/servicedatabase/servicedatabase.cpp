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

#include <QDir>
#include "servicedatabase.h"
#include <qserviceinterfacedescriptor.h>
#include <qserviceinterfacedescriptor_p.h>

//database name
#define RESOLVERDATABASE "services.db"

//database table names
#define SERVICE_TABLE "Service"
#define INTERFACE_TABLE "Interface"
#define DEFAULTS_TABLE "Defaults"

//default connection
#define RESOLVERDATABASE_DEFAULT_CONNECTION "qt_sql_default_connection"

//separator
#define RESOLVERDATABASE_PATH_SEPARATOR "//"

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
    setError(UnknownError);
}

void DBError::setError(ErrorCode error, const QString &text)
{
    m_error = error;
    switch (error) {
        case(DatabaseNotOpen):
            m_text = "Database not open";
            break;
        case(InvalidDatabaseConnection):
            m_text = "Invalid database connection";
            break;
        case(SqlError):
            m_text = text;
            break;
        case(NotFound):
            m_text = text;
            break;
        case(ComponentAlreadyRegistered):
            m_text = text;
            break;
        case(IfaceImplAlreadyRegistered):
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
int ServiceDatabase::open()
{
    int result(0);
    if (!iDatabaseOpen){
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

        if (database.isValid()){
            //Create or open resolver database
            if (!database.isOpen()) {
                iDatabaseOpen = database.open();
            } else {
                iDatabaseOpen = true;
            }
        } else {
            result = ServiceDatabase::SFW_ERROR_INVALID_DATABASE_CONNECTION;
        }

        if(iDatabaseOpen) {
            //Check database structure (tables) and recreate tables if neccessary
            //If one of the tables is missing remove all tables and recreate them
            //This operation is required in order to avoid data coruption
            if (!checkTables()) {
                if(dropTables() == 0) {
                    result = createTables();
                }
                else {
                    result = ServiceDatabase::SFW_ERROR_CANNOT_DROP_TABLES;
                }
            } 
        } else {
            result = ServiceDatabase::SFW_ERROR_CANNOT_OPEN_DATABASE;
        }

        if (result != ServiceDatabase::SFW_ERROR_DB_RECREATED && result != 0) {
            close();
        }
        iDatabaseOpen = (result == 0 || result == ServiceDatabase::SFW_ERROR_DB_RECREATED)? true: false;
    }
    return result;
}

bool ServiceDatabase::registerService(ServiceMetaData &service)
{
    if(!checkConnection()) {
        qWarning() << "ServiceDatabase::registerService():-"
                    << "Problem:" << qPrintable(m_lastError.text());
        return false;
    }

    QSqlDatabase database = QSqlDatabase::database();
    QSqlQuery query(database);

    //Begin Transaction
    if (!database.transaction()) {
        m_lastError.setSQLError(database.lastError().text());
        qWarning() << "ServiceDatabase::registerService():-"
                    << "Unable to begin transaction"
                    << "\nReason:" << m_lastError.text();
        return false;
    }

    QString statement("SELECT Name from Service WHERE FilePath=? COLLATE NOCASE");
    QList<QVariant> bindValues;
    bindValues.append(service.filePath());
    if (!executeQuery(&query, statement, bindValues)) {
        databaseRollback(&query, &database);
        qWarning() << "ServiceDatabase::registerService():-"
                    << qPrintable(m_lastError.text());
        return false;
    }

    if (query.next()) {
        QString alreadyRegisteredService = query.value(EBindIndex).toString();
        QString errorText;
        errorText = "Cannot register service \"%1\". Service component \"%2\" already "
                    "registered to service \"%3\".  \"%3\" must first be deregistered "
                    "for new registration to take place.";

        m_lastError.setError(DBError::ComponentAlreadyRegistered,
                errorText.arg(service.name())
                        .arg(service.filePath())
                        .arg(alreadyRegisteredService));
        databaseRollback(&query, &database);
        qWarning() << "ServiceDatabase::registerService():-"
                    << "Problem:" << qPrintable(m_lastError.text());
        return false;
    }

    statement = "INSERT INTO Service(Name,FilePath,Description) VALUES(?,?,?)";
    bindValues.clear();
    bindValues.append(service.name());
    bindValues.append(service.filePath());
    bindValues.append(service.description());

    if (!executeQuery(&query, statement, bindValues)) {
        databaseRollback(&query, &database);
        qWarning() << "ServiceDatabase::registerService():-"
                    << qPrintable(m_lastError.text());
        return false;
    }

    int serviceID = query.lastInsertId().toInt();
    Q_ASSERT(serviceID > -1);

    QList <QServiceInterfaceDescriptor> interfaces = service.getInterfaces();
    int interfaceID = -1;
    bool ok = false;
    foreach (const QServiceInterfaceDescriptor &interface, interfaces) {
        interfaceID = getInterfaceID(&query, interface, &ok);
        if (!ok) {
            databaseRollback(&query, &database);
            qWarning() << "ServiceDatabase::registerService():-"
                        << "Unable to confirm if implementation version"
                        << (QString::number(interface.majorVersion()) + "."
                           + QString::number(interface.minorVersion())).toAscii()
                        << "for interface" << interface.interfaceName()
                        << "is already registered for service "
                        << interface.serviceName()
                        << qPrintable(QString("\n") + m_lastError.text());
            return false;
        }

        if (interfaceID > -1) {
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
            qWarning() << "ServiceDatabase::registerService():-"
                        << "Problem:" << qPrintable(m_lastError.text());
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
            qWarning() << "ServiceDatabase::registerService()"
                        << "Problem: Unable to confirm if interface"
                        << interface.interfaceName()
                        << "already has a default implementation";
            return false;
        }

        if (defaultInterface.isValid())
            continue;

        //default does not already exist so create one
        interfaceID = getInterfaceID(&query, interface, &ok);
        if (!ok) {
            databaseRollback(&query, &database);
            qWarning() << "ServiceDatabase::registerService():-"
                        << "Unable to retrieve interfaceID for "
                           "interface" << interface.interfaceName()
                            <<  qPrintable(QString("\n") + m_lastError.text());
            return false;
        }

        statement = "INSERT INTO Defaults(InterfaceName, InterfaceID) VALUES(?,?)";
        bindValues.clear();
        bindValues.append(interface.interfaceName());
        bindValues.append(interfaceID);
        if (!executeQuery(&query, statement, bindValues)) {
            databaseRollback(&query, &database);
            qWarning() << "ServiceDatabase::registerService():-"
                        << qPrintable(m_lastError.text());
            return false;
        }
    }

    //End Transaction
    databaseCommit(&query, &database);
    return true;
}

int ServiceDatabase::getInterfaceID(QSqlQuery *query, const QServiceInterfaceDescriptor &interface, bool *ok)
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
        return -1;
    }
    if (!query->next()) {
        if (ok != NULL)
            *ok = true;
        return -1;
    }
    if (ok != NULL)
        *ok = true;
    int interfaceID = query->value(EBindIndex).toInt();
    return interfaceID;
}

/*!
    Helper method that saves interface related data in the Interface table
    @param aQuery database query
    @param anInterface interface metadata
    @param aServiceID service identifier 
    @return 0 if operation was successfull, error code otherwise
*/
bool ServiceDatabase::insertInterfaceData(QSqlQuery *query,const QServiceInterfaceDescriptor &interface, int serviceID)
{
    QString statement = "INSERT INTO Interface(ServiceID,Name,Description, VerMaj, VerMin, Capabilities) "
                        "VALUES(?,?,?,?,?,?)";
    QList<QVariant> bindValues;
    bindValues.append(serviceID);
    bindValues.append(interface.interfaceName());
    bindValues.append(interface.property(QServiceInterfaceDescriptor::InterfaceDescription));
    bindValues.append(interface.majorVersion());
    bindValues.append(interface.minorVersion());
    bindValues.append(interface.property(QServiceInterfaceDescriptor::Capabilities).toStringList().join(","));

    if (!executeQuery(query, statement, bindValues)) {
        qWarning() << "ServiceDatabase::insertInterfaceData():-"
                    << qPrintable(m_lastError.text());
        return false;
    }

    return true;
}

/*!
    Helper method that executes a sql statement
    @param aQuery database query
    @param aStatement sql statement
    @return true if operation was successfull, false otherwise
*/
bool ServiceDatabase::executeQuery(QSqlQuery &aQuery, const QString &aStatement)
{
    bool bSuccess(false);
    if (aQuery.prepare(aStatement)) {
        if (aQuery.exec()) {
            bSuccess = true;
        }
    }
    return bSuccess;
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
        qWarning() << "ServiceDatabase::getInterfaces():-"
                    << "Problem:" << qPrintable(m_lastError.text());
        return interfaces;
    }

    QSqlDatabase database = QSqlDatabase::database();
    QSqlQuery query(database);

    //Prepare search query, bind criteria values and execute search
    QString selectComponent = "SELECT Interface.Name, "
                                "Service.Name, Interface.VerMaj, "
                                "Interface.VerMin, Interface.Capabilities, "
                                "Service.FilePath, "
                                "Service.Description, "
                                "Interface.Description ";
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
        qWarning() << "ServiceDatabase::getInterfaces():-"
                    << "Problem:" << qPrintable(m_lastError.text());
        return interfaces;
    }

    QServiceInterfaceDescriptor interface;
    interface.d = new QServiceInterfaceDescriptorPrivate;
    QStringList capabilities;
    while(query.next()){
        interface.d->interfaceName =query.value(EBindIndex).toString();
        interface.d->serviceName = query.value(EBindIndex1).toString();
        interface.d->major = query.value(EBindIndex2).toInt();
        interface.d->minor = query.value(EBindIndex3).toInt();
        capabilities = query.value(EBindIndex4).toString().split(",");

        if (capabilities.count() == 1 && capabilities[0].isEmpty()) {
            interface.d->properties[QServiceInterfaceDescriptor::Capabilities]
                = QStringList();
        } else {
            interface.d->properties[QServiceInterfaceDescriptor::Capabilities] 
                = capabilities;
        }

        interface.d->properties[QServiceInterfaceDescriptor::FilePath]
            = query.value(EBindIndex5).toString();
        interface.d->properties[QServiceInterfaceDescriptor::ServiceDescription]
            = query.value(EBindIndex6).toString();
        interface.d->properties[QServiceInterfaceDescriptor::InterfaceDescription]
            = query.value(EBindIndex7).toString();
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
        qWarning() << "ServiceDatabase::getServiceNames():-"
                    << "Problem:" << qPrintable(m_lastError.text());
        return services;
    }
    QSqlDatabase database = QSqlDatabase::database();
    QSqlQuery query(database);
    QString selectComponent("SELECT DISTINCT Service.Name ");
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
        qWarning() << "ServiceDatabase::getServiceNames():-"
                    << qPrintable(m_lastError.text());
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

int ServiceDatabase::getService(const QServiceInterfaceDescriptor &interface, ServiceInfo &serviceInfo) const
{
    int result(0);
    serviceInfo.clear();

    if (!interface.isValid())
        return ServiceDatabase::SFW_ERROR_INVALID_SEARCH_CRITERIA;

    if(!iDatabaseOpen)
        return ServiceDatabase::SFW_ERROR_DATABASE_NOT_OPEN;

    QSqlDatabase database = QSqlDatabase::database();
    if (!database.isValid())
        return ServiceDatabase::SFW_ERROR_INVALID_DATABASE_CONNECTION;

    //Prepare search query, bind criteria values and execute search
    QSqlQuery query(database);
    QList<QVariant> bindValues;
    QString selectComponent = "SELECT DISTINCT Service.Name, Service.FilePath, Service.Description ";
    QString fromComponent = "FROM Interface, Service ";
    QString whereComponent = "WHERE Service.ID = Interface.ServiceID "
                            " AND Service.Name = ? COLLATE NOCASE "
                            " AND Interface.Name = ? COLLATE NOCASE "
                            " AND Interface.VerMaj = ? AND Interface.VerMin = ? ";
    bindValues.append(interface.serviceName());
    bindValues.append(interface.interfaceName());
    bindValues.append(interface.majorVersion());
    bindValues.append(interface.minorVersion());

    if(!query.prepare(selectComponent + fromComponent + whereComponent)) {
        result=query.lastError().number();
        query.finish();
        query.clear();
    } else {
        foreach(QVariant bindValue, bindValues)
            query.addBindValue(bindValue);
        if (!query.exec()) {
            result=query.lastError().number();
            query.finish();
            query.clear();
        } else {
            if (!query.exec()) {
                result = query.lastError().number();
                query.finish();
                query.clear();
            } else if (query.next()) {
                serviceInfo.setName(query.value(EBindIndex).toString());
                serviceInfo.setFilePath(query.value(EBindIndex1).toString());
                serviceInfo.setDescription(query.value(EBindIndex2).toString());
           } else {
               result = ServiceDatabase::SFW_ERROR_NO_SERVICE_FOUND;
           }
       }
    }
    query.finish();
    query.clear();

    return result;
    return 0;
}

QServiceInterfaceDescriptor ServiceDatabase::defaultServiceInterface(const QString &interfaceName, bool *ok)
{
    QServiceInterfaceDescriptor interface;
    if (!checkConnection())
    {
        if (ok != NULL)
            *ok = false;
        qWarning() << "ServiceDatabase::defaultServiceInterface():-"
                    << "Problem:" << qPrintable(m_lastError.text());
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
        qWarning() << "ServiceDatabase::defaultServiceInterface():-"
                    << qPrintable(m_lastError.text());
        return interface;
    }

    int interfaceID = -1;
    if (!query.next())
    {
        if ( ok != NULL )
            *ok = true;
        return interface;
    }
    else
        interfaceID = query.value(EBindIndex).toInt();
    Q_ASSERT(interfaceID > -1);

    statement = "SELECT Interface.Name, "
                        "Service.Name, Interface.VerMaj, "
                        "Interface.VerMin, Interface.Capabilities, "
                        "Service.FilePath, "
                        "Service.Description, "
                        "Interface.Description "
                    "FROM Service, Interface "
                    "WHERE Service.ID = Interface.ServiceID AND Interface.ID = ?";
    bindValues.clear();
    bindValues.append(interfaceID);
    if(!executeQuery(&query, statement, bindValues))
    {
        if (ok != NULL)
            *ok = false;
        qWarning() << "ServiceDatabase::defaultServiceInterface():-"
                    << qPrintable(m_lastError.text());
        return interface;
    }

    if(!query.next()) {
        QString errorText;
        errorText = "Database integrity has been corrupted, InterfaceID: %1, does not "
                    "exist in the Interface table for interface: %2";
        m_lastError.setSQLError(errorText.arg(interfaceID).arg(interfaceName));

        if (ok != NULL)
            *ok = false;

        qWarning() << "ServiceDatabase::defaultServiceInterface():-"
                    << "Problem:" << qPrintable(m_lastError.text());
        return interface;
    }

    interface.d = new QServiceInterfaceDescriptorPrivate;
    interface.d->interfaceName =query.value(EBindIndex).toString();
    interface.d->serviceName = query.value(EBindIndex1).toString();
    interface.d->major = query.value(EBindIndex2).toInt();
    interface.d->minor = query.value(EBindIndex3).toInt();

    QStringList capabilities;
    capabilities = query.value(EBindIndex4).toString().split(",");
    if (capabilities.count() == 1 && capabilities[0].isEmpty()) {
            interface.d->properties[QServiceInterfaceDescriptor::Capabilities]
                = QStringList();
    } else {
            interface.d->properties[QServiceInterfaceDescriptor::Capabilities]
                = capabilities;
    }

    interface.d->properties[QServiceInterfaceDescriptor::FilePath]
        = query.value(EBindIndex5).toString();
    interface.d->properties[QServiceInterfaceDescriptor::ServiceDescription]
        = query.value(EBindIndex6).toString();
    interface.d->properties[QServiceInterfaceDescriptor::InterfaceDescription]
        = query.value(EBindIndex7).toString();

    if (ok != NULL)
        *ok = true;
    return interface;
}

bool ServiceDatabase::setDefaultService(const QString &serviceName, const QString &interfaceName)
{
    if(!checkConnection()) {
        qWarning() << "ServiceDatabase::setDefaultService(QString,QString):-"
                    << "Problem:" << qPrintable(m_lastError.text());
        return false;
    }

    QSqlDatabase database = QSqlDatabase::database();
    QSqlQuery query(database);

    //Begin Transaction
    if (!database.transaction()) {
        m_lastError.setSQLError(database.lastError().text());
        qWarning() << "ServiceDatabase::setDefaultService(QString,Qtring):-"
                    << "Problem: Unable to begin transaction"
                    << "\nReason:" << qPrintable(m_lastError.text());
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
        database.rollback();
        qWarning() << "ServiceDatabase::setDefaultService(QString,QString):-"
                    << qPrintable(m_lastError.text());
        return false;
    }

    if (!query.next()) {
        QString errorText;
        errorText = "No implementation for interface \"%1\" found for service \"%2\"";
        m_lastError.setNotFoundError(errorText.arg(interfaceName).arg(serviceName));

        database.rollback();
        qWarning() << "ServiceDatabase::setDefaultService(QString,QString):-"
                    << "Problem: Unable to set default service"
                    << "\nReason:" << qPrintable(m_lastError.text());
        return false;
    }

    int interfaceID = -1;
    interfaceID = query.value(EBindIndex).toInt();
    Q_ASSERT(interfaceID > -1);

    statement = "UPDATE Defaults "
                "SET InterfaceID = ? "
                "WHERE InterfaceName = ? COLLATE NOCASE";

    bindValues.clear();
    bindValues.append(interfaceID);
    bindValues.append(interfaceName);
    if(!executeQuery(&query, statement, bindValues)) {
        database.rollback();
        qWarning() << "ServiceDatabase::setDefaultService(QString,QString):-"
                    << qPrintable(m_lastError.text());
        return false;
    }

    //End Transaction
    query.finish();
    query.clear();
    database.commit();

    return true;
}

bool ServiceDatabase::setDefaultService(const QServiceInterfaceDescriptor &interface)
{
    if(!checkConnection()) {
        qWarning() << "ServiceDatabase::setDefaultService(QServiceInterfaceDescriptor):-"
            << "Problem:" << qPrintable(m_lastError.text());
        return false;
    }

    QSqlDatabase database = QSqlDatabase::database();
    QSqlQuery query(database);

    //Begin Transaction
    if(!database.transaction()) {
        m_lastError.setSQLError(database.lastError().text());
        qWarning() << "ServiceDatabase::setDefaultService(QServiceInterfaceDescriptor):-"
            << "Problem: Unable to begin transaction"
            << "\nReason:" << qPrintable(m_lastError.text());
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
        database.rollback();
        qWarning() << "ServiceDatabase::setDefaultService(QServiceInterfaceDescriptor):-"
                    << qPrintable(m_lastError.text());
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

        database.rollback();
        qWarning() << "ServiceDatbase::setDefaultService(QServiceInterfaceDescriptor):-"
            << "Problem: Unable to set default service"
            << "\nReason:" << qPrintable(m_lastError.text());
        return false;
    }

    int interfaceID = -1;
    interfaceID = query.value(EBindIndex).toInt();
    Q_ASSERT(interfaceID > -1);

    statement = "UPDATE Defaults "
        "SET InterfaceID = ? "
        "WHERE InterfaceName = ? COLLATE NOCASE";
    bindValues.clear();
    bindValues.append(interfaceID);
    bindValues.append(interface.interfaceName());

    if (!executeQuery(&query, statement, bindValues)) {
        database.rollback();
        qWarning() << "ServiceDatabase::setDefaultService(QServiceInterfaceDescriptor):-"
                    << qPrintable(m_lastError.text());
        return false;
    }

    //End Transaction
    query.finish();
    query.clear();
    database.commit();

    return true;
}

int ServiceDatabase::unregisterService(const QString &serviceName)
{
    if (!iDatabaseOpen) 
        return ServiceDatabase::SFW_ERROR_DATABASE_NOT_OPEN;

    QSqlDatabase database = QSqlDatabase::database();
    if (!database.isValid())
        return ServiceDatabase::SFW_ERROR_INVALID_DATABASE_CONNECTION;

    //Determine the Service ID given the service name
    QSqlQuery query(database);
    if(!query.prepare("SELECT Service.ID from Service WHERE Service.Name = ? COLLATE NOCASE"))
        return query.lastError().number();
    query.addBindValue(serviceName);
    if(!query.exec())
        return query.lastError().number();
    if (!query.next())
        return  ServiceDatabase::SFW_ERROR_NO_SERVICE_FOUND;
    int serviceId = query.value(EBindIndex).toInt();

    //Delete all entries in the Service table matching the Service ID
    bool isActive = database.transaction();
    if(!query.prepare("DELETE FROM Service WHERE Service.ID = ?"))
        return query.lastError().number();
    query.addBindValue(serviceId);
    if (!query.exec()) {
        if (isActive)
            database.rollback();
        return query.lastError().number();
    }

    //Delete all entries in the Interface table matching the Service ID
    if (!query.prepare("DELETE FROM Interface WHERE Interface.ServiceID = ?")) {
        if (isActive)
            database.rollback();
        return query.lastError().number();
    }
    query.addBindValue(serviceId);
    if (!query.exec()) {
        if (isActive)
            database.rollback();
        return query.lastError().number();
    }

    if (isActive)
        database.commit();
    return 0;
}

/*!
    Closes the service resolver database
    Custom error codes: \n
    SFW_ERROR_CANNONT_CLOSE_DATABASE in case database can't be closed \n
    SFW_ERROR_INVALID_DATABASE_CONNECTION in case database connection is invalid \n
    @return 0 if operation was successfull, error code otherwise  
*/
int ServiceDatabase::close()
{
    int result(0);
    if(iDatabaseOpen) {
        QSqlDatabase database = QSqlDatabase::database();
        if (database.isValid()){
            if(database.isOpen()) {
                database.close();
                iDatabaseOpen = database.isOpen();
                result = !iDatabaseOpen? 0: ServiceDatabase::SFW_ERROR_CANNONT_CLOSE_DATABASE;
               
            }
        } else {
            result = ServiceDatabase::SFW_ERROR_INVALID_DATABASE_CONNECTION;
        }
    }
    return result;
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
    Creates the database tables: Service, Module, Interface
    @return 0 if operation is successful, error code otherwise
*/
int ServiceDatabase::createTables()
{
    int result(0);
    //Execute transaction for creating database tables
    bool isActive(false);
    QSqlDatabase database = QSqlDatabase::database();
    isActive = database.transaction();

    QSqlQuery query(database);
    if(!executeQuery(query,"CREATE TABLE Service("
                "ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,"
                "Name TEXT NOT NULL, "
                "FilePath TEXT NOT NULL, "
                "Description TEXT)")) {
        result = query.lastError().number();
        query.finish();
        query.clear();
        if (isActive) {
            database.rollback();
        }
        return result;
    }

    if(!executeQuery(query,"CREATE TABLE Interface("
                "ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,"
                "ServiceID INTEGER NOT NULL, "
                "Name TEXT NOT NULL, "
                "Description TEXT, "
                "VerMaj INTEGER NOT NULL, "
                "VerMin INTEGER NOT NULL, "
                "Capabilities TEXT)")) {
        result = query.lastError().number();
        query.finish();
        query.clear();
        if (isActive) {
            database.rollback();
        }
        return result;
    }

    if(!executeQuery(query,"CREATE TABLE Defaults("
                "InterfaceName TEXT PRIMARY KEY UNIQUE,"
                "InterfaceID TEXT)")) {
        result = query.lastError().number();
        query.finish();
        query.clear();
        if (isActive) {
            database.rollback();
        }
        return result;
    }

    query.finish();
    query.clear();
    if (isActive) {
        database.commit();
    }

    return ServiceDatabase::SFW_ERROR_DB_RECREATED;
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
        && tables.contains(INTERFACE_TABLE)){
            bTables = true;
    }

    return bTables;
}

/*!
    Removes service related tables from database
    @return 0 if was sucessful, error code otherwise
*/
int ServiceDatabase::dropTables()
{
    int result(0);
    //Execute transaction for deleting the database tables
    bool isActive(false); 
    QSqlDatabase database = QSqlDatabase::database();
    QStringList tables = database.tables();
    if (tables.size() > 0) {
        isActive = database.transaction();
        QSqlQuery query(database);

        if ((tables.contains(SERVICE_TABLE)) 
            && (!executeQuery(query, "DROP TABLE Service"))) {
                result = query.lastError().number();
                query.finish();
                query.clear();
                if (isActive) {
                    database.rollback();
                }
        }
        if ((result == 0) && (tables.contains(INTERFACE_TABLE)) 
            && (!executeQuery(query,"DROP TABLE Interface"))) {
                result = query.lastError().number();
                query.finish();
                query.clear();
                if (isActive) {
                    database.rollback();
                }
        }

        if ((result == 0) && (tables.contains(DEFAULTS_TABLE)) 
            && (!executeQuery(query,"DROP TABLE Interface"))) {
                result = query.lastError().number();
                query.finish();
                query.clear();
                if (isActive) {
                    database.rollback();
                }
        }

        if (result == 0) {
            query.finish();
            query.clear(); 
            if (isActive) {
                database.commit();
            }
        }
    }
    return result;
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


QT_END_NAMESPACE
