@echo OFF
del grass_test.csv


%= Vertex shader without texture, shaded =%
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 1
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 4
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 16
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 64
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 256
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 1024

%= Vertex shader without texture, not shaded =%
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 1 --pure_color
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 4 --pure_color
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 16 --pure_color
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 64 --pure_color
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 256 --pure_color
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 1024 --pure_color

%= Vertex shader with texture, shaded =%
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 1 --use_grass_tex
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 4 --use_grass_tex
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 16 --use_grass_tex
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 64 --use_grass_tex
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 256 --use_grass_tex
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 1024 --use_grass_tex

%= Vertex shader with texture, not shaded =%
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 1 --pure_color --use_grass_tex
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 4 --pure_color --use_grass_tex
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 16 --pure_color --use_grass_tex
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 64 --pure_color --use_grass_tex
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 256 --pure_color --use_grass_tex
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 1024 --pure_color --use_grass_tex

%= Geometry shader without texture, not shaded, instanced, one blade =%
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 4
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 16
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 64
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 256
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1024

%= Geometry shader without texture, shaded, instanced, one blade =%
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --pure_color
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 4 --pure_color
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 16 --pure_color
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 64 --pure_color
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 256 --pure_color
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1024 --pure_color

%= Geometry shader with texture, not shaded, instanced, one blade =%
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --use_grass_tex
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 4 --use_grass_tex
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 16 --use_grass_tex
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 64 --use_grass_tex
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 256 --use_grass_tex
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1024 --use_grass_tex

%= Geometry shader with texture, shaded, instanced, one blade =%
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --pure_color --use_grass_tex
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 4 --pure_color --use_grass_tex
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 16 --pure_color --use_grass_tex
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 64 --pure_color --use_grass_tex
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 256 --pure_color --use_grass_tex
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1024 --pure_color --use_grass_tex

%= Geometry shader without texture, not shaded, instanced, multi blade =%
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile -1 --gs_use_end_primitive --gs_blades_per_run 1
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile -1 --gs_use_end_primitive --gs_blades_per_run 2
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile -1 --gs_use_end_primitive --gs_blades_per_run 4
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile -1 --gs_use_end_primitive --gs_blades_per_run 8

%= Geometry shader without texture, shaded, instanced, multi blade =%
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile -1 --gs_use_end_primitive --pure_color --gs_blades_per_run 1
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile -1 --gs_use_end_primitive --pure_color --gs_blades_per_run 2
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile -1 --gs_use_end_primitive --pure_color --gs_blades_per_run 4
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile -1 --gs_use_end_primitive --pure_color --gs_blades_per_run 8

%= Geometry shader with texture, not shaded, instanced, multi blade =%
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile -1 --gs_use_end_primitive --use_grass_tex --gs_blades_per_run 1
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile -1 --gs_use_end_primitive --use_grass_tex --gs_blades_per_run 2
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile -1 --gs_use_end_primitive --use_grass_tex --gs_blades_per_run 4
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile -1 --gs_use_end_primitive --use_grass_tex --gs_blades_per_run 8

%= Geometry shader with texture, shaded, instanced, multi blade =%
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile -1 --gs_use_end_primitive --pure_color --use_grass_tex --gs_blades_per_run 1
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile -1 --gs_use_end_primitive --pure_color --use_grass_tex --gs_blades_per_run 2
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile -1 --gs_use_end_primitive --pure_color --use_grass_tex --gs_blades_per_run 4
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile -1 --gs_use_end_primitive --pure_color --use_grass_tex --gs_blades_per_run 8

%= Geometry shader without texture, not shaded, not instanced, multi blade =%
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --gs_blades_per_run 1
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --gs_blades_per_run 2
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --gs_blades_per_run 4
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --gs_blades_per_run 8

%= Geometry shader without texture, shaded, not instanced, multi blade =%
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --pure_color --gs_blades_per_run 1
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --pure_color --gs_blades_per_run 2
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --pure_color --gs_blades_per_run 4
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --pure_color --gs_blades_per_run 8

%= Geometry shader with texture, not shaded, not instanced, multi blade =%
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --use_grass_tex --gs_blades_per_run 1
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --use_grass_tex --gs_blades_per_run 2
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --use_grass_tex --gs_blades_per_run 4
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --use_grass_tex --gs_blades_per_run 8

%= Geometry shader with texture, shaded, not instanced, multi blade =%
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --pure_color --use_grass_tex --gs_blades_per_run 1
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --pure_color --use_grass_tex --gs_blades_per_run 2
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --pure_color --use_grass_tex --gs_blades_per_run 4
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --pure_color --use_grass_tex --gs_blades_per_run 8