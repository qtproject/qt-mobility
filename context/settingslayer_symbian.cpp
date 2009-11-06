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
#include "settingslayer_symbian.h"

#include <QDebug>

Q_GLOBAL_STATIC(SymbianSettingsLayer, symbianSettingsLayer);
QVALUESPACE_AUTO_INSTALL_LAYER(SymbianSettingsLayer);

SymbianSettingsLayer::SymbianSettingsLayer()
{
    qDebug("SymbianSettingsLayer::SymbianSettingsLayer()");
}

SymbianSettingsLayer::~SymbianSettingsLayer()
{
    qDebug("SymbianSettingsLayer::~SymbianSettingsLayer()");
}

QString SymbianSettingsLayer::name()
{
    qDebug("QString SymbianSettingsLayer::name()");
    return QLatin1String("Symbian Settings Layer");
}

QUuid SymbianSettingsLayer::id()
{
    qDebug("");
    return QVALUESPACE_SYMBIAN_SETTINGS_LAYER;
}

unsigned int SymbianSettingsLayer::order()
{
    qDebug("");
    return 0;
}

QValueSpace::LayerOptions SymbianSettingsLayer::layerOptions() const
{
    qDebug("");
    return QValueSpace::UnspecifiedLayer;
}

SymbianSettingsLayer *SymbianSettingsLayer::instance()
{
    qDebug("");
    return symbianSettingsLayer();
}

bool SymbianSettingsLayer::startup(Type type)
{
    qDebug("bool SymbianSettingsLayer::startup(Type type)");
    return true;
}

bool SymbianSettingsLayer::value(Handle handle, QVariant *data)
{
    qDebug("bool SymbianSettingsLayer::value(Handle handle, QVariant *data)");
    return false;
}

bool SymbianSettingsLayer::value(Handle handle, const QString &subPath, QVariant *data)
{
    qDebug("bool SymbianSettingsLayer::value(Handle handle, const QString &subPath, QVariant *data)");
    return false;
}

QSet<QString> SymbianSettingsLayer::children(Handle handle)
{
    qDebug("QSet<QString> SymbianSettingsLayer::children(Handle handle)");
    return QSet<QString>();
}

QAbstractValueSpaceLayer::Handle SymbianSettingsLayer::item(Handle parent, const QString &path)
{
    qDebug("QAbstractValueSpaceLayer::Handle SymbianSettingsLayer::item(Handle parent, const QString &path)");
    return 0;
}

void SymbianSettingsLayer::setProperty(Handle handle, Properties properties)
{
    qDebug("void SymbianSettingsLayer::setProperty(Handle handle, Properties properties)");
}

void SymbianSettingsLayer::removeHandle(Handle handle)
{
    qDebug("void SymbianSettingsLayer::removeHandle(Handle handle)");
}

bool SymbianSettingsLayer::setValue(QValueSpaceProvider *creator, Handle handle, const QString &subPath,
    const QVariant &data)
{
    qDebug("bool SymbianSettingsLayer::setValue(QValueSpaceProvider *creator, Handle handle, const QString &subPath, const QVariant &data)");
    return false;
}

void SymbianSettingsLayer::sync()
{
    qDebug("void SymbianSettingsLayer::sync()");
}

bool SymbianSettingsLayer::removeSubTree(QValueSpaceProvider *creator, Handle handle)
{
    qDebug("bool SymbianSettingsLayer::removeSubTree(QValueSpaceProvider *creator, Handle handle)");
    return false;
}

bool SymbianSettingsLayer::removeValue(QValueSpaceProvider *creator,
    Handle handle,
    const QString &subPath)
{
    qDebug("bool SymbianSettingsLayer::removeValue(QValueSpaceProvider *creator, Handle handle, const QString &subPath)");
    return false;
}

void SymbianSettingsLayer::addWatch(QValueSpaceProvider *, Handle)
{
    qDebug("void SymbianSettingsLayer::addWatch(QValueSpaceProvider *, Handle)");
}

void SymbianSettingsLayer::removeWatches(QValueSpaceProvider *, Handle)
{
    qDebug("void SymbianSettingsLayer::removeWatches(QValueSpaceProvider *, Handle)");
}

bool SymbianSettingsLayer::supportsInterestNotification() const
{
    qDebug("bool SymbianSettingsLayer::supportsInterestNotification() const");
    return false;
}

bool SymbianSettingsLayer::notifyInterest(Handle, bool)
{
    qDebug("bool SymbianSettingsLayer::notifyInterest(Handle, bool)");
    return false;
}

QT_END_NAMESPACE
