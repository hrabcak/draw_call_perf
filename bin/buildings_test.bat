@echo off
del test.csv

dctest64.exe --test0 --buildings-scene --blocks_per_tile 1
dctest64.exe --test0 --buildings-scene --blocks_per_tile 2
dctest64.exe --test0 --buildings-scene --blocks_per_tile 4
dctest64.exe --test0 --buildings-scene --blocks_per_tile 8
dctest64.exe --test0 --buildings-scene --blocks_per_tile 16
dctest64.exe --test0 --buildings-scene --blocks_per_tile 32
dctest64.exe --test0 --buildings-scene --blocks_per_tile 64
dctest64.exe --test0 --buildings-scene --blocks_per_tile 128
dctest64.exe --test0 --buildings-scene --blocks_per_tile 256
dctest64.exe --test0 --buildings-scene --blocks_per_tile 512
dctest64.exe --test0 --buildings-scene --blocks_per_tile 1024
dctest64.exe --test0 --buildings-scene --blocks_per_tile 2048
dctest64.exe --test0 --buildings-scene --blocks_per_tile 4096
dctest64.exe --test0 --buildings-scene --blocks_per_tile 8192