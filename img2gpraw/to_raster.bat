@echo off
rem goto to disk
%~d0
rem goto folder
cd %~dp0
rm2gpraw.exe -i%1 -lv 
%GPRINT% -f%1.filelist -l
