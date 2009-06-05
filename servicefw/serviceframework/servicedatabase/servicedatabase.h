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

#ifndef QSERVICEDATABASE_H_
#define QSERVICEDATABASE_H_

#include "serviceinfo_p.h"
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
            ComponentAlreadyRegistered,
            IfaceImplAlreadyRegistered,
            ServiceNotFound,
            ImplNotFound,
            SqlError,
            InvalidSearchCriteria,
            CannotCloseDatabase,
            UnknownError
        };
        DBError();
        void setError(ErrorCode error, const QString &errorText = QString());
        void setSQLError(const QString &errorText) {
            m_error = SqlError;
            m_text = errorText;
        }
        void setImplNotFoundError(const QString &errorText) {
            m_error = ImplNotFound;
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
        void setDatabasePath(const QString &aDatabasePath);
        QString databasePath() const;

        bool registerService(ServiceMetaData &service);
        bool unregisterService(const QString &serviceName);

        QList<QServiceInterfaceDescriptor> getInterfaces(const QServiceFilter &filter, bool *ok = 0);
        QStringList getServiceNames(const QString &interfaceName, bool *ok =0);

        QServiceInterfaceDescriptor defaultServiceInterface(const QString &interfaceName, bool *ok = 0);
        bool setDefaultService(const QString &serviceName, const QString &interfaceName);
        bool setDefaultService(const QServiceInterfaceDescriptor &interface);

        DBError lastError() const { return m_lastError; }

Q_SIGNALS:
        void serviceAdded(const QString& serviceName);
        void serviceRemoved(const QString& serviceName);

    private:
        bool createTables();
        bool dropTables();
        bool checkTables();

        QString getInterfaceID(QSqlQuery *query, const QServiceInterfaceDescriptor &interface, bool *ok = 0);
        bool executeQuery(QSqlQuery *query, const QString &statement, const QList<QVariant> &bindValues = QList<QVariant>());
        bool insertInterfaceData(QSqlQuery *query, const QServiceInterfaceDescriptor &anInterface, const QString &serviceID);
        void databaseCommit(QSqlQuery *query, QSqlDatabase *database);
        void databaseRollback(QSqlQuery *query, QSqlDatabase *database);
        bool populateInterfaceProperties(QServiceInterfaceDescriptor *descriptor, const QString &interfaceID);
        bool populateServiceProperties(QServiceInterfaceDescriptor *descriptor, const QString &serviceID);

        bool checkConnection();

        QString iDatabasePath;
        bool iDatabaseOpen;
        DBError m_lastError;
};

QT_END_NAMESPACE
QT_END_HEADER

#endif /*QSERVICEDATABASE_H_*/
