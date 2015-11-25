@echo OFF
del grass_test.csv

%= Vertex shader without texture, not shaded, without idx_buf =%
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 1 --pure_color
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 4 --pure_color
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 16 --pure_color
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 64 --pure_color
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 256 --pure_color
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 1024 --pure_color
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 4096 --pure_color

%= Vertex shader without texture, not shaded, with idx_buf, triangle strip =%
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 1 --pure_color --use_idx_buf
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 4 --pure_color --use_idx_buf
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 16 --pure_color --use_idx_buf
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 64 --pure_color --use_idx_buf
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 256 --pure_color --use_idx_buf
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 1024 --pure_color --use_idx_buf
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 4096 --pure_color --use_idx_buf

%= Vertex shader without texture, not shaded, with idx_buf, triangles = %
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 1 --pure_color --use_idx_buf --use_triangles
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 4 --pure_color --use_idx_buf --use_triangles
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 16 --pure_color --use_idx_buf --use_triangles
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 64 --pure_color --use_idx_buf --use_triangles
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 256 --pure_color --use_idx_buf --use_triangles
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 1024 --pure_color --use_idx_buf --use_triangles
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 4096 --pure_color --use_idx_buf --use_triangles
