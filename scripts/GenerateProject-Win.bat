@echo off
::push dir to ../ (Seagull-Engine)
pushd ..\
call vendor\bin\premake\premake5win.exe vs2019
::go back to the dir (scripts/)
popd

pause