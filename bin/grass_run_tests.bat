@echo OFF
del grass_test.csv

%= Vertex shader without texture, not shaded =%
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 1 --pure_color
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 4 --pure_color
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 16 --pure_color
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 64 --pure_color
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 256 --pure_color
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 1024 --pure_color

%= Geometry shader without texture, not shaded, not instanced, multi blade =%
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --pure_color --gs_blades_per_run 1
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --pure_color --gs_blades_per_run 2
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --pure_color --gs_blades_per_run 4
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --pure_color --gs_blades_per_run 8
