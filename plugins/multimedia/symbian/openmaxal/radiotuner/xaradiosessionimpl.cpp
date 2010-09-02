/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "xaradiosessionimpl.h"
#include "xaradiosessionimplobserver.h"
#include <xaradioitfext.h>

#ifdef PLUGIN_SYMBIAN_TRACE_ENABLED
#   include <e32debug.h>
#endif /* PLUGIN_SYMBIAN_TRACE_ENABLED */

#ifdef PLUGIN_SYMBIAN_TRACE_ENABLED
#   define SF_TRACE_FN_ENT_PRINT        RDebug::Printf(">%s", __PRETTY_FUNCTION__)
#   define SF_TRACE_FN_ENT_EXT_PRINT        RDebug::Printf("><%s", __PRETTY_FUNCTION__)
#   define SF_TRACE_FN_EXT_PRINT        RDebug::Printf("<%s", __PRETTY_FUNCTION__)
#   define SF_PRINT1_LOG(s) RDebug::Print(s)
#   define SF_PRINT2_LOG(s,a1) RDebug::Print(s,a1)
#   define SF_PRINT3_LOG(s,a1,a2) RDebug::Print(s,a1,a2)
#   define SF_PRINT4_LOG(s,a1,a2,a3) RDebug::Print(s,a1,a2,a3)
#else
#   define SF_TRACE_FN_ENT_PRINT
#   define SF_TRACE_FN_ENT_EXT_PRINT
#   define SF_TRACE_FN_EXT_PRINT
#   define SF_PRINT1_LOG(s)
#   define SF_PRINT2_LOG(s,a1)
#   define SF_PRINT3_LOG(s,a1,a2)
#   define SF_PRINT4_LOG(s,a1,a2,a3)
#endif /* PLUGIN_SYMBIAN_TRACE_ENABLED */

#define RET_ERR_IF_ERR(e) \
    if (e != 0) \
        return e;

#define MAX_NUMBER_INTERFACES 20
#define FM_STEP 100000; // Hz (.1 MHz)

/*
 * function declarations.
 * */
void EngineObjectCallback(XAObjectItf caller, const void */*pContext*/,
                        XAuint32 event, XAresult result, XAuint32 /*param*/,
                        void */*pInterface*/);

void RadioCallback(XARadioItf caller, void* pContext, XAuint32 event, XAuint32 eventIntData, XAboolean eventBooleanData);
void NokiaVolumeExtItfCallback(XANokiaVolumeExtItf caller, void* pContext, XAuint32 event, XAboolean eventBooleanData);
void NokiaLinearVolumeItfCallback(XANokiaLinearVolumeItf caller, void* pContext, XAuint32 event, XAboolean eventBooleanData);
void PlayItfCallbackForRadio(XAPlayItf caller, void* pContext, XAuint32 event);

XARadioSessionImpl::XARadioSessionImpl(XARadioSessionImplObserver& parent)
:iParent(parent),
iRadio(NULL),
iEngine(NULL),
iPlayer(NULL),
iSearching(EFalse),
iRadioAvailable(EFalse),
iState(QRadioTuner::StoppedState)
{
    iAvailabilityError = QtMultimediaKit::NoError;
}

XARadioSessionImpl::~XARadioSessionImpl()
{
    if (iRadio) {
        SF_PRINT1_LOG(_L("XARadioSessionImpl::~XARadioSessionImpl(): Deleting Radio Device..."));
        (*iRadio)->Destroy(iRadio);
        iRadio = NULL;
        SF_PRINT1_LOG(_L("XARadioSessionImpl::~XARadioSessionImpl(): Deleted Radio Device"));
    }
    if (iPlayer) {
        SF_PRINT1_LOG(_L("XARadioSessionImpl::~XARadioSessionImpl(): Deleting player..."));
        (*iPlayer)->Destroy(iPlayer);
        iPlayer = NULL;
        SF_PRINT1_LOG(_L("XARadioSessionImpl::~XARadioSessionImpl(): Deleted iPlayer"));
    }
    if ( iEngine ) {
        SF_PRINT1_LOG(_L("XARadioSessionImpl::~XARadioSessionImpl(): Deleting engine..."));
        (*iEngine)->Destroy(iEngine);
        iEngine = NULL;
        SF_PRINT1_LOG(_L("XARadioSessionImpl::~XARadioSessionImpl(): Deleted engine"));
    }
}

QRadioTuner::Error XARadioSessionImpl::PostConstruct()
{
    XAresult res = CreateEngine();
    if (res != KErrNone)
        return QRadioTuner::ResourceError;
    else
        return QRadioTuner::NoError;
}

TInt XARadioSessionImpl::CreateEngine()
{
    SF_TRACE_FN_ENT_PRINT;
    XAboolean required[MAX_NUMBER_INTERFACES];
    XAInterfaceID iidArray[MAX_NUMBER_INTERFACES];
    XAuint32 noOfInterfaces = 0;
    int i;
    XAresult res;

    XAEngineOption EngineOption[] =
    {
        {
        (XAuint32) XA_ENGINEOPTION_THREADSAFE,
        (XAuint32) XA_BOOLEAN_TRUE
        }
    };

    /* Create XA engine */
    if (!iEngine) {
        SF_PRINT1_LOG(_L("XARadioSessionImpl::CreateEngine: Creating Engine..."));
        res = xaCreateEngine(&iEngine, 1, EngineOption, 0, NULL, NULL);
        RET_ERR_IF_ERR(CheckErr(res));
        res = (*iEngine)->RegisterCallback(iEngine, EngineObjectCallback, NULL);
        RET_ERR_IF_ERR(CheckErr(res));

        SF_PRINT1_LOG(_L("XARadioSessionImpl::CreateEngine: Realizing..."));
        res = (*iEngine)->Realize(iEngine, XA_BOOLEAN_FALSE);
        RET_ERR_IF_ERR(CheckErr(res));

        // Create Engine Interface:
        SF_PRINT1_LOG(_L("XARadioSessionImpl::CreateEngine: Creating Engine Interface"));
        RET_ERR_IF_ERR(CheckErr((*iEngine)->GetInterface(iEngine, XA_IID_ENGINE, (void*)&iEngineItf)));

        // Create Radio Device and interface(s):
        if (!iRadio) {
            SF_PRINT1_LOG(_L("XARadioSessionImpl::CreateEngine: Creating Radio Device"));
            res = (*iEngineItf)->CreateRadioDevice(iEngineItf,&iRadio, 0, NULL, NULL);
            RET_ERR_IF_ERR(CheckErr(res));

            SF_PRINT1_LOG(_L("XARadioSessionImpl::CreateEngine: Realize Radio Device"));
            res = (*iRadio)->Realize(iRadio, XA_BOOLEAN_FALSE);
            RET_ERR_IF_ERR(CheckErr(res));

            // Get Radio interface:
            SF_PRINT1_LOG(_L("XARadioSessionImpl::CreateEngine: Get Radio Interface"));
            res = (*iRadio)->GetInterface(iRadio, XA_IID_RADIO, (void*)&iRadioItf);
            RET_ERR_IF_ERR(CheckErr(res));
            iRadioAvailable = ETrue;
            // Register Radio Callback:
            SF_PRINT1_LOG(_L("XARadioSessionImpl::CreateEngine: Create Radio Callback:"));
            res = (*iRadioItf)->RegisterRadioCallback(iRadioItf, RadioCallback, (void*)this);
            RET_ERR_IF_ERR(CheckErr(res));
        }
        XADataSource            audioSource;
        XADataLocator_IODevice  locatorIODevice;
        XADataSink              audioSink;
        XADataLocator_OutputMix locator_outputmix;

        /* Init arrays required[] and iidArray[] */
        for (i = 0; i < MAX_NUMBER_INTERFACES; i++) {
            required[i] = XA_BOOLEAN_FALSE;
            iidArray[i] = XA_IID_NULL;
        }

        iidArray[0] = XA_IID_NOKIAVOLUMEEXT;
        iidArray[1] = XA_IID_NOKIALINEARVOLUME;
        noOfInterfaces = 2;

        locatorIODevice.locatorType = XA_DATALOCATOR_IODEVICE;
        locatorIODevice.deviceType  = XA_IODEVICE_RADIO;
        locatorIODevice.deviceID    = 0; /* ignored */
        locatorIODevice.device      = iRadio;
        audioSource.pLocator        = (void*) &locatorIODevice;
        audioSource.pFormat         = NULL;

        /* Setup the data sink structure */
        locator_outputmix.locatorType = XA_DEFAULTDEVICEID_AUDIOOUTPUT;
        locator_outputmix.outputMix   = NULL;
        audioSink.pLocator            = (void*) &locator_outputmix;
        audioSink.pFormat             = NULL;

        SF_PRINT1_LOG(_L("XARadioSessionImpl::CreateEngine: Create Media Player:"));
        res = (*iEngineItf)->CreateMediaPlayer(iEngineItf, &iPlayer, &audioSource, NULL, &audioSink, NULL, NULL, NULL, noOfInterfaces, iidArray, required);
        RET_ERR_IF_ERR(CheckErr(res));

        SF_PRINT1_LOG(_L("XARadioSessionImpl::CreateEngine: Realize Media Player:"));
        res = (*iPlayer)->Realize(iPlayer, XA_BOOLEAN_FALSE);
        RET_ERR_IF_ERR(CheckErr(res));
        SF_PRINT1_LOG(_L("XARadioSessionImpl::CreateEngine: Get Play Interface from player:"));
        res = (*iPlayer)->GetInterface(iPlayer, XA_IID_PLAY, (void*) &iPlayItf);
        RET_ERR_IF_ERR(CheckErr(res));
        SF_PRINT1_LOG(_L("XARadioSessionImpl::CreateEngine: Create PlayItf Callback:"));
        res = (*iPlayItf)->RegisterCallback(iPlayItf, PlayItfCallbackForRadio, (void*)this);
        RET_ERR_IF_ERR(CheckErr(res));

        // Get Volume Interfaces specific for Nokia impl:
        SF_PRINT1_LOG(_L("XARadioSessionImpl::CreateEngine: Get NokiaVolumeExt Interface"));
        res = (*iPlayer)->GetInterface(iPlayer, XA_IID_NOKIAVOLUMEEXT, (void*)&iNokiaVolumeExtItf);
        RET_ERR_IF_ERR(CheckErr(res));

        SF_PRINT1_LOG(_L("XARadioSessionImpl::CreateEngine: Get NokiaLinearVolume Interface"));
        res = (*iPlayer)->GetInterface(iPlayer, XA_IID_NOKIALINEARVOLUME, (void*)&iNokiaLinearVolumeItf);
        RET_ERR_IF_ERR(CheckErr(res));

        // Register Volume Callbacks:
        SF_PRINT1_LOG(_L("XARadioSessionImpl::CreateEngine: Create NokiaVolumeExtItf Callback:"));
        res = (*iNokiaVolumeExtItf)->RegisterVolumeCallback(iNokiaVolumeExtItf, NokiaVolumeExtItfCallback, (void*)this);
        RET_ERR_IF_ERR(CheckErr(res));
        res = (*iNokiaVolumeExtItf)->SetCallbackEventsMask(iNokiaVolumeExtItf,(XA_NOKIAVOLUMEEXT_EVENT_MUTE_CHANGED));
        RET_ERR_IF_ERR(CheckErr(res));
        SF_PRINT1_LOG(_L("XARadioSessionImpl::CreateEngine: Create NokiaLinearVolumeItf Callback:"));
        res = (*iNokiaLinearVolumeItf)->RegisterVolumeCallback(iNokiaLinearVolumeItf, NokiaLinearVolumeItfCallback, (void*)this);
        RET_ERR_IF_ERR(CheckErr(res));
        res = (*iNokiaLinearVolumeItf)->SetCallbackEventsMask(iNokiaLinearVolumeItf,(XA_NOKIALINEARVOLUME_EVENT_VOLUME_CHANGED));
        RET_ERR_IF_ERR(CheckErr(res));
    }

    SF_TRACE_FN_EXT_PRINT;
    return EFalse;
}

QRadioTuner::State XARadioSessionImpl::State() const
{
    SF_TRACE_FN_ENT_EXT_PRINT;
    return iState;
}

QtMultimediaKit::AvailabilityError XARadioSessionImpl::AvailabilityError() const
{
    SF_TRACE_FN_ENT_EXT_PRINT;
    return iAvailabilityError;
}

 bool XARadioSessionImpl::IsAvailable() const
{
    SF_TRACE_FN_ENT_EXT_PRINT;
    return iRadioAvailable;
}

QRadioTuner::Band XARadioSessionImpl::Band() const
{
    SF_TRACE_FN_ENT_EXT_PRINT;
    return iBand;
}

void XARadioSessionImpl::SetBand(QRadioTuner::Band band)
{
    if (band != QRadioTuner::FM)
        iParent.CBError(QRadioTuner::OpenError);
    else
        iBand = band;
}

bool XARadioSessionImpl::IsBandSupported(QRadioTuner::Band band) const
{
    if (band == QRadioTuner::FM)
        return ETrue;
    else
        return EFalse;
}

// Returns the number of Hertz to increment the frequency by when stepping through frequencies within a given band.
TInt XARadioSessionImpl::FrequencyStep(QRadioTuner::Band /*band*/) const
{
    TInt freqStep = FM_STEP;
    return (int)freqStep;
}

bool XARadioSessionImpl::IsStereo() //const
{
    bool isStereo = EFalse;
    QRadioTuner::StereoMode mode = StereoMode();
    if (mode == QRadioTuner::ForceStereo || mode == QRadioTuner::Auto)
        isStereo = ETrue;
    return isStereo;
}

bool XARadioSessionImpl::IsMuted() const
{
    SF_TRACE_FN_ENT_PRINT;
    XAboolean isMuted = EFalse;
    (*iNokiaVolumeExtItf)->GetMute(iNokiaVolumeExtItf, &isMuted );
    SF_PRINT2_LOG(_L("XARadioSessionImpl::IsMuted: isMuted = %d"), isMuted);

    SF_TRACE_FN_EXT_PRINT;
    return isMuted;
}

bool XARadioSessionImpl::IsSearching() const
{
    //iSearching is set when seek (QT:searchForward-backward)
    // iSearching is cleared when SearchingStatusChanged is called or StopSeeking is called
    return iSearching;
}

TInt XARadioSessionImpl::GetFrequency()
{
    SF_TRACE_FN_ENT_PRINT;

    XAuint32 freq = 0;
    XAresult res = (*iRadioItf)->GetFrequency(iRadioItf, &freq );
    RET_ERR_IF_ERR(CheckErr(res));
    SF_PRINT2_LOG(_L("XARadioSessionImpl::GetFrequency: Frequency = %d"), freq);

    SF_TRACE_FN_EXT_PRINT;
    return (int)freq;
}

TInt XARadioSessionImpl::GetFrequencyRange()
{
    SF_TRACE_FN_ENT_PRINT;
    XAuint8 range = 0;

    XAresult res = (*iRadioItf)->GetFreqRange(iRadioItf, &range);
    RET_ERR_IF_ERR(CheckErr(res));
    SF_PRINT2_LOG(_L("XARadioSessionImpl::GetFrequencyRange: Frequency Range = %d"), range);

    SF_TRACE_FN_EXT_PRINT;
    return (int)range;
}

TInt XARadioSessionImpl::GetFrequencyRangeProperties(TInt range, TInt &minFreq, TInt &maxFreq)
{
    SF_TRACE_FN_ENT_PRINT;
    XAuint32 freqInterval = 0;
    XAresult res = (*iRadioItf)->GetFreqRangeProperties(iRadioItf, (XAuint8)range, (XAuint32*)&minFreq,(XAuint32*)&maxFreq, (XAuint32*)&freqInterval);
    RET_ERR_IF_ERR(CheckErr(res));
    SF_PRINT3_LOG(_L("XARadioSessionImpl::GetFrequencyRangeProperties: minFreq = %d, maxFreq = %d"), minFreq, maxFreq);

    SF_TRACE_FN_EXT_PRINT;
    return res;
}

TInt XARadioSessionImpl::SetFrequency(TInt aFreq)
{
    SF_TRACE_FN_ENT_PRINT;

    SF_PRINT2_LOG(_L("XARadioSessionImpl::SetFrequency: Setting Frequency to: %d"), aFreq);
    XAresult res = (*iRadioItf)->SetFrequency(iRadioItf, aFreq );
    RET_ERR_IF_ERR(CheckErr(res));

    SF_TRACE_FN_EXT_PRINT;
    return res;
}

QRadioTuner::StereoMode XARadioSessionImpl::StereoMode()
{
    SF_TRACE_FN_ENT_PRINT;
    QRadioTuner::StereoMode qtStereoMode;
    XAuint32 symStereoMode;
    (*iRadioItf)->GetStereoMode(iRadioItf, &symStereoMode);

    if (symStereoMode == XA_STEREOMODE_MONO)
        qtStereoMode = QRadioTuner::ForceMono;
    else if (symStereoMode == XA_STEREOMODE_STEREO)
        qtStereoMode = QRadioTuner::ForceStereo;
    else
        qtStereoMode = QRadioTuner::Auto;

    SF_TRACE_FN_EXT_PRINT;
    return qtStereoMode;
}

TInt XARadioSessionImpl::SetStereoMode(QRadioTuner::StereoMode qtStereoMode)
{
    SF_TRACE_FN_ENT_PRINT;
    XAuint32 symStereoMode;

    if (qtStereoMode == QRadioTuner::ForceMono)
        symStereoMode = XA_STEREOMODE_MONO;
    else if (qtStereoMode == QRadioTuner::ForceStereo)
        symStereoMode = XA_STEREOMODE_STEREO;
    else
        symStereoMode = XA_STEREOMODE_AUTO;

    XAresult res = (*iRadioItf)->SetStereoMode(iRadioItf, (symStereoMode));
    SF_TRACE_FN_EXT_PRINT;
    return res;
}

TInt XARadioSessionImpl::GetSignalStrength()
{
    SF_TRACE_FN_ENT_PRINT;
    XAuint32 signalStrength = 0;

    (*iRadioItf)->GetSignalStrength(iRadioItf, &signalStrength );
    SF_PRINT2_LOG(_L("XARadioSessionImpl::GetSignalStrength: Signal Strength = %d"), signalStrength);
    SF_TRACE_FN_EXT_PRINT;
    return (int)signalStrength;
}

TInt XARadioSessionImpl::GetVolume()
{
    SF_TRACE_FN_ENT_PRINT;
    XAuint32 vol;
    if (iPlayer && iNokiaLinearVolumeItf) {
        (*iNokiaLinearVolumeItf)->GetVolumeLevel(iNokiaLinearVolumeItf, &vol );
        SF_PRINT2_LOG(_L("XARadioSessionImpl::GetVolume: Volume = %d"), vol);
    }
    SF_TRACE_FN_EXT_PRINT;
    return (TInt)vol;
}

TInt XARadioSessionImpl::SetVolume(TInt aVolume)
{
    SF_TRACE_FN_ENT_PRINT;
    XAuint32 newVolume = 0;
    SF_PRINT2_LOG(_L("XARadioSessionImpl::SetVolume: Setting volume to: %d"), aVolume);
    if (iPlayer && iNokiaLinearVolumeItf) {
        newVolume = aVolume;
        XAresult res = (*iNokiaLinearVolumeItf)->SetVolumeLevel(iNokiaLinearVolumeItf, &newVolume );
    }
    SF_TRACE_FN_EXT_PRINT;
    return (TInt)newVolume;
}

TInt XARadioSessionImpl::SetMuted(TBool aMuted)
{
    SF_TRACE_FN_ENT_PRINT;
    XAresult res = (*iNokiaVolumeExtItf)->SetMute(iNokiaVolumeExtItf, aMuted);
    SF_TRACE_FN_EXT_PRINT;
    return res;
}

TInt XARadioSessionImpl::Seek(TBool aDirection)
{
    SF_TRACE_FN_ENT_PRINT;
    iSearching = true;
    XAresult res = (*iRadioItf)->Seek(iRadioItf, aDirection );
    SF_TRACE_FN_EXT_PRINT;
    return res;
}

TInt XARadioSessionImpl::StopSeeking()
{
    SF_TRACE_FN_ENT_PRINT;
    XAresult res = (*iRadioItf)->StopSeeking(iRadioItf);
    iSearching = EFalse;
    SF_TRACE_FN_EXT_PRINT;
    return res;
}

void XARadioSessionImpl::Start()
{
    SF_TRACE_FN_ENT_PRINT;
    if (iPlayItf) {
        XAresult res = (*iPlayItf)->SetPlayState(iPlayItf, XA_PLAYSTATE_PLAYING);
        // add error handling if res != 0 (call errorCB)
    }
    SF_TRACE_FN_EXT_PRINT;
}

void XARadioSessionImpl::Stop()
{
    SF_TRACE_FN_ENT_PRINT;
    if (iPlayItf) {
        XAresult res = (*iPlayItf)->SetPlayState(iPlayItf, XA_PLAYSTATE_STOPPED);
        // add error handling if res != 0 (call errorCB)
    }
    SF_TRACE_FN_EXT_PRINT;
}

QRadioTuner::Error XARadioSessionImpl::Error()
{
    SF_TRACE_FN_ENT_EXT_PRINT;
    return QRadioTuner::NoError;
}

//TInt XARadioSessionImpl::ErrorString();
//    {
//    SF_TRACE_FN_ENT_PRINT;

//    SF_TRACE_FN_EXT_PRINT;
//    }

void XARadioSessionImpl::StateChanged(QRadioTuner::State state)
{
    SF_TRACE_FN_ENT_PRINT;
    iState = state;
    iParent.CBStateChanged(state);
    SF_TRACE_FN_EXT_PRINT;
}

void XARadioSessionImpl::FrequencyChanged(XAuint32 freq)
{
    SF_TRACE_FN_ENT_PRINT;
    iParent.CBFrequencyChanged(freq);
    SF_TRACE_FN_EXT_PRINT;
}

void XARadioSessionImpl::SearchingChanged(TBool isSearching)
{
    SF_TRACE_FN_ENT_PRINT;
    iSearching = EFalse;
    iParent.CBSearchingChanged(isSearching);
    SF_TRACE_FN_EXT_PRINT;
}

void XARadioSessionImpl::StereoStatusChanged(TBool stereoStatus)
{
    SF_TRACE_FN_ENT_PRINT;
    iParent.CBStereoStatusChanged(stereoStatus);
    SF_TRACE_FN_EXT_PRINT;
}

void XARadioSessionImpl::SignalStrengthChanged(TBool stereoStatus)
{
    SF_TRACE_FN_ENT_PRINT;
    iParent.CBSignalStrengthChanged(stereoStatus);
    SF_TRACE_FN_EXT_PRINT;
}

void XARadioSessionImpl::VolumeChanged()
{
    SF_TRACE_FN_ENT_PRINT;
    int vol = 0;
    iParent.CBVolumeChanged(vol);
    SF_TRACE_FN_EXT_PRINT;
}

void XARadioSessionImpl::MutedChanged(TBool mute)
{
    SF_TRACE_FN_ENT_PRINT;
    iParent.CBMutedChanged(mute);
    SF_TRACE_FN_EXT_PRINT;
}

void EngineObjectCallback(XAObjectItf /*caller*/,
                          const void */*pContext*/,
#ifdef PLUGIN_SYMBIAN_TRACE_ENABLED
                          XAuint32 event,
#else
                          XAuint32 /*event*/,
#endif /*PLUGIN_SYMBIAN_TRACE_ENABLED*/
                          XAresult /*result*/,
                          XAuint32 /*param*/,
                          void */*pInterface*/)
{
    SF_PRINT2_LOG(_L("Engine object event: 0x%x\n"), (int)event);
}

void RadioCallback(XARadioItf /*caller*/,
                   void* pContext,
                   XAuint32 event,
                   XAuint32 eventIntData,
                   XAboolean eventBooleanData)
{
    XAuint32 freq;
    XAboolean stereoStatus(XA_BOOLEAN_FALSE);

    switch (event) {
    case XA_RADIO_EVENT_ANTENNA_STATUS_CHANGED:
        SF_PRINT1_LOG(_L("RadioCallback: XA_RADIO_EVENT_ANTENNA_STATUS_CHANGED"));
        // Qt API has no callback defined for this event.
        break;
    case XA_RADIO_EVENT_FREQUENCY_CHANGED:
        freq = eventIntData;
        SF_PRINT2_LOG(_L("RadioCallback: XA_RADIO_EVENT_FREQUENCY_CHANGED to: %d"), freq);
        if (pContext)
            ((XARadioSessionImpl*)pContext)->FrequencyChanged(freq);
        break;
    case XA_RADIO_EVENT_FREQUENCY_RANGE_CHANGED:
        SF_PRINT1_LOG(_L("RadioCallback: XA_RADIO_EVENT_FREQUENCY_RANGE_CHANGED"));
        // Qt API has no callback defined for this event.
        break;
    case XA_RADIO_EVENT_PRESET_CHANGED:
        SF_PRINT1_LOG(_L("RadioCallback: XA_RADIO_EVENT_PRESET_CHANGED"));
        // Qt API has no callback defined for this event.
        break;
    case XA_RADIO_EVENT_SEEK_COMPLETED:
        SF_PRINT1_LOG(_L("RadioCallback: XA_RADIO_EVENT_SEEK_COMPLETED"));
        if (pContext)
            ((XARadioSessionImpl*)pContext)->SearchingChanged(false);
        break;
     case XA_RADIO_EVENT_STEREO_STATUS_CHANGED:
        stereoStatus = eventBooleanData;
        SF_PRINT2_LOG(_L("RadioCallback: XA_RADIO_EVENT_STEREO_STATUS_CHANGED: %d"), stereoStatus);
        if (pContext)
            ((XARadioSessionImpl*)pContext)->StereoStatusChanged(stereoStatus);
        break;
    case XA_RADIO_EVENT_SIGNAL_STRENGTH_CHANGED:
        SF_PRINT1_LOG(_L("RadioCallback: XA_RADIO_EVENT_SIGNAL_STRENGTH_CHANGED"));
        if (pContext)
            ((XARadioSessionImpl*)pContext)->SignalStrengthChanged(stereoStatus);
        break;
    default:
        SF_PRINT1_LOG(_L("RadioCallback: default"));
        break;
    }
}

void NokiaVolumeExtItfCallback(XANokiaVolumeExtItf /*caller*/,
                               void* pContext,
                               XAuint32 event,
                               XAboolean eventBooleanData)
{
    XAboolean mute;
    switch (event) {
    case XA_NOKIAVOLUMEEXT_EVENT_MUTE_CHANGED:
        mute = eventBooleanData;
        SF_PRINT2_LOG(_L("NokiaVolumeExtItfCallback: XA_NOKIAVOLUMEEXT_EVENT_MUTE_CHANGED to: %d"), mute);
        if (pContext)
            ((XARadioSessionImpl*)pContext)->MutedChanged(mute);
        break;
    default:
        SF_PRINT1_LOG(_L("NokiaVolumeExtItfCallback: default"));
        break;
    }
}

void NokiaLinearVolumeItfCallback(XANokiaLinearVolumeItf /*caller*/,
                                  void* pContext,
                                  XAuint32 event,
                                  XAboolean /*eventBooleanData*/)
{
    switch (event) {
    case XA_NOKIALINEARVOLUME_EVENT_VOLUME_CHANGED:
        if (pContext)
            ((XARadioSessionImpl*)pContext)->VolumeChanged();
        break;
    default:
        SF_PRINT1_LOG(_L("NokiaLinearVolumeItfCallback: default"));
        break;
    }
}

void PlayItfCallbackForRadio(XAPlayItf /*caller*/,
                             void* pContext,
                             XAuint32 event)
{
    switch (event) {
    case XA_PLAYEVENT_HEADMOVING:
        if (pContext)
            ((XARadioSessionImpl*)pContext)->StateChanged(QRadioTuner::ActiveState);
        break;
    case XA_PLAYEVENT_HEADSTALLED:
        if (pContext)
            ((XARadioSessionImpl*)pContext)->StateChanged(QRadioTuner::StoppedState);
        break;
    default:
        SF_PRINT1_LOG(_L("NokiaLinearVolumeItfCallback: default"));
        break;
    }
}

TInt XARadioSessionImpl::CheckErr(XAresult res)
{
    TInt status(KErrGeneral);
    switch(res) {
    case XA_RESULT_SUCCESS:
        //SF_PRINT1_LOG(_L("XA_RESULT_SUCCESS"));
        status = KErrNone;
        break;
    case XA_RESULT_PRECONDITIONS_VIOLATED:
        SF_PRINT1_LOG(_L("XA_RESULT_PRECONDITIONS_VIOLATED"));
        break;
    case XA_RESULT_PARAMETER_INVALID:
        SF_PRINT1_LOG(_L("XA_RESULT_PARAMETER_INVALID"));
        break;
    case XA_RESULT_MEMORY_FAILURE:
        SF_PRINT1_LOG(_L("XA_RESULT_MEMORY_FAILURE"));
        iAvailabilityError = QtMultimediaKit::ResourceError;
        break;
    case XA_RESULT_RESOURCE_ERROR:
        SF_PRINT1_LOG(_L("XA_RESULT_RESOURCE_ERROR"));
        iAvailabilityError = QtMultimediaKit::ResourceError;
        break;
    case XA_RESULT_RESOURCE_LOST:
        SF_PRINT1_LOG(_L("XA_RESULT_RESOURCE_LOST"));
        iAvailabilityError = QtMultimediaKit::ResourceError;
        break;
    case XA_RESULT_IO_ERROR:
        SF_PRINT1_LOG(_L("XA_RESULT_IO_ERROR"));
        break;
    case XA_RESULT_BUFFER_INSUFFICIENT:
        SF_PRINT1_LOG(_L("XA_RESULT_BUFFER_INSUFFICIENT"));
        break;
    case XA_RESULT_CONTENT_CORRUPTED:
        SF_PRINT1_LOG(_L("XA_RESULT_CONTENT_CORRUPTED"));
        break;
    case XA_RESULT_CONTENT_UNSUPPORTED:
        SF_PRINT1_LOG(_L("XA_RESULT_CONTENT_UNSUPPORTED"));
        break;
    case XA_RESULT_CONTENT_NOT_FOUND:
        SF_PRINT1_LOG(_L("XA_RESULT_CONTENT_NOT_FOUND"));
        break;
    case XA_RESULT_PERMISSION_DENIED:
        SF_PRINT1_LOG(_L("XA_RESULT_PERMISSION_DENIED"));
        break;
    case XA_RESULT_FEATURE_UNSUPPORTED:
        SF_PRINT1_LOG(_L("XA_RESULT_FEATURE_UNSUPPORTED"));
        break;
    case XA_RESULT_INTERNAL_ERROR:
        SF_PRINT1_LOG(_L("XA_RESULT_INTERNAL_ERROR"));
        break;
    case XA_RESULT_UNKNOWN_ERROR:
        SF_PRINT1_LOG(_L("XA_RESULT_UNKNOWN_ERROR"));
        break;
    case XA_RESULT_OPERATION_ABORTED:
        SF_PRINT1_LOG(_L("XA_RESULT_OPERATION_ABORTED"));
        break;
    case XA_RESULT_CONTROL_LOST:
        SF_PRINT1_LOG(_L("XA_RESULT_CONTROL_LOST"));
        break;
    default:
        SF_PRINT1_LOG(_L("Unknown Error!!!"));
    }
    return status;
}
