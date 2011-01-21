set EPOC_TOOLS_ROOT=\epoc32\tools
set CERTIFICATE_PATH=\\begrp101\groups6\PCCONN\Tools\RnD_certs

%EPOC_TOOLS_ROOT%\makesis nfctestserviceprovider_template.pkg nfctestserviceprovider_unsigned.sis

%EPOC_TOOLS_ROOT%\signsis -s nfctestserviceprovider_unsigned.sis nfctestserviceprovider.sisx %CERTIFICATE_PATH%\Nokia_RnDCert_02.der %CERTIFICATE_PATH%\Nokia_RnDCert_02.key

del nfctestserviceprovider_unsigned.sis
