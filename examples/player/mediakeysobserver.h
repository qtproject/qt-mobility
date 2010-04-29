/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

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

