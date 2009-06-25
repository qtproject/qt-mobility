#ifndef QWMPPLAYERSERVICE_H
#define QWMPPLAYERSERVICE_H

#include <QtCore/qobject.h>

#include "qwmpevents.h"
#include "qwmpglobal.h"

#include <wmp.h>

class QMediaMetaData;
class QMediaPlayerControl;

class Q_WMP_EXPORT QMediaPlayerService : public QObject
{
    Q_OBJECT
public:
    QMediaPlayerService(QObject *parent = 0) : QObject(parent) {}

    virtual QWidget *createWidget() = 0;

    virtual QObject *videoOutput() const = 0;
    virtual void setVideoOutput(QObject *output) = 0;

    virtual QMediaPlayerControl *control() = 0;
    virtual QMediaMetaData *metaData() = 0;
};

class QWmpMetaData;
class QWmpPlayerControl;

class Q_WMP_EXPORT QWmpPlayerService : public QMediaPlayerService, public QWmpEvents
{
    Q_OBJECT
public:
    QWmpPlayerService(QObject *parent = 0);
    ~QWmpPlayerService();

    QMediaPlayerControl *control();
    QMediaMetaData *metaData();

    QWidget *createWidget();

    QObject *videoOutput() const;
    void setVideoOutput(QObject *output);

    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **object);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IWMPEvents
    void STDMETHODCALLTYPE PlayStateChange(long NewState);
    void STDMETHODCALLTYPE Buffering(VARIANT_BOOL Start);
    void STDMETHODCALLTYPE PositionChange(double oldPosition, double newPosition);
    void STDMETHODCALLTYPE CurrentItemChange(IDispatch *pdispMedia);

private:
    volatile LONG m_ref;
    IWMPPlayer4 *m_player;
    QObject *m_videoOutput;
    QWmpPlayerControl *m_control;
    QWmpMetaData *m_metaData;
    IConnectionPoint *m_connectionPoint;
    DWORD m_adviseCookie;


};

#endif
