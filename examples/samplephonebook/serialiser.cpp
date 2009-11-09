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

#include "serialiser.h"
#include <QUuid>

QString Serialiser::escaped(const QString& input)
{
    QString retn = "";
    for (int i = 0; i < input.length(); i++) {
        QChar currChar = input.at(i);
        if (currChar == '\\' ||
                currChar == '=' ||
                currChar == ',' ||
                currChar == ';' ||
                currChar == ':') {
            // we need to escape this character.
            retn += '\\'; // escape with a single backslash.
        }
        retn += currChar;
    }

    return retn;
}

QString Serialiser::convertDetail(const QContact& contact, const QContactDetail& detail, const QString& vcardField)
{
    // the format of the converted detail will be:
    // X-com-nokia-mobility-contacts-vcard-detail-UUID;vcardField;definitionName;key=value,key=value;preferredFor=actionId,actionId
    // where each of the elements are escaped strings.
    QString retn = "X-com-nokia-mobility-contacts-vcard-detail-";
    retn += escaped(QUuid::createUuid().toString());
    retn += ":";
    retn += escaped(vcardField);
    retn += ";";
    retn += escaped(detail.definitionName());
    retn += ";";

    QVariantMap vals = detail.values();
    foreach (const QString& key, vals.keys()) {
        retn += escaped(key);
        retn += "=";
        retn += escaped(vals.value(key).toString());
        retn += ",";
    }
    retn.chop(1);
    retn += ";";

    retn += "preferredFor=";
    foreach(const QContactActionDescriptor& actionDescr, contact.availableActions(QString())) {
        if (contact.isPreferredDetail(actionDescr.actionName(), detail)) {
            retn += escaped(actionDescr.actionName()) + ",";
        }
    }

    if (retn.endsWith(","))
        retn.chop(1);

    return retn;
}

QStringList Serialiser::findActionIdsFromCustomString(const QString& customString)
{
    QString pref = "preferredFor=";
    int startPos = customString.indexOf(pref) + pref.length();
    if (startPos <= pref.length() || startPos >= customString.length())
        return QStringList();

    QStringList retn;
    QString currentActionId = "";
    for (int i = startPos; i < customString.length(); i++) {
        QChar currChar = customString.at(i);
        if (currChar == '\\') {
            i += 1;
            currentActionId += customString.at(i);
        } else if (currChar == ',') {
            if (currentActionId.length() > 0) {
                retn << currentActionId;
            }

            currentActionId = "";
        }
    }

    if (currentActionId.length() > 0)
        retn << currentActionId;

    return retn;
}

QString Serialiser::findLinkStringFromCustomString(const QString& customString)
{
    // check to see if the custom detail was put there by us.
    if (customString.indexOf("X-com-nokia-mobility-contacts-vcard-detail-") != 0) {
        return QString(); // no link.
    }

    // parse the detail.  In this implementation, we ignore the linked vcard field.
    int sectionCount = 0;
    QString linkString = "";
    for (int i = 0; i < customString.length(); i++) {
        QChar currChar = customString.at(i);

        // the second section is the link string.  grab it.
        if (sectionCount > 0) {
            if (currChar == '\\') {
                linkString += customString.at(i+1);
            } else {
                linkString += currChar;
            }
        }

        if (currChar == ';' || currChar == ':') {
            // check to see whether we've finished copying the link string.
            sectionCount += 1;
            if (sectionCount == 2) {
                linkString.chop(1); // chop off the trailing ';'
                return linkString;
            }
        } else if (currChar == '\\') {
            // skip the escaped character.
            i += 1;
        }
    }

    return QString(); // no link.
}

QContactDetail Serialiser::convertCustomString(const QString& customString)
{
    int nextSection = 0;
    int oldNextSection = 0;

    // check to see if the custom detail was put there by us.
    if (customString.indexOf("X-com-nokia-mobility-contacts-vcard-detail-") != 0) {
        return QContactDetail();
    }

    // parse the detail.  In this implementation, we ignore the linked vcard field.
    int sectionCount = 0;
    for (int i = nextSection; i < customString.length(); i++) {
        QChar currChar = customString.at(i);
        if (currChar == ';' || currChar == ':') {
            // check to see whether we've finished skipping past the unused sections.
            sectionCount += 1;
            if (sectionCount == 2) {
                nextSection = i + 1;
                break;
            }
        } else if (currChar == '\\') {
            // skip the escaped character.
            i += 1;
        }
    }

    // then, we parse the definitionName from the string.
    QString definitionName = "";
    for (int i = nextSection; i < customString.length(); i++) {
        QChar currChar = customString.at(i);
        if (currChar == ';') {
            nextSection = i + 1;
            break;
        } else if (currChar == '\\') {
            // the next character is escaped.
            i += 1;
        }
        definitionName += customString.at(i);
    }

    // check to see whether this custom string was built by us
    if (nextSection == oldNextSection) {
        // nope; just return an empty detail.
        return QContactDetail();
    }

    // then, we parse the values.
    QVariantMap values;
    QString currKey = "";
    QString currVal = "";
    for (int i = nextSection; i < customString.length(); i++) {
        QChar currChar = customString.at(i);
        if (currChar == ';') {
            // finished with the key/value pairs.
            nextSection = i+1;
            break;
        } else if (currChar == '=') {
            // we have found the key.  now find the value
            for (int j = i + 1; j < customString.length(); j++) {
                currChar = customString.at(j);
                if (currChar == ',') {
                    // finished with this key/value pair
                    i = j;
                    break;
                } else if (currChar == ';') {
                    // finished with all key/value pairs.
                    i = j - 1; // we want to see this char next time around.
                    break;
                } else if (currChar == '\\') {
                    j += 1;
                    currVal += customString.at(j);
                } else {
                    currVal += currChar;
                }
            }

            // and insert it into the map
            values.insert(currKey, currVal);
            currKey = "";
            currVal = "";
        } else if (currChar == ',') {
            // finished with this key/value pair.
            // this should have been handled in the above case...
            currKey = "";
            currVal = "";
        } else if (currChar == '\\') {
            // an escaped character in the key
            i += 1;
            currKey += customString.at(i);
        } else {
            // a normal character in the key.
            currKey += currChar;
        }
    }

    // check to see whether this custom string was built by us
    if (nextSection == oldNextSection) {
        // nope; just return an empty detail.
        return QContactDetail();
    }

    // we parse the attributes.
    QMap<QString, QString> attrs;
    currKey = "";
    QString currAttr = "";
    for (int i = nextSection; i < customString.length(); i++) {
        QChar currChar = customString.at(i);
        if (currChar == ';') {
            // finished with the key/value pairs.
            nextSection = i+1;
            break;
        } else if (currChar == '=') {
            // we have found the key.  now find the value
            for (int j = i + 1; j < customString.length(); j++) {
               currChar = customString.at(j);
                if (currChar == ',') {
                    // finished with this key/value pair
                    i = j;
                    break;
                } else if (currChar == ';') {
                    // finished with all key/value pairs.
                    i = j - 1; // we want to see this char next time around.
                    break;
                } else if (currChar == '\\') {
                    j += 1;
                    currAttr += customString.at(j);
                } else {
                    currAttr += currChar;
                }
            }

            // and insert it into the map
            values.insert(currKey, currAttr);
            currKey = "";
            currAttr = "";
       } else if (currChar == ',') {
            // finished with this key/value pair.
            // this should have been handled in the above case...
            currKey = "";
            currVal = "";
        } else if (currChar == '\\') {
            // an escaped character in the key
            i += 1;
            currKey += customString.at(i);
        } else {
            // a normal character in the key.
            currKey += currChar;
        }
    }

    // check to see whether this custom string was built by us
    if (nextSection == oldNextSection) {
        // nope; just return an empty detail.
        return QContactDetail();
    }

    // having parsed the definitionName, values, attributes, and preferences we build the detail.
    QContactDetail retn(definitionName);
    QStringList keys = values.keys();
    foreach (const QString& key, keys)
        retn.setValue(key, values.value(key));
    return retn;
}

QContact Serialiser::convertVcard(const QStringList& vcardLines)
{
    QMultiMap<QString, QContactDetail> vcardDetails;  // details built from vcard-supported fields
    QMultiMap<QString, QContactDetail> customDetails; // details built from x-custom fields which are linked to vcardDetails
    QList<QContactDetail> unlinkedCustomDetails;      // details built from x-custom fields

    QMap<QString, QContactDetail> customPreferredForUuid; // map of detail uuid to detail which has some associated preference information.
    QMap<QString, QStringList> customPreferredFor;  // map of detail uuid to list of actionds for which the detail is preferred.
    QList<QContactDetail> vcardPreferredFor;  // list of details build from vcard-supported fields which had the pref flag set.

    QContact retn;
    QStringList strippedVcardLines = removeClutter(vcardLines);
    foreach (const QString& line, strippedVcardLines) {
        if (line.startsWith("X-com-nokia-mobility-contacts-vcard-detail-")) {
            QContactDetail det = convertCustomString(line);
            if (!det.isEmpty()) {
                // the line was a custom detail that we saved.
                QString linkString = findLinkStringFromCustomString(line);
                if (linkString.isEmpty()) {
                    unlinkedCustomDetails.append(det);
                } else {
                    customDetails.insert(linkString, det);
                }

                QString detailUuid = QUuid::createUuid().toString();
                customPreferredForUuid.insert(detailUuid, det);
                customPreferredFor.insert(detailUuid, findActionIdsFromCustomString(line));
            }
        } else {
            QContactDetail det = parsePropertyType(line);
            QString defId = det.definitionName();
            if (defId.isEmpty()) {
                // this must be another program's custom field.  ignore it.
            } else {
                // this is a built-in vcard supported field.  build the detail.
                QStringList contexts = parseContext(line);
                if (!contexts.isEmpty())
                    det.setContexts(contexts);
                if (defId == QContactPhoneNumber::DefinitionName) {
                    det.setValue(QContactPhoneNumber::FieldSubTypes, parseAttributes(line));
                    det.setValue(QContactPhoneNumber::FieldNumber, parseValue(line));
                    vcardDetails.insert("TEL", det);
                } else if (defId == QContactEmailAddress::DefinitionName) {
                    det.setValue(QContactEmailAddress::FieldEmailAddress, parseValue(line));
                    vcardDetails.insert("EMAIL", det);
                } else if (defId == "Url") {
                    det.setValue(QContactUrl::FieldSubType, parseAttributes(line));
                    det.setValue(QContactUrl::DefinitionName, parseValue(line));
                    vcardDetails.insert("URL", det);
                } else if (defId == QContactAvatar::DefinitionName) {
                    det.setValue(QContactAvatar::FieldSubType, parseAttributes(line));
                    det.setValue(QContactAvatar::FieldAvatar, parseValue(line));
                    vcardDetails.insert("PHOTO", det);
                } else if (defId == QContactGuid::DefinitionName) {
                    det.setValue(QContactGuid::FieldGuid, parseValue(line));
                    vcardDetails.insert("UID", det);
                } else if (defId == QContactAddress::DefinitionName) {
                    det.setValue(QContactAddress::FieldSubTypes, parseAttributes(line));
                    QStringList fieldValues = parseValue(line).split(";");
                    // ignore values 0 and 1 (extended and postal address) in this implementation
                    det.setValue(QContactAddress::FieldStreet, fieldValues.value(2));
                    det.setValue(QContactAddress::FieldLocality, fieldValues.value(3));
                    det.setValue(QContactAddress::FieldRegion, fieldValues.value(4));
                    det.setValue(QContactAddress::FieldPostcode, fieldValues.value(5));
                    det.setValue(QContactAddress::FieldCountry, fieldValues.value(6));
                    vcardDetails.insert("ADR", det);
                } else if (defId == QContactName::DefinitionName) {
                    QStringList fieldValues = parseValue(line).split(";");
                    if (vcardDetails.contains("N")) {
                        // modify the existing name
                        QContactDetail currName = vcardDetails.value("N");
                        currName.setValue(QContactName::FieldPrefix, fieldValues.value(3));
                        currName.setValue(QContactName::FieldFirst, fieldValues.value(1));
                        currName.setValue(QContactName::FieldMiddle, fieldValues.value(2));
                        currName.setValue(QContactName::FieldLast, fieldValues.value(0));
                        currName.setValue(QContactName::FieldSuffix, fieldValues.value(4));
                        vcardDetails.replace("N", currName);
                    } else {
                        // create a new name if no existing name stored.
                        det.setValue(QContactName::FieldPrefix, fieldValues.value(3));
                        det.setValue(QContactName::FieldFirst, fieldValues.value(1));
                        det.setValue(QContactName::FieldMiddle, fieldValues.value(2));
                        det.setValue(QContactName::FieldLast, fieldValues.value(0));
                        det.setValue(QContactName::FieldSuffix, fieldValues.value(4));
                        vcardDetails.insert("N", det);
                    }
                } else {
                    // we don't know how to map this value type.  Ignore the line.
                    continue;
                }

                if (parsePreferred(line)) {
                    vcardPreferredFor.append(det);
                }
            }
        }
   }

    // at this stage, we have built two sets of details
    // the first set is built from our custom lines, which contain all the information
    // the second set is built from the vcard-builtin lines.
    // we now want to reconcile the two sets by throwing out any custom-set details
    //     which are not related to any built-in line but should be.
    foreach (const QString& linkString, customDetails.keys()) {
        foreach (const QContactDetail& linkedDetail, customDetails.values(linkString)) {
            bool found = false;
            QContactDetail toRemove;
            foreach (const QContactDetail& vcardDetail, vcardDetails.values(linkString)) {
                if (linkedDetail.values() == vcardDetail.values()) {
                    toRemove = vcardDetail;
                    found = true;
                    break;
                }
            }

            if (!found) {
                // this supposedly linked detail has no corresponding vcard detail.
                // it is an orphan.  ignore it.
            } else {
                // remove the vcard detail from the map!
                vcardDetails.remove(linkString, toRemove);
                vcardPreferredFor.removeOne(toRemove);

                // save the more verbose (ie, built from custom string) detail.
                toRemove = linkedDetail; // copy so we can call non-const function.
                retn.saveDetail(&toRemove);
            }
        }
    }

    // any details which are left in the vcardDetails map should be included
    foreach (const QContactDetail& det, vcardDetails.values()) {
        QContactDetail temp = det;
        retn.saveDetail(&temp);
    }

    // as should any unlinked custom details.
    foreach (const QContactDetail& det, unlinkedCustomDetails) {
        QContactDetail temp = det;
        retn.saveDetail(&temp);
    }

    // now set any preferences found in the vcard should be set
    //foreach (const QContactDetail& det, vcardPreferredFor) {
        // there is no granularity here; hence, we assume that if any detail is listed as preferred,
        // then it is the preferred detail for every action available to it.

        // new:
        //foreach (QContactAbstractAction* impl, QContactAbstractAction::implementations()) {
        //    foreach (const QString& actionId, impl->actionsForDetail(det)) {
        //        retn.setPreferredDetail(actionId, det);
        //    }
        //}

        // old:
        //foreach (const QString& actionId, QContactManager::availableActions(det.definitionName())) {
        //    retn.setPreferredDetail(actionId, det);
        //}
    //}

    // and now we set any preferences saved in the custom strings
    foreach (const QString& detailUuid, customPreferredForUuid.keys()) {
        QContactDetail det = customPreferredForUuid.value(detailUuid);
        foreach (const QStringList& actionIds, customPreferredFor.values(detailUuid)) {
            foreach (const QString& actionId, actionIds) {
                retn.setPreferredDetail(actionId, det);
            }
        }
    }

    return retn;
}

QStringList Serialiser::removeClutter(const QStringList& vcard)
{
    QStringList copy;

    foreach (QString line, vcard) {
        QString firstBit = line.split(":", QString::SkipEmptyParts).value(0);
        if (firstBit == "BEGIN" || firstBit == "VERSION" || firstBit == "CLASS" || firstBit == "PRODID" || firstBit == "REV" || firstBit == "END") {
            // ignore this line.
        } else {
            copy.append(line);
        }
    }

    return copy;
}

QContactDetail Serialiser::parsePropertyType(const QString& line)
{
    // depending on the line signature, return a QContact<PropertyType> object.
    QStringList semiColonSplit = line.split(";");
    QStringList colonSplit = line.split(":");
    if (semiColonSplit.size() == 0 && colonSplit.size() == 0)
        return QContactDetail();

    if (semiColonSplit.size() != 0) {
        if (semiColonSplit.at(0) == "TEL") {
            return QContactPhoneNumber();
        } else if (semiColonSplit.at(0) == "EMAIL") {
            return QContactEmailAddress();
        } else if (semiColonSplit.at(0) == "ADR") {
            return QContactAddress();
        } else if (semiColonSplit.at(0) == "PHOTO") {
            return QContactAvatar();
        } else if (semiColonSplit.at(0) == "UID") {
            return QContactGuid();
        }
    }

    // ok, so we have dealt with the cases where a semi colon proceeds the first token.
    // Now we must deal with the fields where the first token is proceeded by a colon.
    if (colonSplit.size() != 0) {
        if (colonSplit.at(0) == "URL") {
            return QContactDetail("Url");
        } else if (colonSplit.at(0) == "N") {
            QContactName name;
            return name;
        } else if (colonSplit.at(0) == "FN") {
            QContactDisplayLabel label;
            return label;
        }
    }

    // otherwise, it's an unknown or custom field.
    return QContactDetail();
}

QStringList Serialiser::parseContext(const QString& line)
{
    // depending on the TYPE= section, return a context (work/home/...)
    // returns the first one found.
    QStringList result;
    QStringList contexts;
    contexts << QContactDetail::ContextHome << QContactDetail::ContextWork << QContactDetail::ContextOther;
    QStringList semiColonSplit = line.split(";");
    for (int i = 0; i < semiColonSplit.size(); i++) {
        QString currSplit = semiColonSplit.at(i);
        if (currSplit.startsWith("TYPE=")) {
            for (int j = 0; j < contexts.size(); j++) {
                QString currContext = contexts.at(j);
                if (currSplit.contains(currContext.toLower()) && !result.contains(currContext)) {
                    result.append(currContext);
                    return result;
                }
            }
        }
   }

    return result;
}

QString Serialiser::parseValue(const QString& line)
{
    // return the settable value of this field
    QStringList colonSplit = line.split(":");
    if (colonSplit.size() != 2) {
        return QString();
    }

    return colonSplit.at(1);
}

QString Serialiser::parseAttributes(const QString& line)
{
    // return any special attributes of this field
    QString result = "";
    QStringList contexts;
    contexts << QContactDetail::ContextHome << QContactDetail::ContextWork << QContactDetail::ContextOther;
    QStringList semiColonSplit = line.split(";");
    for (int i = 0; i < semiColonSplit.size(); i++) {
        QString currSplit = semiColonSplit.at(i);
        if (currSplit.startsWith("TYPE=")) {
            // get rid of the TYPE= preamble, then remove any Contexts/Pref
            QString possibleAttrs = currSplit.remove(0,5);
            int startIndex = -1;

            // firstly, remove pref if it exists.
            startIndex = possibleAttrs.indexOf("pref");
            if (startIndex != -1)
                 possibleAttrs = possibleAttrs.remove(startIndex, 4);

            // then, remove any contexts
            for (int j = 0; j < contexts.size(); j++) {
                startIndex = possibleAttrs.indexOf(contexts.at(j).toLower());
                if (startIndex != -1) {
                    possibleAttrs = possibleAttrs.remove(startIndex, contexts.at(j).length());
                }
            }

            // finally, remove any value
            startIndex = possibleAttrs.indexOf(":");
            if (startIndex != -1) {
                possibleAttrs = possibleAttrs.left(startIndex);
            }

            // now we should be left with just attributes, separated by ",".
            QStringList attributes = possibleAttrs.split(",", QString::SkipEmptyParts);
            foreach (QString attribute, attributes) {
                // special cases: deal with the vcard TYPE attributes that don't
                // map directly to our QContactDetail SubType attributes.
                if (attribute == "cell") {
                    result += "Mobile,";
                } else if (attribute == "fax") {
                    result += "Facsimile,";
                } else if (attribute == "msg") {
                    result += "Messaging,";
                } else if (attribute == "dom") {
                    result += "Domestic,";
                } else if (attribute == "intl") {
                    result += "International,";
                } else {
                    QString firstLetter = "";
                    firstLetter += attribute.at(0);
                    firstLetter = firstLetter.toUpper();
                    QString rest = attribute.remove(0,1);
                    result += firstLetter + rest + ",";
                }
            }

            // remove the trailing `,'
            result.chop(1);
        }
    }

    return result;
}

bool Serialiser::parsePreferred(const QString& line)
{
    // return whether this field is preferred
    QStringList semiColonSplit = line.split(";");
    for (int i = 0; i < semiColonSplit.size(); i++) {
        QString currSplit = semiColonSplit.at(i);
        if (currSplit.startsWith("TYPE=")) {
            if (currSplit.contains("pref")) {
                return true;
            }
        }
    }

    return false;
}

bool Serialiser::detailIsPreferredForAnything(const QContact& contact, const QContactDetail& detail)
{
    foreach (const QContactActionDescriptor& actionDescr, contact.availableActions(QString())) {
        if (contact.isPreferredDetail(actionDescr.actionName(), detail)) {
            return true;
        }
    }

    return false;
}

QStringList Serialiser::convertContact(const QContact& contact)
{
    QStringList vcard;
    QContact c = contact;

    QDateTime curr = QDateTime::currentDateTime();
    QString entry = "REV:" + curr.toString("yyyy-MM-dd hh:mm:ss");
    vcard << "BEGIN:VCARD" << "VERSION:3.0" << "CLASS:PUBLIC" << "PRODID:-//qcontactvcardserializer from Nokia.com//NONSGML Version 1//EN" << entry;

    QStringList customVcardFields;

    QStringList vcardFieldsWithValues;
    foreach (const QContactDetail& det, contact.details()) {
        // we look at each detail of the contact
        // if the detail can be mapped to a non-singular vcard field, we create the vcard entry and the custom entry
        // if the detail can be mapped to a singular vcard field, we check to see that another detail which also
        //     maps to the same vcard field hasn't been saved as the vcard entry before doing the above.
        QString definitionName = det.definitionName();
        if (definitionName == QContactName::DefinitionName) {
            bool saved = false;

            // only one name is allowed
            if (!vcardFieldsWithValues.contains("N")) {
                entry = "N:" + det.value(QContactName::FieldLast) + ";" + det.value(QContactName::FieldFirst) + ";" + det.value(QContactName::FieldMiddle) + ";" + det.value(QContactName::FieldPrefix) + det.value(QContactName::FieldSuffix);
                vcardFieldsWithValues << "N";
                vcard << entry;
                customVcardFields << convertDetail(contact, det, "N");
                saved = true;
            }

            // if not already saved, save as a generic custom field.
            if (!saved) {
                customVcardFields << convertDetail(contact, det);
            }
        } else if (definitionName == QContactDisplayLabel::DefinitionName) {
            // only one formatted name is allowed
            if (!vcardFieldsWithValues.contains("FN")) {
                entry = "FN:" + det.value(QContactDisplayLabel::FieldLabel);
                vcard << entry;
                vcardFieldsWithValues << "FN";
                customVcardFields << convertDetail(contact, det, "FN");
            }
        } else if (definitionName == QContactAddress::DefinitionName) {
            // any number of address fields are allowed.
            entry = "ADR;TYPE=";
            QString typestr = "";
            QStringList subTypes = det.value<QStringList>(QContactAddress::FieldSubTypes);
            QStringList contexts = det.contexts();
            if (contexts.contains(QContactDetail::ContextHome)) typestr += "home,";
            if (contexts.contains(QContactDetail::ContextWork)) typestr += "work,";
            if (contexts.contains(QContactDetail::ContextOther)) typestr += "other,";
            if (subTypes.contains(QContactAddress::SubTypeDomestic)) typestr += "dom,";
            if (subTypes.contains(QContactAddress::SubTypeInternational)) typestr += "intl,";
            if (subTypes.contains(QContactAddress::SubTypeParcel)) typestr += "parcel,";
            if (subTypes.contains(QContactAddress::SubTypePostal)) typestr += "postal,";
            if (detailIsPreferredForAnything(contact, det)) typestr += "pref,";
            typestr.chop(1);
            entry += typestr + ":;;" + det.value(QContactAddress::FieldStreet) + ";" + det.value(QContactAddress::FieldLocality) + ";" + det.value(QContactAddress::FieldRegion) + ";" + det.value(QContactAddress::FieldPostcode) + ";" + det.value(QContactAddress::FieldCountry);
            vcard << entry;
            //entry = "LABEL;TYPE=" + typestr + ":" + det.value(QContactAddress::FieldDisplayLabel);
            //vcard << entry;
            vcardFieldsWithValues << "ADR";
            vcardFieldsWithValues << "LABEL";
            customVcardFields << convertDetail(contact, det, "ADR");
        } else if (definitionName == "Url") {
            // any number of Url fields are allowed.
            entry = "URL:" + det.value("Url");
            vcard << entry;
            vcardFieldsWithValues << "URL";
            customVcardFields << convertDetail(contact, det, "URL");
        } else if (definitionName == QContactEmailAddress::DefinitionName) {
            // any number of email address fields are allowed.
            entry = "EMAIL;TYPE=internet";
            if (detailIsPreferredForAnything(contact, det)) entry += ",pref";
            entry += ":" + det.value(QContactEmailAddress::FieldEmailAddress);
            vcard << entry;
            vcardFieldsWithValues << "EMAIL";
            customVcardFields << convertDetail(contact, det, "EMAIL");
        } else if (definitionName == QContactPhoneNumber::DefinitionName) {
            // any number of telephone fields are allowed.
            entry = "TEL;TYPE=";
            QString typeStr = "";
            QStringList subTypes = det.value<QStringList>(QContactPhoneNumber::FieldSubTypes);
            QStringList contexts = det.contexts();
            if (subTypes.contains(QContactPhoneNumber::SubTypeMobile)) typeStr += "cell,";
            if (subTypes.contains(QContactPhoneNumber::SubTypeFacsimile)) typeStr += "fax,";
            if (subTypes.contains(QContactPhoneNumber::SubTypeVideo)) typeStr += "video,";
            if (subTypes.contains(QContactPhoneNumber::SubTypePager)) typeStr += "pager,";
            if (subTypes.contains(QContactPhoneNumber::SubTypeModem)) typeStr += "modem,";
            if (subTypes.contains(QContactPhoneNumber::SubTypeBulletinBoardSystem)) typeStr += "bbs,";
            if (subTypes.contains(QContactPhoneNumber::SubTypeMessagingCapable)) typeStr += "msg,";
            if (subTypes.contains(QContactPhoneNumber::SubTypeLandline) || typeStr.isEmpty()) typeStr += "voice,";
            if (contexts.contains(QContactDetail::ContextHome)) typeStr = "home," + typeStr;
            if (contexts.contains(QContactDetail::ContextWork)) typeStr = "work," + typeStr;
            if (contexts.contains(QContactDetail::ContextOther)) typeStr = "other," + typeStr;
            if (detailIsPreferredForAnything(contact, det)) typeStr += "pref,";
            typeStr.chop(1);
            entry += typeStr + ":" + det.value(QContactPhoneNumber::FieldNumber);
            vcard << entry;
            vcardFieldsWithValues << "TEL";
            customVcardFields << convertDetail(contact, det, "TEL");
        } else if (definitionName == QContactAvatar::DefinitionName) {
            // only one photo field is allowed per vcard, according to the spec
            if (vcardFieldsWithValues.contains("PHOTO")) {
                // vcard already contains a photo field; save this one as a custom field only.
                customVcardFields << convertDetail(contact, det);
            } else {
                // vcard does not yet contain a photo field; save this as the photo field.
                vcardFieldsWithValues << "PHOTO";
                entry = "PHOTO;VALUE=uri:" + det.value(QContactAvatar::FieldAvatar);
                customVcardFields << convertDetail(contact, det, "PHOTO");
            }
        } else if (definitionName == QContactGuid::DefinitionName) {
            // only one UID field is allowed per vcard, according to the spec
            if (vcardFieldsWithValues.contains("UID")) {
                // vcard already contains a photo field; save this one as a custom field only.
                customVcardFields << convertDetail(contact, det);
            } else {
                // vcard does not yet contain a UID field; save this as the UID field.
                vcardFieldsWithValues << "UID";
                entry = "UID:" + det.value(QContactGuid::FieldGuid);
                customVcardFields << convertDetail(contact, det, "UID");
            }
        } else {
            // there is no vcard field mapping for this detail;
            // so, simply save it as a custom field.
            customVcardFields << convertDetail(contact, det); // doesn't map to any vcard field
        }
    }

    // now append our custom vcard fields and end the vcard.
    vcard << customVcardFields;
    vcard << "END:VCARD";

    return vcard;
}
