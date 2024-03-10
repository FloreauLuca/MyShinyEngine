set /p inputPath="Enter shader path : "

.\Externals\vulkan-1.3.275.0\Bin\glslc.exe %inputPath%.vert -o %inputPath%_vert.spv
.\Externals\vulkan-1.3.275.0\Bin\glslc.exe %inputPath%.frag -o %inputPath%_frag.spv
pause