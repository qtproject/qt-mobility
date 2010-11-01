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

#ifndef QNEARFIELDMANAGER_SYMBIAN_P_H_
#define QNEARFIELDMANAGER_SYMBIAN_P_H_


#include "qnearfieldmanager_p.h"
#include "qnearfieldtarget.h"
#include "qndeffilter.h"

#include <QtCore/QObject>
#include <QtCore/QMetaMethod>
#include <QPointer>

QTM_USE_NAMESPACE

class CNearFieldManager;

class QNearFieldManagerPrivateImpl : public QtMobility::QNearFieldManagerPrivate
{
    Q_OBJECT

public:
    QNearFieldManagerPrivateImpl();
    ~QNearFieldManagerPrivateImpl();

    int registerTargetDetectedHandler(QNearFieldTarget::Type targetType,
                                      QObject *object, const QMetaMethod &method);
    int registerTargetDetectedHandler(QNearFieldTarget::Type targetType,
                                      const QNdefFilter &filter,
                                      QObject *object, const QMetaMethod &method);

    bool unregisterTargetDetectedHandler(int id);

private://call back function by symbian backend implementation
	void targetFound(QNearFieldTarget* target);
	void targetDisconnected();
	
private slots:
	void invokeTargetDetectedHandler();
	
private:
    struct Callback {
        QNearFieldTarget::Type targetType;
        QNdefFilter filter;

        QObject *object;
        QMetaMethod method;
    };

    int getFreeId();

    QList<Callback> m_registeredHandlers;
    QList<int> m_freeIds;
    
    CNearFieldManager* m_symbianbackend;
    friend class CNearFieldManager;
    
    QPointer<QNearFieldTarget> m_target;
};

#endif /* QNEARFIELDMANAGER_SYMBIAN_P_H_ */
