@echo off
del /q a.exe
D: && cd %cd%
C:\MinGW\bin\g++.exe *.cpp -o find_largest -static-libgcc -static-libstdc++ -lwinmm -lgdi32 -O1 -O2 -O3
pause
start find_largest.exe