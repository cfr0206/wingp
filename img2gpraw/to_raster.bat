@echo off
rem ��������� �� ����
%~d0
rem ��������� � �����
cd %~dp0
rm2gpraw.exe -i%1 -lv 
%GPRINT% -f%1.filelist -l
