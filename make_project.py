import subprocess, os, shutil, json
from argparse import ArgumentParser
from types import SimpleNamespace

def main():
	parser = ArgumentParser(description='Project prepare script.')

	parser.add_argument('-ex', type=str, action='store', help='Shader compiler executable path.')
	parser.add_argument('-wd', type=str, action='store', default=os.getcwd(), help='Working directory path.')
	parser.add_argument('-bd', type=str, action='store', help='Output binary directory.')
	args = parser.parse_args()

	working_directory = args.wd
	binary_directory = args.bd
	engine_embedded_resources_directory = os.path.join(binary_directory, "embed")
	embed_directory = os.path.join(working_directory, "embed")

	shader_compiler_api = "vk13" # parse from config
	shader_compile_type = "release" # parse from config

	with open(os.path.join(embed_directory, "config.json"), "r") as configfile:
		engine_config = json.load(configfile, object_hook=lambda d: SimpleNamespace(**d))
		shader_compiler_api = engine_config.api

	shader_compiler_path = args.ex
	shader_input_directory = os.path.join(embed_directory, "shaders")
	shader_output_directory = os.path.join(engine_embedded_resources_directory, "shader_libraries")
	shader_common_directory = os.path.join(shader_input_directory, "common")
	shader_includes_directory = os.path.join(shader_input_directory, "includes")

	shader_compiler_comand = [
		shader_compiler_path, 
		"-in", shader_input_directory,
		"-out", shader_output_directory,
		"-common", shader_common_directory,
		"-includes", shader_includes_directory,
		"-api", shader_compiler_api,
		"-build_type", shader_compile_type
		]
	
	subprocess.Popen(shader_compiler_comand).communicate()

	shutil.copy(os.path.join(embed_directory, "shaders.json"), os.path.join(engine_embedded_resources_directory, "shaders.json"))
	shutil.copy(os.path.join(embed_directory, "materialeditor.json"), os.path.join(engine_embedded_resources_directory, "materialeditor.json"))
	shutil.copy(os.path.join(embed_directory, "imgui.ini"), os.path.join(binary_directory, "imgui.ini"))
	shutil.copy(os.path.join(embed_directory, "frameconfig.json"), os.path.join(engine_embedded_resources_directory, "frameconfig.json"))
	shutil.copy(os.path.join(embed_directory, "config.json"), os.path.join(engine_embedded_resources_directory, "config.json"))
	shutil.copytree(os.path.join(embed_directory, "font"), os.path.join(engine_embedded_resources_directory, "font"))
	shutil.copytree(os.path.join(embed_directory, "icon"), os.path.join(engine_embedded_resources_directory, "icon"))

if __name__ == "__main__":
	main()