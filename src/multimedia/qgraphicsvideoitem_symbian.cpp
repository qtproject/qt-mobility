/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtCore/QDebug>
#include <QtCore/QEvent>
#include <QtCore/QPointer>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsView>
#include <QtGui/QPainter>

#include <qgraphicsvideoitem.h>
#include <qmediaobject.h>
#include <qmediaservice.h>
#include <qmediaservice.h>
#include <qvideorenderercontrol.h>
#include <qvideosurfaceformat.h>
#include <qvideowidgetcontrol.h>

#ifndef QT_NO_EGL
#include <qeglimagevideosurface_symbian_p.h>
#include <EGL/egl.h>
#endif

// This graphics item can operate in either of two modes: "direct" and "renderer".
// These correspond to the backend control APIs used to render the content
// into the QPaintDevice: QVideoWidgetControl and QVideoRendererControl.
//
// Which mode to use depends on a custom property _q_preferredVideoRenderingPath,
// of type QString, which can be set on an ancestor of the QGraphicsVideoItem
// (e.g. on the QGraphicsView).  This property may take the following values:
//
// auto:     Automatically switch between "widget" and "renderer" modes
// direct:   Always use "direct" mode
// renderer: Use "renderer" mode if available when the item is created.  Do
//           not subsequently switch to "direct" even if fast-path conditions
//           are met.
//
// In 'auto' mode, if all the following conditions are met, 'renderer' is used;
// otherwise 'direct' is used.
// * graphics system is opengl or openvg
// * current media service is mediaplayer (not camera)
// * current media service provides QVideoRendererControl
// * video item extent is less than full screen
// * paint device is a QWidget


//-----------------------------------------------------------------------------
// Namespace
//-----------------------------------------------------------------------------

Q_DECLARE_METATYPE(WId)
Q_DECLARE_METATYPE(QVideoSurfaceFormat)

static const QEvent::Type EnableViewportTransparencyEvent =
    static_cast<QEvent::Type>(QEvent::registerEventType());

QT_BEGIN_NAMESPACE

class QGraphicsVideoItemImplBase;
class QGraphicsVideoItemWidgetImpl;

#ifndef QT_NO_EGL
class QGraphicsVideoItemEglRendererImpl;
#endif

//-----------------------------------------------------------------------------
// Debugging macros
//-----------------------------------------------------------------------------

// Expand the bounding rectangle to be the same size as the item's size, and
// fill non-video areas with black borders
//#define GRAPHICSVIDEOITEM_SHOW_BORDERS

// Draw a dashed line around the item's bounding rectangle
//#define GRAPHICSVIDEOITEM_SHOW_RECTS


//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

static const char *PreferredRenderingPathProperty = "_q_preferredVideoRenderingPath";
static const char *CurrentRenderingPathProperty   = "_q_currentVideoRenderingPath";
static const QString RenderingPathAuto     = "auto";
static const QString RenderingPathDefault  = "default";
static const QString RenderingPathRenderer = "renderer";
static const QString RenderingPathDirect   = "direct";
static const QString DefaultPreferredRenderingPath  = RenderingPathAuto;


//-----------------------------------------------------------------------------
// QGraphicsVideoItemPrivate
//-----------------------------------------------------------------------------

class QGraphicsVideoItemPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(QGraphicsVideoItem)
public:
    QGraphicsVideoItemPrivate(QGraphicsVideoItem *parent);
    ~QGraphicsVideoItemPrivate();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);
    void itemChange(QGraphicsVideoItem::GraphicsItemChange change,
                    const QVariant &value);
    QMediaObject *mediaObject() const;
    bool setMediaObject(QMediaObject *mediaObject);
    Qt::AspectRatioMode aspectRatioMode() const;
    void setAspectRatioMode(Qt::AspectRatioMode mode);
    QPointF offset() const;
    void setOffset(const QPointF &offset);
    QSizeF size() const;
    void setSize(const QSizeF &size);
    QRectF rect() const;
    QRectF boundingRect() const;
    QSize nativeSize() const;

    void prepareGeometryChange();

    void _q_present();
    void _q_updateNativeSize();
    void _q_serviceDestroyed();
    void _q_mediaObjectDestroyed();

    static QGraphicsVideoItemPrivate *getPtrHelper(QGraphicsVideoItem *item);

private slots:
    void screenSizeChanged();
    void rendererControlError();

private:
    friend class QGraphicsVideoItemImplBase;
    QGraphicsVideoItemImplBase* impl();
    const QGraphicsVideoItemImplBase* impl() const;
    QString getPreferredRenderingPath() const;
    bool setViewport(QGraphicsView *view);
    bool setTransform(const QTransform &transform);
    bool setWidgetPaintDevice(bool widget);
    void restoreViewportState();
    void geometryChanged();
    void determineGraphicsSystem();
    bool useDirectMode() const;
    void destroyImpl();
    void createImpl();
    void createImpl(bool direct);
    void recreateImpl(bool force = false);

private:
    friend class QGraphicsVideoItemWidgetImpl;
#ifndef QT_NO_EGL
    friend class QGraphicsVideoItemEglRendererImpl;
#endif
    QGraphicsVideoItem *q_ptr;
    QGraphicsVideoItemImplBase *m_impl;
    bool m_isDirect;
    QMediaService *m_service;
    bool m_serviceSupportsRendererControl;
    bool m_rendererControlError;
    QMediaObject *m_mediaObject;
    Qt::AspectRatioMode m_aspectRatioMode;
    QPointer<QGraphicsView> m_viewport;
    QGraphicsView::ViewportUpdateMode m_savedViewportUpdateMode;
    bool m_viewportTransparencyEnabled;
    QTransform m_transform;
    QRectF m_rect;
    QRectF m_boundingRect;
    QSize m_nativeSize;
    QSize m_screenSize;
    bool m_widgetPaintDevice;
    QString m_graphicsSystem;
    bool m_geometryChanged;
};


//-----------------------------------------------------------------------------
// QGraphicsVideoItemImplBase
//-----------------------------------------------------------------------------

class QGraphicsVideoItemImplBase : public QObject
{
    Q_OBJECT
public:
    ~QGraphicsVideoItemImplBase();

    virtual void paint(QPainter *painter, QWidget *widget) = 0;
    virtual void itemChange(QGraphicsVideoItem::GraphicsItemChange change,
                            const QVariant &value) = 0;
    virtual void present() = 0;
    virtual void updateNativeSize() = 0;
    virtual void viewportChanged();
    virtual void aspectRatioModeChanged();
    virtual bool isReadyToPaint() const;
    void updateGeometry();

protected:
    Q_DECLARE_PUBLIC(QGraphicsVideoItemPrivate)
    inline QGraphicsVideoItem* qq_func() { return q_ptr->q_func(); }
    inline const QGraphicsVideoItem* qq_func() const { return q_ptr->q_func(); }
    QGraphicsVideoItemImplBase(QGraphicsVideoItemPrivate *parent);
    virtual void doUpdateGeometry() = 0;

private:
    void customEvent(QEvent *event);

protected:
    QGraphicsVideoItemPrivate *q_ptr;
    QRectF m_contentRect;
    // The following rectangles are in global (screen) coordinates
    QRect m_screenRect;
    QRect m_boundingScreenRect;
    QRect m_contentScreenRect;
    QRect m_viewportScreenRect;
};

QGraphicsVideoItemImplBase::QGraphicsVideoItemImplBase(QGraphicsVideoItemPrivate *parent)
    :   QObject(parent)
    ,   q_ptr(parent)
{

}

QGraphicsVideoItemImplBase::~QGraphicsVideoItemImplBase()
{
    Q_Q(QGraphicsVideoItemPrivate);
    q->restoreViewportState();
}

// Helper
QRect toScreen(const QRectF &rect, const QTransform &transform, const QWidget *viewport)
{
    const QRectF screenRectF = transform.mapRect(rect);
    const QRect screenRect(screenRectF.topLeft().toPoint(), screenRectF.size().toSize());
    return QRect(viewport->mapToGlobal(screenRect.topLeft()), screenRect.size());
}

void QGraphicsVideoItemImplBase::updateGeometry()
{
    Q_Q(QGraphicsVideoItemPrivate);
    q->prepareGeometryChange();
    QSizeF videoSize;
    if (q->m_nativeSize.isEmpty()) {
        videoSize = q->m_rect.size();
    } else if (q->m_aspectRatioMode == Qt::IgnoreAspectRatio) {
        videoSize = q->m_rect.size();
    } else {
        // KeepAspectRatio or KeepAspectRatioByExpanding
        videoSize = q->m_nativeSize;
        videoSize.scale(q->m_rect.size(), q->m_aspectRatioMode);
    }
    m_contentRect = QRectF(QPointF(0, 0), videoSize);
#ifdef GRAPHICSVIDEOITEM_SHOW_BORDERS
    q->m_boundingRect = q->m_rect;
#else
    m_contentRect.moveCenter(q->m_rect.center());
    q->m_boundingRect = m_contentRect.intersected(q->m_rect);
#endif
    m_boundingScreenRect = QRect();
    m_contentScreenRect = QRect();
    m_viewportScreenRect = QRect();
    if (q->m_viewport) {
        m_screenRect = toScreen(q->m_rect, q->m_transform, q->m_viewport);
        m_boundingScreenRect = toScreen(q->m_boundingRect, q->m_transform, q->m_viewport);
        m_contentScreenRect = toScreen(m_contentRect, q->m_transform, q->m_viewport);
        m_viewportScreenRect = QRect(q->m_viewport->viewport()->mapToGlobal(QPoint(0, 0)),
                                     q->m_viewport->viewport()->size());
    }
    VERBOSE_TRACE("QGraphicsVideoItemImplBase::updateGeometry" << qtThisPtr()
                  << "nativeSize" << q->m_nativeSize
                  << "rect" << q->m_rect
                  << "aspectRatioMode" << q->m_aspectRatioMode);
    VERBOSE_TRACE("QGraphicsVideoItemImplBase::updateGeometry" << qtThisPtr()
                  << "boundingRect" << q->m_boundingRect
                  << "contentRect" << m_contentRect);
    VERBOSE_TRACE("QGraphicsVideoItemImplBase::updateGeometry" << qtThisPtr()
                  << "screenRect" << m_screenRect
                  << "boundingScreenRect" << m_boundingScreenRect
                  << "contentScreenRect" << m_contentScreenRect
                  << "viewportScreenRect" << m_viewportScreenRect);
    doUpdateGeometry();
}

void QGraphicsVideoItemImplBase::viewportChanged()
{
    // Default implementation does nothing
}

void QGraphicsVideoItemImplBase::aspectRatioModeChanged()
{
    // Default implementation does nothing
}

bool QGraphicsVideoItemImplBase::isReadyToPaint() const
{
    return true;
}

void QGraphicsVideoItemImplBase::customEvent(QEvent *event)
{
    Q_Q(QGraphicsVideoItemPrivate);
    if (event->type() == EnableViewportTransparencyEvent && q->m_viewport) {
        TRACE("QGraphicsVideoItemImplBase::customEvent" << qtThisPtr()
              << "EnableViewportTransparencyEvent");
        q->m_viewport->window()->setAttribute(Qt::WA_TranslucentBackground);
        q->m_viewport->window()->update();
        q->m_viewportTransparencyEnabled = true;
    }
    QObject::customEvent(event);
}


//-----------------------------------------------------------------------------
// QGraphicsVideoItemWidgetImpl
//-----------------------------------------------------------------------------

class QGraphicsVideoItemWidgetImpl : public QGraphicsVideoItemImplBase
{
    Q_OBJECT
public:
    QGraphicsVideoItemWidgetImpl(QGraphicsVideoItemPrivate *parent);
    ~QGraphicsVideoItemWidgetImpl();

    // QGraphicsVideoItemImplBase
    void paint(QPainter *painter, QWidget *widget);
    void itemChange(QGraphicsVideoItem::GraphicsItemChange change,
                    const QVariant &value);
    void present();
    void updateNativeSize();
    bool hasContent() const;

    bool eventFilter(QObject *watched, QEvent *event);

private slots:
    void updateItemAncestors();
    void updateWidgetOrdinalPosition();
    void hasContentChanged();

private:
    // QGraphicsVideoItemImplBase
    void doUpdateGeometry();
    void viewportChanged();
    void aspectRatioModeChanged();

    void setVisible(bool visible);
    void setWithinViewBounds(bool within);
    void updateTopWinId();
    void updateViewportAncestorEventFilters();
    void updateWidgetVisibility();
    QWidget *videoWidget() const;

private:
    QVideoWidgetControl *m_widgetControl;
    QList<QPointer<QObject> > m_viewportAncestors;
    QList<QPointer<QObject> > m_itemAncestors;
    bool m_visible;
    bool m_withinViewBounds;
};

QGraphicsVideoItemWidgetImpl::QGraphicsVideoItemWidgetImpl(QGraphicsVideoItemPrivate *parent)
    :   QGraphicsVideoItemImplBase(parent)
    ,   m_widgetControl(0)
    ,   m_visible(true)
    ,   m_withinViewBounds(false)
{
    Q_Q(QGraphicsVideoItemPrivate);
    TRACE("QGraphicsVideoItemWidgetImpl::QGraphicsVideoItemWidgetImpl" << qtThisPtr());
    qRegisterMetaType<WId>("WId");
    m_visible = qq_func()->isVisible();
    updateItemAncestors();
    m_widgetControl = qobject_cast<QVideoWidgetControl *>(
        q->m_service->requestControl(QVideoWidgetControl_iid));
    if (m_widgetControl) {
        connect(m_widgetControl, SIGNAL(nativeSizeChanged()),
                qq_func(), SLOT(_q_updateNativeSize()));
        connect(m_widgetControl, SIGNAL(hasContentChanged()),
                this, SLOT(hasContentChanged()));
        m_widgetControl->setAspectRatioMode(q->aspectRatioMode());
        updateGeometry();
        updateTopWinId();
        // Delay invocation of updateWidgetOrdinalPosition until after construction is
        // complete, because it relies on this item having been added to the
        // QGraphicsScene
        QMetaObject::invokeMethod(this, "updateWidgetOrdinalPosition", Qt::QueuedConnection);
        updateWidgetVisibility();
    }
}

QGraphicsVideoItemWidgetImpl::~QGraphicsVideoItemWidgetImpl()
{
    TRACE("QGraphicsVideoItemWidgetImpl::~QGraphicsVideoItemWidgetImpl" << qtThisPtr());
    Q_Q(QGraphicsVideoItemPrivate);
    if (m_widgetControl) {
        m_widgetControl->disconnect();
        if (q->m_service)
            q->m_service->releaseControl(m_widgetControl);
    }
}

void QGraphicsVideoItemWidgetImpl::paint(QPainter *painter, QWidget *widget)
{
    VERBOSE_TRACE("QGraphicsVideoItemWidgetImpl::paint" << qtThisPtr());
    Q_Q(QGraphicsVideoItemPrivate);
    if (widget && !widget->window()->testAttribute(Qt::WA_TranslucentBackground)) {
        // On Symbian, setting Qt::WA_TranslucentBackground can cause the
        // current window surface to be replaced.  Because of this, it cannot
        // safely be changed from the context of the viewport paintEvent(), so we
        // queue a custom event to set the attribute.
        QEvent *event = new QEvent(EnableViewportTransparencyEvent);
        QCoreApplication::instance()->postEvent(this, event);
    }
    const QPainter::CompositionMode oldCompositionMode = painter->compositionMode();
    painter->setCompositionMode(QPainter::CompositionMode_Source);
    const QColor color = hasContent() ? Qt::transparent : Qt::black;
    painter->fillRect(q->boundingRect(), color);
    painter->setCompositionMode(oldCompositionMode);
}

void QGraphicsVideoItemWidgetImpl::itemChange(QGraphicsVideoItem::GraphicsItemChange change,
                                                  const QVariant &value)
{
    Q_Q(QGraphicsVideoItemPrivate);
    switch (change) {
    case QGraphicsVideoItem::ItemScenePositionHasChanged:
        VERBOSE_TRACE("QGraphicsVideoItemWidgetImpl::itemChange" << qtThisPtr()
                      << "ItemScenePositionHasChanged" << value);
        qq_func()->update(q->boundingRect());
        break;
    case QGraphicsVideoItem::ItemVisibleChange:
        TRACE("QGraphicsVideoItemWidgetImpl::itemChange" << qtThisPtr()
              << "ItemVisibleChange" << value);
        setVisible(value.toBool());
        break;
    case QGraphicsVideoItem::ItemZValueHasChanged:
        TRACE("QGraphicsVideoItemWidgetImpl::itemChange" << qtThisPtr()
              << "ItemZValueHasChanged" << value);
        updateWidgetOrdinalPosition();
        break;
    default:
        break;
    }
}

void QGraphicsVideoItemWidgetImpl::present()
{
    // Do nothing
}

void QGraphicsVideoItemWidgetImpl::updateNativeSize()
{
    Q_Q(QGraphicsVideoItemPrivate);
    QSize size;
    if (m_widgetControl)
        size = m_widgetControl->property("nativeSize").toSize();
    TRACE("QGraphicsVideoItemWidgetImpl::updateNativeSize" << qtThisPtr()
          << "size" << size);
    if (q->m_nativeSize != size)
        q->m_nativeSize = size;
}

bool QGraphicsVideoItemWidgetImpl::hasContent() const
{
    return m_widgetControl && m_widgetControl->property("hasContent").value<bool>();
}

bool QGraphicsVideoItemWidgetImpl::eventFilter(QObject *watched, QEvent *event)
{
    Q_Q(QGraphicsVideoItemPrivate);
    bool updateViewportAncestorEventFiltersRequired = false;
    bool updateGeometryRequired = false;
    foreach (QPointer<QObject> target, m_viewportAncestors) {
        if (watched == target.data()) {
            switch (event->type()) {
            case QEvent::ParentChange:
                updateViewportAncestorEventFiltersRequired = true;
                break;
            case QEvent::WinIdChange:
                updateViewportAncestorEventFiltersRequired = true;
                updateTopWinId();
                break;
            case QEvent::Move:
            case QEvent::Resize:
                updateGeometryRequired = true;
                break;
            }
        }
    }
    if (updateViewportAncestorEventFiltersRequired)
        updateViewportAncestorEventFilters();
    if (updateGeometryRequired)
        updateGeometry();
    if (watched == q->m_viewport) {
        switch (event->type()) {
        case QEvent::Show:
            setVisible(true);
            break;
        case QEvent::Hide:
            setVisible(false);
            break;
        }
    }
    return QObject::eventFilter(watched, event);
}

void QGraphicsVideoItemWidgetImpl::viewportChanged()
{
    TRACE("QGraphicsVideoItemWidgetImpl::viewportChanged" << qtThisPtr());
    Q_Q(QGraphicsVideoItemPrivate);
    updateTopWinId();
    if (q->m_viewport) {
        q->m_viewport->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
        updateWidgetOrdinalPosition();
    }
    updateViewportAncestorEventFilters();
}

void QGraphicsVideoItemWidgetImpl::aspectRatioModeChanged()
{
    TRACE("QGraphicsVideoItemWidgetImpl::aspectRatioModeChanged" << qtThisPtr());
    Q_Q(QGraphicsVideoItemPrivate);
    if (m_widgetControl)
        m_widgetControl->setAspectRatioMode(q->aspectRatioMode());
}

void QGraphicsVideoItemWidgetImpl::setVisible(bool visible)
{
    if (m_visible != visible) {
        TRACE("QGraphicsVideoItemWidgetImpl::setVisible" << qtThisPtr()
              << "visible" << visible);
        m_visible = visible;
        updateWidgetVisibility();
    }
}

void QGraphicsVideoItemWidgetImpl::setWithinViewBounds(bool within)
{
    if (m_withinViewBounds != within) {
        TRACE("QGraphicsVideoItemWidgetImpl::setWithinViewBounds" << qtThisPtr()
              << "within" << within);
        m_withinViewBounds = within;
        updateWidgetVisibility();
    }
}

void QGraphicsVideoItemWidgetImpl::updateItemAncestors()
{
    TRACE("QGraphicsVideoItemWidgetImpl::updateItemAncestors" << qtThisPtr());
    // We need to monitor the ancestors of this item to check for zOrder
    // changes and reparenting, both of which influence the stacking order
    // of this item and so require changes to the backend window ordinal position.
    foreach (QPointer<QObject> target, m_itemAncestors) {
        if (target) {
            disconnect(target, SIGNAL(zChanged()), this, SLOT(updateWidgetOrdinalPosition()));
            disconnect(target, SIGNAL(parentChanged()), this, SLOT(updateItemAncestors()));
            disconnect(target, SIGNAL(parentChanged()), this, SLOT(updateWidgetOrdinalPosition()));
        }
    }
    m_itemAncestors.clear();
    QGraphicsItem *item = qq_func();
    while (item) {
        if (QGraphicsObject *object = item->toGraphicsObject()) {
            connect(object, SIGNAL(zChanged()), this, SLOT(updateWidgetOrdinalPosition()));
            connect(object, SIGNAL(parentChanged()), this, SLOT(updateItemAncestors()));
            connect(object, SIGNAL(parentChanged()), this, SLOT(updateWidgetOrdinalPosition()));
            m_itemAncestors.append(object);
        }
        item = item->parentItem();
    }
}

void QGraphicsVideoItemWidgetImpl::updateTopWinId()
{
    TRACE("QGraphicsVideoItemWidgetImpl::updateTopWinId" << qtThisPtr());
    Q_Q(QGraphicsVideoItemPrivate);
    if (m_widgetControl) {
        WId topWinId = q->m_viewport ? q->m_viewport->effectiveWinId() : 0;
        m_widgetControl->setProperty("topWinId", QVariant::fromValue<WId>(topWinId));
    }
}

void QGraphicsVideoItemWidgetImpl::updateWidgetOrdinalPosition()
{
    TRACE("QGraphicsVideoItemWidgetImpl::updateWidgetOrdinalPosition" << qtThisPtr());
    Q_Q(QGraphicsVideoItemPrivate);
    if (q->m_viewport) {
        QGraphicsScene *scene = q->m_viewport->scene();
        const QGraphicsScene::ItemIndexMethod indexMethod = scene->itemIndexMethod();
        scene->setItemIndexMethod(QGraphicsScene::BspTreeIndex);
        QT_TRY {
            const QList<QGraphicsItem*> items = q->m_viewport->items();
            QList<QGraphicsVideoItem*> graphicsVideoItems;
            foreach (QGraphicsItem *item, items)
                if (QGraphicsVideoItem *x = qobject_cast<QGraphicsVideoItem *>(item->toGraphicsObject()))
                    graphicsVideoItems.append(x);
            int ordinalPosition = 1;
            foreach (QGraphicsVideoItem *item, graphicsVideoItems) {
                QGraphicsVideoItemPrivate *d = QGraphicsVideoItemPrivate::getPtrHelper(item);
                if (QGraphicsVideoItemWidgetImpl *dd =
                    qobject_cast<QGraphicsVideoItemWidgetImpl *>(d->impl()))
                    if (QVideoWidgetControl *widgetControl = dd->m_widgetControl)
                        widgetControl->setProperty("ordinalPosition", ordinalPosition++);
            }
        } QT_CATCH(...) {
            scene->setItemIndexMethod(indexMethod);
            QT_RETHROW;
        }
        scene->setItemIndexMethod(indexMethod);
    }
}

void QGraphicsVideoItemWidgetImpl::hasContentChanged()
{
    TRACE("QGraphicsVideoItemWidgetImpl::hasContentChanged" << qtThisPtr()
          << "hasContent" << hasContent());
    qq_func()->update();
}

void QGraphicsVideoItemWidgetImpl::doUpdateGeometry()
{
    Q_Q(QGraphicsVideoItemPrivate);
    if (QWidget *widget = videoWidget()) {
        QRect widgetRect;
        QRect extentRect;
        if (q->m_viewport) {
            widgetRect = m_screenRect.intersected(m_viewportScreenRect);
            // Without this, a line of transparent pixels is visible round the edge of the
            // item.  This is probably down to an error in conversion between scene and
            // screen coordinates, but the root cause has not yet been tracked down.
            widgetRect.adjust(-1, -1, 4, 4);
            const QPoint offset((widgetRect.width() - m_contentScreenRect.width()) / 2,
                                (widgetRect.height() - m_contentScreenRect.height()) / 2);
            extentRect = QRect(offset, m_contentScreenRect.size());
        }
        const qreal angle = q->m_transform.map(QLineF(0, 0, 1, 0)).angle();
        VERBOSE_TRACE("QGraphicsVideoItemWidgetImpl::doUpdateGeometry" << qtThisPtr()
                      << "widgetRect" << widgetRect
                      << "extentRect" << extentRect
                      << "angle" << angle);
        setWithinViewBounds(!widgetRect.size().isEmpty());
        widget->setGeometry(widgetRect);
        m_widgetControl->setProperty("extentRect", QVariant::fromValue<QRect>(extentRect));
        m_widgetControl->setProperty("rotation", QVariant::fromValue<qreal>(angle));
    }
}

void QGraphicsVideoItemWidgetImpl::updateViewportAncestorEventFilters()
{
    TRACE("QGraphicsVideoItemWidgetImpl::updateViewportAncestorEventFilters" << qtThisPtr());
    Q_Q(QGraphicsVideoItemPrivate);
    // In order to determine when the absolute screen position of the item
    // changes, we need to receive move events sent to m_viewport
    // or any of its ancestors.
    foreach (QPointer<QObject> target, m_viewportAncestors)
        if (target)
            target->removeEventFilter(this);
    m_viewportAncestors.clear();
    QObject *target = q->m_viewport;
    while (target) {
        target->installEventFilter(this);
        m_viewportAncestors.append(target);
        target = target->parent();
    }
}

void QGraphicsVideoItemWidgetImpl::updateWidgetVisibility()
{
    TRACE("QGraphicsVideoItemWidgetImpl::updateWidgetVisibility" << qtThisPtr()
          << "widget" << qtVoidPtr(videoWidget())
          << "visible" << m_visible
          << "withinViewBounds" << m_withinViewBounds);
    if (QWidget *widget = videoWidget())
        widget->setVisible(m_visible && m_withinViewBounds);
}

QWidget *QGraphicsVideoItemWidgetImpl::videoWidget() const
{
    return m_widgetControl ? m_widgetControl->videoWidget() : 0;
}


//-----------------------------------------------------------------------------
// QGraphicsVideoItemEglRendererImpl
//-----------------------------------------------------------------------------

#ifndef QT_NO_EGL

class QGraphicsVideoItemEglRendererImpl : public QGraphicsVideoItemImplBase
{
    Q_OBJECT
public:
    QGraphicsVideoItemEglRendererImpl(const QString &graphicsSystem,
                                      QGraphicsVideoItemPrivate *parent);
    ~QGraphicsVideoItemEglRendererImpl();

    // QGraphicsVideoItemImplBase
    void paint(QPainter *painter, QWidget *widget);
    void itemChange(QGraphicsVideoItem::GraphicsItemChange change, const QVariant &value);
    void present();
    void updateNativeSize();
    bool isReadyToPaint() const;

    bool hasRendererControl() const;

private:
    // QGraphicsVideoItemImplBase
    void doUpdateGeometry();

private:
    const QString m_graphicsSystem;
    QVideoRendererControl *m_rendererControl;
    QEglImageVideoSurface *m_surface;
    bool m_updatePaintDevice;
    QRectF m_sourceRect;
};

QGraphicsVideoItemEglRendererImpl::QGraphicsVideoItemEglRendererImpl(const QString &graphicsSystem,
                                                                     QGraphicsVideoItemPrivate *parent)
    :   QGraphicsVideoItemImplBase(parent)
    ,   m_graphicsSystem(graphicsSystem)
    ,   m_rendererControl(0)
    ,   m_surface(0)
    ,   m_updatePaintDevice(true)
{
    Q_Q(QGraphicsVideoItemPrivate);
    TRACE("QGraphicsVideoItemEglRendererImpl::QGraphicsVideoItemEglRendererImpl" << qtThisPtr()
          << "graphicsSystem" << graphicsSystem << "q" << q << "q->m_service" << q->m_service);
    m_rendererControl = qobject_cast<QVideoRendererControl *>(
        q->m_service->requestControl(QVideoRendererControl_iid));
    TRACE("QGraphicsVideoItemEglRendererImpl::QGraphicsVideoItemEglRendererImpl" << qtThisPtr()
          << "rendererControl" << m_rendererControl);
    if (m_rendererControl) {
        connect(m_rendererControl, SIGNAL(error()),
                q, SLOT(rendererControlError()));
        qRegisterMetaType<QVideoSurfaceFormat>();
#ifndef QT_NO_OPENGL
        if ("opengl" == m_graphicsSystem)
            m_surface = new QEglImageGLVideoSurface(this);
#endif // !QT_NO_OPENGL
#ifndef QT_NO_OPENVG
        if ("openvg" == m_graphicsSystem)
            m_surface = new QEglImageVGVideoSurface(this);
#endif // !QT_NO_OPENVG
        Q_ASSERT(m_surface);
        connect(m_surface, SIGNAL(frameChanged()),
                qq_func(), SLOT(_q_present()));
        connect(m_surface, SIGNAL(surfaceFormatChanged(QVideoSurfaceFormat)),
                qq_func(), SLOT(_q_updateNativeSize()), Qt::QueuedConnection);
    }
}

QGraphicsVideoItemEglRendererImpl::~QGraphicsVideoItemEglRendererImpl()
{
    TRACE("QGraphicsVideoItemEglRendererImpl::~QGraphicsVideoItemEglRendererImpl" << qtThisPtr());
    Q_Q(QGraphicsVideoItemPrivate);
    if (m_surface) {
        m_surface->disconnect();
        m_surface->stop();
    }
    if (m_rendererControl) {
        m_rendererControl->setSurface(0);
        if (q->m_service)
            q->m_service->releaseControl(m_rendererControl);
    }
}

void QGraphicsVideoItemEglRendererImpl::paint(QPainter *painter, QWidget *widget)
{
    VERBOSE_TRACE("QGraphicsVideoItemEglRendererImpl::paint" << qtThisPtr());
    Q_Q(QGraphicsVideoItemPrivate);
    if (m_surface && m_rendererControl && m_updatePaintDevice) {
        m_updatePaintDevice = false;
        if (widget)
            connect(widget, SIGNAL(destroyed()), m_surface, SLOT(viewportDestroyed()));
        if (m_rendererControl->surface() != m_surface)
            m_rendererControl->setSurface(m_surface);
    }
    painter->fillRect(q->m_boundingRect, QBrush(Qt::black));
    if (isReadyToPaint()) {
        QRectF targetRect = m_contentRect;
        targetRect.moveCenter(q->m_boundingRect.center());
        targetRect = targetRect.intersected(q->m_boundingRect);
        m_surface->paint(painter, m_sourceRect, targetRect);
        m_surface->setReady(true);
    }
}

void QGraphicsVideoItemEglRendererImpl::itemChange(QGraphicsVideoItem::GraphicsItemChange change,
                                              const QVariant &value)
{
    Q_UNUSED(change)
    Q_UNUSED(value)
    // Do nothing
}

void QGraphicsVideoItemEglRendererImpl::present()
{
    VERBOSE_TRACE("QGraphicsVideoItemEglRendererImpl::present" << qtThisPtr());
    Q_Q(QGraphicsVideoItemPrivate);
    qq_func()->update(q->m_boundingRect);
}

void QGraphicsVideoItemEglRendererImpl::updateNativeSize()
{
    TRACE("QGraphicsVideoItemEglRendererImpl::updateNativeSize" << qtThisPtr()
          << "sizeHint" << m_surface->surfaceFormat().sizeHint());
    Q_Q(QGraphicsVideoItemPrivate);
    QSize size = m_surface->surfaceFormat().sizeHint();
    if (size.isEmpty() && m_rendererControl)
        size = m_rendererControl->property("nativeSize").toSize();
    if (q->m_nativeSize != size)
        q->m_nativeSize = size;
}

bool QGraphicsVideoItemEglRendererImpl::hasRendererControl() const
{
    bool result = false;
    if (m_rendererControl) {
        // Check that the control implementation is the correct type, i.e. it
        // produces video frames as EGL images.
        const QString className(m_rendererControl->metaObject()->className());
        TRACE("QGraphicsVideoItemEglRendererImpl::hasRendererControl" << qtThisPtr()
              << "className" << className);
        result = (className == "S60VideoEglRendererControl");
    }
    return result;
}

bool QGraphicsVideoItemEglRendererImpl::isReadyToPaint() const
{
    return (m_surface && m_surface->isActive());
}

void QGraphicsVideoItemEglRendererImpl::doUpdateGeometry()
{
    Q_Q(QGraphicsVideoItemPrivate);
    if (q->m_nativeSize.isEmpty()) {
        // Do nothing
    } else if (q->m_aspectRatioMode == Qt::IgnoreAspectRatio) {
        m_sourceRect = QRectF(0, 0, 1, 1);
    } else if (q->m_aspectRatioMode == Qt::KeepAspectRatio) {
        m_sourceRect = QRectF(0, 0, 1, 1);
    } else if (q->m_aspectRatioMode == Qt::KeepAspectRatioByExpanding) {
        QSizeF size = q->m_rect.size();
        size.scale(q->m_nativeSize, Qt::KeepAspectRatio);
        const qreal w = size.width() / q->m_nativeSize.width();
        const qreal h = size.height() / q->m_nativeSize.height();
        m_sourceRect = QRectF(0, 0, w, h);
        m_sourceRect.moveCenter(QPointF(0.5, 0.5));
    }
    VERBOSE_TRACE("QGraphicsVideoItemEglRendererImpl::doUpdateGeometry" << qtThisPtr()
                  << "sourceRect" << m_sourceRect);
}

#endif // !QT_NO_EGL


//-----------------------------------------------------------------------------
// QGraphicsVideoItemPrivate
//-----------------------------------------------------------------------------

QGraphicsVideoItemPrivate::QGraphicsVideoItemPrivate(QGraphicsVideoItem *parent)
    :   QObject(parent)
    ,   q_ptr(0)
    ,   m_impl(0)
    ,   m_isDirect(false)
    ,   m_service(0)
    ,   m_serviceSupportsRendererControl(true)
    ,   m_rendererControlError(false)
    ,   m_mediaObject(0)
    ,   m_aspectRatioMode(Qt::KeepAspectRatio)
    ,   m_viewport(0)
    ,   m_savedViewportUpdateMode(QGraphicsView::FullViewportUpdate)
    ,   m_viewportTransparencyEnabled(false)
    ,   m_nativeSize(320, 240)
    ,   m_screenSize(QApplication::desktop()->size())
    ,   m_widgetPaintDevice(true)
{
    TRACE("QGraphicsVideoItemPrivate::QGraphicsVideoItemPrivate" << qtThisPtr()
          << "parent" << qtVoidPtr(parent)
          << "screenSize" << m_screenSize);
    connect(QApplication::desktop(), SIGNAL(resized(int)),
            this, SLOT(screenSizeChanged()));
}

QGraphicsVideoItemPrivate::~QGraphicsVideoItemPrivate()
{
    TRACE("QGraphicsVideoItemPrivate::~QGraphicsVideoItemPrivate" << qtThisPtr());
}

void QGraphicsVideoItemPrivate::paint(QPainter *painter,
                                      const QStyleOptionGraphicsItem *option,
                                      QWidget *widget)
{
    VERBOSE_TRACE("QGraphicsVideoItemPrivate::paint" << qtThisPtr()
                  << "paintDeviceType" << painter->device()->devType());
    Q_Q(QGraphicsVideoItem);
    Q_UNUSED(option)
    if (m_graphicsSystem.isEmpty())
        determineGraphicsSystem();
    QGraphicsView *view = 0;
    QGraphicsScene *scene = q->scene();
    if (scene && !scene->views().isEmpty())
        view = scene->views().first();
    m_geometryChanged |= setViewport(view);
    m_geometryChanged |= setTransform(painter->combinedTransform());
    m_geometryChanged |= setWidgetPaintDevice(QInternal::Widget == painter->device()->devType());
    if (m_service) {
        if (!m_impl) {
            createImpl();
        } else if (m_geometryChanged) {
            recreateImpl();
            m_impl->updateGeometry();
            m_geometryChanged = false;
        }
    }
    if (m_impl)
        m_impl->paint(painter, widget);
}

void QGraphicsVideoItemPrivate::itemChange(QGraphicsVideoItem::GraphicsItemChange change,
                                           const QVariant &value)
{
    if (m_impl)
        m_impl->itemChange(change, value);
}

QMediaObject *QGraphicsVideoItemPrivate::mediaObject() const
{
    return m_mediaObject;
}

bool QGraphicsVideoItemPrivate::setMediaObject(QMediaObject *mediaObject)
{
    TRACE("QGraphicsVideoItemPrivate::setMediaObject" << qtThisPtr()
          << "mediaObject" << mediaObject);
    Q_Q(QGraphicsVideoItem);
    bool bound = false;
    if (m_mediaObject != mediaObject) {
        m_rendererControlError = false;
        destroyImpl();
        m_mediaObject = mediaObject;
        if (m_mediaObject) {
            m_service = m_mediaObject->service();
            if (m_service) {
                connect(m_service, SIGNAL(destroyed()),
                        q, SLOT(_q_serviceDestroyed()));
                bound = true;
                q->update();
            }
        }
    }
    return bound;
}

Qt::AspectRatioMode QGraphicsVideoItemPrivate::aspectRatioMode() const
{
    return m_aspectRatioMode;
}

void QGraphicsVideoItemPrivate::setAspectRatioMode(Qt::AspectRatioMode mode)
{
    TRACE("QGraphicsVideoItemPrivate::setAspectRatioMode" << qtThisPtr()
          << "mode" << mode);
    const bool changed = (m_aspectRatioMode != mode);
    m_aspectRatioMode = mode;
    if (changed && m_impl) {
        m_impl->aspectRatioModeChanged();
        m_impl->updateGeometry();
    }
}

QPointF QGraphicsVideoItemPrivate::offset() const
{
    return m_rect.topLeft();
}

void QGraphicsVideoItemPrivate::setOffset(const QPointF &offset)
{
    TRACE("QGraphicsVideoItemPrivate::setOffset" << qtThisPtr()
          << "offset" << offset);
    const bool changed = (this->offset() != offset);
    m_rect.setTopLeft(offset);
    if (changed)
        geometryChanged();
}

QSizeF QGraphicsVideoItemPrivate::size() const
{
    return m_rect.size();
}

void QGraphicsVideoItemPrivate::setSize(const QSizeF &size)
{
    TRACE("QGraphicsVideoItemPrivate::setSize" << qtThisPtr()
          << "size" << size);
    const bool changed = (this->size() != size);
    m_rect.setSize(size);
    if (changed)
        geometryChanged();
}

QRectF QGraphicsVideoItemPrivate::boundingRect() const
{
    return m_boundingRect;
}

QSize QGraphicsVideoItemPrivate::nativeSize() const
{
    return m_nativeSize;
}

void QGraphicsVideoItemPrivate::prepareGeometryChange()
{
    VERBOSE_TRACE("QGraphicsVideoItemPrivate::prepareGeometryChange" << qtThisPtr());
    Q_Q(QGraphicsVideoItem);
    q->prepareGeometryChange();
}

void QGraphicsVideoItemPrivate::_q_present()
{
    VERBOSE_TRACE("QGraphicsVideoItemPrivate::_q_present" << qtThisPtr());
    if (m_impl)
        m_impl->present();
}

void QGraphicsVideoItemPrivate::_q_updateNativeSize()
{
    TRACE("QGraphicsVideoItemPrivate::_q_updateNativeSize" << qtThisPtr());
    Q_Q(QGraphicsVideoItem);
    const QSize oldNativeSize = m_nativeSize;
    if (m_impl)
        m_impl->updateNativeSize();
    if (oldNativeSize != m_nativeSize) {
        TRACE("QGraphicsVideoItemPrivate::_q_updateNativeSize" << qtThisPtr()
              << "old" << oldNativeSize << "new" << m_nativeSize);
        geometryChanged();
        emit q->nativeSizeChanged(m_nativeSize);
    }
}

void QGraphicsVideoItemPrivate::_q_serviceDestroyed()
{
    TRACE("QGraphicsVideoItemPrivate::_q_serviceDestroyed" << qtThisPtr());
    Q_Q(QGraphicsVideoItem);
    if (m_service)
        m_service->disconnect(q);
    m_service = 0;
    destroyImpl();
}

void QGraphicsVideoItemPrivate::_q_mediaObjectDestroyed()
{
    TRACE("QGraphicsVideoItemPrivate::_q_mediaObjectDestroyed" << qtThisPtr());
    m_mediaObject = 0;
    _q_serviceDestroyed();
}

QGraphicsVideoItemPrivate *QGraphicsVideoItemPrivate::getPtrHelper(QGraphicsVideoItem *item)
{
    return item->d_ptr;
}

void QGraphicsVideoItemPrivate::screenSizeChanged()
{
    m_screenSize = QApplication::desktop()->size();
    TRACE("QGraphicsVideoItemPrivate::screenSizeChanged" << qtThisPtr()
          << "size" << m_screenSize);
    recreateImpl();
}

void QGraphicsVideoItemPrivate::rendererControlError()
{
    TRACE("QGraphicsVideoItemPrivate::rendererControlError" << qtThisPtr());
    qWarning() << "QGraphicsVideoItem renderer error - falling back to direct rendering";
    m_rendererControlError = true;
    recreateImpl(true);
}

// Helper
QString getPreferredRenderingPathHelper(const QObject *item)
{
    QString path;
    const QObject *obj = item;
    while (obj) {
        VERBOSE_TRACE("QGraphicsVideoItemPrivate" << qtVoidPtr(item)
                      << "ancestor" << qtVoidPtr(obj));
        QVariant v = obj->property(PreferredRenderingPathProperty);
        if (v.isValid()) {
            const QString mode = v.toString();
            if (mode == RenderingPathAuto ||
                mode == RenderingPathRenderer ||
                mode == RenderingPathDirect) {
                VERBOSE_TRACE("QGraphicsVideoItemPrivate" << qtVoidPtr(item)
                              << "preferred rendering path" << mode
                              << "from ancestor" << qtVoidPtr(obj));
                path = mode;
                break;
            }
        }
        obj = obj->parent();
    }
    return path;
}

inline QGraphicsVideoItemImplBase *QGraphicsVideoItemPrivate::impl()
{
    return reinterpret_cast<QGraphicsVideoItemImplBase *>(qGetPtrHelper(m_impl));
}

inline const QGraphicsVideoItemImplBase *QGraphicsVideoItemPrivate::impl() const
{
    return reinterpret_cast<const QGraphicsVideoItemImplBase *>(qGetPtrHelper(m_impl));
}

QString QGraphicsVideoItemPrivate::getPreferredRenderingPath() const
{
    QString path = getPreferredRenderingPathHelper(this);
    if (path.isEmpty() && m_viewport)
        path = getPreferredRenderingPathHelper(m_viewport);
    if (path.isEmpty())
        path = DefaultPreferredRenderingPath;
    return path;
}

bool QGraphicsVideoItemPrivate::setViewport(QGraphicsView *view)
{
    bool changed = false;
    if (view != m_viewport) {
        TRACE("QGraphicsVideoItemPrivate::setViewport" << qtThisPtr()
              << "view" << qtVoidPtr(view));
        restoreViewportState();
        m_viewport = view;
        if (m_viewport)
            m_savedViewportUpdateMode = m_viewport->viewportUpdateMode();
        if (m_impl)
            m_impl->viewportChanged();
        changed = true;
    }
    return changed;
}

bool QGraphicsVideoItemPrivate::setTransform(const QTransform &transform)
{
    bool changed = false;
    if (m_transform != transform) {
        VERBOSE_TRACE("QGraphicsVideoItemPrivate::setTransform" << qtThisPtr());
        m_transform = transform;
        changed = true;
    }
    return changed;
}

bool QGraphicsVideoItemPrivate::setWidgetPaintDevice(bool widget)
{
    bool changed = false;
    if (m_widgetPaintDevice != widget) {
        VERBOSE_TRACE("QGraphicsVideoItemPrivate::setWidgetPaintDevice" << qtThisPtr()
                      << "widget" << widget);
        m_widgetPaintDevice = widget;
        changed = true;
    }
    return changed;
}

void QGraphicsVideoItemPrivate::restoreViewportState()
{
    TRACE("QGraphicsVideoItemPrivate::restoreViewportState" << qtThisPtr()
          << "viewport" << qtVoidPtr(m_viewport.data())
          << "savedViewportUpdateMode" << m_savedViewportUpdateMode
          << "viewportTransparencyEnabled" << m_viewportTransparencyEnabled);
    if (m_viewport) {
        m_viewport->setViewportUpdateMode(m_savedViewportUpdateMode);
        if (m_viewportTransparencyEnabled)
            m_viewport->window()->setAttribute(Qt::WA_TranslucentBackground, false);
    }
    m_viewportTransparencyEnabled = false;
}

void QGraphicsVideoItemPrivate::geometryChanged()
{
    Q_Q(QGraphicsVideoItem);
    VERBOSE_TRACE("QGraphicsVideoItemPrivate::geometryChanged" << qtThisPtr());
    m_geometryChanged = true;
    q->update();
}

void QGraphicsVideoItemPrivate::determineGraphicsSystem()
{
    Q_ASSERT(m_graphicsSystem.isEmpty());
    m_graphicsSystem = "raster";
#ifndef QT_NO_EGL
    if (EGL_NO_CONTEXT != eglGetCurrentContext()) {
        const EGLenum api = eglQueryAPI();
        if (EGL_OPENGL_ES_API == api)
            m_graphicsSystem = "opengl";
        else if (EGL_OPENVG_API == api)
            m_graphicsSystem = "openvg";
    }
#endif
    TRACE("QGraphicsVideoItemPrivate::determineGraphicsSystem" << qtThisPtr()
          << "graphicsSystem" << m_graphicsSystem);
}

void QGraphicsVideoItemPrivate::destroyImpl()
{
    Q_Q(QGraphicsVideoItem);
    if (m_impl) {
        TRACE("QGraphicsVideoItemPrivate::destroyImpl" << qtThisPtr());
        delete m_impl;
        m_impl = 0;
        m_isDirect = false;
    }
    q->setProperty(CurrentRenderingPathProperty, "");
}

bool QGraphicsVideoItemPrivate::useDirectMode() const
{
    Q_ASSERT(!m_graphicsSystem.isEmpty());
    const bool rasterGraphicsSystem = ("raster" == m_graphicsSystem);
    bool isFullScreen = false;
    if (m_viewport) {
        const QRectF boundingScreenRectF = m_transform.mapRect(m_rect);
        QRect boundingScreenRect(boundingScreenRectF.topLeft().toPoint(), boundingScreenRectF.size().toSize());
        boundingScreenRect = QRect(m_viewport->mapToGlobal(boundingScreenRect.topLeft()), boundingScreenRect.size());
        QRect screenRect(QPoint(0,0), m_screenSize);
        isFullScreen = (boundingScreenRect.intersected(screenRect).size() == m_screenSize);
    }
    const bool result = rasterGraphicsSystem
                        || !m_serviceSupportsRendererControl
                        || m_rendererControlError
                        || (m_widgetPaintDevice
                            && isFullScreen);
    if (!m_impl || (result != m_isDirect))
        TRACE("QGraphicsVideoItemPrivate::useDirectMode" << qtThisPtr()
              << "d" << qtVoidPtr(m_impl)
              << "isDirect" << m_isDirect
              << "graphicsSystem" << m_graphicsSystem
              << "serviceSupportsRendererControl" << m_serviceSupportsRendererControl
              << "rendererControlError" << m_rendererControlError
              << "widgetPaintDevice" << m_widgetPaintDevice
              << "isFullScreen" << isFullScreen
              << "result" << result);
    return result;
}

void QGraphicsVideoItemPrivate::createImpl()
{
    const QString preferredRenderingPath = getPreferredRenderingPath();
    TRACE("QGraphicsVideoItemPrivate::createImpl" << qtThisPtr()
          << "preferredRenderingPath" << preferredRenderingPath);
    bool widget = true;
    if (RenderingPathRenderer == preferredRenderingPath)
        widget = false;
    else if (RenderingPathAuto == preferredRenderingPath)
        widget = useDirectMode();
    createImpl(widget);
}

void QGraphicsVideoItemPrivate::createImpl(bool direct)
{
    Q_ASSERT(!m_graphicsSystem.isEmpty());
    TRACE("QGraphicsVideoItemPrivate::createImpl" << qtThisPtr()
          << "graphicsSystem" << m_graphicsSystem << "direct" << direct);
    Q_Q(QGraphicsVideoItem);
    Q_ASSERT(!m_impl);
    if ("opengl" != m_graphicsSystem && "openvg" != m_graphicsSystem && !direct) {
        TRACE("QGraphicsVideoItemPrivate::createImpl" << qtThisPtr()
              << "invalid graphics system - falling back to direct mode");
        direct = true;
    }
    if (direct) {
        m_impl = new QGraphicsVideoItemWidgetImpl(this);
    } else {
#ifndef QT_NO_EGL
        QGraphicsVideoItemEglRendererImpl *renderer = new QGraphicsVideoItemEglRendererImpl(m_graphicsSystem, this);
        if (renderer->hasRendererControl()) {
            m_impl = renderer;
        } else {
            TRACE("QGraphicsVideoItemPrivate::createImpl" << qtThisPtr()
                  << "failed to create QVideoRendererControl - falling back to QVideoWidgetControl");
            // Avoid attempting to recreate QGraphicsVideoItemEglRendererImpl in future
            m_serviceSupportsRendererControl = false;
            delete renderer;
            m_impl = new QGraphicsVideoItemWidgetImpl(this);
        }
#endif // QT_NO_EGL
    }
    Q_ASSERT(m_impl);
    m_isDirect = (qobject_cast<QGraphicsVideoItemWidgetImpl *>(m_impl) != 0);
    const QString currentPath = m_isDirect ? RenderingPathDirect : RenderingPathRenderer;
    q->setProperty(CurrentRenderingPathProperty, currentPath);
    m_impl->q_ptr = this;
    TRACE("QGraphicsVideoItemPrivate::createImpl" << qtThisPtr() << "q" << qtVoidPtr(q)
          << "isDirect" << m_isDirect << "currentPath" << currentPath);
    q->update();
}

void QGraphicsVideoItemPrivate::recreateImpl(bool force)
{
    bool recreate = force;
    bool direct = m_isDirect;
    if (force)
        destroyImpl();
    if (force || ("auto" == getPreferredRenderingPath()))
        direct = useDirectMode();
    if (!recreate)
        recreate = (m_impl && m_impl->isReadyToPaint()) && (direct != m_isDirect);
    if (recreate) {
        TRACE("QGraphicsVideoItemPrivate::recreateImpl" << qtThisPtr()
              << "force" << force
              << "isDirect" << m_isDirect
              << "useDirectMode" << direct);
        destroyImpl();
        createImpl(direct);
    }
}


//-----------------------------------------------------------------------------
// QGraphicsVideoItem
//-----------------------------------------------------------------------------

QGraphicsVideoItem::QGraphicsVideoItem(QGraphicsItem *parent)
:   QGraphicsObject(parent)
,   d_ptr(new QGraphicsVideoItemPrivate(this))
{
    d_ptr->q_ptr = this;
    setCacheMode(NoCache);
    setFlag(QGraphicsItem::ItemIgnoresParentOpacity);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
}

QGraphicsVideoItem::~QGraphicsVideoItem()
{

}

QMediaObject *QGraphicsVideoItem::mediaObject() const
{
    return d_func()->mediaObject();
}

bool QGraphicsVideoItem::setMediaObject(QMediaObject *object)
{
    return d_func()->setMediaObject(object);
}

Qt::AspectRatioMode QGraphicsVideoItem::aspectRatioMode() const
{
    return d_func()->aspectRatioMode();
}

void QGraphicsVideoItem::setAspectRatioMode(Qt::AspectRatioMode mode)
{
    d_func()->setAspectRatioMode(mode);
}

QPointF QGraphicsVideoItem::offset() const
{
    return d_func()->offset();
}

void QGraphicsVideoItem::setOffset(const QPointF &offset)
{
    d_func()->setOffset(offset);
}

QSizeF QGraphicsVideoItem::size() const
{
    return d_func()->size();
}

void QGraphicsVideoItem::setSize(const QSizeF &size)
{
    d_func()->setSize(size);
}

QSizeF QGraphicsVideoItem::nativeSize() const
{
    return d_func()->nativeSize();
}

QRectF QGraphicsVideoItem::boundingRect() const
{
    return d_func()->boundingRect();
}

void QGraphicsVideoItem::paint(QPainter *painter,
                               const QStyleOptionGraphicsItem *option,
                               QWidget *widget)
{
    Q_D(QGraphicsVideoItem);
    d->paint(painter, option, widget);
#ifdef GRAPHICSVIDEOITEM_SHOW_RECTS
    if (!boundingRect().isEmpty()) {
        painter->save();
        painter->setPen(QPen(Qt::white, 1.0, Qt::DashLine));
        painter->drawRect(boundingRect());
        painter->setPen(QPen(Qt::red, 1.0, Qt::DashLine));
        painter->drawRect(d->m_rect);
        painter->restore();
    }
#endif
}

QVariant QGraphicsVideoItem::itemChange(GraphicsItemChange change,
                                        const QVariant &value)
{
    Q_D(QGraphicsVideoItem);
    d->itemChange(change, value);
    return QGraphicsItem::itemChange(change, value);
}

void QGraphicsVideoItem::timerEvent(QTimerEvent *event)
{
    QGraphicsObject::timerEvent(event);
}

#include "qgraphicsvideoitem_symbian.moc"
#include "moc_qgraphicsvideoitem.cpp"

QT_END_NAMESPACE
