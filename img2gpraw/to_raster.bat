rem переходим на диск
%~d0
rem переходим в папку
cd %~dp0
rm2gpraw.exe %1 -l
rem %GPRINT% -i %1
