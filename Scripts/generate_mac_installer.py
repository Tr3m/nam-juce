import os
import argparse
import inspect

# ================================================================================

def generateUninstaller():
    print("Generating Uninstall Script...")

    product_name_escaped = product_name.replace(" ", "\\ ")

    uninstall_script = \
    f"""
    #!/bin/bash

    echo "Uninstalling {product_name}..."

    sudo rm -rf /Applications/{product_name_escaped}.app
    sudo rm -rf /Library/Audio/Plug-Ins/VST/{product_name_escaped}.vst3
    sudo rm -rf /Library/Audio/Plug-Ins/Components/{product_name_escaped}.component

    sudo pkgutil --forget {common_identifier}.app.pkg.{pkg_name}
    sudo pkgutil --forget {common_identifier}.vst3.pkg.{pkg_name}
    sudo pkgutil --forget {common_identifier}.au.pkg.{pkg_name}

    echo "Uninstall Finished!"
    """

    print(f"Exporting uninstall script as {installer_dir}/uninstall-{pkg_name}...\n")

    if args.dryrun:
        print(f'===== UNINSTALL SCRIPT =====\n\n{uninstall_script}\n\n===== END UNINSTALL SCRIPT =====\n')
    else:
        with open(f'{installer_dir}/uninstall-{pkg_name}', "w") as f:
            f.write(uninstall_script)

        os.system(f"sudo chmod +x {installer_dir}/uninstall-{pkg_name}" if args.sudo else f"chmod +x {installer_dir}/uninstall-{pkg_name}")
        print("Export Finished!\n")


# ================================================================================

def generate_preinstall_script(target_bundle, target_pkg, template, output):
    with open (template, 'r') as f:
        script = f.read().strip()

    target_append = f'''
if remove_if_matching_bundle "${target_bundle}" "${{EXPECTED_BUNDLE_IDS[@]}}"; then
    forget_packages "{target_pkg}" "${{EXPECTED_PACKAGE_IDS[@]}}"
fi

exit 0
    '''

    script += f'\n{target_append}'
    
    if args.dryrun:
        print(f'\n{script}')
        print(f'Write to: {output}')
    else:
        with open(output, 'w') as f:
            f.write(script)

        os.chmod(output, 0o755)


def generate_standalone_postinstall_script(template, output):
    with open (template, 'r') as f:
        script = f.read().strip()

    if args.dryrun:
        print(f'\n{script}')
        print(f'Write to: {output}')
    else:
        with open(output, 'w') as f:
            f.write(script)

        os.chmod(output, 0o755)

# ================================================================================

def set_component_plist(path_to_target, output):
    os.system(f'pkgbuild --analyze --root {path_to_target} {output}')
    os.system(f'plutil -replace BundleIsRelocatable -bool NO {output}')

# ================================================================================

parser = argparse.ArgumentParser(description='')
parser.add_argument('--dryrun', '-d', dest='dryrun', action='store_true', help='Dry run.')
parser.add_argument('--clear', '-c', dest='clear_artefacts', action='store_true', help='Clear Artefacts folder after installer is generated.')
parser.add_argument('--name', '-n', type=str, help='Override installer executable name.')
parser.add_argument('--archive', '-a', dest='archive', action='store_true', help='Archive reslulting executable.')
parser.add_argument('--uninstall', '-u', dest='uninstall', action='store_true', help='Generate Uninstall Script.')
parser.add_argument('--sudo', '-s', dest='sudo', action='store_true', help='Request sudo priviledges.')
parser.add_argument('--suffix', '-sf', dest='suffix', type=str, help='Specify dmg suffix')
parser.add_argument('--export', '-e', dest='export', action='store_true', help='Export Build Commands')
args = parser.parse_args()

os.system("sudo clear" if args.sudo else "clear") # Get sudo priviledges at the start of the script to avoid getting prompted later on...

script_root_dir = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))
repo_dir = os.path.abspath(os.path.join(script_root_dir, os.pardir))

with open(f'{repo_dir}/CMakeLists.txt', 'r') as f:
	cmake_lists_file = f.read()

project_name = [s for s in cmake_lists_file.split("\n") if "set(PROJECT_NAME" in s][0].strip().split("set(PROJECT_NAME \"")[1].replace("\")", "").strip()
version = [s for s in cmake_lists_file.split("\n") if "set(PLUGIN_VERSION" in s][0].strip().split("PLUGIN_VERSION ")[1].replace(")", "").strip()
product_name = [s for s in cmake_lists_file.split("\n") if "PRODUCT_NAME" in s][0].strip().split("PRODUCT_NAME \"")[1].replace("\"", "").strip()
pkg_name = product_name.replace(" ", "")
installer_dir =  f"{repo_dir}/Installers/macos"
pkg_output_dir =f"{installer_dir}/pkg"
artefacts_dir = f"{repo_dir}/build/{project_name}_artefacts"
common_identifier = "com.ManosLabrakis"

if args.name != None:
	pkg_name = str(args.name).strip().replace(" ", "")

print(f'{project_name} {version}\n')

if args.uninstall:
    generateUninstaller()
    exit()

if not os.path.isdir(f'{pkg_output_dir}'):
	print(f'Creating {pkg_output_dir}...\n')
	if not args.dryrun:
		os.system(f"mkdir -p {pkg_output_dir}")

if not os.path.isdir(f'{pkg_output_dir}/scripts/app'):
    os.system(f"mkdir -p {pkg_output_dir}/scripts/app")
if not os.path.isdir(f'{pkg_output_dir}/scripts/vst'):
    os.system(f"mkdir -p {pkg_output_dir}/scripts/vst")
if not os.path.isdir(f'{pkg_output_dir}/scripts/au'):
    os.system(f"mkdir -p {pkg_output_dir}/scripts/au")

# Build Packages

# Standalone Package
print("Creating Standalone PKG...\n")
generate_preinstall_script("APP", "app", f'{script_root_dir}/preinstall.sh', f'{pkg_output_dir}/scripts/app/preinstall')
generate_standalone_postinstall_script(f'{script_root_dir}/postinstall.sh', f'{pkg_output_dir}/scripts/app/postinstall') 
set_component_plist(f'{artefacts_dir}/Release/Standalone', f'{pkg_output_dir}/app-components.plist')
standalone_command = f'pkgbuild --root {artefacts_dir}/Release/Standalone/ --component-plist {pkg_output_dir}/app-components.plist --identifier {common_identifier}.app.pkg.{pkg_name} --version {version} --scripts {pkg_output_dir}/scripts/app --install-location "/Applications" {pkg_output_dir}/{pkg_name}_APP.pkg'
if args.dryrun:
	print(f'{standalone_command}\n')
else:
    if args.export:
        print(standalone_command)
    else:
        os.system(standalone_command)
    print("\n")

# VST3 Package
print("Creating VST3 PKG...\n")
generate_preinstall_script("VST", "vst3", f'{script_root_dir}/preinstall.sh', f'{pkg_output_dir}/scripts/vst/preinstall')
set_component_plist(f'{artefacts_dir}/Release/VST3', f'{pkg_output_dir}/vst-components.plist')
vst3_command = f'pkgbuild --root {artefacts_dir}/Release/VST3/ --component-plist {pkg_output_dir}/vst-components.plist --identifier {common_identifier}.vst3.pkg.{pkg_name} --version {version} --scripts {pkg_output_dir}/scripts/vst --install-location "/Library/Audio/Plug-Ins/VST" {pkg_output_dir}/{pkg_name}_VST3.pkg'
if args.dryrun:
	print(f'{vst3_command}\n')
else:
    if args.export:
        print(vst3_command)
    else:
        os.system(vst3_command)
    print("\n")

# AU Package
print("Creating AU PKG...\n")
generate_preinstall_script("AU", "au", f'{script_root_dir}/preinstall.sh', f'{pkg_output_dir}/scripts/au/preinstall')
set_component_plist(f'{artefacts_dir}/Release/AU', f'{pkg_output_dir}/au-components.plist')
au_command = f'pkgbuild --root {artefacts_dir}/Release/AU/ --component-plist {pkg_output_dir}/au-components.plist --identifier {common_identifier}.au.pkg.{pkg_name} --version {version} --scripts {pkg_output_dir}/scripts/au --install-location "/Library/Audio/Plug-Ins/Components" {pkg_output_dir}/{pkg_name}_AU.pkg'
if args.dryrun:
	print(f'{au_command}\n')
else:
    if args.export:
        print(au_command)
    else:
        os.system(au_command)
    print("\n")

# Uninstaller
generateUninstaller()

VST3_PKG_REF= f'<pkg-ref id="com.ManosLabrakis.vst3.pkg.{pkg_name}"/>'
VST3_CHOICE= f'<line choice="com.ManosLabrakis.vst3.pkg.{pkg_name}"/>'
VST3_CHOICE_DEF= f'<choice id="com.ManosLabrakis.vst3.pkg.{pkg_name}" visible="true" start_selected="true" title="VST3 Plug-in"><pkg-ref id="com.ManosLabrakis.vst3.pkg.{pkg_name}"/></choice><pkg-ref id="com.ManosLabrakis.vst3.pkg.{pkg_name}" version="{version}" onConclusion="none">{pkg_name}_VST3.pkg</pkg-ref>'

AU_PKG_REF= f'<pkg-ref id="com.ManosLabrakis.au.pkg.{pkg_name}"/>'
AU_CHOICE= f'<line choice="com.ManosLabrakis.au.pkg.{pkg_name}"/>'
AU_CHOICE_DEF= f'<choice id="com.ManosLabrakis.au.pkg.{pkg_name}" visible="true" start_selected="true" title="Audio Unit (v2) Plug-in"><pkg-ref id="com.ManosLabrakis.au.pkg.{pkg_name}"/></choice><pkg-ref id="com.ManosLabrakis.au.pkg.{pkg_name}" version="{version}" onConclusion="none">{pkg_name}_AU.pkg</pkg-ref>'

APP_PKG_REF= f'<pkg-ref id="com.ManosLabrakis.app.pkg.{pkg_name}"/>'
APP_CHOICE= f'<line choice="com.ManosLabrakis.app.pkg.{pkg_name}"/>'
APP_CHOICE_DEF= f'<choice id="com.ManosLabrakis.app.pkg.{pkg_name}" visible="true" start_selected="true" title="Standalone App"><pkg-ref id="com.ManosLabrakis.app.pkg.{pkg_name}"/></choice><pkg-ref id="com.ManosLabrakis.app.pkg.{pkg_name}" version="{version}" onConclusion="none">{pkg_name}_APP.pkg</pkg-ref>'

# Generate distribution xml and invoke productbuild

xml_config = \
f'''<?xml version="1.0" encoding="utf-8"?>
<installer-gui-script minSpecVersion="1">
    <title>{pkg_name} {version}</title>
    <license file="{script_root_dir}/resources/license.rtf" mime-type="application/rtf"/>
    <background file="{pkg_name}-installer-bg.png" alignment="topleft" scaling="none"/>
    {VST3_PKG_REF}
    {AU_PKG_REF}
    {APP_PKG_REF}
    <options require-scripts="false" customize="always" hostArchitectures="arm64,x86_64"/>
    <choices-outline>
        {VST3_CHOICE}
        {AU_CHOICE}        
        {APP_CHOICE}
    </choices-outline>
    {VST3_CHOICE_DEF}
    {AU_CHOICE_DEF}
    {APP_CHOICE_DEF}
</installer-gui-script>'''

print(f'Writing xml to {installer_dir}/distribution.xml...\n')

if args.dryrun:
    print(f'===== XML CONFIG =====\n\n{xml_config}\n\n===== END XML CONFIG =====\n')

if not args.dryrun:
    with open(f'{installer_dir}/distribution.xml' , 'w') as f:
        f.write(xml_config)

build_command = f'productbuild --resources {installer_dir} --distribution {installer_dir}/distribution.xml --package-path {pkg_output_dir} "{installer_dir}/{pkg_name}-setup.pkg"'


if args.dryrun:
    print(f'{build_command}\n')
else:
    if args.export:
        print(build_command)
    else:
        os.system(build_command)
    print("\n")

# Set installer icon

print("Setting installer icon...\n")

installer_pkg_path = f'{installer_dir}/{pkg_name}-setup.pkg'
icon_path = f'{repo_dir}/Assets/ICON.png'

set_icon_script = \
f"""
sips -z 128 128 {icon_path} -s format icns --out {installer_dir}/tmp.icns
echo "read 'icns' (-16455) \\"{installer_dir}/tmp.icns\\";" >> {installer_dir}/tmpicns.rsrc
cd {installer_dir}
Rez -a tmpicns.rsrc -o {pkg_name}-setup.pkg
SetFile -a C {pkg_name}-setup.pkg
rm tmp.icns && rm tmpicns.rsrc
"""

if args.dryrun or args.export:
    print(f'===== PKG ICON SCRIPT =====\n\n{set_icon_script}\n\n===== END PKG ICON SCRIPT =====\n')
else:
    os.system(set_icon_script)
    print("\n")

if args.clear_artefacts:
    print(f'Clearing {artefacts_dir}...\n')
    if not args.dryrun:
        os.system(f'sudo rm -rf {artefacts_dir}/Release/' if args.sudo else f'rm -rf {artefacts_dir}/Release/')

# Create dmg archive
# https://github.com/create-dmg/create-dmg

if args.archive:
    exec_name = f'{pkg_name}-setup.pkg'
    archive_name = f'{pkg_name}-v{version}-{args.suffix if args.suffix else "mac"}.dmg'
    dmg_output_dir = f"{installer_dir}/installer"
    archive_dest = f'{dmg_output_dir}/{archive_name}'

    if not os.path.exists(f'{dmg_output_dir}'):
        print(f'Creating {dmg_output_dir}...\n')
        if not args.dryrun:
            os.system(f'mkdir {dmg_output_dir}')
    else:
        os.system(f'rm {dmg_output_dir}/*')

    print("Setting up output directory...\n")

    if not args.dryrun:
        os.system(f'cp {installer_dir}/{exec_name} {dmg_output_dir} && cp {installer_dir}/uninstall-{pkg_name} {dmg_output_dir}')

    clone_command = f'git clone https://github.com/create-dmg/create-dmg {installer_dir}/create-dmg'

    if not os.path.exists(f'{installer_dir}/create-dmg/create-dmg'):
        if args.dryrun:
            print(f'{clone_command}\n')
        else:
            os.system(clone_command)
            print("\n")
    
    if os.path.exists(f'{installer_dir}/{archive_name}'):
        if args.dryrun:
            print(f'rm {installer_dir}/{archive_name}\n')
        else:
            os.system(f'rm {installer_dir}/{archive_name}')
            print("\n")

    print(f'Archiving as {archive_dest}\n')

    archive_command = f'sh {installer_dir}/create-dmg/create-dmg --volname {pkg_name} {installer_dir}/{archive_name} {dmg_output_dir}'

    if args.dryrun:
        print(f'{archive_command}\n')
    else:
        os.system(archive_command)
        os.system(f'rm -rf {dmg_output_dir}')
        print("\n")

print("Done!")
