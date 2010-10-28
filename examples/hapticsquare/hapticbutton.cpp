#include "hapticbutton.h"
#include <QPainter>

HapticButton::HapticButton(const QString &label) :
    QWidget(0), m_label(label), m_checked(false), m_checkable(false)
{
    setMinimumSize(100, 100);
}

void HapticButton::mousePressEvent(QMouseEvent *)
{
    if (m_checkable) {
        m_checked = !m_checked;
        emit toggled(m_checked);
    } else {
        emit clicked();
    }
}

void HapticButton::paintEvent(QPaintEvent *)
{
    QPainter paint(this);

    QRect r(1, 1, width()-2, height()-2);
    paint.drawRoundedRect(r, 10, 10);
    paint.drawText(r, Qt::AlignCenter, m_label);
}
