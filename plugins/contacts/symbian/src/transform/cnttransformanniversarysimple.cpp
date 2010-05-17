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
#include "CntTransformAnniversarySimple.h"

QList<CContactItemField *> CntTransformAnniversarySimple::transformDetailL(const QContactDetail &detail)
{
    if(detail.definitionName() != QContactAnniversary::DefinitionName)
       User::Leave(KErrArgument);

    QList<CContactItemField *> fieldList;

    //cast to anniversary
    const QContactAnniversary &anniversary(static_cast<const QContactAnniversary&>(detail));

    //create new field
    TDateTime dateTime(anniversary.originalDate().year(),
            TMonth(anniversary.originalDate().month() - 1),
            anniversary.originalDate().day() - 1, 0, 0, 0, 0);
    CContactItemField* newField = CContactItemField::NewLC(KStorageTypeDateTime, KUidContactFieldAnniversary);
    newField->DateTimeStorage()->SetTime(dateTime);
    newField->SetMapping(KUidContactFieldVCardMapAnniversary);

    fieldList.append(newField);
    CleanupStack::Pop(newField);

    return fieldList;
}

QContactDetail *CntTransformAnniversarySimple::transformItemField(const CContactItemField& field, const QContact &contact)
{
    Q_UNUSED(contact);

    QContactAnniversary *anniversary = new QContactAnniversary();

    CContactDateField* storage = field.DateTimeStorage();
    TTime time(storage->Time());
    QDate qDate(time.DateTime().Year(), time.DateTime().Month() + 1, time.DateTime().Day() + 1);
    anniversary->setOriginalDate(qDate);

    return anniversary;
}

bool CntTransformAnniversarySimple::supportsDetail(QString detailName) const
{
    bool ret = false;
    if (detailName == QContactAnniversary::DefinitionName) {
        ret = true;
    }
    return ret;
}

QList<TUid> CntTransformAnniversarySimple::supportedFields() const
{
    return QList<TUid>()
        << KUidContactFieldAnniversary;
}

QList<TUid> CntTransformAnniversarySimple::supportedSortingFieldTypes(QString /*detailFieldName*/) const
{
    // Sorting not supported
    return QList<TUid>();
}

/*!
 * Checks whether the subtype is supported
 *
 * \a subType The subtype to be checked
 * \return True if this subtype is supported
 */
bool CntTransformAnniversarySimple::supportsSubType(const QString& /*subType*/) const
{
    return false;
}

/*!
 * Returns the filed id corresponding to a field
 *
 * \a fieldName The name of the supported field
 * \return fieldId for the fieldName, 0  if not supported
 */
quint32 CntTransformAnniversarySimple::getIdForField(const QString& /*fieldName*/) const
{
    return 0;
}

/*!
 * Modifies the detail definitions. The default detail definitions are
 * queried from QContactManagerEngine::schemaDefinitions and then modified
 * with this function in the transform leaf classes.
 *
 * \a definitions The detail definitions to modify.
 * \a contactType The contact type the definitions apply for.
 */
void CntTransformAnniversarySimple::detailDefinitions(QMap<QString, QContactDetailDefinition> &definitions, const QString& contactType) const
{
    Q_UNUSED(contactType);

    if(definitions.contains(QContactAnniversary::DefinitionName)) {
        QContactDetailDefinition d = definitions.value(QContactAnniversary::DefinitionName);
        QMap<QString, QContactDetailFieldDefinition> fields = d.fields();

        // Following fields not supported in symbian (pre-10.1) back-end, remove
        fields.remove(QContactAnniversary::FieldCalendarId);
        fields.remove(QContactAnniversary::FieldEvent);

        // Sub-types not supported in symbian back-end, remove
        fields.remove(QContactAnniversary::FieldSubType);

        // Context not supported in symbian back-end, remove
        fields.remove(QContactAnniversary::FieldContext);

        d.setFields(fields);
        d.setUnique(true);

        // Replace original definitions
        definitions.insert(d.name(), d);
    }
}
