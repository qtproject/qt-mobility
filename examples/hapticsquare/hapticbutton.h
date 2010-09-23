#ifndef HAPTICBUTTON_H
#define HAPTICBUTTON_H

#include <QWidget>

class HapticButton : public QWidget
{
    Q_OBJECT
public:
    explicit HapticButton(const QString &label);

protected:
    void mousePressEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *e);

private:
    QString m_label;
    bool m_checked;
    bool m_checkable;

signals:
    void clicked();
    void toggled(bool on);

public slots:
    void setCheckable(bool isCheckable) { m_checkable=isCheckable; }

};

#endif // HAPTICBUTTON_H
