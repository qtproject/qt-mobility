#ifndef QWMPEVENTS_H
#define QWMPEVENTS_H

#include "qwmpglobal.h"

#include <wmp.h>

class Q_WMP_EXPORT QWmpEvents : public IWMPEvents3
{
public:
    // IWMPEvents

    void STDMETHODCALLTYPE OpenStateChange(long NewState);
    void STDMETHODCALLTYPE PlayStateChange(long NewState);
    void STDMETHODCALLTYPE AudioLanguageChange(long LangID);
    void STDMETHODCALLTYPE StatusChange();
    void STDMETHODCALLTYPE ScriptCommand(BSTR scType, BSTR Param);
    void STDMETHODCALLTYPE NewStream();    
    void STDMETHODCALLTYPE Disconnect(long Result);
    void STDMETHODCALLTYPE Buffering(VARIANT_BOOL Start);
    void STDMETHODCALLTYPE Error();
    void STDMETHODCALLTYPE Warning(long WarningType, long Param, BSTR Description);
    void STDMETHODCALLTYPE EndOfStream(long Result);
    void STDMETHODCALLTYPE PositionChange(double oldPosition, double newPosition);
    void STDMETHODCALLTYPE MarkerHit(long MarkerNum);
    void STDMETHODCALLTYPE DurationUnitChange(long NewDurationUnit);
    void STDMETHODCALLTYPE CdromMediaChange(long CdromNum);
    void STDMETHODCALLTYPE PlaylistChange(IDispatch *Playlist, WMPPlaylistChangeEventType change);
    void STDMETHODCALLTYPE CurrentPlaylistChange(WMPPlaylistChangeEventType change);
    void STDMETHODCALLTYPE CurrentPlaylistItemAvailable(BSTR bstrItemName);
    void STDMETHODCALLTYPE MediaChange(IDispatch *Item);
    void STDMETHODCALLTYPE CurrentMediaItemAvailable(BSTR bstrItemName);
    void STDMETHODCALLTYPE CurrentItemChange(IDispatch *pdispMedia);
    void STDMETHODCALLTYPE MediaCollectionChange();
    void STDMETHODCALLTYPE MediaCollectionAttributeStringAdded(
            BSTR bstrAttribName, BSTR bstrAttribVal);
    void STDMETHODCALLTYPE MediaCollectionAttributeStringRemoved(
            BSTR bstrAttribName, BSTR bstrAttribVal);
    void STDMETHODCALLTYPE MediaCollectionAttributeStringChanged(
            BSTR bstrAttribName, BSTR bstrOldAttribVal, BSTR bstrNewAttribVal);
    void STDMETHODCALLTYPE PlaylistCollectionChange();
    void STDMETHODCALLTYPE PlaylistCollectionPlaylistAdded(BSTR bstrPlaylistName);
    void STDMETHODCALLTYPE PlaylistCollectionPlaylistRemoved(BSTR bstrPlaylistName);
    void STDMETHODCALLTYPE PlaylistCollectionPlaylistSetAsDeleted(
            BSTR bstrPlaylistName, VARIANT_BOOL varfIsDeleted);
    void STDMETHODCALLTYPE ModeChange(BSTR ModeName, VARIANT_BOOL NewValue);
    void STDMETHODCALLTYPE MediaError(IDispatch *pMediaObject);
    void STDMETHODCALLTYPE OpenPlaylistSwitch(IDispatch *pItem);
    void STDMETHODCALLTYPE DomainChange(BSTR strDomain);
    void STDMETHODCALLTYPE SwitchedToPlayerApplication();
    void STDMETHODCALLTYPE SwitchedToControl();
    void STDMETHODCALLTYPE PlayerDockedStateChange();
    void STDMETHODCALLTYPE PlayerReconnect();
    void STDMETHODCALLTYPE Click(short nButton, short nShiftState, long fX, long fY);
    void STDMETHODCALLTYPE DoubleClick(short nButton, short nShiftState, long fX, long fY);
    void STDMETHODCALLTYPE KeyDown(short nKeyCode, short nShiftState);
    void STDMETHODCALLTYPE KeyPress(short nKeyAscii);
    void STDMETHODCALLTYPE KeyUp(short nKeyCode, short nShiftState);
    void STDMETHODCALLTYPE MouseDown(short nButton, short nShiftState, long fX, long fY);
    void STDMETHODCALLTYPE MouseMove(short nButton, short nShiftState, long fX, long fY);
    void STDMETHODCALLTYPE MouseUp(short nButton, short nShiftState, long fX, long fY);

    // IWMPEvents2
    void STDMETHODCALLTYPE DeviceConnect(IWMPSyncDevice *pDevice);
    void STDMETHODCALLTYPE DeviceDisconnect(IWMPSyncDevice *pDevice);
    void STDMETHODCALLTYPE DeviceStatusChange(IWMPSyncDevice *pDevice, WMPDeviceStatus NewStatus);
    void STDMETHODCALLTYPE DeviceSyncStateChange(IWMPSyncDevice *pDevice, WMPSyncState NewState);
    void STDMETHODCALLTYPE DeviceSyncError(IWMPSyncDevice *pDevice, IDispatch *pMedia);
    void STDMETHODCALLTYPE CreatePartnershipComplete(IWMPSyncDevice *pDevice, HRESULT hrResult);

    // IWMPEvents3
    void STDMETHODCALLTYPE CdromRipStateChange(IWMPCdromRip *pCdromRip, WMPRipState wmprs);
    void STDMETHODCALLTYPE CdromRipMediaError(IWMPCdromRip *pCdromRip, IDispatch *pMedia);
    void STDMETHODCALLTYPE CdromBurnStateChange(IWMPCdromBurn *pCdromBurn, WMPBurnState wmpbs);
    void STDMETHODCALLTYPE CdromBurnMediaError(IWMPCdromBurn *pCdromBurn, IDispatch *pMedia);
    void STDMETHODCALLTYPE CdromBurnError(IWMPCdromBurn *pCdromBurn, HRESULT hrError);
    void STDMETHODCALLTYPE LibraryConnect(IWMPLibrary *pLibrary);
    void STDMETHODCALLTYPE LibraryDisconnect(IWMPLibrary *pLibrary);
    void STDMETHODCALLTYPE FolderScanStateChange(WMPFolderScanState wmpfss);
    void STDMETHODCALLTYPE StringCollectionChange(
            IDispatch *pdispStringCollection,
            WMPStringCollectionChangeEventType change,
            long lCollectionIndex);
    void STDMETHODCALLTYPE MediaCollectionMediaAdded(IDispatch *pdispMedia);    
    void STDMETHODCALLTYPE MediaCollectionMediaRemoved(IDispatch *pdispMedia);
};

#endif