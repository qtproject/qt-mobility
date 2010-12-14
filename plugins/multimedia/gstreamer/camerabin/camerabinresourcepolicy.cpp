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

#include "camerabinresourcepolicy.h"

#ifdef Q_WS_MAEMO_6
#define HAVE_RESOURCE_POLICY
#endif

//#define DEBUG_RESOURCE_POLICY
#include <QtCore/qdebug.h>

#ifdef HAVE_RESOURCE_POLICY
#include <policy/resource.h>
#include <policy/resources.h>
#include <policy/resource-set.h>
#endif

CamerabinResourcePolicy::CamerabinResourcePolicy(QObject *parent) :
    QObject(parent),
    m_resourceSet(NoResources)
{
#ifdef HAVE_RESOURCE_POLICY
    ResourcePolicy::ResourceSet *resourceSet;

    //loaded resource set is also kept requested for image and video capture sets
    resourceSet = new ResourcePolicy::ResourceSet("camera", this);

    resourceSet->addResource(ResourcePolicy::VideoRecorderType);
    ResourcePolicy::LensCoverResource *lensCoverResource = new ResourcePolicy::LensCoverResource;
    lensCoverResource->setOptional(true);
    resourceSet->addResourceObject(lensCoverResource);
    resourceSet->update();

    m_resources.append(resourceSet);
    m_resourceStatuses.append(Initial);

    //still image resources:
    resourceSet = new ResourcePolicy::ResourceSet("camera", this);
    resourceSet->addResource(ResourcePolicy::VideoPlaybackType);
    resourceSet->addResource(ResourcePolicy::LedsType);
    resourceSet->update();

    m_resources.append(resourceSet);
    m_resourceStatuses.append(Initial);

    //video capture resources:
    resourceSet = new ResourcePolicy::ResourceSet("camera", this);

    resourceSet->addResource(ResourcePolicy::VideoPlaybackType);
    resourceSet->addResource(ResourcePolicy::AudioPlaybackType);
    resourceSet->addResource(ResourcePolicy::AudioRecorderType);
    resourceSet->addResource(ResourcePolicy::LedsType);
    resourceSet->update();

    m_resources.append(resourceSet);
    m_resourceStatuses.append(Initial);

    foreach (ResourcePolicy::ResourceSet *resource, m_resources) {
        connect(resource, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType>)),
                this, SLOT(handleResourcesGranted()));
        connect(resource, SIGNAL(resourcesDenied()),
                this, SLOT(handleResourcesDenied()));
        connect(resource, SIGNAL(lostResources()),
                this, SLOT(handleResourcesLost()));
    }
#endif
}

CamerabinResourcePolicy::~CamerabinResourcePolicy()
{
}

CamerabinResourcePolicy::ResourceSet CamerabinResourcePolicy::resourceSet() const
{
    return m_resourceSet;
}

void CamerabinResourcePolicy::setResourceSet(CamerabinResourcePolicy::ResourceSet set)
{
    if (m_resourceSet == set)
        return;
    m_resourceSet = set;

#ifdef DEBUG_RESOURCE_POLICY
    qDebug() << Q_FUNC_INFO << set;
#endif

#ifdef HAVE_RESOURCE_POLICY
    m_requestedSets.clear();
    switch (set) {
    case NoResources:
        break;
    case LoadedResources:
        m_requestedSets << LoadedResourcesSet;
        break;
    case ImageCaptureResources:
        m_requestedSets << LoadedResourcesSet << ImageResourcesSet;
        break;
    case VideoCaptureResources:
        m_requestedSets << LoadedResourcesSet << VideoResouresSet;
        break;
    }

    //release the resource we don't need any more
    for (int i=0; i<m_resources.count(); i++) {
        if (!m_requestedSets.contains(i) && m_resourceStatuses[i] != Initial) {
            m_resourceStatuses[i] = Initial;
            m_resources[i]->release();
        }
    }

    //and acquire ones we need
    foreach (int i, m_requestedSets) {
        if (m_resourceStatuses[i] == Initial) {
            m_resourceStatuses[i] = RequestedResource;
            m_resources[i]->acquire();
        }
    }
#endif
}

bool CamerabinResourcePolicy::isResourcesGranted() const
{
    foreach (int i, m_requestedSets) {
        if (m_resourceStatuses[i] != GrantedResource)
            return false;
    }

    return true;
}



void CamerabinResourcePolicy::handleResourcesGranted()
{
#ifdef HAVE_RESOURCE_POLICY
    for (int i=0; i<m_resources.count(); i++) {
        if (sender() == m_resources[i]) {
            m_resourceStatuses[i] = GrantedResource;

#ifdef DEBUG_RESOURCE_POLICY
            qDebug() << Q_FUNC_INFO << "Resource granted" << i;
#endif
            if (m_requestedSets.contains(i)) {
                if (isResourcesGranted())
                    emit resourcesGranted();
            }
            break;
        }
    }
#endif
}

void CamerabinResourcePolicy::handleResourcesDenied()
{
#ifdef HAVE_RESOURCE_POLICY
    for (int i=0; i<m_resources.count(); i++) {
        if (sender() == m_resources[i]) {
            bool wasGranted = isResourcesGranted();
            m_resourceStatuses[i] = RequestedResource;

#ifdef DEBUG_RESOURCE_POLICY
            qDebug() << Q_FUNC_INFO << "Resource denied" << i << "was granted:" << wasGranted;
#endif
            if (wasGranted && m_requestedSets.contains(i))
                emit resourcesDenied();

            break;
        }
    }
#endif
}

void CamerabinResourcePolicy::handleResourcesLost()
{
#ifdef HAVE_RESOURCE_POLICY
    for (int i=0; i<m_resources.count(); i++) {
        if (sender() == m_resources[i]) {
            bool wasGranted = isResourcesGranted();
            m_resourceStatuses[i] = RequestedResource;

#ifdef DEBUG_RESOURCE_POLICY
            qDebug() << Q_FUNC_INFO << "Resource lost" << i << "was granted:" << wasGranted;
#endif

            if (wasGranted && m_requestedSets.contains(i))
                emit resourcesLost();

            break;
        }
    }
#endif
}
