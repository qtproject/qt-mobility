#include "qwmpplayercontrol.h"

#include <QtCore/qurl.h>

QWmpPlayerControl::QWmpPlayerControl(IWMPCore3 *player, QObject *parent)
        : QMediaPlayerControl(parent)
        , m_player(player)
        , m_controls(0)
        , m_settings(0)
{
    m_player->get_controls(&m_controls);
    m_player->get_settings(&m_settings);
    m_player->get_network(&m_network);

}

QWmpPlayerControl::~QWmpPlayerControl()
{
    // if (m_controls) m_controls->Release(); ?
    // if (m_settings) m_settings->Release(); ?
}

qint64 QWmpPlayerControl::position() const
{
    double position;

    if (m_controls)
        m_controls->get_currentPosition(&position);

    return position;
}

int QWmpPlayerControl::bufferingProgress() const
{
    long progress = 0;

    if (m_network)
        m_network->get_bufferingProgress(&progress);

    return progress;
}


int QWmpPlayerControl::volume() const
{
    long volume = 0;

    if (m_settings)
        m_settings->get_volume(&volume);

    return volume;
}

bool QWmpPlayerControl::isMuted() const
{
    VARIANT_BOOL mute = FALSE;

    if (m_settings)
        m_settings->get_mute(&mute);

    return mute;
}

QUrl QWmpPlayerControl::url() const
{
    BSTR string;
    if (m_player && m_player->get_URL(&string) == S_OK) {
        QUrl url(QString::fromWCharArray(string, SysStringLen(string)), QUrl::StrictMode);

        SysFreeString(string);

        return url;
    } else {
        return QUrl();
    }
}

void QWmpPlayerControl::setUrl(const QUrl &url)
{
    if (m_player) {
        BSTR string = SysAllocString(reinterpret_cast<const wchar_t *>(url.toString().unicode()));

        m_player->put_URL(string);

        SysFreeString(string);
    }
}

void QWmpPlayerControl::play()
{
    if (m_controls)
        m_controls->play();
}

void QWmpPlayerControl::pause()
{
    if (m_controls)
        m_controls->pause();
}

void QWmpPlayerControl::stop()
{
    if (m_controls)
        m_controls->stop();
}


void QWmpPlayerControl::setVolume(int volume)
{
    if (m_settings)
        m_settings->put_volume(volume);
}

void QWmpPlayerControl::setMuted(bool muted)
{
    if (m_settings)
        m_settings->put_mute(muted ? TRUE : FALSE);
}
