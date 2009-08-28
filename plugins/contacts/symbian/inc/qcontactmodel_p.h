/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Symbian Foundation License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#ifndef QCONTACTMODELPRIVATE_H
#define QCONTACTMODELPRIVATE_H

#include <QSharedData>
#include <QMap>

#include "qcontactmanager.h"

class QContactModelData : public QSharedData
{
public:
    QContactModelData() { }
    ~QContactModelData() { }

public:
    QContactManager* m_contactManager;
    QList<QList<QVariant> > m_data;
    QList<QString> m_columns;
	QMap<QUniqueId, QContactGroup> m_groups;
};

#endif // QCONTACTMODELPRIVATE_H

