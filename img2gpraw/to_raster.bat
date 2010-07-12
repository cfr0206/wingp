@echo off
rem переходим на диск
%~d0
rem переходим в папку
cd %~dp0
rm2gpraw.exe -i%1 -lv 
%GPRINT% -f%1.filelist -l
