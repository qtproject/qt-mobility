/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "wmihelper.h"
#include <QDebug>

#include <Objbase.h>
#include <Wbemidl.h>
#include <Oleauto.h>

WMIHelper::WMIHelper()
{
    HRESULT hres;
    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (hres == S_FALSE) {
        qWarning() << "Failed to initialize COM library.";
        return;
    }

    wbemLocator = NULL;

    hres = CoCreateInstance(CLSID_WbemLocator,0,CLSCTX_INPROC_SERVER,
                            IID_IWbemLocator, (LPVOID *) &wbemLocator);

    if (hres != S_OK) {
        qWarning() << "Failed to create IWbemLocator object.";
        return;
    }
}

WMIHelper::~WMIHelper()
{
    CoUninitialize();
}

QVariant WMIHelper::getWMIData(const QString &wmiNamespace, const QString &className, const QString &classProperty)
{
    qWarning() << wmiNamespace << className << classProperty;
    QVariant returnVariant;
    HRESULT hres;
    IWbemServices *wbemServices = NULL;
    hres = wbemLocator->ConnectServer(::SysAllocString(wmiNamespace.utf16()),NULL,NULL,0,NULL,0,0,&wbemServices);

    if (hres != WBEM_S_NO_ERROR){
        qWarning() << "Could not connect";
        wbemLocator->Release();
        return returnVariant;
    }

    /////////////////////
    hres = CoSetProxyBlanket( wbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
                              RPC_C_AUTHN_LEVEL_CALL,  RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE );

    if (hres != S_OK) {
        qWarning() << "Could not set proxy blanket";
        wbemServices->Release();
        wbemLocator->Release();
        return returnVariant;
    }
    ////////////////////////
    IEnumWbemClassObject* wbemEnumerator = NULL;
    BSTR bstrWQL = ::SysAllocString(L"WQL");
    QString aString = "SELECT * FROM " + className;
    BSTR bstrQuery = ::SysAllocString(aString.utf16());

    hres = wbemServices->ExecQuery( bstrWQL,  bstrQuery,
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL,&wbemEnumerator);

    if (hres != WBEM_S_NO_ERROR){
        qWarning() << "WMI Query failed.";
        wbemLocator->Release();
        wbemEnumerator->Release();
        return returnVariant;
    }
    ///////////////////////
    IWbemClassObject *wbemCLassObject;
    ULONG result = 0;
    while (wbemEnumerator) {
        HRESULT hr = wbemEnumerator->Next(WBEM_INFINITE, 1,&wbemCLassObject, &result);
        if(0 == result){
            qWarning() << "resuilt == 0";
            break;
        }

        VARIANT msVariant;
        CIMTYPE variantType;
        hr = wbemCLassObject->Get(classProperty.utf16(), 0, &msVariant, &variantType, 0);
        switch(variantType) {
        case CIM_STRING:
        case CIM_CHAR16:
            {
                QString str((QChar*)msVariant.bstrVal, wcslen(msVariant.bstrVal));
                qWarning() << str;
                QVariant vs(str);
                returnVariant = vs;
            }
            break;
        case CIM_BOOLEAN:
            {
                QVariant vb(msVariant.boolVal);
                qWarning() << vb.toBool();
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
                qWarning() << vb.toUInt();
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
        wbemCLassObject->Release();
    }

    wbemServices->Release();
    wbemLocator->Release();
    wbemEnumerator->Release();
    return returnVariant;
}
