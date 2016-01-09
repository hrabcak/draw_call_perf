@echo OFF
del grass_test.csv

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



