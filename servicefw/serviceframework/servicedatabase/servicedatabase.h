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
** contained in Technology Preview License Agreement accompanying
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

#ifndef QSERVICEDATABASE_H_
#define QSERVICEDATABASE_H_

#include <servicemetadata_p.h>
#include <qservicefilter.h>
#include <QtSql>
#include <QList>

#include "qserviceglobal.h"

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

class QServiceInterfaceDescriptor;
class DBError
{
    public:
        enum ErrorCode {
            NoError,
            DatabaseNotOpen = -2000,
            InvalidDatabaseConnection,
            LocationAlreadyRegistered,
            IfaceImplAlreadyRegistered,
            NotFound,
            SqlError,
            InvalidSearchCriteria,
            IfaceIDNotExternal,
            CannotCreateDbDir,
            CannotOpenSystemDb,
            CannotOpenUserDb,
            CannotCreateSystemDbDir,
            CannotCreateUserDbDir,
            ExternalIfaceIDFound,
            InvalidDescriptorScope,
            InvalidDatabaseFile,
            NoWritePermissions,
            UnknownError
        };
        DBError();
        void setError(ErrorCode error, const QString &errorText = QString());
        void setSQLError(const QString &errorText) {
            m_error = SqlError;
            m_text = errorText;
        }
        void setNotFoundError(const QString &errorText) {
            m_error = NotFound;
            m_text = errorText;
        }
        QString text() const { return m_text; }
        ErrorCode errorCode() const { return m_error; }
    private:
        QString m_text;
        ErrorCode m_error;
};

class Q_SFW_EXPORT ServiceDatabase : public QObject
{
    Q_OBJECT

    public:
        ServiceDatabase(void);

        virtual ~ServiceDatabase();

        bool open();
        bool close();

        bool isOpen() const;
        void setDatabasePath(const QString &databasePath);
        QString databasePath() const;

        bool registerService(ServiceMetaData &service);
        bool unregisterService(const QString &serviceName);

        QList<QServiceInterfaceDescriptor> getInterfaces(const QServiceFilter &filter);
        QServiceInterfaceDescriptor getInterface(const QString &interfaceID);
        QString getInterfaceID(const QServiceInterfaceDescriptor &interface);
        QStringList getServiceNames(const QString &interfaceName);

        QServiceInterfaceDescriptor defaultServiceInterface(const QString &interfaceName,
                                    QString *interfaceID = 0, bool inTransaction = false);
        bool setDefaultService(const QServiceInterfaceDescriptor &interface,
                                const QString &externalInterfaceID = QString());
        QStringList externalDefaultInterfaceIDs();
        bool removeExternalDefaultServiceInterface(const QString &interfaceID);

        DBError lastError() const { return m_lastError; }

Q_SIGNALS:
        void serviceAdded(const QString& serviceName);
        void serviceRemoved(const QString& serviceName);

    private:
        enum TransactionType{Read, Write};

        bool createTables();
        bool dropTables();
        bool checkTables();

        bool checkConnection();

        bool executeQuery(QSqlQuery *query, const QString &statement, const QList<QVariant> &bindValues = QList<QVariant>());
        QString getInterfaceID(QSqlQuery *query, const QServiceInterfaceDescriptor &interface);
        bool insertInterfaceData(QSqlQuery *query, const QServiceInterfaceDescriptor &anInterface, const QString &serviceID);

        bool beginTransaction(QSqlQuery *query, TransactionType);
        bool commitTransaction(QSqlQuery *query);
        bool rollbackTransaction(QSqlQuery *query);

        bool populateInterfaceProperties(QServiceInterfaceDescriptor *descriptor, const QString &interfaceID);
        bool populateServiceProperties(QServiceInterfaceDescriptor *descriptor, const QString &serviceID);

        QString m_databasePath;
        QString m_connectionName;
        bool m_isDatabaseOpen;
        bool m_inTransaction;
        DBError m_lastError;
};

QT_END_NAMESPACE
QT_END_HEADER

#endif /*QSERVICEDATABASE_H_*/
