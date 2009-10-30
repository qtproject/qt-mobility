TEMPLATE = subdirs
TARGET = 

CONFIG += ordered

SUBDIRS += performance \   
           ut_transformcontactdata \
           ut_qcontactsymbianengine_p \
           ut_sqlfiltering \
           ut_cntrelationship
           
