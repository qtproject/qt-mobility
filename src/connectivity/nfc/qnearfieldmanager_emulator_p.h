/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QNEARFIELDMANAGER_EMULATOR_H
#define QNEARFIELDMANAGER_EMULATOR_H

#include "qnearfieldmanager_p.h"
#include "qnearfieldtarget.h"
#include "qndeffilter.h"

#include <QtCore/QObject>
#include <QtCore/QMetaMethod>

QTM_USE_NAMESPACE

class TagBase;
class QNearFieldManagerPrivateImpl : public QtMobility::QNearFieldManagerPrivate
{
    Q_OBJECT

public:
    QNearFieldManagerPrivateImpl();
    ~QNearFieldManagerPrivateImpl();

    void reset();

    bool startTargetDetection(const QList<QNearFieldTarget::Type> &targetTypes);
    void stopTargetDetection();

    int registerTargetDetectedHandler(QObject *object, const QMetaMethod &method);
    int registerTargetDetectedHandler(const QNdefFilter &filter,
                                      QObject *object, const QMetaMethod &method);

    bool unregisterTargetDetectedHandler(int id);

private slots:
    void tagActivated(TagBase *tag);
    void tagDeactivated(TagBase *tag);

private:
    struct Callback {
        QNdefFilter filter;

        QObject *object;
        QMetaMethod method;
    };

    int getFreeId();
    void ndefReceived(const QNdefMessage &message, QNearFieldTarget *target);

    QList<Callback> m_registeredHandlers;
    QList<int> m_freeIds;
    QMap<TagBase *, QNearFieldTarget *> m_targets;
    QList<QNearFieldTarget::Type> m_detectTargetTypes;
};

#endif // QNEARFIELDMANAGER_EMULATOR_H
