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

#include "cntimagerescaler.h"
#include "cntdbconsts_internal.h"
#include "t_upscalerhelper.h"

_LIT(KDestDir, "c:\\data\\cnttestdata\\");

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
