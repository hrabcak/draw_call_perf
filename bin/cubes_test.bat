@echo off
del /Q cubes_test.csv

dctest64.exe --test2 --mesh-size 2 --tex-size32 --var_cube_size --use-vbo
dctest64.exe --test2 --mesh-size 3 --tex-size32 --var_cube_size --use-vbo
dctest64.exe --test2 --mesh-size 4 --tex-size32 --var_cube_size --use-vbo
dctest64.exe --test2 --mesh-size 5 --tex-size32 --var_cube_size --use-vbo
dctest64.exe --test2 --mesh-size 6 --tex-size32 --var_cube_size --use-vbo
dctest64.exe --test2 --mesh-size 7 --tex-size32 --var_cube_size --use-vbo
dctest64.exe --test2 --mesh-size 8 --tex-size32 --var_cube_size --use-vbo
dctest64.exe --test2 --mesh-size 9 --tex-size32 --var_cube_size --use-vbo
dctest64.exe --test2 --mesh-size 10 --tex-size32 --var_cube_size --use-vbo
dctest64.exe --test2 --mesh-size 11 --tex-size32 --var_cube_size --use-vbo
dctest64.exe --test2 --mesh-size 12 --tex-size32 --var_cube_size --use-vbo
dctest64.exe --test2 --mesh-size 13 --tex-size32 --var_cube_size --use-vbo

dctest64.exe --test2 --mesh-size 2 --tex-size32 --var_cube_size 
dctest64.exe --test2 --mesh-size 3 --tex-size32 --var_cube_size 
dctest64.exe --test2 --mesh-size 4 --tex-size32 --var_cube_size 
dctest64.exe --test2 --mesh-size 5 --tex-size32 --var_cube_size 
dctest64.exe --test2 --mesh-size 6 --tex-size32 --var_cube_size 
dctest64.exe --test2 --mesh-size 7 --tex-size32 --var_cube_size 
dctest64.exe --test2 --mesh-size 8 --tex-size32 --var_cube_size 
dctest64.exe --test2 --mesh-size 9 --tex-size32 --var_cube_size 
dctest64.exe --test2 --mesh-size 10 --tex-size32 --var_cube_size 
dctest64.exe --test2 --mesh-size 11 --tex-size32 --var_cube_size 
dctest64.exe --test2 --mesh-size 12 --tex-size32 --var_cube_size 
dctest64.exe --test2 --mesh-size 13 --tex-size32 --var_cube_size 

dctest64.exe --test2 --mesh-size 2 --tex-size32
dctest64.exe --test2 --mesh-size 3 --tex-size32
dctest64.exe --test2 --mesh-size 4 --tex-size32
dctest64.exe --test2 --mesh-size 5 --tex-size32
dctest64.exe --test2 --mesh-size 6 --tex-size32
dctest64.exe --test2 --mesh-size 7 --tex-size32
dctest64.exe --test2 --mesh-size 8 --tex-size32
dctest64.exe --test2 --mesh-size 9 --tex-size32
dctest64.exe --test2 --mesh-size 10 --tex-size32
dctest64.exe --test2 --mesh-size 11 --tex-size32
dctest64.exe --test2 --mesh-size 12 --tex-size32
dctest64.exe --test2 --mesh-size 13 --tex-size32

dctest64.exe --test3 --mesh-size 2 --tex-size32
dctest64.exe --test3 --mesh-size 3 --tex-size32
dctest64.exe --test3 --mesh-size 4 --tex-size32
dctest64.exe --test3 --mesh-size 5 --tex-size32
dctest64.exe --test3 --mesh-size 6 --tex-size32
dctest64.exe --test3 --mesh-size 7 --tex-size32
dctest64.exe --test3 --mesh-size 8 --tex-size32
dctest64.exe --test3 --mesh-size 9 --tex-size32
dctest64.exe --test3 --mesh-size 10 --tex-size32
dctest64.exe --test3 --mesh-size 11 --tex-size32
dctest64.exe --test3 --mesh-size 12 --tex-size32
dctest64.exe --test3 --mesh-size 13 --tex-size32

dctest64.exe --tex-mode0 --test0 --tex-size32 --mesh-size 7
dctest64.exe --tex-mode0 --test1 --tex-size32 --mesh-size 7
dctest64.exe --tex-mode0 --test2 --tex-size32 --mesh-size 7

dctest64.exe --tex-mode1 --test0  --tex-size32 --mesh-size 7
dctest64.exe --tex-mode1 --test1 --tex-size32 --mesh-size 7
dctest64.exe --tex-mode1 --test4 --tex-size32 --mesh-size 7

dctest64.exe --tex-mode2 --test0 --tex-size32 --mesh-size 7
dctest64.exe --tex-mode2 --test1 --tex-size32 --mesh-size 7
dctest64.exe --tex-mode2 --test2 --tex-size32 --mesh-size 7

dctest64.exe --tex-mode3 --test0 --tex-size32 --mesh-size 7
dctest64.exe --tex-mode3 --test1 --tex-size32 --mesh-size 7
dctest64.exe --tex-mode3 --test2 --tex-size32 --mesh-size 7

dctest64.exe --tex-mode4 --test0 --tex-size32 --mesh-size 7
dctest64.exe --tex-mode4 --test1 --tex-size32 --mesh-size 7
dctest64.exe --tex-mode4 --test4 --tex-size32 --mesh-size 7

dctest64.exe --tex-freq0 --test2 --tex-size32 --mesh-size 7  
dctest64.exe --tex-freq0 --test4 --tex-size32 --mesh-size 7 

dctest64.exe --tex-freq1 --test2 --tex-size32 --mesh-size 7
dctest64.exe --tex-freq1 --test4 --tex-size32 --mesh-size 7

dctest64.exe --tex-freq2 --test2 --tex-size32 --mesh-size 7
dctest64.exe --tex-freq2 --test4 --tex-size32 --mesh-size 7

dctest64.exe --tex-freq3 --test2 --tex-size32 --mesh-size 7
dctest64.exe --tex-freq3 --test4 --tex-size32 --mesh-size 7