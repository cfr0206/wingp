@echo off
rem goto to disk
%~d0
rem goto folder
cd %~dp0
rem set LANG=en
img2gpraw.exe -i%1 -l 
%GPRINT% -f%1.filelist -l
