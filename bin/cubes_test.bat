@echo off
del /Q cubes_test.csv

echo Starting cube tests.. Estimated time is 6 minutes...

echo Running test 1/71
dctest64.exe --test2 --mesh-size 2 --tex-size32 --var_cube_size --use-vbo
echo Running test 2/71
dctest64.exe --test2 --mesh-size 3 --tex-size32 --var_cube_size --use-vbo
echo Running test 3/71
dctest64.exe --test2 --mesh-size 4 --tex-size32 --var_cube_size --use-vbo
echo Running test 4/71
dctest64.exe --test2 --mesh-size 5 --tex-size32 --var_cube_size --use-vbo
echo Running test 5/71
dctest64.exe --test2 --mesh-size 6 --tex-size32 --var_cube_size --use-vbo
echo Running test 6/71
dctest64.exe --test2 --mesh-size 7 --tex-size32 --var_cube_size --use-vbo
echo Running test 7/71
dctest64.exe --test2 --mesh-size 8 --tex-size32 --var_cube_size --use-vbo
echo Running test 8/71
dctest64.exe --test2 --mesh-size 9 --tex-size32 --var_cube_size --use-vbo
echo Running test 9/71
dctest64.exe --test2 --mesh-size 10 --tex-size32 --var_cube_size --use-vbo
echo Running test 10/71
dctest64.exe --test2 --mesh-size 11 --tex-size32 --var_cube_size --use-vbo
echo Running test 11/71
dctest64.exe --test2 --mesh-size 12 --tex-size32 --var_cube_size --use-vbo
echo Running test 12/71
dctest64.exe --test2 --mesh-size 13 --tex-size32 --var_cube_size --use-vbo

echo Running test 13/71
dctest64.exe --test2 --mesh-size 2 --tex-size32 --var_cube_size 
echo Running test 14/71
dctest64.exe --test2 --mesh-size 3 --tex-size32 --var_cube_size 
echo Running test 15/71
dctest64.exe --test2 --mesh-size 4 --tex-size32 --var_cube_size 
echo Running test 16/71
dctest64.exe --test2 --mesh-size 5 --tex-size32 --var_cube_size 
echo Running test 17/71
dctest64.exe --test2 --mesh-size 6 --tex-size32 --var_cube_size 
echo Running test 18/71
dctest64.exe --test2 --mesh-size 7 --tex-size32 --var_cube_size 
echo Running test 19/71
dctest64.exe --test2 --mesh-size 8 --tex-size32 --var_cube_size 
echo Running test 20/71
dctest64.exe --test2 --mesh-size 9 --tex-size32 --var_cube_size 
echo Running test 21/71
dctest64.exe --test2 --mesh-size 10 --tex-size32 --var_cube_size 
echo Running test 22/71
dctest64.exe --test2 --mesh-size 11 --tex-size32 --var_cube_size 
echo Running test 23/71
dctest64.exe --test2 --mesh-size 12 --tex-size32 --var_cube_size 
echo Running test 24/71
dctest64.exe --test2 --mesh-size 13 --tex-size32 --var_cube_size 

echo Running test 25/71
dctest64.exe --test2 --mesh-size 2 --tex-size32
echo Running test 26/71
dctest64.exe --test2 --mesh-size 3 --tex-size32
echo Running test 27/71
dctest64.exe --test2 --mesh-size 4 --tex-size32
echo Running test 28/71
dctest64.exe --test2 --mesh-size 5 --tex-size32
echo Running test 29/71
dctest64.exe --test2 --mesh-size 6 --tex-size32
echo Running test 30/71
dctest64.exe --test2 --mesh-size 7 --tex-size32
echo Running test 31/71
dctest64.exe --test2 --mesh-size 8 --tex-size32
echo Running test 32/71
dctest64.exe --test2 --mesh-size 9 --tex-size32
echo Running test 33/71
dctest64.exe --test2 --mesh-size 10 --tex-size32
echo Running test 34/71
dctest64.exe --test2 --mesh-size 11 --tex-size32
echo Running test 35/71
dctest64.exe --test2 --mesh-size 12 --tex-size32
echo Running test 36/71
dctest64.exe --test2 --mesh-size 13 --tex-size32

echo Running test 37/71
dctest64.exe --test3 --mesh-size 2 --tex-size32
echo Running test 38/71
dctest64.exe --test3 --mesh-size 3 --tex-size32
echo Running test 39/71
dctest64.exe --test3 --mesh-size 4 --tex-size32
echo Running test 40/71
dctest64.exe --test3 --mesh-size 5 --tex-size32
echo Running test 41/71
dctest64.exe --test3 --mesh-size 6 --tex-size32
echo Running test 42/71
dctest64.exe --test3 --mesh-size 7 --tex-size32
echo Running test 43/71
dctest64.exe --test3 --mesh-size 8 --tex-size32
echo Running test 44/71
dctest64.exe --test3 --mesh-size 9 --tex-size32
echo Running test 45/71
dctest64.exe --test3 --mesh-size 10 --tex-size32
echo Running test 46/71
dctest64.exe --test3 --mesh-size 11 --tex-size32
echo Running test 47/71
dctest64.exe --test3 --mesh-size 12 --tex-size32
echo Running test 48/71
dctest64.exe --test3 --mesh-size 13 --tex-size32

echo Running test 49/71
dctest64.exe --tex-mode0 --test0 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 50/71
dctest64.exe --tex-mode0 --test1 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 51/71
dctest64.exe --tex-mode0 --test2 --tex-size32 --mesh-size 7 --var_cube_size

echo Running test 52/71
dctest64.exe --tex-mode1 --test0  --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 53/71
dctest64.exe --tex-mode1 --test1 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 54/71
dctest64.exe --tex-mode1 --test4 --tex-size32 --mesh-size 7 --var_cube_size

echo Running test 55/71
dctest64.exe --tex-mode2 --test0 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 56/71
dctest64.exe --tex-mode2 --test1 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 57/71
dctest64.exe --tex-mode2 --test2 --tex-size32 --mesh-size 7 --var_cube_size

echo Running test 58/71
dctest64.exe --tex-mode3 --test0 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 59/71
dctest64.exe --tex-mode3 --test1 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 60/71
dctest64.exe --tex-mode3 --test2 --tex-size32 --mesh-size 7 --var_cube_size

echo Running test 61/71
dctest64.exe --tex-mode4 --test0 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 62/71
dctest64.exe --tex-mode4 --test1 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 63/71
dctest64.exe --tex-mode4 --test4 --tex-size32 --mesh-size 7 --var_cube_size

echo Running test 64/71
dctest64.exe --tex-freq0 --test2 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 65/71
dctest64.exe --tex-freq0 --test4 --tex-size32 --mesh-size 7 --var_cube_size 

echo Running test 66/71
dctest64.exe --tex-freq1 --test2 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 67/71
dctest64.exe --tex-freq1 --test4 --tex-size32 --mesh-size 7 --var_cube_size

echo Running test 68/71
dctest64.exe --tex-freq2 --test2 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 69/71
dctest64.exe --tex-freq2 --test4 --tex-size32 --mesh-size 7 --var_cube_size

echo Running test 70/71
dctest64.exe --tex-freq3 --test2 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 71/71
dctest64.exe --tex-freq3 --test4 --tex-size32 --mesh-size 7 --var_cube_size

dctest64.exe --send-cube-data