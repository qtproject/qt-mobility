call qmake
call sbs reallyclean
call qmake
call sbs -c winscw_udeb
call copy \epoc32\release\winscw\udeb\ut_cntsqlsearch.exe \epoc32\release\winscw\udeb\z\sys\bin\test.exe 
call epoc
