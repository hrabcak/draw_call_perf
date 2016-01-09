@echo off
del buildings_test.csv

dctest64.exe --test0 --buildings-scene --blocks_per_tile 1
if %errorlevel% neq 0 exit /b %errorlevel%
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

dctest64.exe --test0 --buildings-scene --blocks_per_tile 1 --buildings-nocull
dctest64.exe --test0 --buildings-scene --blocks_per_tile 2 --buildings-nocull
dctest64.exe --test0 --buildings-scene --blocks_per_tile 4 --buildings-nocull
dctest64.exe --test0 --buildings-scene --blocks_per_tile 8 --buildings-nocull
dctest64.exe --test0 --buildings-scene --blocks_per_tile 16 --buildings-nocull
dctest64.exe --test0 --buildings-scene --blocks_per_tile 32 --buildings-nocull
dctest64.exe --test0 --buildings-scene --blocks_per_tile 64 --buildings-nocull
dctest64.exe --test0 --buildings-scene --blocks_per_tile 128 --buildings-nocull
dctest64.exe --test0 --buildings-scene --blocks_per_tile 256 --buildings-nocull
dctest64.exe --test0 --buildings-scene --blocks_per_tile 512 --buildings-nocull
dctest64.exe --test0 --buildings-scene --blocks_per_tile 1024 --buildings-nocull
dctest64.exe --test0 --buildings-scene --blocks_per_tile 2048 --buildings-nocull
dctest64.exe --test0 --buildings-scene --blocks_per_tile 4096 --buildings-nocull
dctest64.exe --test0 --buildings-scene --blocks_per_tile 8192 --buildings-nocull

if %errorlevel% neq 0 exit /b %errorlevel%

dctest64.exe --send-buildings-data