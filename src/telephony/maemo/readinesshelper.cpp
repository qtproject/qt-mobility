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

#include <QtDBus/QDBusError>
#include <QtCore/QSharedData>
#include <QtCore/QTimer>
#include <QtCore/QDebug>

#include "maemo/readinesshelper.h"
#include "maemo/dbusproxy.h"
#include "maemo/constants.h"
#include "maemo/pendingready.h"

namespace DBus
{
    ReadinessHelper::Introspectable::Introspectable()
        : mPriv(new Private(QSet<uint>(), Features(), QStringList(), 0, 0, false))
    {
    }

    ReadinessHelper::Introspectable::Introspectable(const QSet<uint> &makesSenseForStatuses,
            const Features &dependsOnFeatures, const QStringList &dependsOnInterfaces,
            IntrospectFunc introspectFunc, void *introspectFuncData, bool critical)
        : mPriv(new Private(makesSenseForStatuses, dependsOnFeatures, dependsOnInterfaces,
                    introspectFunc, introspectFuncData, critical))
    {
    }

    ReadinessHelper::Introspectable::Introspectable(const Introspectable &other)
        : mPriv(other.mPriv)
    {
    }

    ReadinessHelper::Introspectable::~Introspectable()
    {
    }

    ReadinessHelper::Introspectable &ReadinessHelper::Introspectable::operator=(
            const Introspectable &other)
    {
        mPriv = other.mPriv;
        return *this;
    }

    ReadinessHelper::Private::Private(
            ReadinessHelper *parent,
            QObject *object,
            uint currentStatus,
            const Introspectables &introspectables)
        : parent(parent),
          object(object),
          proxy(qobject_cast<DBusProxy*>(object)),
          currentStatus(currentStatus),
          introspectables(introspectables),
          pendingStatusChange(false),
          pendingStatus(-1)
    {
        Introspectables::const_iterator i = introspectables.constBegin();
        Introspectables::const_iterator end = introspectables.constEnd();
        while (i != end) {
            Feature feature = i.key();
            Introspectable introspectable = i.value();
            Q_ASSERT(introspectable.mPriv->introspectFunc != 0);
            supportedStatuses += introspectable.mPriv->makesSenseForStatuses;
            supportedFeatures += feature;
            ++i;
        }

        if (proxy) {
            parent->connect(proxy,
                    SIGNAL(invalidated(DBus::DBusProxy *, const QString &, const QString &)),
                    SLOT(onProxyInvalidated(DBus::DBusProxy *, const QString &, const QString &)));
        }
    }

    ReadinessHelper::Private::~Private()
    {
        abortOperations(QLatin1String(TELEPATHY_ERROR_CANCELLED), QLatin1String("Destroyed"));
    }

    void ReadinessHelper::Private::setCurrentStatus(uint newStatus)
    {
        if (inFlightFeatures.isEmpty()) {
            currentStatus = newStatus;
            satisfiedFeatures.clear();
            missingFeatures.clear();
            pendingFeatures = requestedFeatures;

            if (supportedStatuses.contains(currentStatus)) {
                QTimer::singleShot(0, parent, SLOT(iterateIntrospection()));
            } else {
                emit parent->statusReady(currentStatus);
            }
        } else {
            pendingStatusChange = true;
            pendingStatus = newStatus;
        }
    }

    void ReadinessHelper::Private::setIntrospectCompleted(const Feature &feature,
            bool success, const QString &errorName, const QString &errorMessage)
    {
        if (pendingStatusChange) {
            inFlightFeatures.remove(feature);
            if (!inFlightFeatures.isEmpty()) {
                return;
            }
            pendingStatusChange = false;
            setCurrentStatus(pendingStatus);
            return;
        }

        Q_ASSERT(pendingFeatures.contains(feature));
        Q_ASSERT(inFlightFeatures.contains(feature));

        if (success) {
            satisfiedFeatures.insert(feature);
        }
        else {
            missingFeatures.insert(feature);
            missingFeaturesErrors.insert(feature,
                    QPair<QString, QString>(errorName, errorMessage));
            if (errorName.isEmpty()) {
            }
        }

        pendingFeatures.remove(feature);
        inFlightFeatures.remove(feature);

        QTimer::singleShot(0, parent, SLOT(iterateIntrospection()));
    }

    void ReadinessHelper::Private::iterateIntrospection()
    {
        if (proxy && !proxy->isValid()) {
            return;
        }

        if (!supportedStatuses.contains(currentStatus)) {
            return;
        }

        Introspectables::const_iterator i = introspectables.constBegin();
        Introspectables::const_iterator end = introspectables.constEnd();
        while (i != end) {
            Feature feature = i.key();
            Introspectable introspectable = i.value();
            Features dependsOnFeatures = introspectable.mPriv->dependsOnFeatures;
            if (!dependsOnFeatures.intersect(missingFeatures).isEmpty()) {
                missingFeatures.insert(feature);
                missingFeaturesErrors.insert(feature,
                        QPair<QString, QString>(QLatin1String(TELEPATHY_ERROR_NOT_AVAILABLE),
                            QLatin1String("Feature depend on other features that are not available")));
            }
            ++i;
        }

        QString errorName;
        QString errorMessage;
        foreach (PendingReady *operation, pendingOperations) {
            if ((operation->requestedFeatures() - (satisfiedFeatures + missingFeatures)).isEmpty()) {
                if (parent->isReady(operation->requestedFeatures(), &errorName, &errorMessage)) {
                    operation->setFinished();
                } else {
                    operation->setFinishedWithError(errorName, errorMessage);
                }
            }
        }

        if ((requestedFeatures - (satisfiedFeatures + missingFeatures)).isEmpty()) {
            emit parent->statusReady(currentStatus);
            return;
        }

        pendingFeatures -= (satisfiedFeatures + missingFeatures);

        Features readyToIntrospect;
        foreach (const Feature &feature, pendingFeatures) {
            if ((introspectables[feature].mPriv->dependsOnFeatures - satisfiedFeatures).isEmpty()) {
                readyToIntrospect.insert(feature);
            }
        }

        foreach (const Feature &feature, readyToIntrospect) {
            if (inFlightFeatures.contains(feature)) {
                continue;
            }

            inFlightFeatures.insert(feature);

            Introspectable introspectable = introspectables[feature];

            if (!introspectable.mPriv->makesSenseForStatuses.contains(currentStatus)) {
                setIntrospectCompleted(feature, true);
                return;
            }

            foreach (const QString &interface, introspectable.mPriv->dependsOnInterfaces) {
                if (!interfaces.contains(interface)) {
                    setIntrospectCompleted(feature, false,
                            QLatin1String(TELEPATHY_ERROR_NOT_AVAILABLE),
                            QLatin1String("Feature depend on interfaces that are not available"));
                    return;
                }
            }
            (*(introspectable.mPriv->introspectFunc))(introspectable.mPriv->introspectFuncData);
        }
    }

    void ReadinessHelper::Private::abortOperations(const QString &errorName,
            const QString &errorMessage)
    {
        foreach (PendingReady *operation, pendingOperations) {
            parent->disconnect(operation,
                    SIGNAL(finished(DBus::PendingOperation*)),
                    parent,
                    SLOT(onOperationFinished(DBus::PendingOperation*)));
            parent->disconnect(operation,
                    SIGNAL(destroyed(QObject*)),
                    parent,
                    SLOT(onOperationDestroyed(QObject*)));
            operation->setFinishedWithError(errorName, errorMessage);
        }
        pendingOperations.clear();
    }

    ReadinessHelper::ReadinessHelper(QObject *object,
            uint currentStatus,
            const Introspectables &introspectables,
            QObject *parent)
        : QObject(parent),
          mPriv(new Private(this, object, currentStatus, introspectables))
    {
    }

    ReadinessHelper::~ReadinessHelper()
    {
        delete mPriv;
    }

    void ReadinessHelper::addIntrospectables(const Introspectables &introspectables)
    {
        Introspectables::const_iterator i = introspectables.constBegin();
        Introspectables::const_iterator end = introspectables.constEnd();
        while (i != end) {
            Feature feature = i.key();
            if (mPriv->introspectables.contains(feature)) {
            } else {
                Introspectable introspectable = i.value();
                mPriv->introspectables.insert(feature, introspectable);
                mPriv->supportedStatuses += introspectable.mPriv->makesSenseForStatuses;
                mPriv->supportedFeatures += feature;
            }

            ++i;
        }
    }

    uint ReadinessHelper::currentStatus() const
    {
        return mPriv->currentStatus;
    }

    void ReadinessHelper::setCurrentStatus(uint currentStatus)
    {
        mPriv->setCurrentStatus(currentStatus);
    }

    QStringList ReadinessHelper::interfaces() const
    {
        return mPriv->interfaces;
    }

    void ReadinessHelper::setInterfaces(const QStringList &interfaces)
    {
        mPriv->interfaces = interfaces;
    }

    Features ReadinessHelper::requestedFeatures() const
    {
        return mPriv->requestedFeatures;
    }

    Features ReadinessHelper::actualFeatures() const
    {
        return mPriv->satisfiedFeatures;
    }

    Features ReadinessHelper::missingFeatures() const
    {
        return mPriv->missingFeatures;
    }

    bool ReadinessHelper::isReady(const Feature &feature,
            QString *errorName, QString *errorMessage) const
    {
        if (mPriv->proxy && !mPriv->proxy->isValid()) {
            if (errorName) {
                *errorName = mPriv->proxy->invalidationReason();
            }
            if (errorMessage) {
                *errorMessage = mPriv->proxy->invalidationMessage();
            }
            return false;
        }

        if (!mPriv->supportedFeatures.contains(feature)) {
            if (errorName) {
                *errorName = QLatin1String(TELEPATHY_ERROR_INVALID_ARGUMENT);
            }
            if (errorMessage) {
                *errorMessage = QLatin1String("Unsupported feature");
            }
            return false;
        }

        bool ret = true;

        if (feature.isCritical()) {
            if (!mPriv->satisfiedFeatures.contains(feature)) {
                ret = false;
            }
        } else {
            if (!mPriv->satisfiedFeatures.contains(feature) &&
                !mPriv->missingFeatures.contains(feature)) {
                ret = false;
            }
        }

        if (!ret) {
            QPair<QString, QString> error = mPriv->missingFeaturesErrors[feature];
            if (errorName) {
                *errorName = error.first;
            }
            if (errorMessage) {
                *errorMessage = error.second;
            }
        }

        return ret;
    }

    bool ReadinessHelper::isReady(const Features &features, QString *errorName, QString *errorMessage) const
    {
        if (mPriv->proxy && !mPriv->proxy->isValid()) {
            if (errorName) {
                *errorName = mPriv->proxy->invalidationReason();
            }
            if (errorMessage) {
                *errorMessage = mPriv->proxy->invalidationMessage();
            }
            return false;
        }

        Q_ASSERT(!features.isEmpty());

        foreach (const Feature &feature, features) {
            if (!isReady(feature, errorName, errorMessage)) {
                return false;
            }
        }
        return true;
    }

    PendingReady *ReadinessHelper::becomeReady(const Features &requestedFeatures)
    {
        Q_ASSERT(!requestedFeatures.isEmpty());

        Features supportedFeatures = mPriv->supportedFeatures;
        if (supportedFeatures.intersect(requestedFeatures) != requestedFeatures) {
            PendingReady *operation =
                new PendingReady(requestedFeatures, mPriv->object, this);
            operation->setFinishedWithError(
                    QLatin1String(TELEPATHY_ERROR_INVALID_ARGUMENT),
                    QLatin1String("Requested features contains unsupported feature"));
            return operation;
        }

        if (mPriv->proxy && !mPriv->proxy->isValid()) {
            PendingReady *operation =
                new PendingReady(requestedFeatures, mPriv->object, this);
            operation->setFinishedWithError(mPriv->proxy->invalidationReason(),
                    mPriv->proxy->invalidationMessage());
            return operation;
        }

        PendingReady *operation;
        foreach (operation, mPriv->pendingOperations) {
            if (operation->requestedFeatures() == requestedFeatures) {
                return operation;
            }
        }

        mPriv->requestedFeatures += requestedFeatures;
        // it will be updated on iterateIntrospection
        mPriv->pendingFeatures += requestedFeatures;

        operation = new PendingReady(requestedFeatures, mPriv->object, this);
        connect(operation,
                SIGNAL(finished(DBus::PendingOperation*)),
                SLOT(onOperationFinished(DBus::PendingOperation*)));
        connect(operation,
                SIGNAL(destroyed(QObject*)),
                SLOT(onOperationDestroyed(QObject*)));
        mPriv->pendingOperations.append(operation);

        QTimer::singleShot(0, this, SLOT(iterateIntrospection()));

        return operation;
    }

    void ReadinessHelper::setIntrospectCompleted(const Feature &feature, bool success,
            const QString &errorName, const QString &errorMessage)
    {
        if (mPriv->proxy && !mPriv->proxy->isValid()) {
            // proxy became invalid, ignore here
            return;
        }
        mPriv->setIntrospectCompleted(feature, success, errorName, errorMessage);
    }

    void ReadinessHelper::setIntrospectCompleted(const Feature &feature, bool success,
            const QDBusError &error)
    {
        setIntrospectCompleted(feature, success, error.name(), error.message());
    }

    void ReadinessHelper::iterateIntrospection()
    {
        mPriv->iterateIntrospection();
    }

    void ReadinessHelper::onProxyInvalidated(DBusProxy *proxy,
            const QString &errorName, const QString &errorMessage)
    {
        // clear satisfied and missing features as we have public methods to get them
        mPriv->satisfiedFeatures.clear();
        mPriv->missingFeatures.clear();

        mPriv->abortOperations(errorName, errorMessage);
    }

    void ReadinessHelper::onOperationFinished(PendingOperation *op)
    {
        disconnect(op,
                   SIGNAL(destroyed(QObject*)),
                   this,
                   SLOT(onOperationDestroyed(QObject*)));
        mPriv->pendingOperations.removeOne(qobject_cast<PendingReady*>(op));
    }

    void ReadinessHelper::onOperationDestroyed(QObject *obj)
    {
        mPriv->pendingOperations.removeOne(qobject_cast<PendingReady*>(obj));
    }
} // DBus
