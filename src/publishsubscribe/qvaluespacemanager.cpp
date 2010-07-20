/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qvaluespacemanager_p.h"

QTM_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QValueSpaceManager, valueSpaceManager);

QValueSpaceManager *QValueSpaceManager::instance()
{
    return valueSpaceManager();
}

QValueSpaceManager::QValueSpaceManager()
: type(Uninit)
{
}

void QValueSpaceManager::initServer()
{
    Q_ASSERT(Uninit == type);

    commonInit(QAbstractValueSpaceLayer::Server);
}

void QValueSpaceManager::init()
{
    if(Uninit != type)
        return; // Already initialized

    commonInit(QAbstractValueSpaceLayer::Client);
}

void QValueSpaceManager::commonInit(QAbstractValueSpaceLayer::Type vsltype)
{
    Q_ASSERT(Uninit == type);

    // Install all the dormant layers
    for(int ii = 0; ii < funcs.count(); ++ii)
        install(funcs[ii]());
    funcs.clear();

    type = (vsltype == QAbstractValueSpaceLayer::Server)?Server:Client;

    for(int ii = 0; ii < layers.count(); ++ii) {
        if(!initLayer(layers.at(ii))) {
            layers.removeAt(ii);
            --ii;
        }
    }
}

bool QValueSpaceManager::isServer() const
{
    return (Server == type);
}

void QValueSpaceManager::install(QAbstractValueSpaceLayer * layer)
{
    Q_ASSERT(Uninit == type);
    Q_ASSERT(layer);
    unsigned int cOrder = layer->order();
    int inserted = -1;
    for(int ii = 0; !inserted && ii < layers.count(); ++ii) {
        unsigned int lOrder = layers.at(ii)->order();
        Q_ASSERT(layer != layers.at(ii));
        if(lOrder < cOrder) {
            // Do nothing
        } else if(lOrder == cOrder) {
            if(layers.at(ii)->id() > layer->id()) {
                layers.insert(ii, layer);
                inserted = ii;
            }
        } else if(lOrder > cOrder) {
            layers.insert(ii, layer);
            inserted = ii;
        }
    }

    if(-1 == inserted) {
        inserted = layers.count();
        layers.append(layer);
    }
}

void QValueSpaceManager::install(QValueSpace::LayerCreateFunc func)
{
    Q_ASSERT(Uninit == type);
    funcs.append(func);
}

QList<QAbstractValueSpaceLayer *> const & QValueSpaceManager::getLayers()
{
    init(); // Fallback init

    return layers;
}

bool QValueSpaceManager::initLayer(QAbstractValueSpaceLayer* layer)
{
    Q_ASSERT(Uninit != type);

    return layer->startup((type==Client)?QAbstractValueSpaceLayer::Client:
                                         QAbstractValueSpaceLayer::Server);
}

QTM_END_NAMESPACE
