/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QWMPEVENTS_H
#define QWMPEVENTS_H

#include <wmp.h>

class QWmpEvents : public IWMPEvents3
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
