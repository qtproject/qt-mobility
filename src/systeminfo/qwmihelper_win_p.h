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

#ifndef WMIHELPER_H
#define WMIHELPER_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#ifndef Q_CC_MINGW
#include <qmobilityglobal.h>
#include <QObject>
#include <QVariant>
#include <QString>
#include <Wbemidl.h>

#include <QStringList>

QTM_BEGIN_NAMESPACE

class WMIHelper : public QObject
{
public:
    WMIHelper(QObject *parent = 0);
    ~WMIHelper();
    QVariant getWMIData();
    QVariant getWMIData(const QString &wmiNamespace,const QString &className, const QStringList &classProperties);
    QList <QVariant> wmiVariantList;
   void setWmiNamespace(const QString &wmiNamespace);
   void setClassName(const QString &className);
   void setClassProperty(const QStringList &classProperties);

   void setConditional(const QString &conditional); //see WQL SQL for WMI)

private:
   IWbemLocator *wbemLocator;
   IWbemServices *wbemServices;
   IWbemClassObject *wbemCLassObject;
   IEnumWbemClassObject *wbemEnumerator;

   QString m_className;
   QStringList m_classProperties;
   QString m_conditional;
   QString m_wmiNamespace;
   QVariant  msVariantToQVariant(VARIANT msVariant, CIMTYPE variantType);
   void initializeWMI(const QString &wmiNamespace);

};

QTM_END_NAMESPACE
#endif
#endif // WMIHELPER_H
