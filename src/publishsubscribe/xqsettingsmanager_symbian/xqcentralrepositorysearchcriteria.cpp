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

#include "xqcentralrepositorysearchcriteria.h"

/*!
    \class XQCentralRepositorySearchCriteria
    \internal

    \brief The XQCentralRepositorySearchCriteria defines search criteria
    used in XQCentralRepositoryUtils::findKeys().
*/


/* 
    Constructs an XQCentralRepositorySearchCriteria object.
    \param repositoryUid Central repository where the key is searched
    \param partialKey Partial Central repository key
    \param bitMask Bit mask for partialKey
*/
XQCentralRepositorySearchCriteria::XQCentralRepositorySearchCriteria(long int repositoryUid,
    unsigned long int partialKey, unsigned long int bitMask)
    : m_repositoryUid(repositoryUid), m_partialKey(partialKey), m_bitMask(bitMask),
    m_negativeCriteria(false)
{
}

/* 
    Destructs an XQCentralRepositorySearchCriteria object.
*/
XQCentralRepositorySearchCriteria::~XQCentralRepositorySearchCriteria()
{
}

/* 
    Sets the value criteria. If this function is not called or value is NULL object
    every key in the repository is returned in XQCentralRepositoryUtils::findKeys().
    \param value Value to be searched. The type is determined with
    QVariant.type() and it must be either QVariant::Int, QVariant::Double,
    QVariant::String or QVariant::ByteArray.
    \param negativeCriteria If true the keys of which values don't match will be returned in
    XQCentralRepositoryUtils::findKeys()
*/
void XQCentralRepositorySearchCriteria::setValueCriteria(const QVariant& value, bool negativeCriteria)
{
    m_value = value;
    m_negativeCriteria = negativeCriteria;
}

/* 
    \return Repository UID
*/
long int XQCentralRepositorySearchCriteria::repositoryUid() const
{
    return m_repositoryUid;
}

/* 
    \return Partial key
*/
unsigned long int XQCentralRepositorySearchCriteria::partialKey() const
{
    return m_partialKey;
}

/* 
    \return Bit mask for the partial key
*/
unsigned long int XQCentralRepositorySearchCriteria::bitMask() const
{
    return m_bitMask;
}

/* 
    \return Value criteria
*/
const QVariant& XQCentralRepositorySearchCriteria::value() const
{
    return m_value;
}

/* 
    \return True if value criteria is negative
*/
bool XQCentralRepositorySearchCriteria::negativeCriteria() const
{
    return m_negativeCriteria;
}
