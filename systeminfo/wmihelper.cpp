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
        qWarning() << "Failed to initialize COM library. Error code = 0x" << hex << hres;
        return;
    }

    pLoc = NULL;

    hres = CoCreateInstance(CLSID_WbemLocator,0,CLSCTX_INPROC_SERVER,
                            IID_IWbemLocator, (LPVOID *) &pLoc);

    if (FAILED(hres)) {
        qWarning() << "Failed to create IWbemLocator object." << " Err code = 0x"<< hex << hres;
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

    IWbemServices *pSvc = NULL;

    hres = pLoc->ConnectServer(
            ::SysAllocString(wmiNamespace.utf16()),NULL,NULL,0,NULL,0,0,&pSvc);

    if (hres != WBEM_S_NO_ERROR){
        qWarning() << "Could not connect. Error code = 0x" << hex << hres;
        pLoc->Release();
        return returnVariant;
    }

    /////////////////////
    hres = CoSetProxyBlanket(
            pSvc,                        // Indicates the proxy to set
            RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
            RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
            NULL,                        // Server principal name
            RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx
            RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
            NULL,                        // client identity
            EOAC_NONE                    // proxy capabilities
            );

    if (FAILED(hres)) {
        qWarning() << "Could not set proxy blanket. Error code = 0x"
                << hex << hres;
        pSvc->Release();
        pLoc->Release();
        return returnVariant;
    }
    ////////////////////////
    IEnumWbemClassObject* pEnumerator = NULL;
    BSTR bstrWQL = ::SysAllocString(L"WQL");
    QString aString = "SELECT * FROM " + className;
    BSTR bstrQuery = ::SysAllocString(aString.utf16());

    hres = pSvc->ExecQuery(
            bstrWQL,
            bstrQuery,
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            NULL,
            &pEnumerator);

    if (hres != WBEM_S_NO_ERROR){
        qWarning() << "WMI Query failed."
                << " Error code = 0x"
                << hex << hres;
        pSvc->Release();
        pLoc->Release();
        pEnumerator->Release();
        return returnVariant;
    }
    ///////////////////////
    IWbemClassObject *pclsObj;
    ULONG uReturn = 0;

    while (pEnumerator) {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,&pclsObj, &uReturn);
//        if(hr != WBEM_S_NO_ERROR) {
//            qWarning() << "enumerating query failed."
//                    << " Error code = 0x"
//                    << hex << hres << hr << GetLastError();
//        }

        if(0 == uReturn){
            break;
        }

        VARIANT vtProp;
        CIMTYPE variantType;
        hr = pclsObj->Get(classProperty.utf16(), 0, &vtProp, &variantType, 0);
        qWarning() << "variant type" << variantType;
        switch(variantType) {
        case CIM_STRING:
//            {
//                QVariant vb(vtProp.bstrVal);
//                returnVariant = vb;
//            }
//            break;
        case CIM_CHAR16:
            {
                QString str((QChar*)vtProp.bstrVal, wcslen(vtProp.bstrVal));
                QVariant vs(str);
                returnVariant = vs;
            }
            break;
        case CIM_BOOLEAN:
            {
                QVariant vb(vtProp.boolVal);
                returnVariant = vb;
            }
            break;
            case CIM_UINT8:
            {
                QVariant vb(vtProp.uintVal);
                returnVariant = vb;
            }
            break;
            case CIM_UINT16:
            {
                QVariant vb(vtProp.uintVal);
                returnVariant = vb;
            }
            case CIM_UINT32:
            {
                QVariant vb(vtProp.uintVal);
                returnVariant = vb;
            }
            break;
        };
        VariantClear(&vtProp);
        pclsObj->Release();
    }

    pSvc->Release();
    pLoc->Release();
    pEnumerator->Release();
    return returnVariant;
}
