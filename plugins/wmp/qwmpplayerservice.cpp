#include "qwmpplayerservice.h"

#include "qwmpplayercontrol.h"

QWmpPlayerService::QWmpPlayerService(QObject *parent)
    : QMediaPlayerService(parent)
    , m_control(0)
    , m_player(0)
{
    if (S_OK == CoCreateInstance(
            __uuidof(WindowsMediaPlayer),
            0,
            CLSCTX_INPROC_SERVER,
            __uuidof(IWMPPlayer4),
            reinterpret_cast<void **>(&m_player))) {
        m_control = new QWmpPlayerControl(m_player, this);
    }
}

QWmpPlayerService::~QWmpPlayerService()
{
    delete m_control;

    if (m_player)
        m_player->Release();
}

QMediaPlayerControl *QWmpPlayerService::control()
{
    return m_control;
}
