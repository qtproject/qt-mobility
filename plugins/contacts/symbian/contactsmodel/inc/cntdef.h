/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#ifndef __CNTDEF_H__
#define __CNTDEF_H__

#include <e32base.h>

#if !( defined __SYMBIAN_CNTMODEL_HIDE_DBMS__ )
#include <d32dbms.h>
#else
#include <s32std.h>
#include <f32file.h>
#endif

#include <cntdef.hrh>


// Constants

/**
@publishedAll 
@released
*/
const TInt KSpeedDialPhoneLength = 64;

/**
@internalTechnology
@released
*/
const TInt KCntMinSpeedDialIndex = 1;

/**
@internalTechnology
@released
*/
const TInt KCntMaxSpeedDialIndex = 9;

/**
Phone Match Length
@internalTechnology
@released
*/
const TInt KMaxPhoneMatchLength = 15;

/**
@internalTechnology
@released
*/
const TInt KLowerSevenDigits = 7;

/**
Used for firstname, lastname, companyname, secondname and their
associated pronunciation fields and e-mail address
@internalTechnology
@released
*/
const TInt KCntMaxTextFieldLength=255;

// Type definitions

/**
@internalTechnology
@released
*/
typedef TBuf<KSpeedDialPhoneLength> TSpeedDialPhoneNumber;

/** Contact item ID. These are used to uniquely identify contact items within a
contacts database. Contact items include contact cards, contact card templates,
contact card groups and own cards. Contact item IDs are also used to uniquely
identify agents.
@publishedAll
@released
*/
typedef TInt32 TContactItemId;

/** A UID which identifies a contact item field's type. The possible values are
defined as UIDs in cntdef.h.
@publishedAll
@released
*/
typedef TUid TFieldType;

/** A contact item field's storage type.

This is an unsigned integer value which identifies the type of data stored
in a contact item field. The possible values are defined in cntdef.hrh as:

- KStorageTypeText (used by CContactTextFields)

- KStorageTypeStore (used by CContactStoreFields)

- KStorageTypeContactItemId (used by CContactAgentFields)

- KStorageTypeDateTime (used by CContactDateFields)

Note that numeric fields are not supported. Numbers (e.g. telephone
numbers) are stored in the database using text fields.
@publishedAll
@released
*/
typedef TUint TStorageType;

/** Machine identifier.

This is used when synchronising the contact database to identify the
machine with which the database has been synchronised.
@see CContactDatabase::SetLastSyncDateL()
@see CContactDatabase::GetLastSyncDateL().
@publishedAll
@released
*/
typedef TInt32 TContactSyncId;

/** Stores the value of KUidContactsDbFile as an integer.
@publishedAll
@released
*/
const TInt KUidContactsDbFileValue=0x100065FF;
/**
The third UID of the contacts database file, c:\\system\\data\\contacts.cdb.
This should be used by file recogniser implementations.
@publishedAll
@released
*/
const TUid KUidContactsDbFile={KUidContactsDbFileValue};


/** NULL contact item ID. Indicates that no contact item is present.
@publishedAll
@released
*/
const TContactItemId KNullContactId=-1;
/**
The ID of the system template.
@publishedAll
@released
*/
const TContactItemId KGoldenTemplateId=0;

/** Identifies a vCard that has been pasted from the clipboard into the Contacts application.
@publishedAll
@released
*/
const TUid KClipboardUidTypeVCard={0x100038f6};

/** Identifies a contact card (CContactCard).
@publishedAll
@released
*/
const TUid KUidContactCard={KUidContactCardValue};
/** Identifies a contact group (CContactGroup).
@publishedAll
@released
*/
const TUid KUidContactGroup={KUidContactGroupValue};
/** Identifies the system template (CContactTemplate).
@publishedAll
@released
*/
const TUid KUidContactTemplate={KUidContactTemplateValue};
/** Identifies an own card (CContactOwnCard).
@publishedAll
@released
*/
const TUid KUidContactOwnCard={KUidContactOwnCardValue}; 
/** Identifies a non-system template (CContactCardTemplate). 
@publishedAll
@released
*/
const TUid KUidContactCardTemplate={KUidContactCardTemplateValue};
/** The UID that identifies ICC entries. These are a special type of
contact item that are stored in the SIM (ICC) card on the phone.

@publishedAll
@released
*/
const TUid KUidContactICCEntry={KUidContactICCEntryValue};

/** Identifies any CContactItem-derived class (all of the above).
@publishedAll
@released
*/
const TUid KUidContactItem={0x10005673};
/** This identifies the CContactCard and CContactGroup classes.
It is used internally by the contacts model.
@publishedAll
@released
*/
const TUid KUidContactCardOrGroup={0x10005F71};


/*
 * Speed-dial UIDs.
 * These are used to indicate the speed-dial position for a particular field.
 * They are added to the content type of a phone number field set as a
 * speed-dial using CContactDatabase::SetFieldAsSpeedDialL().
 */
/** Field maps to the first speed dial position.
@publishedAll
@released
*/
const TUid KUidSpeedDialOne={KUidSpeedDialOneValue};
/** Field maps to the second speed dial position.
@publishedAll
@released
*/
const TUid KUidSpeedDialTwo={KUidSpeedDialTwoValue};
/** Field maps to the third speed dial position.
@publishedAll
@released
*/
const TUid KUidSpeedDialThree={KUidSpeedDialThreeValue};
/** Field maps to the fourth speed dial position.
@publishedAll
@released
*/
const TUid KUidSpeedDialFour={KUidSpeedDialFourValue};
/** Field maps to the fifth speed dial position.
@publishedAll
@released
*/
const TUid KUidSpeedDialFive={KUidSpeedDialFiveValue};
/** Field maps to the sixth speed dial position.
@publishedAll
@released
*/
const TUid KUidSpeedDialSix={KUidSpeedDialSixValue};
/** Field maps to the seventh speed dial position.
@publishedAll
@released
*/
const TUid KUidSpeedDialSeven={KUidSpeedDialSevenValue};
/** Field maps to the eighth speed dial position.
@publishedAll
@released
*/
const TUid KUidSpeedDialEight={KUidSpeedDialEightValue};
/** Field maps to the ninth speed dial position.
@publishedAll
@released
*/
const TUid KUidSpeedDialNine={KUidSpeedDialNineValue};

/*
 * Contact field type UIDs.
 */
/** Address field.
@publishedAll
@released
*/
const TUid KUidContactFieldAddress={KUidContactFieldAddressValue};
/** Post office box field.
@publishedAll
@released
*/
const TUid KUidContactFieldPostOffice={KUidContactFieldPostOfficeValue};
/** Extended address field.
@publishedAll
@released
*/
const TUid KUidContactFieldExtendedAddress={KUidContactFieldExtendedAddressValue};
/** Locality field.
@publishedAll
@released
*/
const TUid KUidContactFieldLocality={KUidContactFieldLocalityValue};
/** Region field.
@publishedAll
@released
*/
const TUid KUidContactFieldRegion={KUidContactFieldRegionValue};
/** Post code field.
@publishedAll
@released
*/
const TUid KUidContactFieldPostcode={KUidContactFieldPostCodeValue};
/** Country field.
@publishedAll
@released
*/
const TUid KUidContactFieldCountry={KUidContactFieldCountryValue};

/** Company name field.
@publishedAll
@released
*/
const TUid KUidContactFieldCompanyName={KUidContactFieldCompanyNameValue};
/** Company name pronunciation field.
@publishedAll
@released
*/
const TUid KUidContactFieldCompanyNamePronunciation={KUidContactFieldCompanyNamePronunciationValue};
/** Phone number field.
@publishedAll
@released
*/
const TUid KUidContactFieldPhoneNumber={KUidContactFieldPhoneNumberValue};
/** Given name field.
@publishedAll
@released
*/
const TUid KUidContactFieldGivenName={KUidContactFieldGivenNameValue};
/** Family name field.
@publishedAll
@released
*/
const TUid KUidContactFieldFamilyName={KUidContactFieldFamilyNameValue};
/** Given name pronunciation field.
@publishedAll
@released
*/
const TUid KUidContactFieldGivenNamePronunciation={KUidContactFieldGivenNamePronunciationValue};
/** Family name pronunciation field.
@publishedAll
@released
*/
const TUid KUidContactFieldFamilyNamePronunciation={KUidContactFieldFamilyNamePronunciationValue};
/** Middle name field.
@publishedAll
@released
*/
const TUid KUidContactFieldAdditionalName={KUidContactFieldAdditionalNameValue};
/** Name suffix field.
@publishedAll
@released
*/
const TUid KUidContactFieldSuffixName={KUidContactFieldSuffixNameValue};
/** Name prefix field.
@publishedAll
@released
*/
const TUid KUidContactFieldPrefixName={KUidContactFieldPrefixNameValue};
/** Hidden field.
@publishedAll
@released
*/
const TUid KUidContactFieldHidden={KUidContactFieldHiddenValue};
/** Hidden field.
@publishedAll
@released
*/
const TUid KUidContactFieldDefinedText={KUidContactFieldDefinedTextValue};
/** Email address field.
@publishedAll
@released
*/
const TUid KUidContactFieldEMail={KUidContactFieldEMailValue};
/** Telephone number used for a messaging service.
@publishedAll
@released
*/
const TUid KUidContactFieldMsg={KUidContactFieldMsgValue};
/** Telephone number used for SMS messages.
@publishedAll
@released
*/
const TUid KUidContactFieldSms={KUidContactFieldSmsValue};
/** Fax number field.
@publishedAll
@released
*/
const TUid KUidContactFieldFax={KUidContactFieldFaxValue};
/** Comment field.
@publishedAll
@released
*/
const TUid KUidContactFieldNote={KUidContactFieldNoteValue};
/** Comment field.
@publishedAll
@released
*/
const TUid KUidContactStorageInline={KUidContactFieldStorageInlineValue};
/** Birthday field.
@publishedAll
@released
*/
const TUid KUidContactFieldBirthday={KUidContactFieldBirthdayValue};
/** URL field.
@publishedAll
@released
*/
const TUid KUidContactFieldUrl={KUidContactFieldUrlValue};
/** Template label field (a label which is used to refer to a template, for instance "work
template", "personal template").
@publishedAll
@released
*/
const TUid KUidContactFieldTemplateLabel={KUidContactFieldTemplateLabelValue};
/** A picture field, for instance a logo or a photo.
@publishedAll
@released
*/
const TUid KUidContactFieldPicture={KUidContactFieldPictureValue};
/** Used internally by the contacts model.
@publishedAll
@released
*/
const TUid KUidContactFieldDTMF={KUidContactFieldDTMFValue};
/** Identifies a ring tone field. This is a ring tone that is associated with a contact item.
@publishedAll
@released
*/
const TUid KUidContactFieldRingTone={KUidContactFieldRingToneValue};
/** Identifies a job title field.
@publishedAll
@released
*/
const TUid KUidContactFieldJobTitle={KUidContactFieldJobTitleValue};
/** Identifies an instant messaging address field.
@publishedAll
@released
*/
const TUid KUidContactFieldIMAddress = { KUidContactFieldIMAddressValue };

/** Identifies an IM or SIP Service Provider field.
*/
const TUid KUidContactFieldServiceProvider = { KUidContactFieldServiceProviderValue };

/** Identifies an presence information about contact.
*/
const TUid KUidContactFieldPresence = { KUidContactFieldPresenceValue };

/** Identifies status message field.
*/
const TUid KUidContactFieldStatusMsg = { KUidContactFieldStatusMsgValue };


/** Identifies a USIM second name field. This field is provided to
store an additional representation of the contact's name, such as a
nickname or a different representation. An example could be a Japanese
contact which has a romanised name and an alternative representation using
kanji (pictogram) characters.
@publishedAll
@released
*/
const TUid KUidContactFieldSecondName = { KUidContactFieldSecondNameValue };
/** Identifies a SIP identity field.
@publishedAll
@released
*/
const TUid KUidContactFieldSIPID = { KUidContactFieldSIPIDValue };
/** Identifies a Assistant field.
@publishedAll
@released
*/
const TUid KUidContactFieldAssistant = { KUidContactFieldAssistantValue };
/** Identifies an Anniversary field.
@publishedAll
@released
*/
const TUid KUidContactFieldAnniversary = { KUidContactFieldAnniversaryValue };
/** Identifies a Spouse field.
@publishedAll
@released
*/
const TUid KUidContactFieldSpouse = { KUidContactFieldSpouseValue };
/** Identifies a Children field.
@publishedAll
@released
*/
const TUid KUidContactFieldChildren = { KUidContactFieldChildrenValue };
/** Identifies a Class field.
@publishedAll
@released
*/
const TUid KUidContactFieldClass = { KUidContactFieldClassValue };
/** Identifies a Department field.
@publishedAll
@released
*/
const TUid KUidContactFieldDepartmentName = { KUidContactFieldDepartmentNameValue };
/*
* CContactICCEntry (Telephony specific) UIDs.
 */
/** Identifies an ICC slot contact field in the contact item.
@publishedAll
@released
*/
const TUid KUidContactFieldICCSlot={KUidContactFieldICCSlotValue};
/** Identifies an ICC Phonebook contact field in the contact item.
@publishedAll
@released
*/
const TUid KUidContactFieldICCPhonebook={KUidContactFieldICCPhonebookValue};
/** Identifies an ICC Group contact field in the contact item.
@publishedAll
@released
*/
const TUid KUidContactFieldICCGroup={KUidContactFieldICCGroupValue};
/** Identifies a voice dial field. This is a voice recording associated with a telephone number
field in the item.
@publishedAll
@released
*/
const TUid KUidContactsVoiceDialField={KUidContactsVoiceDialFieldValue};
/** Identifies a Geo field in the contact item. 
@publishedAll
@released
*/
const TUid KUidContactFieldGEO={KUidContactFieldGEOValue};
/** Indicates no field present.
@publishedAll
@released
*/
const TUid KUidContactFieldNone={KUidContactFieldNoneValue};
/** Used in field type matching to indicate that all field types match.
@publishedAll
@released
*/
const TInt32 KUidContactFieldMatchAllValue=0x110FFF22;
/** Used in field type matching to indicate that all field types match.
@publishedAll
@released
*/
const TUid KUidContactFieldMatchAll={KUidContactFieldMatchAllValue};

/*
 * Contact field type vCard mappings.
 * The vCard mapping describes how the field should be handled by the
 * vCard import/export code.
 */
/** Field type maps to the Post office box field in an ADR vCard property value.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapPOSTOFFICE={KIntContactFieldVCardMapPOSTOFFICE};
/** Field type maps to the Extended address field in an ADR vCard property value.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapEXTENDEDADR={KIntContactFieldVCardMapEXTENDEDADR};
/** Field type maps to vCard property ADR.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapADR={KIntContactFieldVCardMapADR};
/** Field type maps to the Locality field in an ADR vCard property value.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapLOCALITY={KIntContactFieldVCardMapLOCALITY};
/** Field type maps to the Region field in an ADR vCard property value.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapREGION={KIntContactFieldVCardMapREGION};
/** Field type maps to the Postcode field in an ADR vCard property value.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapPOSTCODE={KIntContactFieldVCardMapPOSTCODE};
/** Field type maps to the Country field in an ADR vCard property value.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapCOUNTRY={KIntContactFieldVCardMapCOUNTRY};

/** Field type maps to vCard property AGENT.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapAGENT={KIntContactFieldVCardMapAGENT};
/** Field type maps to vCard property BDAY.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapBDAY={KIntContactFieldVCardMapBDAY};
/** Field type maps to vCard property EMAIL.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapEMAILINTERNET={KIntContactFieldVCardMapEMAILINTERNET};
/** Field type maps to vCard property GEO.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapGEO={KIntContactFieldVCardMapGEO};
/** Field type maps to vCard property LABEL.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapLABEL={KIntContactFieldVCardMapLABEL};
/** Field type maps to vCard property LOGO.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapLOGO={KIntContactFieldVCardMapLOGO};
/** Field type maps to vCard property MAILER.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapMAILER={KIntContactFieldVCardMapMAILER};
/** Field type maps to vCard property NOTE.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapNOTE={KIntContactFieldVCardMapNOTE};
/** Field type maps to vCard property ORG.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapORG={KIntContactFieldVCardMapORG};
/** Field type maps to vCard X-IRMC-ORG parameter of property SOUND.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapORGPronunciation={KIntContactFieldVCardMapORGPronunciation};
/** Field type maps to vCard property PHOTO.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapPHOTO={KIntContactFieldVCardMapPHOTO};
/** Field type maps to vCard property ROLE.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapROLE={KIntContactFieldVCardMapROLE};
/** Field type maps to vCard property SOUND.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapSOUND={KIntContactFieldVCardMapSOUND};
/** Field type maps to vCard property TEL.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapTEL={KIntContactFieldVCardMapTEL};
/** Field type maps to vCard property parameter FAX.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapTELFAX={KIntContactFieldVCardMapTELFAX};
/** Field type maps to vCard property TITLE.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapTITLE={KIntContactFieldVCardMapTITLE};
/** Field type maps to vCard property URL.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapURL={KIntContactFieldVCardMapURL};
/** Field maps to the vCard property N (name). Must be used in conjunction with
a name-related field type (e.g. KUidContactFieldGivenName) to form the given
name field mapping.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapUnusedN={KIntContactFieldVCardMapUnusedN};
/** Field type maps to vCard property FN (the display name).
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapUnusedFN={KIntContactFieldVCardMapUnusedFN};
/** Mapping between the vCard property and field type is not required.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapNotRequired={KIntContactFieldVCardMapNotRequired};
/** Unknown mapping between a field type and a vCard extension property.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapUnknownXDash={KIntContactFieldVCardMapUnknownXDash};
/** Unknown mapping between field type and non-extension vCard property.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapUnknown={KIntContactFieldVCardMapUnknown};
/** Field type maps to vCard property UID.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapUID={KIntContactFieldVCardMapUID};
/** Field type maps to vCard property parameter WORK.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapWORK={KIntContactFieldVCardMapWORK};
/** Field type maps to vCard property parameter HOME.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapHOME={KIntContactFieldVCardMapHOME};
/** Field type maps to vCard property parameter MSG.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapMSG={KIntContactFieldVCardMapMSG};
/** Field type maps to vCard property parameter VOICE.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapVOICE={KIntContactFieldVCardMapVOICE};
/** Field type maps to vCard property parameter FAX.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapFAX={KIntContactFieldVCardMapFAX};
/** Field type maps to vCard property parameter PREF.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapPREF={KIntContactFieldVCardMapPREF};
/** Field type maps to vCard property parameter CELL.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapCELL={KIntContactFieldVCardMapCELL};
/** Field type maps to vCard property parameter INTERNET.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapINTERNET={KIntContactFieldVCardMapINTERNET};
/** Field type maps to vCard property parameter PAGER.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapPAGER={KIntContactFieldVCardMapPAGER};
/** Field type maps to vCard property parameter BBS.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapBBS={KIntContactFieldVCardMapBBS};
/** Field type maps to vCard property parameter MODEM.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapMODEM={KIntContactFieldVCardMapMODEM};
/** Field type maps to vCard property parameter CAR.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapCAR={KIntContactFieldVCardMapCAR};
/** Field type maps to vCard property parameter ISDN.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapISDN={KIntContactFieldVCardMapISDN};
/** Field type maps to vCard property parameter VIDEO.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapVIDEO={KIntContactFieldVCardMapVIDEO};
/** Field type maps to vCard property parameter DOM.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapDOM={KIntContactFieldVCardMapDOM};
/** Field type maps to vCard property parameter INTL.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapINTL={KIntContactFieldVCardMapINTL};
/** Field type maps to vCard property parameter POSTAL.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapPOSTAL={KIntContactFieldVCardMapPOSTAL};
/** Field type maps to vCard property parameter PARCEL.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapPARCEL={KIntContactFieldVCardMapPARCEL};
/** Field type maps to vCard property parameter value GIF.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapGIF={KIntContactFieldVCardMapGIF};
/** Field type maps to vCard property parameter value CGM.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapCGM={KIntContactFieldVCardMapCGM};
/** Field type maps to vCard property parameter value WMF.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapWMF={KIntContactFieldVCardMapWMF};
/** Field type maps to vCard property parameter value BMP.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapBMP={KIntContactFieldVCardMapBMP};
/** Field type maps to vCard property parameter value MET.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapMET={KIntContactFieldVCardMapMET};
/** Field type maps to vCard property parameter value PMB.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapPMB={KIntContactFieldVCardMapPMB};
/** Field type maps to vCard property parameter value DIB.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapDIB={KIntContactFieldVCardMapDIB};
/** Field type maps to vCard property parameter value PICT.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapPICT={KIntContactFieldVCardMapPICT};
/** Field type maps to vCard property parameter value TIFF.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapTIFF={KIntContactFieldVCardMapTIFF};
/** Field type maps to vCard property parameter value PDF.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapPDF={KIntContactFieldVCardMapPDF};
/** Field type maps to vCard property parameter value PS.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapPS={KIntContactFieldVCardMapPS};
/** Field type maps to vCard property parameter value JPEG.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapJPEG={KIntContactFieldVCardMapJPEG};
/** Field type maps to vCard property parameter value MPEG.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapMPEG={KIntContactFieldVCardMapMPEG};
/** Field type maps to vCard property parameter value MPEG2.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapMPEG2={KIntContactFieldVCardMapMPEG2};
/** Field type maps to vCard property parameter value AVI.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapAVI={KIntContactFieldVCardMapAVI};
/** Field type maps to vCard property parameter value QTIME.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapQTIME={KIntContactFieldVCardMapQTIME};
/** Field type maps to vCard property TZ.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapTZ={KIntContactFieldVCardMapTZ};
/** Field type maps to vCard property KEY.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapKEY={KIntContactFieldVCardMapKEY};
/** Field type maps to vCard property parameter value X509.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapX509={KIntContactFieldVCardMapX509};
/** Field type maps to vCard property parameter value PGP.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapPGP={KIntContactFieldVCardMapPGP};
/** Used internally by the contacts model.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapSMIME={KIntContactFieldVCardMapSMIME};
/** The field contains a Wireless Village instant messaging ID.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapWV={KIntContactFieldVCardMapWV};
/** Field type mapping of a vCard property to contacts Second Name Field
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapSECONDNAME={KIntContactFieldVCardMapSECONDNAME};
/** Field type mapping of a vCard property to contacts SIP Identity Field.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapSIPID={KIntContactFieldVCardMapSIPID};
/** Field type maps to vCard extension property parameter value POC
(Push to Talk Over Cellular).
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapPOC={KIntContactFieldVCardMapPOC};
/** Field type maps to vCard extension property parameter value SWIS
("See What I See").
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapSWIS={KIntContactFieldVCardMapSWIS};
/** Field type maps to vCard extension property parameter value VOIP
(Voice Over IP).
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapVOIP={KIntContactFieldVCardMapVOIP};
/** Field type maps to vCard extension property parameter value Assistant
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapAssistant={KIntContactFieldVCardMapAssistant};
/** Field type maps to vCard extension property parameter value AssistantTel
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapAssistantTel={KIntContactFieldVCardMapAssistantTel};
/** Field type maps to vCard extension property parameter value Anniversary
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapAnniversary={KIntContactFieldVCardMapAnniversary};
/** Field type maps to vCard extension property parameter value Spouse
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapSpouse={KIntContactFieldVCardMapSpouse};
/** Field type maps to vCard extension property parameter value Children
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapChildren={KIntContactFieldVCardMapChildren};
/** Field type maps to vCard extension property parameter value Class
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapClass={KIntContactFieldVCardMapClass};
/** Field type maps to vCard extension property parameter value Department
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapDepartment={KIntContactFieldVCardMapDepartment};


/** Favourites ranking field.
@publishedAll
@released
*/
const TUid KUidContactFieldFavourite={KUidContactFieldFavouriteValue};

/** Field type maps to vCard extension property parameter value Favourite.
@publishedAll
@released
*/
const TUid KUidContactFieldVCardMapFavourite={KIntContactFieldVCardMapFavourite};


/** Name of the TYPE property parameter, for which the values are work, home etc. 
@publishedAll
@released
*/
_LIT(KVersitParamType,"TYPE");
/** Name of the WORK property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamWork,"WORK");
/** Name of the HOME property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamHome,"HOME");
/** Name of the MSG property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamMsg,"MSG");
/** Name of the VOICE property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamVoice,"VOICE");
/** Name of the FAX property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamFax,"FAX");
/** Name of the PREF property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamPref,"PREF");
/** Name of the CELL property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamCell,"CELL");
/** Name of the PAGER property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamPager,"PAGER");
/** Name of the BBS property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamBbs,"BBS");
/** Name of the MODEM property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamModem,"MODEM");
/** Name of the CAR property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamCar,"CAR");
/** Name of the ISDN property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamIsdn,"ISDN");
/** Name of the VIDEO property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamVideo,"VIDEO");
/** Name of the DOM property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamDom,"DOM");
/** Name of the GIF property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamGif,"GIF");
/** Name of the CGM property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamCgm,"CGM");
/** Name of the WMF property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamWmf,"WMF");
/** Name of the BMP property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamBmp,"BMP");
/** Name of the MET property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamMet,"MET");
/** Name of the PMB property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamPmb,"PMB");
/** Name of the DIB property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamDib,"DIB");
/** Name of the PICT property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamPict,"PICT");
/** Name of the TIFF property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamTiff,"TIFF");
/** Name of the PDF property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamPdf,"PDF");
/** Name of the PS property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamPs,"PS");
/** Name of the JPEG property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamJpeg,"JPEG");
/** Name of the MPEG property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamMpeg,"MPEG");
/** Name of the MPEG2 property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamMpeg2,"MPEG2");
/** Name of the AVI property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamAvi,"AVI");
/** Name of the QTIME property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamQtime,"QTIME");
/** Name of the X509 property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamX509,"X509");
/** Name of the PGP property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamPGP,"PGP");
/** Name of the INTERNET property parameter.
@publishedAll
@released
*/
_LIT(KVersitParamINTERNET,"INTERNET");

/** 8 bit name of the TYPE property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8Type,"TYPE");
/** 8 bit name of the WORK property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8Work,"WORK");
/** 8 bit name of the HOME property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8Home,"HOME");
/** 8 bit name of the MSG property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8Msg,"MSG");
/** 8 bit name of the VOICE property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8Voice,"VOICE");
/** 8 bit name of the FAX property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8Fax,"FAX");
/** 8 bit name of the PREF property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8Pref,"PREF");
/** 8 bit name of the CELL property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8Cell,"CELL");
/** 8 bit name of the PAGER property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8Pager,"PAGER");
/** 8 bit name of the BBS property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8Bbs,"BBS");
/** 8 bit name of the MODEM property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8Modem,"MODEM");
/** 8 bit name of the CAR property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8Car,"CAR");
/** 8 bit name of the ISDN property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8Isdn,"ISDN");
/** 8 bit name of the VIDEO property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8Video,"VIDEO");
/** 8 bit name of the DOM property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8Dom,"DOM");
/** 8 bit name of the GIF property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8Gif,"GIF");
/** 8 bit name of the CGM property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8Cgm,"CGM");
/** 8 bit name of the WMF property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8Wmf,"WMF");
/** 8 bit name of the BMP property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8Bmp,"BMP");
/** 8 bit name of the MET property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8Met,"MET");
/** 8 bit name of the PMB property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8Pmb,"PMB");
/** 8 bit name of the DIB property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8Dib,"DIB");
/** 8 bit name of the PICT property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8Pict,"PICT");
/** 8 bit name of the TIFF property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8Tiff,"TIFF");
/** 8 bit name of the PDF property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8Pdf,"PDF");
/** 8 bit name of the PS property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8Ps,"PS");
/** 8 bit name of the JPEG property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8Jpeg,"JPEG");
/** 8 bit name of the MPEG property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8Mpeg,"MPEG");
/** 8 bit name of the MPEG2 property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8Mpeg2,"MPEG2");
/** 8 bit name of the AVI property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8Avi,"AVI");
/** 8 bit name of the QTIME property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8Qtime,"QTIME");
/** 8 bit name of the X509 property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8X509,"X509");
/** 8 bit name of the PGP property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8PGP,"PGP");
/** 8 bit name of the INTERNET property parameter.
@publishedAll
@released
*/
_LIT8(KVersitParam8Internet,"INTERNET");

/**
8 bit name of the X-IRMC-N property parameter (family or given
name pronunciation).
@publishedAll
@released
*/
_LIT8(KVersitParam8NamePrn,"X-IRMC-N");
/**
8 bit name of the X-IRMC-ORG property parameter (company
name pronunciation).
@publishedAll
@released
*/
_LIT8(KVersitParam8CompanyPrn,"X-IRMC-ORG");
/**
8 bit name of the X-IRMC- property parameter (pronunciation field prefix).
@publishedAll
@released
*/
_LIT8(KVersitParam8PronunciationPrefix,"X-IRMC-");



class CContactIdArray : public CBase
/** Array of contact item IDs (TContactItemIds).

Instances of this class are used in several contact database functions,
for instance CContactDatabase::DeleteContactsL().
@publishedAll
@released
*/
	{
public:
	IMPORT_C static CContactIdArray* NewL();
	IMPORT_C static CContactIdArray* NewLC();
	IMPORT_C static CContactIdArray* NewL(const CContactIdArray* aArray);
	IMPORT_C static CContactIdArray* NewLC(const CContactIdArray* aArray);
	static CContactIdArray* NewLC(RReadStream& aStream);
	IMPORT_C ~CContactIdArray();
	inline const TContactItemId& operator[](TInt aIndex) const;
	inline TContactItemId& operator[](TInt aIndex);
	inline TInt Count() const;
	inline void Reset();
	IMPORT_C TInt Find(TContactItemId aId) const;
	IMPORT_C void AddL(TContactItemId aId);

	inline void Remove(TInt aIndex);
	inline void Remove(TInt aIndex,TInt aCount);
	inline void InsertL(TInt aIndex,TContactItemId aId);
	IMPORT_C void MoveL(TInt aOldIndex,TInt aNewIndex);
	IMPORT_C void ReverseOrder();
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
public:  // intended for internal Contacts Model usage only
	void Sort();
private:
	CContactIdArray();
	void ConstructL();
	void CloneL(const CContactIdArray* aArray);
private:
	CArrayFixFlat<TContactItemId> *iIds;
	};
inline const TContactItemId& CContactIdArray::operator[](TInt aIndex) const
/** Gets the indexed TContactItemId.

@param aIndex The position of the contact ID within the array, relative to zero.
This must be non-negative and less than the number of objects in the array
otherwise the operator raises a panic.
@return A reference to a const element of the array. */
	{ return (*iIds)[aIndex]; }

inline TContactItemId& CContactIdArray::operator[](TInt aIndex)
/** Gets the indexed TContactItemId.

@param aIndex The position of the contact ID within the array, relative to zero.
This must be non-negative and less than the number of objects in the array
otherwise the operator raises a panic.
@return A reference to a non-const element of the array. */
	{ return (*iIds)[aIndex]; }

inline TInt CContactIdArray::Count() const
/** Gets the number of contact IDs in the array.

@return The number of contact IDs in the array. */
	{ return iIds->Count(); }

inline void CContactIdArray::Reset()
/** Removes all contact IDs from the array. */
	{ iIds->Reset(); }

inline void CContactIdArray::Remove(TInt aIndex)
/** Removes the indexed contact ID from the array.

The index value must not be negative and must not be greater than the number
of elements in the array, otherwise the function raises a panic.

@param aIndex The index of the contact ID to remove. */
	{ iIds->Delete(aIndex); }

inline void CContactIdArray::Remove(TInt aIndex,TInt aCount)
/** Removes a block of contact IDs from the array.

This function raises a panic if any of the following are true:-

- aCount is negative

- aIndex is negative or is greater than the number of elements currently in
the array

- the sum of aIndex and aCount is greater than the number of elements currently
in the array

@param aIndex The index of the first contact ID to remove.
@param aCount The number of contiguous contact IDs to delete from the array.
If this is not specified, a value of one is assumed. */
	{ iIds->Delete(aIndex,aCount); }

inline void CContactIdArray::InsertL(TInt aIndex,TContactItemId aId)
/** Inserts a contact ID into the array.

The index must be valid or a panic occurs.

The function may attempt to expand the array buffer. If there is insufficient
memory available, the function leaves. The leave code is one of the system
error codes. If the function leaves, the array is left in the state it was
in before the call.

@param aIndex The index at which to insert the contact ID.
@param aId The contact ID to insert. */
	{ iIds->InsertL(aIndex,aId); }
	

/** Shows supported event action types that are used when deleting
a contact or an array of contacts
@internalTechnology
@released
*/
enum TCntSendEventAction
	{
	EDeferEvent              = 0,
	ESendEvent               = 1,
	ESendUnknownChangesEvent = 2,
	};

#endif
