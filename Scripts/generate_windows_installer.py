import os
import argparse
import inspect
import py7zr

# ========================================================================

def get_changelog(changelog_path, version):
    with open (changelog_path, "r") as f:
        changelog = f.read().strip()

    changelog = changelog.replace("Changelog", f'Changelog (v{version})')

    return changelog

# ========================================================================

parser = argparse.ArgumentParser(description='')
parser.add_argument('--dryrun', '-d', dest='dryrun', action='store_true', help='Dry run.')
parser.add_argument('--name', '-n', type=str, help='Override installer executable name.')
parser.add_argument('--archive', '-a', dest='archive', action='store_true', help='Archive reslulting executable.')
args = parser.parse_args()

if not os.name == 'nt':
    print("Script only usable in Windows!\nExiting...")
    exit()

os.system('cls')

script_root_dir = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))
repo_dir = os.path.abspath(os.path.join(script_root_dir, os.pardir))

with open(f'{repo_dir}/CMakeLists.txt', 'r') as f:
	cmake_lists_file = f.read()

project_name = [s for s in cmake_lists_file.split("\n") if "set(PROJECT_NAME" in s][0].strip().split("set(PROJECT_NAME \"")[1].replace("\")", "").strip()
version = [s for s in cmake_lists_file.split("\n") if "set(PLUGIN_VERSION" in s][0].strip().split("PLUGIN_VERSION ")[1].replace(")", "").strip()
product_name = [s for s in cmake_lists_file.split("\n") if "PRODUCT_NAME" in s][0].strip().split("PRODUCT_NAME \"")[1].replace("\"", "").strip()
app_publisher = [s for s in cmake_lists_file.split("\n") if "COMPANY_NAME" in s][0].strip().split("COMPANY_NAME \"")[1].replace("\"", "").strip()
artefacts_dir = f"{repo_dir}\\build\\{project_name}_artefacts"
installer_exe_name = product_name.replace(" ", "")

if args.name != None:
	installer_exe_name = str(args.name).strip().replace(" ", "") 

print(f'{project_name} {version}\n')

binary_output_dir = f"{repo_dir}/Installers/windows"

if not os.path.exists(f'{binary_output_dir}'):
	print(f'Creating {repo_dir}\\Installers\\windows...\n')
	if not args.dryrun:
		os.system(f"mkdir {repo_dir}\\Installers\\windows")

# Generate Inno Setup installer script

iss_script_body = \
f"""
[Setup]
AppName={product_name}
AppPublisher={app_publisher}
OutputBaseFilename={installer_exe_name if args.name == None else args.name}-setup
AppVersion={version}
AppId={{{{B2ABFFFD-7CF1-4DCB-B6A2-9B5714275278}}
WizardStyle=modern
Compression=lzma2
SolidCompression=yes
SetupLogging=yes
DefaultDirName={{autopf64}}\\{product_name}
DefaultGroupName={product_name}
OutputDir={repo_dir}\\Installers\\windows
WizardSmallImageFile={repo_dir}\\Scripts\\resources\\icon.bmp
WizardImageFile={repo_dir}\\Scripts\\resources\\banner.bmp
LicenseFile={repo_dir}\\Scripts\\resources\\license.rtf
InfoBeforeFile={repo_dir}\\Scripts\\resources\\THIRD-PARTY-NOTICES.txt


[Types]
Name: "full"; Description: "Full installation"
Name: "custom"; Description: "Custom installation"; Flags: iscustom

[Components]
Name: "VST3"; Description: "VST3"; Types: full custom; Flags: fixed 
Name: "Standalone"; Description: "Standalone"; Types: full;

[Files]
Source: "{artefacts_dir}\\Release\\Standalone\\{product_name}.exe"; DestDir: "{{app}}"; Components: Standalone
Source: "{artefacts_dir}\\Release\\VST3\\{product_name}.vst3\\Contents\\x86_64-win\\{product_name}.vst3"; DestDir: "{{autopf64}}\\Common Files\\VST3"; Components: VST3

[Icons]
Name: "{{group}}\\{product_name}"; Filename: "{{app}}\\{product_name}.exe"; Components: Standalone
Name: "{{group}}\\Uninstall {product_name}"; Filename: "{{app}}\\unins000.exe"
"""

if args.dryrun:
    print(f'===== ISS SCRIPT =====\n\n{iss_script_body}\n\n===== END ISS SCRIPT =====\n')

print(f'Writing iss script to {repo_dir}\\Installers\\windows\\{installer_exe_name}.iss...\n')

if not args.dryrun:
    with open(f'{repo_dir}/Installers/windows/{installer_exe_name}.iss' , 'w') as f:
        f.write(iss_script_body)

# Download and invoke Inno Setup portable
# https://github.com/portapps/innosetup-portable/

isp_dir = f'{repo_dir}\\Installers\\windows\\isp'

if not os.path.exists(f'{binary_output_dir}/isp'):
	print(f'Dowanloading inno setup portable...\n')
	if not args.dryrun:
		
		os.system(f"mkdir {isp_dir}")

		dl_link = "https://github.com/portapps/innosetup-portable/releases/download/6.2.0-5/innosetup-portable-win32-6.2.0-5.7z"

		os.system(f"curl -L -o {repo_dir}\\Installers\\windows\\isp\\isp.7z {dl_link}")
		archive = py7zr.SevenZipFile(f'{repo_dir}/Installers/windows/isp/isp.7z', mode='r')
		archive.extractall(path=f"{repo_dir}/Installers/windows/isp/")
		archive.close()
		os.system(f'del {isp_dir}\\isp.7z')

print("Compiling installer...\n")

issc_command = f'{isp_dir}\\app\\ISCC.exe {repo_dir}\\Installers\\windows\\{installer_exe_name}.iss'

if args.dryrun:
	print(f"{issc_command}\n")
else:
	os.system(issc_command)

print(f'Exporting Changelog...')
changelog = get_changelog(f'{repo_dir}/CHANGELOG.md', version)

if args.dryrun:
    print("====== CHANGELOG ======")
    print(changelog)
    print("====== END CHANGELOG ======")
    print(f'Output to: {repo_dir}\\Installers\\windows\\CHANGELOG.txt\n')
else:
    with open(f'{repo_dir}\\Installers\\windows\\CHANGELOG.txt', "w") as f:
        f.write(changelog)

if args.archive:
    exec_name = f'{installer_exe_name if args.name == None else args.name}-setup.exe'
    archive_name = f'{installer_exe_name if args.name == None else args.name}-v{version}-win.zip'
    archive_dest = f'{repo_dir}\\Installers\\windows'
    print(f'Archiving as {archive_dest}\n')

    archive_command = f'powershell Compress-Archive\
            -Path {archive_dest}\\{exec_name}, {archive_dest}\\CHANGELOG.txt\
            {archive_dest}\\{archive_name}'

    if args.dryrun:
        print(f'{archive_command}\n')
    else:
        if os.path.exists(f'{archive_dest}\\{archive_name}'):
            os.remove(f'{archive_dest}\\{archive_name}')
        os.system(archive_command)
        print("\n")

print("Done!")
