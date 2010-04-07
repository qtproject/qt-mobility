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

#ifndef SENSORBACKENDSYM_H
#define SENSORBACKENDSYM_H

// QT Mobility Sensor API headers
#include <qsensorbackend.h>

// Standard Symbian Headers
#include <e32std.h>
#include <e32cmn.h>

// Sensor client headers
// Common Sensor Headers
#include <sensrvchannel.h>
#include <sensrvchannelinfo.h>
#include <sensrvtypes.h>
#include <sensrvchannelfinder.h>
#include <sensrvdatalistener.h>
#include <sensrvpropertylistener.h> 

// Internal Headers
#include "sensorbackenddatasym.h"

QTM_USE_NAMESPACE

class CSensorBackendSym : public CBase, public QSensorBackend, public MSensrvDataListener, public MSensrvPropertyListener
    {
    public:
        // From QSensorBackend
        
        /**
         *  start is used to start listening to the sensor
         */
        virtual void start();
        
        /*
         * stop is used to stop listening to the sensor
         */
        virtual void stop();
        
        /*
         * Backend Destructor
         */
        ~CSensorBackendSym();

        // From MSensrvDataListener
        
        /*
         * DataReceived is called by the Sensor Server when ever data is available in the
         * sensor buffer
         */
        void  DataReceived(CSensrvChannel &aChannel, TInt aCount, TInt aDataLost);
        
        /**
         * DataError is called to indicate an error, fatal errors are inrecoverable
         */
        void  DataError(CSensrvChannel &aChannel, TSensrvErrorSeverity aError);
        
        /*
         * GetDataListenerInterfaceL is used to get a pointer to the sensor server backend
         * It is not required for QT Mobility Sensors API
         */
        void  GetDataListenerInterfaceL(TUid aInterfaceUid, TAny *&aInterface);
        
        // From MSensrvProeprtyListener        
        
        /**
         * Notification about the changed value of a property.
         */
        void  PropertyChanged (CSensrvChannel &aChannel, const TSensrvProperty &aChangedProperty);
           
        /**
         *  Property listening failed.
         */
        void  PropertyError (CSensrvChannel &aChannel, TSensrvErrorSeverity aError);
          
        /**
         * Set a listener for the indication, if the setting of the property succeeded.
         */ 
        void  SetPropertySuccessIndicationChanged (TSetPropertySuccessIndicator aIndication);
           
        /*
         * Returns a pointer to a specified interface extension - to allow future extension of this class without breaking binary compatibility.
         */
        void  GetPropertyListenerInterfaceL (TUid aInterfaceUid, TAny *&aInterface);
        
        /*
         * To enable/disable data/property listening
         */ 
        void SetListening(TBool aDataListening, TBool aPropertyListening);
            
    protected:
        
        /*
         * Default C++ constructor
         */
        CSensorBackendSym(QSensor *sensor);
        
        /**
         * Deriving class implements this if it requires property change notification
         */
        virtual void HandlePropertyChange(CSensrvChannel &aChannel, const TSensrvProperty &aChangedProperty);
        
        /*
         * RecvData is used to retrieve the sensor reading from sensor server
         * It is implemented the the sensor concrete class and handles sensor specific
         * reading data and provides conversion and utility code
         */
        virtual void RecvData(CSensrvChannel &aChannel) = 0;
        
        /*
         * InitializeL is used to create and init the sensor server objects
         */ 
        void InitializeL();
        
        /*
         * SetProperty is used to set property to the channel
         */         
        TInt SetProperty(TSensrvPropertyId aPropertyId, TSensrvPropertyType aPropertyType, TSensrvArrayIndex aArrayIndex, TReal aValue);
        
        /*
         * SetMeasurementRange is used to check measurement range type and set the measurement range
         */
        TInt SetMeasurementRange();
        
        /*
         * SetDataRate is used to calculate appropriate data rate for given interval and set that interval to the channel
         */
        TInt SetDataRate();
        
        /*
         * SetProperties is used to set properties on the channel before start data listening 
         */
        void SetProperties();
        
        /*
         * GetPropertiesL used to get the properties from sensor server and sets as metadata for Qt.
         */
        void GetPropertiesL();
       
        /*
         * GetDescription used to get description of sensor from symbian and set
         * as description in Qt
         */
        void GetDescription();
        
        /*
         * GetDataRate used to get available datarates from symbian and set
         * as availableDataRates in Qt
         */
        void GetDataRate();
        
        /*
         * GetMeasurementrangeAndAccuracy used to get measurement ranges and accuracy from
         * symbian and set as outputRanges in Qt
         */
        void GetMeasurementrangeAndAccuracy();
        
        /*
         * Close is used to release all the sensor server objects
         * May change when error handling is supported by mobility apis
         */
        TInt Close();
        
        /*
         * FindSensorL is used to find if a specific sensor is available on the
         * device, if FindSensorL leaves then the sensor is not supported
         */
        void FindSensorL();
        
        /*
         * OpenSensorChannelL is used to open the channel for sensor as indicated by
         * iPrivateData.iChannelInfo
         */
        void OpenSensorChannelL();
        
        /*
         * CloseSensorChannel is used to close the sensor channel and release the
         * resources
         */
        void CloseSensorChannelL();
        
        /*
         * Used to start listening to the sensor
         */
        void StartListeningL();
        
        /*
         * Used to stop listening to the sensor
         */
        void StopListeningL();

    protected:
        TSensorBackendDataSym iBackendData;
    };

#endif //SENSORBACKENDSYM_H

