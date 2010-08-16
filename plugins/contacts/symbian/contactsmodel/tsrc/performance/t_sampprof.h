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


#ifndef __T_SAMPPROF_H__
#define __T_SAMPPROF_H__


//************************************************************************************************************
#include "t_perfdbcreator.h"


//************************************************************************************************************
enum TSortOrder
   {
   ELFP=0x00,
   ELFC=0x01
   };


//************************************************************************************************************
//   CTestLoop
//************************************************************************************************************
class CTestLoop : public CActive, MContactViewObserver
   {
   public:
	   static CTestLoop* NewL(TInt aTes, TInt aCount, TSortOrder aSortOrder, TBool aProf);

   private:
      CTestLoop(TInt aTes, TInt aCount, TSortOrder aSortOrder, TBool aProf);
	   void ConstructL();
	   ~CTestLoop();
	   
	   virtual void RunL();
	   virtual void DoCancel();
      virtual TInt RunError(TInt aError);
	   void CompleteSelf(TInt aReason=KErrNone);	         
      	   
	   void CreateRemoteViewL();
      virtual void HandleContactViewEvent(const CContactViewBase &aView, const TContactViewEvent &aEvent);	   	   

	   void OpenDatabaseL(const TDesC& aDbName);
	   void CloseDb();
	   
   private:
      CContactDatabase*       iContactsDb;
      CContactRemoteView*     iContactRemoteView;
      TInt                    iTest;
      TInt                    iCount;
		TInt							iLoopCounter;
      TSortOrder					iSortOrder;
      TBool 						iProf;
   };

#endif  //__T_SAMPPROF_H__
