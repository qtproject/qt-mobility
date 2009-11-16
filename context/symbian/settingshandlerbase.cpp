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
