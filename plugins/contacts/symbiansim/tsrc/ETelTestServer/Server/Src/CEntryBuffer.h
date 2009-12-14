/*
* ==============================================================================
*  Name        : CEntryBuffer.h
*  Part of     : MM ETel Test Server / ETelTestServer.dll
*  Interface   : 
*  Description : 
*  Version     : 
*
*  Copyright © 2002 Nokia Corporation.
*  This material, including documentation and any related 
*  computer programs, is protected by copyright controlled by 
*  Nokia Corporation. All rights are reserved. Copying, 
*  including reproducing, storing, adapting or translating, any 
*  or all of this material requires the prior written consent of 
*  Nokia Corporation. This material also contains confidential 
*  information which may not be disclosed to others without the 
*  prior written consent of Nokia Corporation.
* ==============================================================================
*/


#ifndef CENTRYBUFFER_H
#define CENTRYBUFFER_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS
const TInt KMaxServerBufSize = 2048;

// FORWARD DECLARATION
class CPhoneBookBuffer;

// CLASS DECLARATION

class TTextField
    {
    public: // Constructor
        inline TTextField() : iTextData( KNullDesC ), iFieldType( 0 ) 
            {
            }
        inline TTextField(const TDesC& aData, TUint8 aFieldType ) 
                : iTextData( aData ), iFieldType( aFieldType ) 
            {
            }
    public: // Data
        TPtrC iTextData;
        // MM ETel field type
        TUint8 iFieldType;
    };

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*/
class CEntryBuffer : public CBase
    {
    public:  // Constructors and destructor
        
        static CEntryBuffer* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CEntryBuffer();

    public: // New functions
        
        /**
        * Reads the client buffer creates an own
        */
        HBufC8* CreateServerBufferLC( TDes8& aClientBuffer, TInt aETelIndex );
        
        /**
        * @return the number of text type fields in the buffer
        */
        TInt NumOfTextFields( TDes8& aClientBuffer );
        
        /**
        * @return the number of fields of given type in the buffer
        */
        TInt NumOfFields( TDes8& aClientBuffer, TUint8 aFieldTag );
        
        /**
        * @return a text field data at given index
        */
        TTextField TextFieldAt( TDes8& aClientBuffer, TInt aIndex );
        
    private: // Construction
        
        /**
        * Constructor.
        */
        CEntryBuffer();

        void ConstructL();

    private:    // Data
        // The ETel buffer modification tool
        CPhoneBookBuffer* iETelReadBuffer;
        CPhoneBookBuffer* iETelWriteBuffer;
        TBuf8<KMaxServerBufSize> iWriteBuffer;
    };

#endif      // CENTRYBUFFER_H   
            
// End of File
