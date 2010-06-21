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
headers.path = epoc32/include
headers.sources = inc/cntdef.h\
                  inc/cntdb.h\
                  inc/cntdbobs.h\
                  inc/cntfield.h\
                  inc/cntfldst.h\
                  inc/cntfilt.h\ 
                  inc/cntitem.h\ 
                  inc/cntview.h\
                  inc/cntviewbase.h\
                  inc/cntsync.h\
                  inc/cntviewfindconfig.h\
                  inc/cntviewfindconfig.inl\
                  inc/cntviewsortplugin.h\
                  inc/cntmodel.rh\
                  inc/cntdef.hrh\
                  inc/cntviewstore.h\
                  inc/cntpredictivesearch.h\
                  cntvcard/cntvcard.h\
                 

for(header, headers.sources):BLD_INF_RULES.prj_exports += "$$header $$deploy.path$$headers.path/$$basename(header)"

