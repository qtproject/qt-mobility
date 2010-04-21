/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "xqsettingsmanager.h"
#include "xqsettingsmanager_p.h"
#include <QVariant>

/* 
    \class XQSettingsManager
    \internal

    \brief The XQSettingsManager class provides methods to handle settings items.
    The settings items can be targetted in Central Repository or Publish And Subscribe.
*/

/* 
    Constructs an XQSettingsManager object with the given parent.
*/
XQSettingsManager::XQSettingsManager(QObject* parent) 
    : QObject(parent), d(new XQSettingsManagerPrivate(this))
{
}

/* 
    Destroys the XQSettingsManager object.
*/
XQSettingsManager::~XQSettingsManager()
{
    delete d;
}

/* 
    \enum XQSettingsManager::Type

    This enum defines the possible item types for an XQSettingsKey object.
*/
/*  \var XQSettingsManager::TypeVariant XQSettingsManager::TypeVariant
    The item type is detected automatically.
*/
/*  \var XQSettingsManager::TypeInt XQSettingsManager::TypeInt
    The item is treated as integer.
*/
/*  \var XQSettingsManager::TypeDouble XQSettingsManager::TypeDouble
    The item is treated as double. NOTE: This is not supported in Publish And Subscribe.
*/
/*  \var XQSettingsManager::TypeString XQSettingsManager::TypeString
    The item is treated as string.
*/
/*  \var XQSettingsManager::TypeByteArray XQSettingsManager::TypeByteArray
    The item is treated as bytearray.
*/



/*!
    \enum XQSettingsManager::Error

    This enum defines the possible errors for an XQSettingsManager object.
*/
/*  \var XQSettingsManager::Error XQSettingsManager::NoError
    No error occured.
*/
/*  \var XQSettingsManager::Error XQSettingsManager::OutOfMemoryError
    Not enough memory.
*/
/*  \var XQSettingsManager::Error XQSettingsManager::NotFoundError
    Item not found error.
*/
/*  \var XQSettingsManager::Error XQSettingsManager::AlreadyExistsError
    Item already exists error.
*/
/*  \var XQSettingsManager::Error XQSettingsManager::NotSupportedError
    Operation with this kind of item type isn't supported error.
*/
/*  \var XQSettingsManager::Error XQSettingsManager::PermissionDeniedError
    Permission denied.
*/
/*  \var XQSettingsManager::Error XQSettingsManager::UnknownError
    Unknown error.
*/


/* 
    Reads an item value.
    \param key XQSettingsKey where the value is read from
    \param type Value type. Default is TypeVariant which means that the type is
    tried to detect automatically.
    \return Settings value as QVariant or null object if an error has occurred.
    \sa error(), writeItemValue()
*/
QVariant XQSettingsManager::readItemValue(const XQSettingsKey& key, XQSettingsManager::Type type)
{
    return d->readItemValue(key, type);
}

/* 
    Writes an item value.
    \param key XQSettingsKey where the value is written to
    \param value Value to be written into the settings item. The type is determined with
    QVariant.type() and it must be either QVariant::Int, QVariant::Double, QVariant::String or
    QVariant::ByteArray.
    \return True if the item was written succesfully, otherwise return false.
    \sa error(), readItemValue()
*/
bool XQSettingsManager::writeItemValue(const XQSettingsKey& key, const QVariant& value)
{
    return d->writeItemValue(key, value);
}

/* 
    Starts monitoring a settings item.
    \param key XQSettingsKey of which changes are monitored.
    \param type Value type. Default is TypeVariant which means that the type is
    tried to detect automatically.
    \return True if monitoring was started succesfully, otherwise return false.
    \sa error(), stopMonitoring()
*/
bool XQSettingsManager::startMonitoring(const XQSettingsKey& key, XQSettingsManager::Type type)
{
    return d->startMonitoring(key, type);
}

/* 
    Stops monitoring a settings item.
    \param key XQSettingsKey of which changes are not monitored any more.
    \return True if monitoring was stopped succesfully, otherwise return false.
    \sa error(), startMonitoring()
*/
bool XQSettingsManager::stopMonitoring(const XQSettingsKey& key)
{
    return d->stopMonitoring(key);
}

/* 
    Returns the type of error that occurred if the latest function call failed. 
    Otherwise it returns NoError
    \return Error code
*/
XQSettingsManager::Error XQSettingsManager::error() const
{
    return d->error();
}
