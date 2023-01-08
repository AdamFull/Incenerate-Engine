import json, os

this_path = os.path.dirname(os.path.realpath(__file__))

def main():
    config = dict()
    with open(os.path.join(this_path, 'material_desc.json')) as jsonfile:
        config = json.load(jsonfile)

    with open(os.path.join(this_path, "sponza.gltf"), "rt") as gltffile:
        with open(os.path.join(this_path, "sponza_new.gltf"), "wt") as fout:
            for line in gltffile:
                need_to_write = True
                for key in config.keys():
                    if key in line:
                        fout.write(line.replace(key, config[key]))
                        need_to_write = False
                        break
                if need_to_write:
                    fout.write(line)

    # for root, dir, files in os.walk(this_path):
    #     for file in files:
    #         if file in config.keys():
    #             file_path = os.path.abspath(os.path.normpath(os.path.join(root, file)))
    #             out_file_path = os.path.abspath(os.path.normpath(os.path.join(root, config[file])))
    #             os.rename(file_path, out_file_path)
            
            

if __name__ == "__main__":
    main()