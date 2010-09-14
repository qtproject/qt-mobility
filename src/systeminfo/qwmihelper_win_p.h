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
    Q_OBJECT

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
    void setupNotfication(const QString &wmiNamespace,const QString &className, const QStringList &classProperties);
    static QVariant  msVariantToQVariant(VARIANT msVariant, CIMTYPE variantType);

    static  WMIHelper *instance();
    void emitNotificationArrived();

Q_SIGNALS:
    void wminotificationArrived();

private:
    IWbemLocator *wbemLocator;
    IWbemServices *wbemServices;
    IWbemClassObject *wbemCLassObject;
    IEnumWbemClassObject *wbemEnumerator;

    QString m_className;
    QStringList m_classProperties;
    QString m_conditional;
    QString m_wmiNamespace;
    void initializeWMI(const QString &wmiNamespace);

};

class EventSink : public IWbemObjectSink
{
    LONG m_lRef;
    bool bDone;

public:
    EventSink() { m_lRef = 0; }
    ~EventSink() { bDone = true; }

    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();
    virtual HRESULT
            STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv);

    virtual HRESULT STDMETHODCALLTYPE Indicate(
            LONG lObjectCount,
            IWbemClassObject __RPC_FAR *__RPC_FAR *apObjArray);

    virtual HRESULT STDMETHODCALLTYPE SetStatus(
            LONG lFlags,
            HRESULT hResult,
            BSTR strParam,
            IWbemClassObject __RPC_FAR *pObjParam);

};

QTM_END_NAMESPACE
#endif
#endif // WMIHELPER_H
