#include "mediakeysobserver.h"

MediaKeysObserver::MediaKeysObserver(QObject *parent) : QObject(parent)
{
    iInterfaceSelector = CRemConInterfaceSelector::NewL();
    iCoreTarget = CRemConCoreApiTarget::NewL(*iInterfaceSelector, *this);
    iInterfaceSelector->OpenTargetL();
}

MediaKeysObserver::~MediaKeysObserver()
{
    delete iInterfaceSelector;
}

void MediaKeysObserver::MrccatoCommand(TRemConCoreApiOperationId aOperationId,
									TRemConCoreApiButtonAction aButtonAct)
{
    TRequestStatus status;
	switch(aOperationId)
	{
	case ERemConCoreApiPausePlayFunction:
		{
		if (aButtonAct == ERemConCoreApiButtonClick) 
		    emit mediaKeyPressed(MediaKeysObserver::EPlayPauseKey);

		//Send the response back to Remcon server       
		iCoreTarget->PausePlayFunctionResponse(status, KErrNone);
		User::WaitForRequest(status);
		break;
		}   

	case ERemConCoreApiStop:
		{
        if (aButtonAct == ERemConCoreApiButtonClick) 
            emit mediaKeyPressed(MediaKeysObserver::EStopKey);
		iCoreTarget->StopResponse(status, KErrNone);
		User::WaitForRequest(status);
		break;
		}
	case ERemConCoreApiRewind:
		{
        if (aButtonAct == ERemConCoreApiButtonClick) 
            emit mediaKeyPressed(MediaKeysObserver::EFastRewindKey);
		iCoreTarget->RewindResponse(status, KErrNone);
		User::WaitForRequest(status);   
		break;
		}       
	case ERemConCoreApiForward:
		{
        if (aButtonAct == ERemConCoreApiButtonClick) 
            emit mediaKeyPressed(MediaKeysObserver::EForwardKey);
		iCoreTarget->ForwardResponse(status, KErrNone);
		User::WaitForRequest(status);
		break;
		}
	case ERemConCoreApiVolumeUp:
		{   
        if (aButtonAct == ERemConCoreApiButtonClick) 
            emit mediaKeyPressed(MediaKeysObserver::EVolIncKey);
		iCoreTarget->VolumeUpResponse(status, KErrNone);
		User::WaitForRequest(status);   

		break;
		}       
	case ERemConCoreApiVolumeDown:
		{
        if (aButtonAct == ERemConCoreApiButtonClick) 
            emit mediaKeyPressed(MediaKeysObserver::EVolDecKey);
		iCoreTarget->VolumeDownResponse(status, KErrNone);
		User::WaitForRequest(status);   
		break;
		}
	case ERemConCoreApiFastForward:
		{
        if (aButtonAct == ERemConCoreApiButtonClick) 
            emit mediaKeyPressed(MediaKeysObserver::EFastForwardKey);
		iCoreTarget->FastForwardResponse(status, KErrNone);
		User::WaitForRequest(status);
		break;
		}              
	case ERemConCoreApiBackward:
		{
        if (aButtonAct == ERemConCoreApiButtonClick) 
            emit mediaKeyPressed(MediaKeysObserver::EBackwardKey);
		iCoreTarget->BackwardResponse(status, KErrNone);
		User::WaitForRequest(status);
		break;
		}                   
	default:
		break;
	}
}
