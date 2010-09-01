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

#ifndef READINESSHELPER_H
#define READINESSHELPER_H

#include <QtCore/QMap>
#include <QtCore/QSet>
#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QStringList>

#include "maemo/features.h"

class QDBusError;

namespace DBus
{
    class DBusProxy;
    class PendingOperation;
    class PendingReady;

    class ReadinessHelper : public QObject
    {
        Q_OBJECT
        Q_DISABLE_COPY(ReadinessHelper)

    public:
        typedef void (*IntrospectFunc)(void *data);

        struct Introspectable {
        public:
            Introspectable();
            Introspectable(const QSet<uint> &makesSenseForStatuses,
                    const Features &dependsOnFeatures,
                    const QStringList &dependsOnInterfaces,
                    IntrospectFunc introspectFunc,
                    void *introspectFuncData,
                    bool critical = false);
            Introspectable(const Introspectable &other);
            ~Introspectable();

            Introspectable &operator=(const Introspectable &other);

        private:
            friend class ReadinessHelper;

            struct Private : public QSharedData
            {
                Private(const QSet<uint> &makesSenseForStatuses,
                        const Features &dependsOnFeatures,
                        const QStringList &dependsOnInterfaces,
                        IntrospectFunc introspectFunc,
                        void *introspectFuncData,
                        bool critical)
                    : makesSenseForStatuses(makesSenseForStatuses),
                    dependsOnFeatures(dependsOnFeatures),
                    dependsOnInterfaces(dependsOnInterfaces),
                    introspectFunc(introspectFunc),
                    introspectFuncData(introspectFuncData),
                    critical(critical) {}

                QSet<uint> makesSenseForStatuses;
                Features dependsOnFeatures;
                QStringList dependsOnInterfaces;
                IntrospectFunc introspectFunc;
                void *introspectFuncData;
                bool critical;
            };

            friend struct Private;
            QExplicitlySharedDataPointer<Private> mPriv;
        };
        typedef QMap<Feature, Introspectable> Introspectables;

        ReadinessHelper(QObject *object,
                uint currentStatus = 0,
                const Introspectables &introspectables = Introspectables(),
                QObject *parent = 0);
        ~ReadinessHelper();

        void addIntrospectables(const Introspectables &introspectables);

        uint currentStatus() const;
        void setCurrentStatus(uint currentStatus);

        QStringList interfaces() const;
        void setInterfaces(const QStringList &interfaces);

        Features requestedFeatures() const;
        Features actualFeatures() const;
        Features missingFeatures() const;

        bool isReady(const Feature &feature,
                QString *errorName = 0, QString *errorMessage = 0) const;
        bool isReady(const Features &features,
                QString *errorName = 0, QString *errorMessage = 0) const;
        PendingReady *becomeReady(const Features &requestedFeatures);

        void setIntrospectCompleted(const Feature &feature, bool success,
                const QString &errorName = QString(),
                const QString &errorMessage = QString());
        void setIntrospectCompleted(const Feature &feature, bool success,
                const QDBusError &error);

    Q_SIGNALS:
        void statusReady(uint status);

    private Q_SLOTS:
        void iterateIntrospection();

        void onProxyInvalidated(DBus::DBusProxy *proxy,
            const QString &errorName, const QString &errorMessage);
        void onOperationFinished(DBus::PendingOperation *op);
        void onOperationDestroyed(QObject *obj);

    private:
        struct Private
        {
            Private(ReadinessHelper *parent,
                    QObject *object,
                    uint currentStatus,
                    const Introspectables &introspectables);
            ~Private();

            void setCurrentStatus(uint newStatus);
            void setIntrospectCompleted(const Feature &feature, bool success,
                    const QString &errorName = QString(),
                    const QString &errorMessage = QString());
            void iterateIntrospection();

            void abortOperations(const QString &errorName, const QString &errorMessage);

            ReadinessHelper *parent;
            QObject *object;
            DBusProxy *proxy;
            uint currentStatus;
            QStringList interfaces;
            Introspectables introspectables;
            QSet<uint> supportedStatuses;
            Features supportedFeatures;
            Features satisfiedFeatures;
            Features requestedFeatures;
            Features missingFeatures;
            Features pendingFeatures;
            Features inFlightFeatures;
            QHash<Feature, QPair<QString, QString> > missingFeaturesErrors;
            QList<PendingReady *> pendingOperations;

            bool pendingStatusChange;
            uint pendingStatus;
        };

        friend struct Private;
        Private *mPriv;
    };
} // DBus

#endif//READINESSHELPER_H
