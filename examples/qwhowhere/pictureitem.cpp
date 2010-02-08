/*
 * Copyright (c) 2010 Nokia Corporation.
 */


#include "pictureitem.h"

PictureItem::PictureItem(QSizeF size, const QPixmap& p, QObject* parent) :
        QObject(parent), QGraphicsPixmapItem(p)
{
    m_size = size;
    QPixmap scaledPixmap = pixmap().scaled(m_size.toSize(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    setPixmap(scaledPixmap);
    setCacheMode(DeviceCoordinateCache);
    m_hide = false;
}

PictureItem::~PictureItem()
{
}

QRectF PictureItem::boundingRect() const
{
    return QRectF(0, 0, m_size.width(), m_size.height());
}

qreal PictureItem::rotationAngleY() const
{
    return m_rotationAngleY;
}

void PictureItem::setRotationAngleY(qreal angle)
{
    if (m_rotationAngleY != angle) {
        m_rotationAngleY = angle;
        QPointF c = boundingRect().center();
        QTransform t;
        t.translate(c.x(), c.y());
        t.rotate(m_rotationAngleY, Qt::YAxis);
        t.translate(-c.x(), -c.y());
        setTransform(t);
    }
}

void PictureItem::setFade(int f)
{
    m_fade = f;

    if ((m_fade == 255 || m_fade == 0) && m_originalPicForFade.isNull()) {
        m_originalPicForFade = pixmap();
    }

    QPixmap pic;
    if (m_originalPicForFade.isNull())
        pic = pixmap();
    else
        pic = m_originalPicForFade;

    QPixmap transparent(pic.size());
    transparent.fill(Qt::transparent);
    QPainter p(&transparent);
    p.setCompositionMode(QPainter::CompositionMode_Source);
    p.drawPixmap(0, 0, pic);
    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p.fillRect(transparent.rect(), QColor(0, 0, 0, m_fade));
    p.end();
    pic = transparent;
    transparent = 0;

    setPixmap(pic);
}

int PictureItem::fade() const
{
    return m_fade;
}

void PictureItem::animatePosition(const QPointF& start, const QPointF& end, int duration)
{
    // Start animate this class
    QPropertyAnimation* anim = new QPropertyAnimation(this, "pos");

    anim->setDuration(duration);
    // position to start animation
    anim->setStartValue(start);
    // end position of animation
    anim->setEndValue(end);
    // easing curve
    anim->setEasingCurve(QEasingCurve::OutElastic);

    // Listen animation finished signal
    QObject::connect(anim, SIGNAL(finished()), this, SLOT(animationFinished()));

    // Start animation and delete QPropertyAnimation class on the end
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

QPropertyAnimation* PictureItem::animateAngle(int startAngle, int endAngle, bool loop)
{
    // Start animate this class
    QPropertyAnimation* anim = new QPropertyAnimation(this, "rotationAngleY");

    if (loop)
        anim->setLoopCount(-1);

    // 3 second duration animation
    anim->setDuration(3000);
    // angle to start animation
    anim->setStartValue(startAngle);
    // end angle of animation
    anim->setEndValue(endAngle);
    // easing curve
    anim->setEasingCurve(QEasingCurve::Linear);

    // Listen animation finished signal
    QObject::connect(anim, SIGNAL(finished()), this, SLOT(animationFinished()));

    // Start animation and delete QPropertyAnimation class on the end
    anim->start(QAbstractAnimation::DeleteWhenStopped);

    return anim;
}

void PictureItem::setHideWhenFinished()
{
    m_hide = true;
}

void PictureItem::stop(QPropertyAnimation* anim)
{
    anim->stop();
    QTransform t;
    setTransform(t);
    update();
}

void PictureItem::animatePosAndAngle(const QPointF& start, const QPointF& end,
                                     int startAngle, int endAngle)
{
    // Create animation group
    // Animations in the group are executed in same time
    QParallelAnimationGroup *group = new QParallelAnimationGroup;

    // Create rotation animation
    QPropertyAnimation* anim1 = new QPropertyAnimation(this, "rotationAngleY");
    anim1->setDuration(3000);
    anim1->setStartValue(startAngle);
    anim1->setEndValue(endAngle);
    anim1->setEasingCurve(QEasingCurve::InOutBack);

    // Create position animation
    QPropertyAnimation* anim2 = new QPropertyAnimation(this, "pos");
    anim2->setDuration(3000);
    anim2->setStartValue(start);
    anim2->setEndValue(end);
    anim2->setEasingCurve(QEasingCurve::InOutElastic);

    // Add animations into the group
    group->addAnimation(anim1);
    group->addAnimation(anim2);

    // Execute group
    group->start(QAbstractAnimation::DeleteWhenStopped);
}


void PictureItem::animateFadeIn()
{
    // Start animate this class
    QPropertyAnimation* anim = new QPropertyAnimation(this, "fade");
    anim->setDuration(1000);
    anim->setStartValue(255);
    anim->setEndValue(0);
    anim->setEasingCurve(QEasingCurve::Linear);

    QObject::connect(anim, SIGNAL(finished()), this, SLOT(animationFinished()));

    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void PictureItem::animateFadeOut()
{
    // Start animate this class
    QPropertyAnimation* anim = new QPropertyAnimation(this, "fade");
    anim->setDuration(1000);
    anim->setStartValue(0);
    anim->setEndValue(255);
    anim->setEasingCurve(QEasingCurve::Linear);

    QObject::connect(anim, SIGNAL(finished()), this, SLOT(animationFinished()));

    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void PictureItem::animateScale(qreal start, qreal end)
{
    // Start animate this class
    QPropertyAnimation* anim = new QPropertyAnimation(this, "scale");
    anim->setDuration(1000);
    anim->setStartValue(start);
    anim->setEndValue(end);
    anim->setEasingCurve(QEasingCurve::InOutBack);

    QObject::connect(anim, SIGNAL(finished()), this, SLOT(animationFinished()));

    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void PictureItem::animationFinished()
{
    if (m_hide) {
        this->setVisible(false);
        m_hide = false;
    }
    // This slot is called when animation ends
}
