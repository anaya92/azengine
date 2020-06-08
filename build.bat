SET RAYLIB_PATH=C:\raylib\raylib
SET COMPILER_PATH=C:\raylib\mingw\bin
SET CC=%COMPILER_PATH%\gcc.exe
SET CFLAGS=%RAYLIB_PATH%\src\raylib.rc.data "-IC:\Program Files (x86)\Lua\5.3\include" "-LE:\Files\programming\azragame\ext\lib" "-LC:\Program Files (x86)\Lua\5.3\lib" -w -s -static -Os -std=c99 -Iexternal -DPLATFORM_DESKTOP
SET LDFLAGS=-lraylib -llua53 -lpthread -lopengl32 -lgdi32 -lwinmm -lasmxml
cmd /c IF EXIST az.exe del /F az.exe
%CC% -o az.exe *.c %CFLAGS% %LDFLAGS%
@echo on
