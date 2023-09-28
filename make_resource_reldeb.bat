@echo off

set workdir_path=%cd%
set binary_directory_path=%workdir_path%\bin\RelWithDebInfo
set shader_compiler_path=%binary_directory_path%\tools\shader_compiler\shader_compiler
set engine_binaries_path=%binary_directory_path%\engine

python make_project.py -ex %shader_compiler_path% -wd %workdir_path% -bd %engine_binaries_path%
pause