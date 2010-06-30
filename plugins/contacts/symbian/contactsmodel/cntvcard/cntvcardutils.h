/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CNTVCARDUTILS_H__
#define __CNTVCARDUTILS_H__

#include <cntvcard.h>
#include "pbapconverter.h"

// Constants
const TUint KContactVCardSpaceCharacter = 0x20;
const TInt KContactMaxVCardPropertyNameLength=75;
const TInt KContactStandardCardValueLength=40;
_LIT(KSingleSpacePropertyValue," ");

// Literal constants
_LIT8(KContactVCardXDashEPOCCNTMODEL,"X-EPOCCNTMODEL");
_LIT(KContactVarVCardXDashEPOCCNTMODEL,"X-EPOCCNTMODEL");
_LIT(KContactVCardFIELDLABEL,"FIELDLABEL");
_LIT8(KContactVCardLABELn,"LABEL%d");
_LIT(KContactVarVCardLABELn,"LABEL%d");
_LIT(KContactVCardLABEL0 ,"LABEL0");
_LIT(KContactVCardLABEL1 ,"LABEL1");
_LIT(KContactVCardLABEL2 ,"LABEL2");
_LIT(KContactVCardLABEL3 ,"LABEL3");
_LIT(KContactVCardLABEL4 ,"LABEL4");
_LIT(KContactVCardLABEL5 ,"LABEL5");
_LIT(KContactVCardLABEL6 ,"LABEL6");
_LIT(KContactVCardFIELDHIDDEN,"FIELDHIDDEN");
_LIT(KContactVCardFIELDREADONLY,"FIELDREADONLY");
//
_LIT(KContactVCardCompositeSupportedButEmptyFieldValue, " ");
_LIT(KContactVCardCompositeUnsupportedFieldValue, "");
_LIT(KContactVCardNonCompositeEmptyFieldValue, "");
_LIT(KContactVCardEmptyFieldValue, "");
_LIT8(KVersitTokenSIPID,"X-SIP");
_LIT8(KVersitParamPOC,"POC");
_LIT8(KVersitParamSWIS,"SWIS");
_LIT8(KVersitParamVOIP,"VOIP");
_LIT8(KVersitTokenWVID,"X-WV-ID");


/**
Export converter, Converts contact items into vCard objects.
also provides support for export of contacts to PBAP client.
 */
class CContactsAppToVCardConverter : public CBase
	{
public:
	~CContactsAppToVCardConverter();
	CContactsAppToVCardConverter(TInt64 aMachineUniqueId, Versit::TVersitCharSet aCharSet, TVCardVersion aVCardVersion);
	CParserVCard* GetContactItemAsVCardL(CContactItem* aMainItem, CArrayPtr<CContactItem>* aAgentItemArray, TInt aOption, TBool aExportPrivateFields);
	void SetFilter(TInt64 aFilter);
	TBool NeedsToBeExported(TPBAPProperties aProp) const;
	TBool IsPBAPExport()const;
private:
	void EncodeItemPropertiesL(const CContactItem& aContactItem,TInt aOption);
	void EncodeFieldPropertiesL(const CContactItem& aContactItem, CArrayPtr<CContactItem>* aAgentItemArray, TInt aOption, TBool aExportPrivateFields);
 	void GetVCardNamePrnL(CContactItemFieldSet& aFields,TInt aOption,TBool aExportPrivateFields);
	void GetVCardNameL(CContactItemFieldSet& aFields,TInt aOption, TBool aExportPrivateFields);
	void GetVCardAddressL(const CContactItem& aContact, TUid aVCardMapping, const TDesC8& aToken, TInt aOption, TBool aExportPrivateFields);
	void MapContactFieldToVCardPropertyL(const CContactItemField& aField,CContactItem* aAgent,TInt aOption,TInt aDuplicateCount,TBool aExportPrivateFields);
	void MapContactMappingToVCardPropertyNameAndTypeL(const CContentType& aContactType,TDes8& aName,TUid& aPropertyUid,CArrayPtr<CParserParam>* aArrayOfParams) const;
	void RetrieveAddressFieldL(const CContactItemFieldSet& aFields, CArrayPtr<CParserParam>& aParams, CDesCArrayFlat& aDesArray, TFieldType aFieldType, TUid aMapping, const TDesC& aLabel, TInt aOption, TBool& aFound,TBool aExportPrivateFields,TBool aWithoutPref=ETrue) const;
	TBool LocateSpecificNameValueL(const CContactItemFieldSet& aFieldSet, TUid aSearchField, CDesCArray& aPropValueArray, CArrayPtr<CParserParam>& aPropParamArray, const TDesC& aLabelPrefix, TInt aOption, TBool aExportPrivateFields);
	static CParserPropertyValueDateTime* CreateDateTimePropertyL(const TTime& aDateTime, TVersitDateTime::TRelativeTime aRelativeTime);
	static CParserPropertyValueDate* CreateDatePropertyL(const TTime& aDateTime);
	void GetVCardOrgInfoL(const CContactItemFieldSet& aFields, TInt aOption, TBool aExportPrivateFields);
	void RemoveNewLineL(CDesCArray& aPropValueArray, const TPtrC& aFieldContents);
	TVCardVersion GetExportVersion()const;
	void GetVCardFormattedNameL(CContactItemFieldSet& aFields, TInt aOption, TBool aExportPrivateFields);
	TPBAPProperties AttributeFromMapping(TUid aMapping) const;
	TBool DefaultInternetParameterRequired(const CContentType& aContactType) const;
	void AddParamToArrayL(CArrayPtr<CParserParam>* aArrayOfParams, CParserParam* aParserParam, TInt32 aMappingUid, TInt32 aFieldTypeUid) const;
private: // UTILITY methods
	static TInt FindAddressFieldByMappingAndType(const CContactItemFieldSet& aFields, TFieldType aFieldType, TUid aMapping);

private:
	CParserVCard* iVCard;
	TInt64 iMachineUniqueId;
	Versit::TVersitCharSet iCharSet;
	TVCardVersion iVCardVersion;
	TInt64 iFilter;
	};


/**
 * Import and update converter
 */
class CVCardToContactsAppConverter : public CBase
	{
public:
	enum TUnknownPropertyBehaviour
		{
		EPreserveAllProperties,
		EDiscardNonXDashUnknownProperties,
		EDiscardAllUnknownProperties
		};

public:
	CContactItem* GetVCardAsContactItemLC(CParserVCard& aSourceVCard, TUnknownPropertyBehaviour aUnknownPropertyBehaviour, TInt aOption);
	void GetVCardUidStringL(CParserVCard& aSourceVCard, TDes& aUidString) const;
	TBool MergeVCardWithContactItemL(CContactItem& aContact,CParserVCard& aSourceVCard,TUnknownPropertyBehaviour aUnknownPropertyBehaviour,TInt aOption);

private:
	CContactItemField* GetVCardPropertyAsContactFieldLC(CParserProperty* aVCardProperty, TInt aOption,TBool& aUnsupportedProperty);
	static CContentType* MapVCardPropertyToContentAndStorageTypeL(const CParserProperty& aVCardProperty, TStorageType& aStorageType);
	//
	static void GetVCardModifiedTimeL(CParserVCard& aVCard,TTime& aLastModified);
	static TInt  GetVCardPropertyTTNumber(CParserProperty* aVCardProperty);
	static void DeleteField(CContactItem& aContact, const CContactItemFieldSet& aFieldSet, TFieldType aFieldType, TUid aMapping);

	static void GetContactNameL(CParserVCard& aVCard,CDesCArray* aName,TInt aOption,CDesCArray* aLabels);
	static TBool GetSpecifiedAddressL(CArrayPtr<CParserProperty>& aProperties, TInt aOption, const TDesC8& aParamMustMatch, const TDesC8& aParamMustNotMatch1, const TDesC8& aParamMustNotMatch2, CDesCArray& aAddressRepository, CDesCArray& aAddressLabelRepository);

	void MergeNameFieldsL(CContactItem& aContact, CVCardItemAndLabel& aNames, TInt aOption, TBool aTreatAsPrn = EFalse);
	void MergeSpecifiedNameFieldL(CContactItem& aContact, TFieldType aFieldType, CVCardItemAndLabel& aNames, TInt aOption, TInt aNameIndex);
	void MergeAddressFieldsL(CContactItem& aContact, const CVCardAddress& aAddress, TInt aOption);
	void MergeSpecifiedAddressFieldL(CContactItem& aContact, const CVCardAddress& aAddresses, const TUid& aFieldUid, const TUid& aMappingUid, TInt aNameIndex, TInt aOption);


private: // UTILITY methods
	TBool TextArrayContainsImportableData(const CDesCArray& aArray) const;
	static void RemoveWhitespaceFromCompositePropertyL(CDesCArray& aArray);
    static TFieldType MapVCardPhotoTypeToFieldType(TPtrC8 aBitmapStringPtr);
	inline void SetImportType(TCntVCardImportType aType) { iImportType = aType; }
	inline TCntVCardImportType ImportType() const { return iImportType; }
	HBufC* EncodeL(const TDesC& aText, TBool aTextTobeTruncated) const;
	TBool PropertyTobeTruncated(const TPtrC8& aFieldName) const;
    static void GetBitMapFormat(const CParserProperty& aVCardProperty, TFieldType& bitMapFormat);
    void AdjustForPrefRule(CContactItem& aContactItem, TFieldType aFieldType, TUid aMapping);

private: // NAME handling
	static void DeleteNameFields(CContactItem& aContact);
	static CVCardItemAndLabel* GetContactNameLC(CParserVCard& aVCard, TInt aOption);
	static CVCardItemAndLabel* GetContactNamePrnLC(CParserVCard& aVCard, TInt aOption);
	void SetNameFieldsL(const CVCardItemAndLabel& aNames, CContactItem& aContact, TInt aOption, TBool aTreatAsPronunciation = EFalse) const;
	void SetNameFieldL(const CVCardItemAndLabel& aNames, CContactItem& aContact, TInt aOption, TInt aIndex, TFieldType aFieldType, TInt& aInsertPos) const;
	static CVCardItemAndLabel* MakeNamesFromItemsL(const CDesCArray& aItems, TInt aOption, const CParserProperty* aProperty );

private: // ADDRESS handling
	static void DeleteAddressFields(CContactItem& aContact, TFieldType aFieldType);
	static void GetAddressesL(CParserVCard& aVCard, TInt aOption, RPointerArray<CVCardAddress>& aAddresses);
	static void GetHomeAndWorkAddressL(CParserVCard& aVCard, CDesCArray* aAddressHome, CDesCArray* aAddressWork, TInt aOption, CDesCArray* aAddressHomeLabels, CDesCArray* aAddressWorkLabels);
	void SetAddressFieldsL(const CVCardAddress& aAddress, CContactItem& aContact, TInt aOption) const;
	void SetAddressFieldL(const CVCardAddress& aAddress,
								 CContactItem& aContact,
								 TInt aOption,
								 TInt aIndex,
								 TFieldType aFieldType,
								 TInt& aInsertPos,
								 TUid aMapping
								 ) const;

	static CVCardAddress* GetSpecifiedAddressLC(const CArrayPtr<CParserProperty>& aProperties,
									  TInt aOption,
									  const TDesC8& aParamMustMatch,
									  const TDesC8& aParamMustNotMatch1,
									  const TDesC8& aParamMustNotMatch2,
									  TUid aMapping
									  );
									  
private: // Organization handling
TInt GetVCardPropertyAsArrayOfValuesL(const CParserVCard& aVCard, const TDesC8& aToken, CDesCArray& aItems);
void SetOrgDetailsL(CContactItem& aContact,  CDesCArray& aItems, const TInt aOption);
void SetSpecificFieldL(CContactItem& aContact,  CDesCArray& aItems, const TUid& aFieldType,const TUid& aMapping, TInt aOption, TInt aStartIndex, TInt aEndIndex);
void MergeOrgDetailsL(CContactItem& aContact,  CDesCArray& aItems, TInt aOption);
void MergeSpecificFieldL(CContactItem& aContact,  CDesCArray& aItems, const TFieldType& aFieldType, const TUid& aMapping, TInt aOption, TInt aStartIndex, TInt aEndIndex);

private: // Class handling
TInt GetSingleInstanceL(const CParserVCard& aVCard, const TDesC8& aToken, TDes& aClass);
void SetSingleInstanceL(CContactItem& aContact, const TDes& aValue,const TFieldType& aFieldType, const TUid& aMapping, TInt aOption);
void MergeSingleInstanceL(CContactItem& aContact, const TDes& aValue,const TFieldType& aFieldType, const TUid& aMapping, TInt aOption);

private: // Parameter handling
	static	TBool IsParameterValuePresent(CArrayPtr<CParserParam>* aParamArray, const TDesC8& aParamName, const TDesC8& aParamValue);

private:
	TUnknownPropertyBehaviour iUnknownPropertyBehaviour;
	TCntVCardImportType iImportType;
	};


/**
 * Generic base class representing a collection of items (e.g. names sub-fields) and their
 * associated labels
 */
class CVCardItemAndLabel : public CBase
	{
public:
	static CVCardItemAndLabel* NewLC();
	~CVCardItemAndLabel();

protected:
	CVCardItemAndLabel();
	void ConstructL();

public:
	void AddItemL(const TDesC& aItem);
	void AddLabelL(const TDesC& aItem);
	//
	TInt ItemCount() const;
	TInt LabelCount() const;
	//
	TPtrC Item(TInt aIndex) const;
	TPtrC Label(TInt aIndex) const;
	//
	TInt FindLabel(const TDesC& aName, TInt& aPosition) const;

public:
	static void CleanUpResetDestroyAndCloseArray(TAny* aArray);

private:
	CDesCArray* iItems;
	CDesCArray* iLabels;
	};


/**
 * Class used to hold address sub-fields and their associated labels.
 */
class CVCardAddress : public CVCardItemAndLabel
	{
public:
	static CVCardAddress* NewLC(TUid aMapping);

private:
	CVCardAddress(TUid aMapping);

public:
	inline TUid Mapping() const { return iMapping; }

private:
	TUid iMapping;
	};

/** Panic enumeration */
enum TCntVCardPanic
	{
	ECntVPanicAgentNotNull,
	ECntVPanicBadParamName,
	ECntVPanicInvalidImportType,
	};

GLREF_C void Panic(TCntVCardPanic aPanic);
GLREF_C void CleanUpResetAndDestroy(TAny *aArray);

#endif
