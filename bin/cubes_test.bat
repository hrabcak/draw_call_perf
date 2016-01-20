@echo off
del /Q cubes_test.csv

echo Starting cube tests.. Estimated time is 9 minutes...

echo Running test 1/103
dctest64.exe --test2 --mesh-size 2 --tex-size32 --var_cube_size --use-vbo
echo Running test 2/103
dctest64.exe --test2 --mesh-size 3 --tex-size32 --var_cube_size --use-vbo
echo Running test 3/103
dctest64.exe --test2 --mesh-size 4 --tex-size32 --var_cube_size --use-vbo
echo Running test 4/103
dctest64.exe --test2 --mesh-size 5 --tex-size32 --var_cube_size --use-vbo
echo Running test 5/103
dctest64.exe --test2 --mesh-size 6 --tex-size32 --var_cube_size --use-vbo
echo Running test 6/103
dctest64.exe --test2 --mesh-size 7 --tex-size32 --var_cube_size --use-vbo
echo Running test 7/103
dctest64.exe --test2 --mesh-size 8 --tex-size32 --var_cube_size --use-vbo
echo Running test 8/103
dctest64.exe --test2 --mesh-size 9 --tex-size32 --var_cube_size --use-vbo
echo Running test 9/103
dctest64.exe --test2 --mesh-size 10 --tex-size32 --var_cube_size --use-vbo
echo Running test 10/103
dctest64.exe --test2 --mesh-size 11 --tex-size32 --var_cube_size --use-vbo
echo Running test 11/103
dctest64.exe --test2 --mesh-size 12 --tex-size32 --var_cube_size --use-vbo
echo Running test 12/103
dctest64.exe --test2 --mesh-size 13 --tex-size32 --var_cube_size --use-vbo

echo Running test 13/103
dctest64.exe --test2 --mesh-size 2 --tex-size32 --var_cube_size 
echo Running test 14/103
dctest64.exe --test2 --mesh-size 3 --tex-size32 --var_cube_size 
echo Running test 15/103
dctest64.exe --test2 --mesh-size 4 --tex-size32 --var_cube_size 
echo Running test 16/103
dctest64.exe --test2 --mesh-size 5 --tex-size32 --var_cube_size 
echo Running test 17/103
dctest64.exe --test2 --mesh-size 6 --tex-size32 --var_cube_size 
echo Running test 18/103
dctest64.exe --test2 --mesh-size 7 --tex-size32 --var_cube_size 
echo Running test 19/103
dctest64.exe --test2 --mesh-size 8 --tex-size32 --var_cube_size 
echo Running test 20/103
dctest64.exe --test2 --mesh-size 9 --tex-size32 --var_cube_size 
echo Running test 21/103
dctest64.exe --test2 --mesh-size 10 --tex-size32 --var_cube_size 
echo Running test 22/103
dctest64.exe --test2 --mesh-size 11 --tex-size32 --var_cube_size 
echo Running test 23/103
dctest64.exe --test2 --mesh-size 12 --tex-size32 --var_cube_size 
echo Running test 24/103
dctest64.exe --test2 --mesh-size 13 --tex-size32 --var_cube_size 

echo Running test 25/103
dctest64.exe --test2 --mesh-size 2 --tex-size32
echo Running test 26/103
dctest64.exe --test2 --mesh-size 3 --tex-size32
echo Running test 27/103
dctest64.exe --test2 --mesh-size 4 --tex-size32
echo Running test 28/103
dctest64.exe --test2 --mesh-size 5 --tex-size32
echo Running test 29/103
dctest64.exe --test2 --mesh-size 6 --tex-size32
echo Running test 30/103
dctest64.exe --test2 --mesh-size 7 --tex-size32
echo Running test 31/103
dctest64.exe --test2 --mesh-size 8 --tex-size32
echo Running test 32/103
dctest64.exe --test2 --mesh-size 9 --tex-size32
echo Running test 33/103
dctest64.exe --test2 --mesh-size 10 --tex-size32
echo Running test 34/103
dctest64.exe --test2 --mesh-size 11 --tex-size32
echo Running test 35/103
dctest64.exe --test2 --mesh-size 12 --tex-size32
echo Running test 36/103
dctest64.exe --test2 --mesh-size 13 --tex-size32

echo Running test 37/103
dctest64.exe --test3 --mesh-size 2 --tex-size32
echo Running test 38/103
dctest64.exe --test3 --mesh-size 3 --tex-size32
echo Running test 39/103
dctest64.exe --test3 --mesh-size 4 --tex-size32
echo Running test 40/103
dctest64.exe --test3 --mesh-size 5 --tex-size32
echo Running test 41/103
dctest64.exe --test3 --mesh-size 6 --tex-size32
echo Running test 42/103
dctest64.exe --test3 --mesh-size 7 --tex-size32
echo Running test 43/103
dctest64.exe --test3 --mesh-size 8 --tex-size32
echo Running test 44/103
dctest64.exe --test3 --mesh-size 9 --tex-size32
echo Running test 45/103
dctest64.exe --test3 --mesh-size 10 --tex-size32
echo Running test 46/103
dctest64.exe --test3 --mesh-size 11 --tex-size32
echo Running test 47/103
dctest64.exe --test3 --mesh-size 12 --tex-size32
echo Running test 48/103
dctest64.exe --test3 --mesh-size 13 --tex-size32

echo Running test 49/103
dctest64.exe --tex-mode0 --test0 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 50/103
dctest64.exe --tex-mode0 --test1 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 51/103
dctest64.exe --tex-mode0 --test2 --tex-size32 --mesh-size 7 --var_cube_size

echo Running test 52/103
dctest64.exe --tex-mode1 --test0  --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 53/103
dctest64.exe --tex-mode1 --test1 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 54/103
dctest64.exe --tex-mode1 --test4 --tex-size32 --mesh-size 7 --var_cube_size

echo Running test 55/103
dctest64.exe --tex-mode2 --test0 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 56/103
dctest64.exe --tex-mode2 --test1 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 57/103
dctest64.exe --tex-mode2 --test2 --tex-size32 --mesh-size 7 --var_cube_size

echo Running test 58/103
dctest64.exe --tex-mode3 --test0 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 59/103
dctest64.exe --tex-mode3 --test1 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 60/103
dctest64.exe --tex-mode3 --test2 --tex-size32 --mesh-size 7 --var_cube_size


echo Running test 61/103
dctest64.exe --tex-mode4 --test0 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 62/103
dctest64.exe --tex-mode4 --test1 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 63/103
dctest64.exe --tex-mode4 --test4 --tex-size32 --mesh-size 7 --var_cube_size

echo Running test 64/103
dctest64.exe --tex-freq0 --tex-mode2 --test2 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 65/103
dctest64.exe --tex-freq1 --tex-mode2 --test2 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 66/103
dctest64.exe --tex-freq2 --tex-mode2 --test2 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 67/103
dctest64.exe --tex-freq3 --tex-mode2 --test2 --tex-size32 --mesh-size 7 --var_cube_size

echo Running test 68/103
dctest64.exe --tex-freq0 --tex-mode3 --test2 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 69/103
dctest64.exe --tex-freq1 --tex-mode3 --test2 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 70/103
dctest64.exe --tex-freq2 --tex-mode3 --test2 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 71/103
dctest64.exe --tex-freq3 --tex-mode3 --test2 --tex-size32 --mesh-size 7 --var_cube_size

echo Running test 72/103
dctest64.exe --tex-freq0 --tex-mode1 --test0 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 73/103
dctest64.exe --tex-freq1 --tex-mode1 --test0 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 74/103
dctest64.exe --tex-freq2 --tex-mode1 --test0 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 75/103
dctest64.exe --tex-freq3 --tex-mode1 --test0 --tex-size32 --mesh-size 7 --var_cube_size

echo Running test 76/103
dctest64.exe --tex-freq0 --tex-mode2 --test0 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 77/103
dctest64.exe --tex-freq1 --tex-mode2 --test0 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 78/103
dctest64.exe --tex-freq2 --tex-mode2 --test0 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 79/103
dctest64.exe --tex-freq3 --tex-mode2 --test0 --tex-size32 --mesh-size 7 --var_cube_size

echo Running test 80/103
dctest64.exe --tex-freq0 --tex-mode3 --test0 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 81/103
dctest64.exe --tex-freq1 --tex-mode3 --test0 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 82/103
dctest64.exe --tex-freq2 --tex-mode3 --test0 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 83/103
dctest64.exe --tex-freq3 --tex-mode3 --test0 --tex-size32 --mesh-size 7 --var_cube_size

echo Running test 84/103
dctest64.exe --tex-freq0 --tex-mode4 --test0 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 85/103
dctest64.exe --tex-freq1 --tex-mode4 --test0 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 86/103
dctest64.exe --tex-freq2 --tex-mode4 --test0 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 87/103
dctest64.exe --tex-freq3 --tex-mode4 --test0 --tex-size32 --mesh-size 7 --var_cube_size

echo Running test 88/103
dctest64.exe --tex-freq0 --tex-mode1 --test4 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 89/103
dctest64.exe --tex-freq1 --tex-mode1 --test4 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 90/103
dctest64.exe --tex-freq2 --tex-mode1 --test4 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 91/103
dctest64.exe --tex-freq3 --tex-mode1 --test4 --tex-size32 --mesh-size 7 --var_cube_size

echo Running test 92/103
dctest64.exe --tex-freq0 --tex-mode2 --test4 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 93/103
dctest64.exe --tex-freq1 --tex-mode2 --test4 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 94/103
dctest64.exe --tex-freq2 --tex-mode2 --test4 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 95/103
dctest64.exe --tex-freq3 --tex-mode2 --test4 --tex-size32 --mesh-size 7 --var_cube_size

echo Running test 96/103
dctest64.exe --tex-freq0 --tex-mode3 --test4 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 97/103
dctest64.exe --tex-freq1 --tex-mode3 --test4 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 98/103
dctest64.exe --tex-freq2 --tex-mode3 --test4 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 99/103
dctest64.exe --tex-freq3 --tex-mode3 --test4 --tex-size32 --mesh-size 7 --var_cube_size

echo Running test 100/103
dctest64.exe --tex-freq0 --tex-mode4 --test4 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 101/103
dctest64.exe --tex-freq1 --tex-mode4 --test4 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 102/103
dctest64.exe --tex-freq2 --tex-mode4 --test4 --tex-size32 --mesh-size 7 --var_cube_size
echo Running test 103/103
dctest64.exe --tex-freq3 --tex-mode4 --test4 --tex-size32 --mesh-size 7 --var_cube_size



dctest64.exe --send-cube-data