@echo off
del /Q cubes_test.csv

dctest64.exe --test2 --mesh-size0
dctest64.exe --test2 --mesh-size1
dctest64.exe --test2 --mesh-size2
dctest64.exe --test2 --mesh-size3
dctest64.exe --test2 --mesh-size4
