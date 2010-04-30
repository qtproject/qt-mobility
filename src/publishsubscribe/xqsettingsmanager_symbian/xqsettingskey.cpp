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

#include "xqsettingskey.h"

/****************************************************
 *
 * XQSettingsKey
 *
 ****************************************************/

/* 
    \class XQSettingsKey
    \internal

    \brief The XQSettingsKey defines the target for XQSettingsManager operations.
*/

/* 
    \enum XQSettingsKey::Target

    This enum defines the possible item target for an XQSettingsKey object.
*/
/*  \var XQSettingsKey::TargetCentralRepository XQSettingsKey::TargetCentralRepository
    The settings key targets to Symbian OS's Central Repository.
*/
/*  \var XQSettingsKey::TargetPublishAndSubscribe XQSettingsKey::TargetPublishAndSubscribe
    The settings key targets to Symbian OS's Publish and Subscribe property.
*/

/* 
    Constructs an XQSettingsKey object.
    \param target This is either XQSettingsKey::TargetCentralRepository or 
    XQSettingsKey::TargetPublishAndSubscribe.
    \param uid In case of Central Repository a CenRep Uid. In case of
    Publish and Subscribe an RProperty category Uid.
    \param key Settings Key
*/
XQSettingsKey::XQSettingsKey(
    XQSettingsKey::Target target, long int uid, unsigned long int key) :
    m_target(target), m_uid(uid), m_key(key)
{   
}

/*!
    Destructs an XQSettingsKey object.
*/
XQSettingsKey::~XQSettingsKey()
{
}

/* 
    Gets a target of the item.

    \return target Either XQSettingsKey::TargetCentralRepository or 
    XQSettingsKey::TargetPublishAndSubscribe.
*/
XQSettingsKey::Target XQSettingsKey::target() const
{
    return m_target;
}

/* 
    Gets a uid of the item

    \return uid. In case of Central Repository a CenRep Uid. In case of
    Publish and Subscribe an RProperty category Uid.
*/
long int XQSettingsKey::uid() const
{    
    return m_uid;
}

/* 
    Gets a key of the item

    \return Key
*/
unsigned long int XQSettingsKey::key() const
{
    return m_key;
}

/* 
    \class XQCentralRepositorySettingsKey
    \internal

    \brief The XQCentralRepositorySettingsKey defines the Central Repository
    target for XQSettingsManager operations.
*/

/* 
    Constructs an XQCentralRepositorySettingsKey object.
    \param repositoryUid CenRep Uid
    \param key Settings Key
*/
XQCentralRepositorySettingsKey::XQCentralRepositorySettingsKey(long int repositoryUid, unsigned long int key) :
    XQSettingsKey(XQSettingsKey::TargetCentralRepository, repositoryUid, key)
{
}

/* 
    Destructs an XQCentralRepositorySettingsKey object.
*/
XQCentralRepositorySettingsKey::~XQCentralRepositorySettingsKey()
{
}

/* 
    \class XQPublishAndSubscribeSettingsKey
    \internal

    \brief The XQPublishAndSubscribeSettingsKey defines the Publish and
    Subscribe target for XQSettingsManager operations.
*/

/* 
    Constructs an XQPublishAndSubscribeSettingsKey object.
    \param categoryUid RProperty category Uid.
    \param key Settings Key
*/
XQPublishAndSubscribeSettingsKey::XQPublishAndSubscribeSettingsKey(long int categoryUid, unsigned long int key) :
    XQSettingsKey(XQSettingsKey::TargetPublishAndSubscribe, categoryUid, key)
{
}

/* 
    Destructs an XQPublishAndSubscribeSettingsKey object.
*/
XQPublishAndSubscribeSettingsKey::~XQPublishAndSubscribeSettingsKey()
{
}

// End of file
