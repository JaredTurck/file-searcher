@echo off
del /q a.exe
D:
cd %cd%
for %%v in ("*.cpp") do C:\MinGW\bin\g++.exe %%v -static-libgcc -static-libstdc++
pause
start a.exe