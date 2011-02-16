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
                             "./cntmatchlog/group/cntmatchlog.mmp"\
                             "./cntvcard/cntvcard.mmp"\
                             "./cntphone/cntphone.mmp"

# Exports
deploy.path = /

# IBY files
iby.path = epoc32/rom/include
iby.sources = cntmodel.iby



#
# The CI system currently builds against latest stable Qt,
# which doesn't get installed to epoc32/include
# Try to handle both cases here. (cntplsql needs Qt)
#
exists($${EPOCROOT}epoc32/include/mw/QtCore) {
message(Adding cntplsql)
BLD_INF_RULES.prj_mmpfiles += "./groupsql/cntplsql.mmp"
iby.sources += cntplsql.iby
}

for(iby, iby.sources):BLD_INF_RULES.prj_exports += "groupsql/$$iby $$deploy.path$$iby.path/$$iby"

# IBY files
matchlogiby.path = epoc32/rom/include
matchlogiby.sources = cntmatchlog.iby 
for(iby, matchlogiby.sources):BLD_INF_RULES.prj_exports += "cntmatchlog/group/$$iby $$deploy.path$$matchlogiby.path/$$iby"

# Seems we currently need to export headers to both epoc32\include and the proper app directory
# (until the build environment gets cleaned up to not have any contactsmodel headers by default)
# otherwise we have problems with stale headers

# these headers come from ./inc and go to APP_LAYER_PUBLIC_EXPORT_PATH
publicincheaders = cntdef.h cntdb.h cntdbobs.h cntfield.h cntfldst.h cntfilt.h cntitem.h cntview.h \
    cntviewbase.h cntsync.h cntviewsortplugin.h cntdef.hrh

# these headers come from ./inc and go to APP_LAYER_PLATFORM_EXPORT_PATH
platformincheaders = cntmodel.rh cntviewfindconfig.h cntviewfindconfig.inl cntviewstore.h \
    cntphonenumparser.h cntviewsortpluginbase.h cntsyncecom.h cntconvertercallback.h cntdb_internal.h

# these headers come from ./cntvcard and go to APP_LAYER_PUBLIC_EXPORT_PATH
publiccntvcardheaders = cntvcard.h

for(header, publicincheaders):BLD_INF_RULES.prj_exports += "./inc/$$header APP_LAYER_PUBLIC_EXPORT_PATH($$header)"
for(header, platformincheaders):BLD_INF_RULES.prj_exports += "./inc/$$header APP_LAYER_PLATFORM_EXPORT_PATH($$header)"
for(header, publiccntvcardheaders):BLD_INF_RULES.prj_exports += "./cntvcard/$$header APP_LAYER_PUBLIC_EXPORT_PATH($$header)"

# Now the duplicate to \epoc32\include. .. . 
for(header, publicincheaders):BLD_INF_RULES.prj_exports += "./inc/$$header /epoc32/include/$$header"
for(header, platformincheaders):BLD_INF_RULES.prj_exports += "./inc/$$header /epoc32/include/$$header"
for(header, publiccntvcardheaders):BLD_INF_RULES.prj_exports += "./cntvcard/$$header /epoc32/include/$$header"


