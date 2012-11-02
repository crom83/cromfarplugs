if %3 == 3 goto exit
if %3 == 4 goto exit
copy texts\file_id.diz "c:\program files (x86)\far18\myplugs\operac4\file_id.diz"
copy texts\operac4.hlf "c:\program files (x86)\far18\myplugs\operac4\operac4.hlf"
copy texts\operac4Eng.lng "c:\program files (x86)\far18\myplugs\operac4\operac4Eng.lng"
copy texts\operac4Rus.lng "c:\program files (x86)\far18\myplugs\operac4\operac4Rus.lng"
copy texts\History.txt "c:\program files (x86)\far18\myplugs\operac4\History.txt"
copy texts\readme.txt "c:\program files (x86)\far18\myplugs\operac4\readme.txt"
if %3 == 2 upx -9 --lzma %1
copy %1 "c:\program files (x86)\far18\myplugs\operac4\operac4.dll"
rem if %3 == 2 copy release\operac4.map "c:\program files\far18\plugins\operac4\operac4.map"
if %3 == 2 RCStamp %2 *.*.+.* -v

:exit