#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).


#
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
# 
# Description:
#
#


TEMPLATE = subdirs
BLD_INF_RULES.prj_mmpfiles = "./groupsql/cntmodel.mmp"\
                             "./group/cntview.mmp"\
                             "./group/template.mmp"\
                             "./groupsql/cntsrv.mmp"\
                             "./groupsql/cntplsql.mmp"

# Exports
deploy.path = /

# IBY files
iby.path = epoc32/rom/include
iby.sources = cntmodel.iby cntplsql.iby

for(iby, iby.sources):BLD_INF_RULES.prj_exports += "groupsql/$$iby $$deploy.path$$iby.path/$$iby"

# Headers
BLD_INF_RULES.prj_exports = "./inc/cntdef.h APP_LAYER_PUBLIC_EXPORT_PATH(cntdef.h)"\
                            "./inc/cntdb.h APP_LAYER_PUBLIC_EXPORT_PATH(cntdb.h)"\
                            "./inc/cntdbobs.h APP_LAYER_PUBLIC_EXPORT_PATH(cntdbobs.h)"\
                            "./inc/cntfield.h APP_LAYER_PUBLIC_EXPORT_PATH(cntfield.h)"\
                            "./inc/cntfldst.h APP_LAYER_PUBLIC_EXPORT_PATH(cntfldst.h)"\
                            "./inc/cntfilt.h APP_LAYER_PUBLIC_EXPORT_PATH(cntfilt.h)"\
                            "./inc/cntitem.h APP_LAYER_PUBLIC_EXPORT_PATH(cntitem.h)"\
                            "./inc/cntview.h APP_LAYER_PUBLIC_EXPORT_PATH(cntview.h)"\
                            "./inc/cntviewbase.h APP_LAYER_PUBLIC_EXPORT_PATH(cntviewbase.h)"\
                            "./inc/cntsync.h APP_LAYER_PUBLIC_EXPORT_PATH(cntsync.h)"\
                            "./inc/cntviewfindconfig.h   APP_LAYER_PLATFORM_EXPORT_PATH(cntviewfindconfig.h)"\
                            "./inc/cntviewfindconfig.inl APP_LAYER_PLATFORM_EXPORT_PATH(cntviewfindconfig.inl)"\
                            "./inc/cntviewsortplugin.h APP_LAYER_PUBLIC_EXPORT_PATH(cntviewsortplugin.h)"\
                            "./inc/cntmodel.rh APP_LAYER_PLATFORM_EXPORT_PATH(cntmodel.rh)"\
                            "./inc/cntdef.hrh APP_LAYER_PUBLIC_EXPORT_PATH(cntdef.hrh)"\
                            "./cntvcard/cntvcard.h APP_LAYER_PUBLIC_EXPORT_PATH(cntvcard.h)"\
                            "./inc/cntviewstore.h APP_LAYER_PLATFORM_EXPORT_PATH(cntviewstore.h)"\
                            "./inc/cntpredictivesearch.h APP_LAYER_PUBLIC_EXPORT_PATH(cntpredictivesearch.h)"

BLD_INF_RULES.prj_exports += "./inc/cntphonenumparser.h APP_LAYER_PLATFORM_EXPORT_PATH(cntphonenumparser.h)"\
                             "./inc/cntviewsortpluginbase.h APP_LAYER_PLATFORM_EXPORT_PATH(cntviewsortpluginbase.h)"\
                             "./inc/cntsyncecom.h APP_LAYER_PLATFORM_EXPORT_PATH(cntsyncecom.h)"\
			 
