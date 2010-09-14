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

#include <QtDBus/QDBusPendingCall>
#include <QtDBus/QDBusPendingCallWatcher>
#include <QtCore/QTimer>
#include <QtCore/QDebug>

#include "maemo/pendingoperation.h"

namespace DBus
{
    PendingOperation::PendingOperation(QObject *parent)
        : QObject(parent),
        finish(false)
    {
    }

    PendingOperation::~PendingOperation()
    {
        if (!finish) {
            qDebug() << "warning: " << this <<
                "still pending when it was deleted - finished will "
                "never be emitted";
        }
    }

    void PendingOperation::emitFinished()
    {
        Q_ASSERT(finish);
        emit finished(this);
        deleteLater();
    }

    void PendingOperation::setFinished()
    {
        if (finish) {
            if (!errorname.isEmpty()) {
                qDebug() << "warning: " << this << "trying to finish with success, but already"
                    " failed with" << errorname << ":" << errormessage;
            } else {
                qDebug() << "warning: " << this << "trying to finish with success, but already"
                    " succeeded";
            }
            return;
        }

        finish = true;
        Q_ASSERT(isValid());
        QTimer::singleShot(0, this, SLOT(emitFinished()));
    }

    void PendingOperation::setFinishedWithError(const QString &name,
            const QString &message)
    {
        if (finish) {
            if (errorname.isEmpty()) {
                qDebug() << "warning: " << this << "trying to fail with" << name <<
                    "but already failed with" << errorName() << ":" <<
                    errorMessage();
            } else {
                qDebug() << "warning: " << this << "trying to fail with" << name <<
                    "but already succeeded";
            }
            return;
        }

        if (name.isEmpty()) {
            qDebug() << "warning: " << this << "should be given a non-empty error name";
            errorname = QLatin1String("org.freedesktop.Telepathy.Qt4.ErrorHandlingError");
        } else {
            errorname = name;
        }

        errormessage = message;
        finish = true;
        Q_ASSERT(isError());
        QTimer::singleShot(0, this, SLOT(emitFinished()));
    }

    void PendingOperation::setFinishedWithError(const QDBusError &error)
    {
        setFinishedWithError(error.name(), error.message());
    }

    bool PendingOperation::isValid() const
    {
        return (finish && errorname.isEmpty());
    }

    bool PendingOperation::isFinished() const
    {
        return finish;
    }

    bool PendingOperation::isError() const
    {
        return (finish && !errorname.isEmpty());
    }

    QString PendingOperation::errorName() const
    {
        return errorname;
    }

    QString PendingOperation::errorMessage() const
    {
        return errormessage;
    }
} // DBus
