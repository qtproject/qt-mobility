/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGEOPOSITIONINFOSOURCES60_H
#define QGEOPOSITIONINFOSOURCES60_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <lbs.h>
#include "qgeopositioninfosource.h"
#include "s60_QMLBackendAO.h"
#include "s60_NotificationCallback.h"

#define MAX_SIZE 5

// CLASS DECLARATION
//forward declaration
class CQMLBackendAO;

class CPosMethodInfo 
{
    public: 
    // A unique id for the positioning module 
    TPositionModuleId  mUid;
    
    // Positioning Module (satellite/ non-satellite)
    QGeoPositionInfoSource::PositioningMethod  mPosMethod;

    // 
    int  mStatus;
    
    // time to first and next fix from the location server
    TTimeIntervalMicroSeconds  mTimeToFirstFix;
    TTimeIntervalMicroSeconds  mTimeToNextFix;
    
    // Accuracy 
    double mhorizontalAccuracy;
    
    // Flags whether the positioning technology is currently available or not.
    bool mIsAvailable;    
};


/**
 *  CQGeoPositionInfoSourceS60
 * 
 */
class CQGeoPositionInfoSourceS60 : public INotificationCallback,
                                   public QGeoPositionInfoSource
                                   
    {
public:
    // Constructors and destructor
    /**
     * Destructor.
     */
    ~CQGeoPositionInfoSourceS60( );

    /**
     * Two-phased constructor.
     */
    static CQGeoPositionInfoSourceS60* NewL(QObject * parent);

    /**
     * Two-phased constructor.
     */
    static CQGeoPositionInfoSourceS60* NewLC(QObject * parent);
    
    /**
     * returns the last known position
     */
    QGeoPositionInfo lastKnownPosition ( bool fromSatellitePositioningMethodsOnly = false ) const;

    /**
     * returns the minimum update interval 
     */
    int minimumUpdateInterval () const;

    /**
     * Sets the preferred PositioningMethod, if available
     * otherwise sets the default poistioning mmethod
     */
    void setPreferredPositioningMethods ( PositioningMethods methods );

    /**
     * Sets the interval for the regular position notifications
     */
    void setUpdateInterval ( int msec );

    /**
     * Returns the supported Positioning Methods 
     */
    PositioningMethods supportedPositioningMethods () const {
            return supportedMethods;
    }

    /**
     * Notification methods from active object. 
     * Notifies device status, position value, and status
     */    
    void UpdateDeviceStatus(void) ;

    /**
     * Update the position info
     */
    void UpdatePosition(HPositionGenericInfo *mPosInfo, int error);
    

    /**
     * Gets the handle of the PositionServer
     */
    RPositionServer& getPositionServer();
    
    /*
     * checks whether the object is valid
     */
    inline TBool isValid()
        {
        if( devStatusUpdateAO && regUpdateAO)
            return TRUE;
        else 
            return FALSE;
        }

    inline TPositionModuleId getCurrentPositonModuleID()
        {
        return currentModuleID;
        }

    inline TPositionModuleId getRequestUpdateModuleID()
        {
        return reqmoduleID;
        }
    
public slots :
    // for request update
    void requestUpdate ( int timeout = 5000 );

    // starts the regular updates
    virtual void startUpdates (); 

    // stops the regular updates
    virtual void stopUpdates ();


private:

    /**
     * Constructor for performing 1st stage construction
     */
    CQGeoPositionInfoSourceS60(QObject * parent = 0);

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();

    void update_status(TPositionModuleInfo aModInfo,TInt aStatus);
    
    void update_available_types(void);
    
    //get the index of the module in the List array
    TInt checkmodule(TPositionModuleId id) const;
    
    //get the index of the position module based on the preferred methods
    TInt getIndexPositionModule(TUint8 aBits,PositioningMethods posMethods = AllPositioningMethods) const;
    
    TPositionModuleId getPreferredPoistionModuleId(
            QGeoPositionInfoSource::PositioningMethods methods);
    
     //get the more accuarte method with time to first fix < than timeout 
    TInt getMoreAccurateMethod(TInt aTimeout,TUint8 aBits);
    
    QGeoPositionInfo getLastknownPositionS60(TPositionModuleId moduleID) const;
   
    void TPositionInfo2QGeoPositionInfo(HPositionGenericInfo *mPosInfo, 
                                                    QGeoPositionInfo& posUpdate);

protected:
		void connectNotify(const char *signal);
		void disconnectNotify(const char *signal);
private:
    /**
    * Active object for requestUpdate
    */
    CQMLBackendAO * reqUpdateAO;
    
    /**
    *prvmoduleID
    */
    TPositionModuleId reqmoduleID;
    
    /**
    * Active object for device status updates
    */               
    CQMLBackendAO * devStatusUpdateAO;
    
    /**
        * Positioner server
        */
    RPositionServer mPositionServer;  
    
    
    /**
    * Active object for regular updates.
    */                       
    CQMLBackendAO * regUpdateAO;
    
    /**
    *  list of supported position methods
    */  
    CPosMethodInfo list[MAX_SIZE];
    
    PositioningMethods supportedMethods;
    
    PositioningMethod currentMethod;
    
    /**
    *  current module ID
    */
    TPositionModuleId currentModuleID;
    
    /**
     * maintaiss the size of thr CPosMethodInfo array
     */
    int ListSize;
    
    /*
     * query for the status
     */
    TPositionModuleStatusEvent  mStatusEvent;

    /*
     * maintain the startUpdates status
     */
    TBool mStartUpdates;
    /*
     * flags for the modules
     */
    TUint8  ModuleFlags;
    };

#endif // CQGEOPOSITIONINFOSOURCES60_H
