vcpkg install vcpkg-tool-ninja libpng freetype opengl glfw3 glew sdl2 sdl2_image
if %errorlevel% neq 0 exit /b %errorlevel%
pip install pypng lz4
if %errorlevel% neq 0 exit /b %errorlevel%
