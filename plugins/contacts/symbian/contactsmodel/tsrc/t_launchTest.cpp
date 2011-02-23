// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

// INCLUDE FILES
#include <t_launchTest.h>

void DoTestsL()
    {
    CLog* loger; 
    RTest test(_L("T_LaunchTest"));
    TInt blankLoc;
    TBuf<100> appName( 0 );
    TBuf<200> parameters( 0 );
    TBuf<200> tempStr( 0 );
    RFile fileread;
    TFileText tf;
    RFs fsSession;
    TUidType uidtype( KNullUid );
    _LIT(KFileNameRead, "C:\\data\\cm_all_tests.bat");
    _LIT(KLogFileName,"t_launchTest.log");
    
    // New loger    
    loger = CLog::NewL( test, KLogFileName );
    CleanupStack::PushL( loger );

    // New process control
    RProcess* process = new RProcess();
    CleanupStack::PushL( process );
    loger->LogLine(
        _L("Run auto testing for contacts model.[cm_all_tests.bat]") );

    // Open cm_all_tests.bat
    User::LeaveIfError( fsSession.Connect() );
    CleanupClosePushL( fsSession );
    User::LeaveIfError( fileread.Open( fsSession, KFileNameRead, EFileStream
        | EFileRead ) );
    CleanupClosePushL( fileread );
    tf.Set( fileread );
    loger->LogLine( _L("Runing:") );

    // Read the testing cases from cm_all_tests.bat and run it.
    for (; KErrNone == tf.Read( tempStr ); )
        {
        if ( tempStr.Length() <= 0 )
            {
            loger->LogLine( _L("Blank line: No Program ") );
            continue;
            }
        // Analyze the command format   
        appName.Copy( _L("z:\\sys\\bin\\") );
        tempStr.TrimAll();
        blankLoc = tempStr.Locate( ' ' );
        if ( 0 > blankLoc )
            {
            // No parameters.
            appName.Append( tempStr );
            parameters = _L( "" );
            }
        else
            {
            // Store parameters.
            appName.Append( tempStr.Left( blankLoc ) );
            parameters = tempStr.Mid( blankLoc + 1 );
            }
        appName.Append( _L(".exe") );
        loger->LogLine( tempStr );

        // Run the command.
        TInt err = process->Create( appName, parameters, uidtype );
        if ( KErrNone != err )
            {
            tempStr.Format( _L("Not Found %S "), &appName );
            loger->LogLine( tempStr );
            continue;
            }
        TRequestStatus status( KRequestPending );
        process->Rendezvous( status );
        process->Resume();
        User::WaitForRequest( status );
        TExitType type = process->ExitType();
        TBuf<200> buf = process->ExitCategory();
        buf.AppendFormat( _L(": process->ExitType %d "), type );
        loger->LogLine( buf );
        process->Close();
        User::After( 1000000 );
        }
    fileread.Close();
    fsSession.Close();
    loger->LogLine( _L("Print any key to exit:"));
    test.Close();
    CleanupStack::PopAndDestroy( 4 ); // fileread, process, fsSession, loger
    }
    


int main( void )
    {
    __UHEAP_MARK;
    TRAPD(err,DoTestsL());
    __UHEAP_MARKEND;
    return KErrNone;
    }

