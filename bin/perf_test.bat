@echo OFF
del /Q grass_test.csv


dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --vs_variable_blades_per_dc --blades_per_dc 65536 --pure_color
if %errorlevel% neq 0 exit /b %errorlevel%
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --vs_variable_blades_per_dc --blades_per_dc 16384 --pure_color
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --vs_variable_blades_per_dc --blades_per_dc 4096 --pure_color
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --vs_variable_blades_per_dc --blades_per_dc 1024 --pure_color
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --vs_variable_blades_per_dc --blades_per_dc 256 --pure_color
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --vs_variable_blades_per_dc --blades_per_dc 64 --pure_color
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --vs_variable_blades_per_dc --blades_per_dc 16 --pure_color
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --vs_variable_blades_per_dc --blades_per_dc 4 --pure_color
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --vs_variable_blades_per_dc --blades_per_dc 1 --pure_color
if %errorlevel% neq 0 exit /b %errorlevel%

dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --vs_variable_blades_per_dc --blades_per_dc 65536 --pure_color --use_idx_buf
if %errorlevel% neq 0 exit /b %errorlevel%
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --vs_variable_blades_per_dc --blades_per_dc 16384 --pure_color --use_idx_buf
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --vs_variable_blades_per_dc --blades_per_dc 4096 --pure_color --use_idx_buf
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --vs_variable_blades_per_dc --blades_per_dc 1024 --pure_color --use_idx_buf
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --vs_variable_blades_per_dc --blades_per_dc 256 --pure_color --use_idx_buf
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --vs_variable_blades_per_dc --blades_per_dc 64 --pure_color --use_idx_buf
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --vs_variable_blades_per_dc --blades_per_dc 16 --pure_color --use_idx_buf
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --vs_variable_blades_per_dc --blades_per_dc 4 --pure_color --use_idx_buf
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --vs_variable_blades_per_dc --blades_per_dc 1 --pure_color --use_idx_buf
if %errorlevel% neq 0 exit /b %errorlevel%


dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --vs_variable_blades_per_dc --blades_per_dc 65536 --pure_color --use_idx_buf --use_triangles
if %errorlevel% neq 0 exit /b %errorlevel%
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --vs_variable_blades_per_dc --blades_per_dc 16384 --pure_color --use_idx_buf --use_triangles
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --vs_variable_blades_per_dc --blades_per_dc 4096 --pure_color --use_idx_buf --use_triangles
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --vs_variable_blades_per_dc --blades_per_dc 1024 --pure_color --use_idx_buf --use_triangles
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --vs_variable_blades_per_dc --blades_per_dc 256 --pure_color --use_idx_buf --use_triangles
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --vs_variable_blades_per_dc --blades_per_dc 64 --pure_color --use_idx_buf --use_triangles
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --vs_variable_blades_per_dc --blades_per_dc 16 --pure_color --use_idx_buf --use_triangles
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --vs_variable_blades_per_dc --blades_per_dc 4 --pure_color --use_idx_buf --use_triangles
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --vs_variable_blades_per_dc --blades_per_dc 1 --pure_color --use_idx_buf --use_triangles

if %errorlevel% neq 0 exit /b %errorlevel%

dctest64.exe --send-grass-data 

del /Q buildings_test.csv

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