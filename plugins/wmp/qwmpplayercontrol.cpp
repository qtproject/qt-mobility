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

}

QWmpPlayerControl::~QWmpPlayerControl()
{
    // if (m_controls) m_controls->Release(); ?
    // if (m_settings) m_settings->Release(); ?
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
        BSTR string = SysAllocString(reinterpret_cast<const wchar_t *>(url.toString().utf16()));

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
