@echo off
del test.csv

dctest64.exe --test0 --tex-mode0 --mesh-size2 --tex-freq1
dctest64.exe --test1 --tex-mode0 --mesh-size2 --tex-freq1
dctest64.exe --test2 --tex-mode0 --mesh-size2 --tex-freq1
dctest64.exe --test3 --tex-mode0 --mesh-size2 --tex-freq1

dctest64.exe --test0 --tex-mode1 --mesh-size2 --tex-freq0
dctest64.exe --test1 --tex-mode1 --mesh-size2 --tex-freq0
rem dctest64.exe --test2 --tex-mode1 --mesh-size2 --tex-freq0
rem dctest64.exe --test3 --tex-mode1 --mesh-size2 --tex-freq0

dctest64.exe --test0 --tex-mode2 --mesh-size2 --tex-freq1
dctest64.exe --test1 --tex-mode2 --mesh-size2 --tex-freq1
dctest64.exe --test2 --tex-mode2 --mesh-size2 --tex-freq1
dctest64.exe --test3 --tex-mode2 --mesh-size2 --tex-freq1

dctest64.exe --test0 --tex-mode3 --mesh-size2 --tex-freq1
dctest64.exe --test1 --tex-mode3 --mesh-size2 --tex-freq1
dctest64.exe --test2 --tex-mode3 --mesh-size2 --tex-freq1
dctest64.exe --test3 --tex-mode3 --mesh-size2 --tex-freq1

dctest64.exe --test2 --tex-mode2 --mesh-size0 --tex-freq1
dctest64.exe --test2 --tex-mode2 --mesh-size1 --tex-freq1
dctest64.exe --test2 --tex-mode2 --mesh-size2 --tex-freq1
dctest64.exe --test2 --tex-mode2 --mesh-size3 --tex-freq1

dctest64.exe --test2 --tex-mode3 --mesh-size2 --tex-freq0
dctest64.exe --test2 --tex-mode3 --mesh-size2 --tex-freq1
dctest64.exe --test2 --tex-mode3 --mesh-size2 --tex-freq2
dctest64.exe --test2 --tex-mode3 --mesh-size2 --tex-freq3
