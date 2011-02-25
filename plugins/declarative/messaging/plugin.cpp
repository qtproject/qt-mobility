/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
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
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <qdeclarative.h>
#include <QDeclarativeExtensionPlugin>
#include "qdeclarativemessagefilter.h"
#include "qdeclarativemessagemodel.h"

class QDeclarativeMessageModelPlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    void registerTypes(const char *uri)
    {
        Q_ASSERT(uri == QLatin1String("QtMobility.messaging"));
        qmlRegisterType<QDeclarativeMessageModel>(uri, 1, 1, "MessageModel");
        qmlRegisterType<QDeclarativeMessageIntersectionFilter>(uri, 1, 1, "MessageIntersectionFilter");
        qmlRegisterType<QDeclarativeMessageUnionFilter>(uri, 1, 1, "MessageUnionFilter");
        qmlRegisterType<QDeclarativeMessageFilter>(uri, 1, 1, "MessageFilter");
        qmlRegisterUncreatableType<QDeclarativeMessageFilterBase>(uri,1,1,"MessageFilterBase",QDeclarativeMessageFilterBase::tr("MessageFilterBase is an abstract class"));
        qRegisterMetaType<QMessageId>("QMessageId");
        qRegisterMetaType<QMessageIdList>("QMessageIdList");
    }
};

Q_EXPORT_PLUGIN2(qmlmessaging, QDeclarativeMessageModelPlugin);

#include "plugin.moc"

