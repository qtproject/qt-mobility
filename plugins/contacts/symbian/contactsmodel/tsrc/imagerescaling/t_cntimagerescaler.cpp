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

#include <e32test.h>
#include <bautils.h>
#include <fbs.h>
#include <e32debug.h>

#include "cntimagerescaler.h"
#include "cntimagerescaleutility.h"
#include "cntdbconsts_internal.h"
#include "t_upscalerhelper.h"

_LIT(KDestDir, "c:\\data\\cnttestdata\\");
_LIT(KImageName, "_timestamp_picture.jpg");
LOCAL_C RTest test(_L("T_CntImageRescaler"));

/**
Test for the contacts image rescaler
*/

class T_CntImageRescaler : public CBase
{
public:
    static T_CntImageRescaler* NewL();
    ~T_CntImageRescaler();

    void testRescaleImageSuccefully();
    void testEmptyArgs();
    void startTest();
    void stopTest();

    void testRescaleUtility();
private:
    T_CntImageRescaler();
    void ConstructL();

private:
    CImageRescaler* iRescaler;
    RFs iFs;
};

T_CntImageRescaler* T_CntImageRescaler::NewL()
    {
    T_CntImageRescaler* self = new(ELeave) T_CntImageRescaler();
    CleanupStack::PushL( self );

    self->ConstructL();

    CleanupStack::Pop(); // self

    return self;
    }

void T_CntImageRescaler::ConstructL()
    {
    User::LeaveIfError(iFs.Connect());

    iRescaler = CImageRescaler::NewL();
    BaflUtils::EnsurePathExistsL(iFs, KDestDir());
    }

T_CntImageRescaler::T_CntImageRescaler() :
    CBase()
    {
    }

T_CntImageRescaler::~T_CntImageRescaler()
    {
    delete iRescaler;
    iFs.Close();
    }

void T_CntImageRescaler::testRescaleImageSuccefully()
    {
    test.Next(_L("Rescale image"));

    TPath dest;
    dest.Append(KDestDir());
    dest.Append(_L("test.jpg"));

    TTime time;
    time.UniversalTime();

    TRAPD(err, iRescaler->ResizeImageL(KSrcImage(), dest));

    TTime time2;
    time2.UniversalTime();

    TInt seconds = time2.MicroSecondsFrom( time ).Int64() / 1000000;

    test.Printf(_L("rescaled in %d seconds\n"), seconds);

    test(err == KErrNone);
    test(BaflUtils::FileExists(iFs, dest));

    TEntry file;
    if (iFs.Entry(dest, file) == KErrNone)
        {
        test(file.iSize <= KMaxImageSize);
        }
    }

void T_CntImageRescaler::testEmptyArgs()
    {
    test.Next(_L("Empty arguments"));

    TPath path;
    TInt err;

    TRAP(err, iRescaler->ResizeImageL(KNullDesC(), path));
    test(err == KErrArgument);

    TRAP(err, iRescaler->ResizeImageL(path, KNullDesC()));
    test(err == KErrArgument);

    TRAP(err, iRescaler->ResizeImageL(path, path));
    test(err == KErrArgument);
    }

void T_CntImageRescaler::testRescaleUtility()
{
    // delete the possible image directory, it must not leave
    // even if the folder was not found. 
    TRAPD( err, TCntImageRescaleUtility::DeleteImageDirectoryL() );
    test( err == KErrNone );
    
    // path for image directory, existense of the directory is not
    // checked
    TPath path = TCntImageRescaleUtility::ImageDirectoryL();
    test( path.Length() > 0 );
    test( path.Find(KImagesFolder) != KErrNotFound );
    
    TPath dir = TCntImageRescaleUtility::CreateImageDirectoryL();
    test( dir.Length() > 0 );
    test( dir.Find( KImagesFolder) != KErrNotFound );
  
    // make a test image file (empty file) 
    RFs fs;
    CleanupClosePushL( fs );
    User::LeaveIfError( fs.Connect() );
    
    TPath imagePath;
    imagePath.Append( dir );
    imagePath.Append( KImageName );
    
    RFile file;
    CleanupClosePushL(file);
    User::LeaveIfError(file.Create( fs, imagePath, EFileWrite ));
    CleanupStack::PopAndDestroy();
    
    CContactItem* item  = CContactItem::NewLC(KUidContactCard);
    CContactItemField* field = CContactItemField::NewL( KStorageTypeText, KUidContactFieldCodImage );
    field->SetMapping( KUidContactFieldVCardMapUnknown );
    item->AddFieldL( *field );

    // add image without GUID
    TRAPD( err2, TCntImageRescaleUtility::StoreImageFieldL( *item, imagePath ) );
    test( err2 == KErrNone );
    
    // then update with GUID value
    _LIT(KGuid, "guid");
    TBufC<4> buffer ( KGuid );
    item->SetUidStringL( buffer );
    
    TRAPD( err3, TCntImageRescaleUtility::UpdateImageNameL( *item ) );
    test( err3 == KErrNone );
    
    CContactItemFieldSet& fields = item->CardFields();
    TInt privateImageIndex = fields.Find( KUidContactFieldCodImage, KUidContactFieldVCardMapUnknown );
    test( privateImageIndex != KErrNotFound );
    
    TPtrC fieldText = fields[privateImageIndex].TextStorage()->Text();
    
    // how it should look like
    TPath newPath;
    newPath.Append( TCntImageRescaleUtility::ImageDirectoryL() );
    newPath.Append( buffer );
    newPath.Append( KImageName );
    RDebug::Print( _L("%S"), &newPath );
    RDebug::Print( _L("%S"), &fieldText );
    
    test( newPath.Compare(fieldText) == 0 );
    BaflUtils::DeleteFile( fs, newPath );
    CleanupStack::PopAndDestroy(2); // item, RFs
}

void T_CntImageRescaler::startTest()
    {
    test.Start(_L("Start tests"));
    }

void T_CntImageRescaler::stopTest()
    {
    TRAP_IGNORE(CFileMan* fileMan = CFileMan::NewL(iFs);
    fileMan->RmDir(KDestDir()); // err not used
    delete fileMan;)

    test.End();
    // Pause the test in eshell
    test.Getch();
    }

LOCAL_C void GenerateTestImageL()
    {
    RFs fs;
    CleanupClosePushL(fs);
    User::LeaveIfError(fs.Connect());

    CTestImageCreator* scaler = CTestImageCreator::NewLC(fs);

    scaler->DecodeL();

    CleanupStack::PopAndDestroy(2);
    }

LOCAL_C void DoTestsL()
	{
    User::LeaveIfError(RFbsSession::Connect());

    GenerateTestImageL();

    T_CntImageRescaler* rescalerTest = T_CntImageRescaler::NewL();

	rescalerTest->startTest();

	rescalerTest->testRescaleImageSuccefully();
    rescalerTest->testEmptyArgs();
    rescalerTest->testRescaleUtility();
    rescalerTest->stopTest();

    delete rescalerTest;
    RFbsSession::Disconnect();
	}

GLDEF_C TInt E32Main()
	{
    // Init
    TInt ret = KErrNone;
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    CActiveScheduler* activeScheduler = new CActiveScheduler();
  	if (!cleanupStack || !activeScheduler)
  	    {
  	    // Delete the one that was successfully created is any
        delete cleanupStack;
        delete activeScheduler;
        ret = KErrNoMemory;
  	    }
  	else
  	    {
  	    CActiveScheduler::Install(activeScheduler);

  	    // Run the tests
  	    TRAPD(err, DoTestsL());
  	
  	    // Cleanup
  	    delete activeScheduler;
  	    delete cleanupStack;
  	    activeScheduler = NULL;
  	    cleanupStack = NULL;
  	
  	    ret = err;
  	    }
  	
    return ret;
    }
