/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef __TESTIMPEXVCARD_SUITEDEFS_H__
#define __TESTIMPEXVCARD_SUITEDEFS_H__

#define MAX_TEXT_MESSAGE 256

/** Generic*/
#include <e32test.h>
#include <s32file.h>
#include <e32std.h>
#include <f32file.h>

// User includes
#include <cntdb.h>
#include <vcard.h>
#include <cntitem.h>
#include <cntfldst.h>
#include "cntvcardutils.h"

_LIT(KOOM, 	"OOM");
const TInt KMaxLengthField = 100;
/** Alarm Server*/
_LIT(KConsoleAlarmAlertServerImg,"ConsoleAlarmAlertServer");

/** Error Statements - CTestImpRevLocalTZ */
_LIT(KErrOpenDB,"OpenDBL returned error code = %d");

/**  TestImpExvCardSuite */
_LIT(KOpenVCFFile, "Opened vCard VCF file %S");
_LIT(KExpectedUTC, "ExpectedUTC");
_LIT(KExpectedBDay, "ExpectedBDay");

/** CTestImpRevLocalTZ */
_LIT(KImportRevLocalTZ, "ImportRevLocalTZ");
_LIT(KPathImportRevLocalTZ, "\\CITExImvCard\\testdata\\ImportREVLocalTZ\\ImportREVLocalTZ");

/** CTestImpRevLocal */
_LIT(KImportRevLocal, "ImportRevLocal");
_LIT(KPathImportRevLocal, "\\CITExImvCard\\testdata\\ImportREVLocal\\ImportREVLocal");

/** CTestImpNoRev */
_LIT(KPathImportNoREv, "\\CITExImvCard\\testdata\\ImportNoRev\\ImportNoRev");

/** CTestImpRevUTC */
_LIT(KImportRevUTC, "ImportRevUTC");
_LIT(KPathImportRevUTC,"\\CITExImvCard\\testdata\\ImportRevUTC\\ImportRevUTC");

/** CTestImpBDay */
_LIT(KImportBDay, "ImportBDay");
_LIT(KPathImportBDay,"\\CITExImvCard\\testdata\\ImportBDay\\ImportBDay");

/** CTestImpNoBDay */
_LIT(KPathImportNoBDay, "\\CITExImvCard\\testdata\\ImportNoBDay\\ImportNoBDay");

/** CTestImpOOM */
_LIT(KImportOOM, "\\CITExImvCard\\testdata\\ImportOOM\\ImportOOM.vcf");

/** CTestExRevUTC */
_LIT(KExportRevUTCFile,"C:\\CITExImvCard\\ExportRevUTCFile.vcf");

/** CTestExBDayLocal */
_LIT(KExportBDayFile,"C:\\CITExImvCard\\ExportBDayLocalFile.vcf");
_LIT(KExportNoBDayFile,"C:\\CITExImvCard\\ExportNoBDayFile.vcf");
_LIT(KVCard, "VCARD");
_LIT(KBDayLocal, "BDAYLOCAL");
_LIT(KName, "NAME");
_LIT(KPhone, "PHONE");

/**CTestExportContact with data*/
_LIT(KExportAsstWithDataFile,			"C:\\Export\\ExportAsstWithDataFile.vcf");
_LIT(KExportAsstTelWithDataFile,		"C:\\Export\\ExportAsstTelWithDataFile.vcf");
_LIT(KExportAnniWithDataFile,			"C:\\Export\\ExportAnniWithDataFile.vcf");
_LIT(KExportSpouseWithDataFile,			"C:\\Export\\ExportSpouseWithDataFile.vcf");
_LIT(KExportChildrenWithDataFile,		"C:\\Export\\ExportChildrenWithDataFile.vcf");
_LIT(KExportClassWithDataFile,			"C:\\Export\\ExportClassWithDataFile.vcf");
_LIT(KExportDeptWithDataFile,			"C:\\Export\\ExportDeptWithDataFile.vcf");
_LIT(KExportCompanyWithDataFile,		"C:\\Export\\ExportCompanyWithDataFile.vcf");

 /**CTestExportContact without data*/
_LIT(KExportAsstWithoutDataFile,		"C:\\Export\\ExportAsstWithoutDataFile.vcf");
_LIT(KExportAsstTelWithoutDataFile,		"C:\\Export\\ExportAsstTelWithoutDataFile.vcf");
_LIT(KExportAnniWithoutDataFile,		"C:\\Export\\ExportAnniWithoutDataFile.vcf");
_LIT(KExportSpouseWithoutDataFile,		"C:\\Export\\ExportSpouseWithoutDataFile.vcf");
_LIT(KExportChildrenWithoutDataFile,	"C:\\Export\\ExportChildrenWithoutDataFile.vcf");
_LIT(KExportClassWithoutDataFile,		"C:\\Export\\ExportClassWithoutDataFile.vcf");
_LIT(KExportDeptWithoutDataFile,		"C:\\Export\\ExportDeptWithoutDataFile.vcf");
_LIT(KExportCompanyWithoutDataFile,		"C:\\Export\\ExportCompanyWithoutDataFile.vcf");

/**CTestExportContact all fields*/
_LIT(KExportAllFieldFile,		"C:\\Export\\ExportAllFieldFile.vcf");
_LIT(KExportAllWithoutDataFile, "C:\\Export\\ExportAllWithoutDataFile.vcf");

/*CTestExportContact empty dept and company fields*/
_LIT(KExportAllFieldWithEmptyCompanyFile,		"C:\\Export\\ExportAllFieldWithEmptyCompanyFile.vcf");
_LIT(KExportAllFieldWithEmptyDeptFile, 			"C:\\Export\\ExportAllFieldWithEmptyDeptFile.vcf");

/**CTestExportContact with data for new file(in Merge Mode)*/
_LIT(KExportAsstWithNewFile,			"C:\\Export\\ExportAsstWithNewFile.vcf");
_LIT(KExportAsstTelWithNewFile,			"C:\\Export\\ExportAsstTelWithNewFile.vcf");
_LIT(KExportAnniWithNewFile,			"C:\\Export\\ExportAnniWithNewFile.vcf");
_LIT(KExportSpouseWithNewFile,			"C:\\Export\\ExportSpouseWithNewFile.vcf");
_LIT(KExportChildrenWithNewFile,		"C:\\Export\\ExportChildrenWithNewFile.vcf");
_LIT(KExportClassWithNewFile,			"C:\\Export\\ExportClassWithNewFile.vcf");
_LIT(KExportDeptWithNewFile,			"C:\\Export\\ExportDeptWithNewFile.vcf");
_LIT(KExportCompanyWithNewFile,			"C:\\Export\\ExportCompanyWithNewFile.vcf");

/**CTestImportContact with field but no data used for vCard in merge mode3*/
_LIT(KImportAsstMergeMode3File,			"\\1286TestData\\ImportAsstMergeMode3File.vcf");
_LIT(KImportAsstTelMergeMode3File,		"\\1286TestData\\ImportAsstTelMergeMode3File.vcf");
_LIT(KImportAnniMergeMode3File,			"\\1286TestData\\ImportAnniMergeMode3File.vcf");
_LIT(KImportSpouseMergeMode3File,		"\\1286TestData\\ImportSpouseMergeMode3File.vcf");
_LIT(KImportChildrenMergeMode3File,		"\\1286TestData\\ImportChildrenMergeMode3File.vcf");
_LIT(KImportClassMergeMode3File,		"\\1286TestData\\ImportClassMergeMode3File.vcf");
_LIT(KImportDeptMergeMode3File,			"\\1286TestData\\ImportDeptMergeMode3File.vcf");
_LIT(KImportCompanyMergeMode3File,		"\\1286TestData\\ImportCompanyMergeMode3File.vcf");

/**CTestImportContact with field but no data used for vCard in merge mode2*/
_LIT(KImportAsstMergeMode2File,			"\\1286TestData\\ImportAsstMergeMode2File.vcf");
_LIT(KImportAsstTelMergeMode2File,		"\\1286TestData\\ImportAsstTelMergeMode2File.vcf");
_LIT(KImportAnniMergeMode2File,			"\\1286TestData\\ImportAnniMergeMode2File.vcf");
_LIT(KImportSpouseMergeMode2File,		"\\1286TestData\\ImportSpouseMergeMode2File.vcf");
_LIT(KImportChildrenMergeMode2File,		"\\1286TestData\\ImportChildrenMergeMode2File.vcf");
_LIT(KImportClassMergeMode2File,		"\\1286TestData\\ImportClassMergeMode2File.vcf");
_LIT(KImportDeptMergeMode2File,			"\\1286TestData\\ImportDeptMergeMode2File.vcf");
_LIT(KImportCompanyMergeMode2File,		"\\1286TestData\\ImportCompanyMergeMode2File.vcf");

/**CTestImportContact field with  currupt data*/
_LIT(KImportAsstCorruptData,			"\\1286TestData\\ImportAsstCorruptData.vcf");
_LIT(KImportAsstTelCorruptData,			"\\1286TestData\\ImportAsstTelCorruptData.vcf");
_LIT(KImportAnniCorruptData,			"\\1286TestData\\ImportAnniCorruptData.vcf");
_LIT(KImportSpouseCorruptData,			"\\1286TestData\\ImportSpouseCorruptData.vcf");
_LIT(KImportChildrenCorruptData,		"\\1286TestData\\ImportChildrenCorruptData.vcf");
_LIT(KImportClassCorruptData,			"\\1286TestData\\ImportClassCorruptData.vcf");
_LIT(KImportDeptCorruptData,			"\\1286TestData\\ImportDeptCorruptData.vcf");
_LIT(KImportCompanyCorruptData,			"\\1286TestData\\ImportCompanyCorruptData.vcf");


#endif
