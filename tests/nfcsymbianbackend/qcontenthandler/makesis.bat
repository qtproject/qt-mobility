set EPOC_TOOLS_ROOT=\epoc32\tools
set CERTIFICATE_PATH=\\begrp101\groups6\PCCONN\Tools\RnD_certs

%EPOC_TOOLS_ROOT%\makesis tst_qcontenthandler_template.pkg tst_qcontenthandler_unsigned.sis

%EPOC_TOOLS_ROOT%\signsis -s tst_qcontenthandler_unsigned.sis tst_qcontenthandler.sisx %CERTIFICATE_PATH%\Nokia_RnDCert_02.der %CERTIFICATE_PATH%\Nokia_RnDCert_02.key

del tst_qcontenthandler_unsigned.sis
