/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifdef SYMBIAN_BACKEND_USE_SQLITE

#ifndef TRANSFORMONLINEACCOUNT_H
#define TRANSFORMONLINEACCOUNT_H

#include "cnttransformcontactdata.h"

QTM_USE_NAMESPACE

class CntTransformOnlineAccount : public CntTransformContactData
{
    enum TPresnceMap
            {
            EPresenceOffline,
            EPresenceAvailable,
            EPresenceHidden,
            EPresenceBusy,
            EPresenceAway,
            EPresenceExtendedAway,
            EPresenceUnknown
            };
public:
    CntTransformOnlineAccount();
    
protected:
	QList<CContactItemField *> transformDetailL(const QContactDetail &detail);
	QContactDetail *transformItemField(const CContactItemField& field, const QContact &contact);
	bool supportsField(TUint32 fieldType) const;
	bool supportsDetail(QString detailName) const;
	QList<TUid> supportedSortingFieldTypes(QString detailFieldName) const;
    bool supportsSubType(const QString& subType) const;
    quint32 getIdForField(const QString& fieldName) const;
    void detailDefinitions(QMap<QString, QContactDetailDefinition> &definitions, const QString& contactType) const;
    void reset();

private:
    quint32 encodePresence(QString aPresence);
    QString decodePresence(quint32 aPresence);
    
private:
    quint32 m_detailCounter;
};

#endif // TRANSFORMONLINEACCOUNT_H

#endif // SYMBIAN_BACKEND_USE_SQLITE
