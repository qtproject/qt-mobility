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

#include "qwmpevents.h"

void QWmpEvents::OpenStateChange(long) {}
void QWmpEvents::PlayStateChange(long) {}
void QWmpEvents::AudioLanguageChange(long) {}
void QWmpEvents::StatusChange() {}
void QWmpEvents::ScriptCommand(BSTR, BSTR) {}
void QWmpEvents::NewStream() {}    
void QWmpEvents::Disconnect(long) {}
void QWmpEvents::Buffering(VARIANT_BOOL) {}
void QWmpEvents::Error() {}
void QWmpEvents::Warning(long, long, BSTR) {}
void QWmpEvents::EndOfStream(long) {}
void QWmpEvents::PositionChange(double, double) {}
void QWmpEvents::MarkerHit(long) {}
void QWmpEvents::DurationUnitChange(long) {}
void QWmpEvents::CdromMediaChange(long) {}
void QWmpEvents::PlaylistChange(IDispatch *, WMPPlaylistChangeEventType) {}
void QWmpEvents::CurrentPlaylistChange(WMPPlaylistChangeEventType) {}
void QWmpEvents::CurrentPlaylistItemAvailable(BSTR) {}
void QWmpEvents::MediaChange(IDispatch *) {}
void QWmpEvents::CurrentMediaItemAvailable(BSTR) {}
void QWmpEvents::CurrentItemChange(IDispatch *) {}
void QWmpEvents::MediaCollectionChange() {}
void QWmpEvents::MediaCollectionAttributeStringAdded(BSTR, BSTR) {}
void QWmpEvents::MediaCollectionAttributeStringRemoved(BSTR, BSTR) {}
void QWmpEvents::MediaCollectionAttributeStringChanged(BSTR, BSTR, BSTR) {}
void QWmpEvents::PlaylistCollectionChange() {}
void QWmpEvents::PlaylistCollectionPlaylistAdded(BSTR) {}
void QWmpEvents::PlaylistCollectionPlaylistRemoved(BSTR) {}
void QWmpEvents::PlaylistCollectionPlaylistSetAsDeleted(BSTR, VARIANT_BOOL) {}
void QWmpEvents::ModeChange(BSTR, VARIANT_BOOL) {}
void QWmpEvents::MediaError(IDispatch *) {}
void QWmpEvents::OpenPlaylistSwitch(IDispatch *) {}
void QWmpEvents::DomainChange(BSTR) {}
void QWmpEvents::SwitchedToPlayerApplication() {}
void QWmpEvents::SwitchedToControl() {}
void QWmpEvents::PlayerDockedStateChange() {}
void QWmpEvents::PlayerReconnect() {}
void QWmpEvents::Click(short, short, long, long) {}
void QWmpEvents::DoubleClick(short, short, long, long) {}
void QWmpEvents::KeyDown(short, short) {}
void QWmpEvents::KeyPress(short) {}
void QWmpEvents::KeyUp(short, short) {}
void QWmpEvents::MouseDown(short, short, long, long) {}
void QWmpEvents::MouseMove(short, short, long, long) {}
void QWmpEvents::MouseUp(short, short, long, long) {}

// IWMPEvents2
void QWmpEvents::DeviceConnect(IWMPSyncDevice *) {}
void QWmpEvents::DeviceDisconnect(IWMPSyncDevice *) {}
void QWmpEvents::DeviceStatusChange(IWMPSyncDevice *, WMPDeviceStatus) {}
void QWmpEvents::DeviceSyncStateChange(IWMPSyncDevice *, WMPSyncState) {}
void QWmpEvents::DeviceSyncError(IWMPSyncDevice *, IDispatch *) {}
void QWmpEvents::CreatePartnershipComplete(IWMPSyncDevice *, HRESULT) {}

// IWMPEvents3
void QWmpEvents::CdromRipStateChange(IWMPCdromRip *, WMPRipState) {}
void QWmpEvents::CdromRipMediaError(IWMPCdromRip *, IDispatch *) {}
void QWmpEvents::CdromBurnStateChange(IWMPCdromBurn *, WMPBurnState) {}
void QWmpEvents::CdromBurnMediaError(IWMPCdromBurn *, IDispatch *) {}
void QWmpEvents::CdromBurnError(IWMPCdromBurn *, HRESULT) {}
void QWmpEvents::LibraryConnect(IWMPLibrary *) {}
void QWmpEvents::LibraryDisconnect(IWMPLibrary *) {}
void QWmpEvents::FolderScanStateChange(WMPFolderScanState) {}
void QWmpEvents::StringCollectionChange(IDispatch *, WMPStringCollectionChangeEventType, long) {}
void QWmpEvents::MediaCollectionMediaAdded(IDispatch *) {}    
void QWmpEvents::MediaCollectionMediaRemoved(IDispatch *) {}
