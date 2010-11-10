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

#ifndef QNEARFIELDMANAGER_H
#define QNEARFIELDMANAGER_H

#include <qmobilityglobal.h>

#include <QtCore/QObject>

#include <qnearfieldtarget.h>
#include <qndefrecord.h>
#include <qndeffilter.h>

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QNearFieldManagerPrivate;
class Q_CONNECTIVITY_EXPORT QNearFieldManager : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QNearFieldManager)

public:
    explicit QNearFieldManager(QObject *parent = 0);
    explicit QNearFieldManager(QNearFieldManagerPrivate *backend, QObject *parent = 0);
    ~QNearFieldManager();

    void startTargetDetection(const QList<QNearFieldTarget::Type> &targetTypes);
    void startTargetDetection(QNearFieldTarget::Type targetType = QNearFieldTarget::AnyTarget);
    void stopTargetDetection();

    template<typename T>
    int registerTargetDetectedHandler(QNearFieldTarget::Type targetType,
                                      QObject *object, const char *method);
    int registerTargetDetectedHandler(QNearFieldTarget::Type targetType,
                                      QObject *object, const char *method);
    int registerTargetDetectedHandler(QNearFieldTarget::Type targetType,
                                      QNdefRecord::TypeNameFormat typeNameFormat,
                                      const QByteArray &type,
                                      QObject *object, const char *method);
    int registerTargetDetectedHandler(QNearFieldTarget::Type targetType,
                                      const QNdefFilter &filter,
                                      QObject *object, const char *method);

    bool unregisterTargetDetectedHandler(int handlerId);

signals:
    void targetDetected(QNearFieldTarget *target);
    void targetLost(QNearFieldTarget *target);
    void transactionDetected(const QByteArray &applicationIdentifier);

private:
    QNearFieldManagerPrivate *d_ptr;
};

template<typename T>
int QNearFieldManager::registerTargetDetectedHandler(QNearFieldTarget::Type targetType,
                                                     QObject *object, const char *method)
{
    T record;

    return registerTargetDetectedHandler(targetType, record.userTypeNameFormat(), record.type(),
                                         object, method);
}

QTM_END_NAMESPACE

QT_END_HEADER

#endif // QNEARFIELDMANAGER_H
