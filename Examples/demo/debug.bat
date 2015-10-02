
SET SWIG_ROOT_DIR=C:\Users\erik\Documents\repos\reposit\swig
SET SWIG_EXE=%SWIG_ROOT_DIR%\Release\swig.exe
SET SWIG_LIB_DIR=%SWIG_ROOT_DIR%\Lib
SET DEMO_INC_DIR=%SWIG_LIB_DIR%\demo

@REM %SWIG_EXE% -help
@REM %SWIG_EXE% -I%SWIG_LIB_DIR% -I%DEMO_INC_DIR% -DSWIG_MSVC -c++ -demo demo.i
%SWIG_EXE% -debug-top 4 -debug-tmsearch -I%SWIG_LIB_DIR% -I%DEMO_INC_DIR% -DSWIG_MSVC -c++ -demo demo.i > debug.txt

