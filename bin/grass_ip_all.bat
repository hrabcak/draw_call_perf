@echo OFF
del grass_test.csv

dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --pure_color --use_idx_buf
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 4 --pure_color --use_idx_buf
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 16 --pure_color --use_idx_buf
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 64 --pure_color --use_idx_buf
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 256 --pure_color --use_idx_buf
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1024 --pure_color --use_idx_buf
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 4096 --pure_color --use_idx_buf

dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --pure_color  --use_idx_buf --ip1f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 4 --pure_color  --use_idx_buf --ip1f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 16  --pure_color --use_idx_buf  --ip1f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 64  --pure_color --use_idx_buf  --ip1f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 256  --pure_color --use_idx_buf  --ip1f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1024  --pure_color --use_idx_buf  --ip1f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 4096  --pure_color --use_idx_buf  --ip1f

dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1  --pure_color --use_idx_buf  --ip2f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 4  --pure_color --use_idx_buf  --ip2f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 16  --pure_color --use_idx_buf  --ip2f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 64  --pure_color --use_idx_buf  --ip2f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 256  --pure_color --use_idx_buf  --ip2f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1024  --pure_color --use_idx_buf  --ip2f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 4096  --pure_color --use_idx_buf  --ip2f

dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1  --pure_color --use_idx_buf  --ip3f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 4  --pure_color --use_idx_buf  --ip3f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 16  --pure_color --use_idx_buf  --ip3f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 64  --pure_color --use_idx_buf  --ip3f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 256  --pure_color --use_idx_buf  --ip3f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1024  --pure_color --use_idx_buf  --ip3f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 4096  --pure_color --use_idx_buf  --ip3f

dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1  --pure_color --use_idx_buf  --ip4f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 4  --pure_color --use_idx_buf  --ip4f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 16  --pure_color --use_idx_buf  --ip4f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 64  --pure_color --use_idx_buf  --ip4f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 256  --pure_color --use_idx_buf  --ip4f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1024  --pure_color --use_idx_buf  --ip4f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 4096  --pure_color --use_idx_buf  --ip4f

%= Geometry shader without texture, not shaded, not instanced, multi blade =%
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --pure_color --gs_blades_per_run 1
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --pure_color --gs_blades_per_run 2
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --pure_color --gs_blades_per_run 4
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --pure_color --gs_blades_per_run 8

%= Geometry shader without texture, not shaded, not instanced, multi blade =%
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --pure_color --gs_blades_per_run 1 --ip1f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --pure_color --gs_blades_per_run 2 --ip1f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --pure_color --gs_blades_per_run 4 --ip1f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --pure_color --gs_blades_per_run 8 --ip1f

%= Geometry shader without texture, not shaded, not instanced, multi blade =%
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --pure_color --gs_blades_per_run 1 --ip2f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --pure_color --gs_blades_per_run 2 --ip2f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --pure_color --gs_blades_per_run 4 --ip2f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --pure_color --gs_blades_per_run 8 --ip2f

%= Geometry shader without texture, not shaded, not instanced, multi blade =%
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --pure_color --gs_blades_per_run 1 --ip3f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --pure_color --gs_blades_per_run 2 --ip3f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --pure_color --gs_blades_per_run 4 --ip3f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --pure_color --gs_blades_per_run 8 --ip3f

%= Geometry shader without texture, not shaded, not instanced, multi blade =%
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --pure_color --gs_blades_per_run 1 --ip4f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --pure_color --gs_blades_per_run 2 --ip4f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --pure_color --gs_blades_per_run 4 --ip4f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 1 --dc_per_tile 1 --gs_use_end_primitive --pure_color --gs_blades_per_run 8 --ip4f



dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 1 --pure_color --use_idx_buf
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 4 --pure_color --use_idx_buf
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 16 --pure_color --use_idx_buf
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 64 --pure_color --use_idx_buf
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 256 --pure_color --use_idx_buf
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 1024 --pure_color --use_idx_buf
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 4096 --pure_color --use_idx_buf

dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 1 --pure_color  --use_idx_buf --ip1f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 4 --pure_color  --use_idx_buf --ip1f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 16  --pure_color --use_idx_buf  --ip1f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 64  --pure_color --use_idx_buf  --ip1f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 256  --pure_color --use_idx_buf  --ip1f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 1024  --pure_color --use_idx_buf  --ip1f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 4096  --pure_color --use_idx_buf  --ip1f

dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 1  --pure_color --use_idx_buf  --ip2f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 4  --pure_color --use_idx_buf  --ip2f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 16  --pure_color --use_idx_buf  --ip2f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 64  --pure_color --use_idx_buf  --ip2f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 256  --pure_color --use_idx_buf  --ip2f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 1024  --pure_color --use_idx_buf  --ip2f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 4096  --pure_color --use_idx_buf  --ip2f

dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 1  --pure_color --use_idx_buf  --ip3f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 4  --pure_color --use_idx_buf  --ip3f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 16  --pure_color --use_idx_buf  --ip3f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 64  --pure_color --use_idx_buf  --ip3f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 256  --pure_color --use_idx_buf  --ip3f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 1024  --pure_color --use_idx_buf  --ip3f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 4096  --pure_color --use_idx_buf  --ip3f

dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 1  --pure_color --use_idx_buf  --ip4f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 4  --pure_color --use_idx_buf  --ip4f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 16  --pure_color --use_idx_buf  --ip4f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 64  --pure_color --use_idx_buf  --ip4f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 256  --pure_color --use_idx_buf  --ip4f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 1024  --pure_color --use_idx_buf  --ip4f
dctest64.exe --test1 --procedural-scene --proc_scene_mode 0 --dc_per_tile 4096  --pure_color --use_idx_buf  --ip4f