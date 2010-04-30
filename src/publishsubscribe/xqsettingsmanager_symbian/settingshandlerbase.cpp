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

#include "settingshandlerbase.h"

#include "xqsettingskey.h"

QVariant SettingsHandlerBase::handleReadItemValue(const XQSettingsKey& key, XQSettingsManager::Type type, TInt& error)
{
    const TInt KRBufDefaultLength = 32;
    switch(type)
    {
        case XQSettingsManager::TypeVariant:
        {
            //Try to read TInt
            TInt intValue;
            error = getValue(key.key(), intValue);
            if (error == KErrNone)
            {
                return QVariant(intValue);
            }

            //Try to read TReal
            TReal realValue;
            error = getValue(key.key(), realValue);
            if (error == KErrNone)
            {
                return QVariant(realValue);
            }

            //Try to read RBuf8
            QVariant byteArrayVariant;
            TRAP(error,
                RBuf8 tdes8Value;
                tdes8Value.CreateL(KRBufDefaultLength);
                CleanupClosePushL(tdes8Value);
                getValueL(key.key(), tdes8Value);
                byteArrayVariant.setValue(QByteArray((const char*)tdes8Value.Ptr(), tdes8Value.Length()));
                CleanupStack::PopAndDestroy(&tdes8Value);
            )
            if (error == KErrNone)
            {
                return byteArrayVariant;
            }
            break;
        }
        case XQSettingsManager::TypeInt:
        {
            //Try to read TInt
            TInt intValue;
            error = getValue(key.key(), intValue);
            if (error == KErrNone)
            {
                return QVariant(intValue);
            }
            break;
        }
        case XQSettingsManager::TypeDouble:
        {
            //Try to read TReal
            TReal realValue;
            error = getValue(key.key(), realValue);
            if (error == KErrNone)
            {
                return QVariant(realValue);
            }
            break;
        }
        case XQSettingsManager::TypeString:
        {
            //Try to read RBuf8
            QVariant stringVariant;
            TRAP(error,
                RBuf16 tdes16Value;
                tdes16Value.CreateL(KRBufDefaultLength);
                CleanupClosePushL(tdes16Value);
                getValueL(key.key(), tdes16Value);
                stringVariant.setValue(QString::fromUtf16(tdes16Value.Ptr(), tdes16Value.Length()));
                CleanupStack::PopAndDestroy(&tdes16Value);
            )
            if (error == KErrNone)
            {
                return stringVariant;
            }
            break;
        }
        case XQSettingsManager::TypeByteArray:
        {
            //Try to read RBuf8
            QVariant byteArrayVariant;
            TRAP(error,
                RBuf8 tdes8Value;
                tdes8Value.CreateL(KRBufDefaultLength);
                CleanupClosePushL(tdes8Value);
                getValueL(key.key(), tdes8Value);
                byteArrayVariant.setValue(QByteArray((const char*)tdes8Value.Ptr(), tdes8Value.Length()));
                CleanupStack::PopAndDestroy(&tdes8Value);
            )
            if (error == KErrNone)
            {
                return byteArrayVariant;
            }
            break;
        }
        default:
        {
            break;
        }
    };

    return QVariant();
}

bool SettingsHandlerBase::handleWriteItemValue(const XQSettingsKey& key, const QVariant& value, TInt& error)
{
    switch (value.type())
    {
        case QVariant::Int:
        {
            error = setValue(key.key(), value.toInt());
            break;
        }
        case QVariant::Double:
        {
            error = setValue(key.key(), value.toDouble());
            break;
        }
        case QVariant::String:
        {
            error = setValue(key.key(), TPtrC16(reinterpret_cast<const TUint16*>(value.toString().utf16())));
            break;
        }
        case QVariant::ByteArray:
        {
            QByteArray byteArray(value.toByteArray());
            error = setValue(key.key(), TPtrC8((TUint8*)(byteArray.constData()), byteArray.size()));
            break;
        }
        default:
        {
            error = KErrArgument;
            break;
        }
    }
    return error == KErrNone;
}
