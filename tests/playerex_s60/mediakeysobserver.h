#ifndef MEDIAKEYSOBSERVER_H_
#define MEDIAKEYSOBSERVER_H_

#include <remconcoreapitargetobserver.h>    
#include <remconcoreapitarget.h>
#include <remconinterfaceselector.h>     
#include <aknappui.h>   
#include <QObject>

class MediaKeysObserver : public QObject, public MRemConCoreApiTargetObserver
{
    Q_OBJECT
    
public:
    enum MediaKeys {
        EVolIncKey,
        EVolDecKey,
        EPlayPauseKey,
        EStopKey,
        EBackwardKey,
        EForwardKey,
        EFastForwardKey,
        EFastRewindKey
    };

    MediaKeysObserver(QObject *parent = 0); 
    virtual ~MediaKeysObserver();

protected: // From public MRemConCoreApiTargetObserver
    void MrccatoCommand(TRemConCoreApiOperationId aOperationId, TRemConCoreApiButtonAction aButtonAct);
    void MrccatoPlay(TRemConCoreApiPlaybackSpeed /*aSpeed*/, TRemConCoreApiButtonAction /*aButtonAct*/) {};         
    void MrccatoTuneFunction(TBool /*aTwoPart*/, TUint /*aMajorChannel*/, TUint /*aMinorChannel*/, TRemConCoreApiButtonAction /*aButtonAct*/) {};        
    void MrccatoSelectDiskFunction(TUint /*aDisk*/, TRemConCoreApiButtonAction /*aButtonAct*/) {};
    void MrccatoSelectAvInputFunction(TUint8 /*aAvInputSignalNumber*/, TRemConCoreApiButtonAction /*aButtonAct*/) {};
    void MrccatoSelectAudioInputFunction(TUint8 /*aAudioInputSignalNumber*/, TRemConCoreApiButtonAction /*aButtonAct*/) {};   
    
Q_SIGNALS:
    void mediaKeyPressed(MediaKeysObserver::MediaKeys key);
    
private:
    CRemConInterfaceSelector* iInterfaceSelector;
    CRemConCoreApiTarget*     iCoreTarget;
};
#endif

