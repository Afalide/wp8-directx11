
@@ECHO OFF

echo.
echo.
echo ---------------------------------------------------------
echo Building the color vertex shader
echo ---------------------------------------------------------

"C:\Program Files (x86)\Windows Kits\8.1\bin\x86\fxc.exe" ^
  /T vs_4_0_level_9_3 ^
  /Zi ^
  /E color_vertex_shader ^
  /Fo color_shader_vertex.bin ^
  color_shader_vertex.hlsl

echo.
echo.
echo ---------------------------------------------------------
echo Building the color pixel shader
echo ---------------------------------------------------------

"C:\Program Files (x86)\Windows Kits\8.1\bin\x86\fxc.exe" ^
  /T ps_4_0_level_9_3 ^
  /Zi ^
  /E color_pixel_shader ^
  /Fo color_shader_pixel.bin ^
  color_shader_pixel.hlsl

echo.
echo.
echo ---------------------------------------------------------
echo Building the texture vertex shader
echo ---------------------------------------------------------

"C:\Program Files (x86)\Windows Kits\8.1\bin\x86\fxc.exe" ^
  /T   vs_4_0_level_9_3            ^
  /Zi                              ^
  /E   texture_vertex_shader       ^
  /Fo  texture_shader_vertex.bin   ^
       texture_shader_vertex.hlsl  

echo.
echo.
echo ---------------------------------------------------------
echo Building the texture pixel shader
echo ---------------------------------------------------------

"C:\Program Files (x86)\Windows Kits\8.1\bin\x86\fxc.exe" ^
  /T   ps_4_0_level_9_3           ^
  /Zi                             ^
  /E   texture_pixel_shader       ^
  /Fo  texture_shader_pixel.bin   ^
       texture_shader_pixel.hlsl 


echo.
pause
