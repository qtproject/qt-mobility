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

#ifndef Q_CC_MINGW
#define _WIN32_DCOM
#include "qwmihelper_win_p.h"
#include <QDebug>
#include <ObjBase.h>
#include <Wbemidl.h>
#include <Oleauto.h>
#include <QStringList>
#include <QUuid>

QTM_BEGIN_NAMESPACE

WMIHelper::WMIHelper(QObject * parent)
        : QObject(parent)
{
   m_conditional = QString();
}

WMIHelper::~WMIHelper()
{
    CoUninitialize();
}

QVariant WMIHelper::getWMIData()
{
   if (!m_wmiNamespace.isEmpty() && !m_className.isEmpty() && !m_classProperties.isEmpty()) {
      return getWMIData(m_wmiNamespace, m_className, m_classProperties);
   }
   return QVariant();
}

void WMIHelper::initializeWMI(const QString &wmiNamespace)
{
    HRESULT hres;
    wbemLocator = 0;

    QUuid wbemLocatorClsid = "4590f811-1d3a-11d0-891f-00aa004b2e24";
    QUuid wbemLocatorIid = "dc12a687-737f-11cf-884d-00aa004b2e24";

    hres = CoCreateInstance(wbemLocatorClsid,0,CLSCTX_INPROC_SERVER,
                            wbemLocatorIid, (LPVOID *) &wbemLocator);

    if (hres == CO_E_NOTINITIALIZED) { // COM was not initialized
        CoInitializeEx(0, COINIT_MULTITHREADED);
        hres = CoCreateInstance(wbemLocatorClsid,0,CLSCTX_INPROC_SERVER,
                                wbemLocatorIid, (LPVOID *) &wbemLocator);
    }

    if (hres != S_OK) {
       qWarning() << "Failed to create IWbemLocator object." << hres;
        return ;
    }
    wbemServices = 0;
    hres = wbemLocator->ConnectServer(::SysAllocString(wmiNamespace.utf16()),0,0,0,0,0,0,&wbemServices);

    if (hres != WBEM_S_NO_ERROR){
        qWarning() << "Could not connect";
        return ;
    }

    hres = CoSetProxyBlanket( wbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, 0,
                              RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, 0, EOAC_NONE );

    if (hres != S_OK) {
       qWarning() << "Could not set proxy blanket" << hres;
        return ;
    }
}

QVariant WMIHelper::getWMIData(const QString &wmiNamespace, const QString &className, const QStringList &classProperty)
{
    initializeWMI(wmiNamespace);
    HRESULT hres;
    QVariant returnVariant;

      wbemEnumerator = 0;

    if (!m_conditional.isEmpty()) {
        if (m_conditional.left(1) != " ") {
            m_conditional.prepend(" ");
        }
    }

    QString aString = "SELECT * FROM " + className + m_conditional;
    BSTR bstrQuery;
    bstrQuery = ::SysAllocString(aString.utf16());

    hres = wbemServices->ExecQuery(L"WQL", bstrQuery,
            WBEM_FLAG_BIDIRECTIONAL | WBEM_FLAG_RETURN_IMMEDIATELY,0,&wbemEnumerator);

    if (hres != WBEM_S_NO_ERROR){
        qWarning() << "WMI Query failed.";
        wbemLocator->Release();
        wbemEnumerator->Release();
        return returnVariant;
    }

    ::SysFreeString(bstrQuery);

    wbemCLassObject = 0;
    ULONG result = 0;

    wmiVariantList.clear();
    while (wbemEnumerator) {
        HRESULT hr = wbemEnumerator->Next(WBEM_INFINITE, 1,&wbemCLassObject, &result);
        if(0 == result){
            break;
        }

        foreach(const QString property, classProperty) {
            VARIANT msVariant;
            CIMTYPE variantType;
            hr = wbemCLassObject->Get(property.utf16(), 0, &msVariant, &variantType, 0);
            returnVariant = msVariantToQVariant(msVariant, variantType);
            wmiVariantList << returnVariant;

            VariantClear(&msVariant);

        }

        wbemCLassObject->Release();
    }

    wbemEnumerator->Release();
    wbemLocator->Release();
    wbemServices->Release();
    return returnVariant;
}

QVariant WMIHelper::msVariantToQVariant(VARIANT msVariant, CIMTYPE variantType)
{
    QVariant returnVariant;
    switch(variantType) {
    case CIM_STRING:
    case CIM_CHAR16:
        {
            QString str((QChar*)msVariant.bstrVal, wcslen(msVariant.bstrVal));
            QVariant vs(str);
            returnVariant = vs;
        }
        break;
    case CIM_BOOLEAN:
        {
            QVariant vb(msVariant.boolVal);
            returnVariant = vb;
        }
        break;
            case CIM_UINT8:
        {
            QVariant vb(msVariant.uintVal);
            returnVariant = vb;
        }
        break;
            case CIM_UINT16:
        {
            QVariant vb(msVariant.uintVal);
            returnVariant = vb;
        }
            case CIM_UINT32:
        {
            QVariant vb(msVariant.uintVal);
            returnVariant = vb;
        }
        break;
    };
    VariantClear(&msVariant);
    return returnVariant;
}

  void WMIHelper::setWmiNamespace(const QString &wmiNamespace)
{
   m_wmiNamespace = wmiNamespace;
}

   void WMIHelper::setClassName(const QString &className)
{
   m_className = className;
}

void WMIHelper::setClassProperty(const QStringList &classProperties)
{
   m_classProperties = classProperties;
}

void WMIHelper::setConditional(const QString &conditional)
{
   m_conditional = conditional;
}

QTM_END_NAMESPACE
#endif
