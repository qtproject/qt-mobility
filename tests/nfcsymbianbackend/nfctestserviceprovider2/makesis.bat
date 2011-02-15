set EPOC_TOOLS_ROOT=\epoc32\tools
set CERTIFICATE_PATH=\\begrp101\groups6\PCCONN\Tools\RnD_certs

%EPOC_TOOLS_ROOT%\makesis nfctestserviceprovider2_template.pkg nfctestserviceprovider2_unsigned.sis

%EPOC_TOOLS_ROOT%\signsis -s nfctestserviceprovider2_unsigned.sis nfctestserviceprovider2.sisx %CERTIFICATE_PATH%\Nokia_RnDCert_02.der %CERTIFICATE_PATH%\Nokia_RnDCert_02.key

del nfctestserviceprovider2_unsigned.sis
