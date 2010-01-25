
#include "connectivityhelper.h"

#include <QMessageBox>
#include <QWidget>
#include <QTimer>

ConnectivityHelper::ConnectivityHelper(QNetworkSession *session, QWidget *parent) : QObject(parent),
        m_session(session)
{
    connect(m_session,
            SIGNAL(error(QNetworkSession::SessionError)),
            this,
            SLOT(error(QNetworkSession::SessionError)));
}

void ConnectivityHelper::error(QNetworkSession::SessionError error)
{
    if (error == QNetworkSession::UnknownSessionError) {
        if (m_session->state() == QNetworkSession::Connecting) {
            QMessageBox msgBox(qobject_cast<QWidget *>(parent()));
            msgBox.setText("This application requires network access to function.");
            msgBox.setInformativeText("Press Cancel to quit the application.");
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Retry);
            int ret = msgBox.exec();
            if (ret == QMessageBox::Retry) {
                QTimer::singleShot(0, m_session, SLOT(open()));
            } else if (ret == QMessageBox::Cancel) {
                emit networkingCancelled();
            }
        }
    } else if (error == QNetworkSession::SessionAbortedError) {
        QMessageBox msgBox(qobject_cast<QWidget *>(parent()));
        msgBox.setText("Out of range of network.");
        msgBox.setInformativeText("Move back into range and press Retry, or press Cancel to quit the application.");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Retry);
        int ret = msgBox.exec();
        if (ret == QMessageBox::Retry) {
            QTimer::singleShot(0, m_session, SLOT(open()));
        } else if (ret == QMessageBox::Cancel) {
            emit networkingCancelled();
        }
    }
}
