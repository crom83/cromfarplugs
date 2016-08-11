@echo off
setlocal enabledelayedexpansion 

set zip="C:\Program Files\7-Zip\7z.exe"

for /F "tokens=3*" %%i  in (..\version.info) do set version=!version!.%%i
set version=!version:~1!

del /q *.7z

for %%p in (x64 x86) do (
	set arch=fardroid_%version%_%%p.7z
	rd /q /s %%p\

	xcopy /s /y /Exclude:exclude.txt ..\Release\%%p\* %%p\FarDroid\ | findstr /e /c:"copied"
	IF ERRORLEVEL 1 EXIT /B 1

	cd %%p
	%zip% a ..\!arch! FarDroid | findstr /b /c:"Everything is Ok" /c:"Creating archive"
	IF ERRORLEVEL 1 EXIT /B 1
	cd ..
)
