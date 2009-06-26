#include "qwmpplayerservice.h"

#include "qevrwidget.h"
#include "qwmpmetadata.h"
#include "qwmpplayercontrol.h"

#include <QtCore/qvariant.h>

#include <wmprealestate.h>

QWmpPlayerService::QWmpPlayerService(QObject *parent)
    : QMediaPlayerService(parent)
    , m_ref(1)
    , m_player(0)
    , m_videoOutput(0)
    , m_control(0)
    , m_metaData(0)
    , m_connectionPoint(0)
    , m_adviseCookie(0)
{
    qRegisterMetaType<IMFActivate *>();

    if (S_OK == CoCreateInstance(
            __uuidof(WindowsMediaPlayer),
            0,
            CLSCTX_INPROC_SERVER,
            __uuidof(IWMPPlayer4),
            reinterpret_cast<void **>(&m_player))) {
        m_control = new QWmpPlayerControl(m_player, this);
        m_metaData = new QWmpMetaData(this);

        IConnectionPointContainer *container = 0;

        if (m_player->QueryInterface(
                IID_IConnectionPointContainer, reinterpret_cast<void **>(&container)) != S_OK) {
            qWarning("No connection point container");
        } else {
            if (container->FindConnectionPoint(__uuidof(IWMPEvents), &m_connectionPoint) != S_OK) {
                qWarning("No connection point for IWMPEvents");
            } else if (m_connectionPoint->Advise(this, &m_adviseCookie) != S_OK) {
                qWarning("Failed to link to connection point");

                m_connectionPoint->Release();
                m_connectionPoint = 0;
            }
            container->Release();
        }   
    }
}

QWmpPlayerService::~QWmpPlayerService()
{
    if (m_connectionPoint) {
        m_connectionPoint->Unadvise(m_adviseCookie);
        m_connectionPoint->Release();
    }

    delete m_control;

    if (m_player)
        m_player->Release();

    Q_ASSERT(m_ref == 1);
}

QMediaPlayerControl *QWmpPlayerService::control()
{
    return m_control;
}

QMediaMetaData *QWmpPlayerService::metaData()
{
    return m_metaData;
}

QWidget *QWmpPlayerService::createWidget()
{
    return new QEvrWidget;
}

QObject *QWmpPlayerService::videoOutput() const
{
    return m_videoOutput;
}

void QWmpPlayerService::setVideoOutput(QObject *output)
{
    m_videoOutput = output;

    IWMPVideoRenderConfig *config = 0;

    if (m_player && m_player->QueryInterface(
            __uuidof(IWMPVideoRenderConfig), reinterpret_cast<void **>(&config)) == S_OK) {
        IMFActivate *activate = 0;

        if (m_videoOutput)
            activate = qvariant_cast<IMFActivate *>(m_videoOutput->property("activate"));

        config->put_presenterActivate(activate);
        config->Release();
    }
}

// IUnknown
HRESULT QWmpPlayerService::QueryInterface(REFIID riid, void **object)
{
    if (riid == __uuidof(IUnknown)
            || riid == __uuidof(IWMPEvents)
            || riid == __uuidof(IWMPEvents2)
            || riid == __uuidof(IWMPEvents3)) {
        AddRef();

        *object = static_cast<IWMPEvents3 *>(this);

        return S_OK;
    } else {
        return E_NOINTERFACE;
    }
}

ULONG QWmpPlayerService::AddRef()
{
    return InterlockedIncrement(&m_ref);
}

ULONG QWmpPlayerService::Release()
{
    ULONG ref = InterlockedDecrement(&m_ref);

    Q_ASSERT(ref != 0);

    return ref;
}

// IWMPEvents
void QWmpPlayerService::PlayStateChange(long NewState)
{
    switch (NewState) {
    case wmppsStopped:
        m_control->setState(QMediaPlayerControl::Stopped);
        break;
    case wmppsPaused:
        m_control->setState(QMediaPlayerControl::Paused);
        break;
    case wmppsPlaying:
        m_control->setState(QMediaPlayerControl::Playing);
        break;
    default:
        break;
    }
}

void QWmpPlayerService::Buffering(VARIANT_BOOL Start)
{
    m_control->setBuffering(Start);
}

void QWmpPlayerService::PositionChange(double oldPosition, double newPosition)
{
    Q_UNUSED(oldPosition);

    m_control->positionChanged(newPosition);
}

void QWmpPlayerService::CurrentItemChange(IDispatch *pdispMedia)
{
    Q_UNUSED(pdispMedia);

    IWMPMedia *media = 0;

    if (m_player->get_currentMedia(&media) == S_OK) {
        double duration = 0;

        media->get_duration(&duration);

        m_control->setDuration(duration);

        m_metaData->setMedia(media);

        media->Release();
    }

    m_control->currentMediaChanged();
}
