C:\VulkanSDK\1.0.61.0\Bin\glslangValidator -V sprite.vert -o ..\..\..\aether3d_build\Samples\sprite_vert.spv
C:\VulkanSDK\1.0.61.0\Bin\glslangValidator -V sprite.frag -o ..\..\..\aether3d_build\Samples\sprite_frag.spv
C:\VulkanSDK\1.0.61.0\Bin\glslangValidator -V unlit.vert -o ..\..\..\aether3d_build\Samples\unlit_vert.spv
C:\VulkanSDK\1.0.61.0\Bin\glslangValidator -V unlit.frag -o ..\..\..\aether3d_build\Samples\unlit_frag.spv
C:\VulkanSDK\1.0.61.0\Bin\glslangValidator -V unlit_skin.vert -o ..\..\..\aether3d_build\Samples\unlit_skin_vert.spv
C:\VulkanSDK\1.0.61.0\Bin\glslangValidator -V skybox.vert -o ..\..\..\aether3d_build\Samples\skybox_vert.spv
C:\VulkanSDK\1.0.61.0\Bin\glslangValidator -V skybox.frag -o ..\..\..\aether3d_build\Samples\skybox_frag.spv
C:\VulkanSDK\1.0.61.0\Bin\glslangValidator -V depthnormals.vert -o ..\..\..\aether3d_build\Samples\depthnormals_vert.spv
C:\VulkanSDK\1.0.61.0\Bin\glslangValidator -V depthnormals.frag -o ..\..\..\aether3d_build\Samples\depthnormals_frag.spv

REM C:\VulkanSDK\1.0.61.0\Bin\spirv-opt ..\..\..\aether3d_build\Samples\sprite_vert.spv --inline-entry-points-exhaustive --convert-local-access-chains --eliminate-local-single-block --eliminate-local-single-store --eliminate-insert-extract --eliminate-dead-code-aggressive --eliminate-dead-branches --merge-blocks --eliminate-local-single-block --eliminate-local-single-store --eliminate-local-multi-store --eliminate-insert-extract --eliminate-dead-code-aggressive --eliminate-common-uniform -o ..\..\..\aether3d_build\Samples\sprite_vert_opt.spv
REM C:\VulkanSDK\1.0.61.0\Bin\spirv-remap --strip all --dce all -i ..\..\..\aether3d_build\Samples\sprite_vert_opt.spv -o ..\..\..\aether3d_build\Samples\
pause
