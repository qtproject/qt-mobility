/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#include <QtDeclarative/qdeclarativeextensionplugin.h>
#include <QtDeclarative/qdeclarative.h>

#include "qdeclarativetelephony.h"

QT_BEGIN_NAMESPACE

QTM_USE_NAMESPACE

using namespace QTelephony;

/*!
    \class QTelephonyDeclarativeModule
    \ingroup telephonyapi
    \brief The QTelephonyDeclarativeModule class contains one function to register the telephony QML objects.
*/
class QTelephonyDeclarativeModule : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    /*!
    \fn  virtual void registerTypes(const char *uri)

    Function that register the telephony objects for QML usage.
    */
    virtual void registerTypes(const char *uri)
    {
        qDebug() << "QTelephonyDeclarativeModule::registerTypes(const char *uri)";

        Q_ASSERT(QLatin1String(uri) == QLatin1String("QtMobility.telephony"));
        qmlRegisterType<QDeclarativeTelephonyCallInfo>(uri, 1, 1, "TelephonyCallInfo");
        qmlRegisterType<QDeclarativeTelephonyCallList>(uri, 1, 1, "TelephonyCallList");
    }
};

QT_END_NAMESPACE

#include "telephony.moc"

Q_EXPORT_PLUGIN2(qtelephonydeclarativemodule, QT_PREPEND_NAMESPACE(QTelephonyDeclarativeModule));
