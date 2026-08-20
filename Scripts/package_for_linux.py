import os
import argparse
import inspect

# ================================================================================

def create_directory_structure(output_dir):
    if os.path.isdir(f'{output_dir}'):
        if not args.dryrun:
            os.system(f'rm -rf {output_dir}')
    else:
        print(f'Creating {output_dir}\n')
        if not args.dryrun:
            os.system(f"mkdir -p {output_dir}")

    print(f'Creating {output_dir}/nam-juce/bin...')
    if not args.dryrun:
        os.system(f'mkdir -p {output_dir}/nam-juce/bin')

    print(f'Creating {output_dir}/nam-juce/vst...')
    if not args.dryrun:
        os.system(f'mkdir -p {output_dir}/nam-juce/vst')

    print(f'Creating {output_dir}/nam-juce/resources...')
    if not args.dryrun:
        os.system(f'mkdir -p {output_dir}/nam-juce/resources')

# ================================================================================

parser = argparse.ArgumentParser(description='')
parser.add_argument('--dryrun', '-d', dest='dryrun', action='store_true', help='Dry run.')
parser.add_argument('--name', '-n', type=str, help='Override installer executable name.')
parser.add_argument('--archive', '-a', dest='archive', action='store_true', help='Archive reslulting executable.')
args = parser.parse_args()

os.system('clear')

script_root_dir = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))
repo_dir = os.path.abspath(os.path.join(script_root_dir, os.pardir))

with open(f'{repo_dir}/CMakeLists.txt', 'r') as f:
	cmake_lists_file = f.read()

project_name = [s for s in cmake_lists_file.split("\n") if "set(PROJECT_NAME" in s][0].strip().split("set(PROJECT_NAME \"")[1].replace("\")", "").strip()
version = [s for s in cmake_lists_file.split("\n") if "set(PLUGIN_VERSION" in s][0].strip().split("PLUGIN_VERSION ")[1].replace(")", "").strip()
product_name = [s for s in cmake_lists_file.split("\n") if "PRODUCT_NAME" in s][0].strip().split("PRODUCT_NAME \"")[1].replace("\"", "").strip()
app_publisher = [s for s in cmake_lists_file.split("\n") if "COMPANY_NAME" in s][0].strip().split("COMPANY_NAME \"")[1].replace("\"", "").strip()
artefacts_dir = f"{repo_dir}/build/{project_name}_artefacts"
pkg_name = f'{product_name.replace(" ", "")}-v{version}-linux_x86_64'

if args.name != None:
	pkg_name = f'{str(args.name).strip().replace(" ", "")}-v{version}-linux_x86_64'

print(f'{project_name} {version}\n')

output_dir = f"{repo_dir}/Installers/linux/{pkg_name}"
create_directory_structure(output_dir)

binary_file = f'{artefacts_dir}/Release/Standalone/Neural Amp Modeler'
vst_file = f'{artefacts_dir}/Release/VST3/Neural Amp Modeler.vst3'
icon_file = f'{repo_dir}/Assets/ICON.png'
license_file = f'{repo_dir}/Scripts/resources/license.rtf'

if os.path.exists(binary_file):
    print(f'Copying Standalone Binary to {output_dir}/nam-juce/bin/nam-juce...')
    if not args.dryrun:
        os.system(f'cp "{artefacts_dir}/Release/Standalone/Neural Amp Modeler" "{output_dir}/nam-juce/bin/nam-juce"')

if os.path.exists(vst_file):
    print(f'Copying Standalone Binary to {output_dir}/nam-juce/vst/Neural Amp Modeler.vst3...')
    if not args.dryrun:
        os.system(f'cp -r "{artefacts_dir}/Release/VST3/Neural Amp Modeler.vst3" "{output_dir}/nam-juce/vst/"')

print(f'Copying icon file to {output_dir}/nam-juce/resources/icon.png')
if not args.dryrun:
    os.system(f'cp "{icon_file}" "{output_dir}/nam-juce/resources/icon.png"')

print(f'Copying licence.rtf to {output_dir}/nam-juce/license.rtf')
if not args.dryrun:
    os.system(f'cp "{license_file}" "{output_dir}/nam-juce/license.rtf"')
    os.system(f'cp "{script_root_dir}/resources/THIRD-PARTY-NOTICES.txt" "{output_dir}/nam-juce/"')

with open(f'{repo_dir}/Scripts/install-nam-juce.sh', 'r') as f:
    install_script = f.read().strip()

install_script = install_script.replace("PLUGIN_VERSION", version)

print("Exporting installation script...")
if args.dryrun:
    print("========= INSTALLATION SCRIPT =========")
    print(install_script)
    print("========= END INSTALLATION SCRIPT =========")
    print(f'\nOutput to: {output_dir}/install-nam-juce.sh')
else:
    with open(f'{output_dir}/install-nam-juce.sh', 'w') as f:
        f.write(install_script)

    os.chmod(f'{output_dir}/install-nam-juce.sh', 0o755)

if args.archive:
    print("Archiving...")
    command = f'tar -czf {output_dir}.tar.gz -C {repo_dir}/Installers/linux {pkg_name}'
    print(command)
    if not args.dryrun:
        os.system(command)

print("Done!")
