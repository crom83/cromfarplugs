copy texts\file_id.diz "c:\program files (x86)\far18\myplugs\fardroid\file_id.diz"
copy texts\fardroid.hlf "c:\program files (x86)\far18\myplugs\fardroid\fardroid.hlf"
copy texts\fardroidEng.lng "c:\program files (x86)\far18\myplugs\fardroid\fardroidEng.lng"
copy texts\fardroidRus.lng "c:\program files (x86)\far18\myplugs\fardroid\fardroidRus.lng"
copy texts\History.txt "c:\program files (x86)\far18\myplugs\fardroid\History.txt"
copy texts\readme.txt "c:\program files (x86)\far18\myplugs\fardroid\readme.txt"
copy %1 "c:\program files (x86)\far18\myplugs\fardroid\fardroid.dll"
if %3 == 2 RCStamp %2 *.*.+.* -v
:exit