
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86_amd64
:: your commands go here, such as:
:: @echo off
lib.exe %* >nul 2>&1
::@echo on
echo ^<ESC^>[32m [32mMerged library files![0m

