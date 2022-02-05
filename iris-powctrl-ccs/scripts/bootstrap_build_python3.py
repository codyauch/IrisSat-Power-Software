# Check python version
import sys
if not (sys.version_info.major == 3 and sys.version_info.minor >= 6):
    print("This script requires Python 3.6 or higher!")
    print("You are using Python {}.{}.".format(sys.version_info.major, sys.version_info.minor))
    sys.exit(1)

################################################################
# Script flags
import argparse
parser = argparse.ArgumentParser()

# Options:
parser.add_argument("--debug", dest="is_debug", default=True, help="Enable debug build")
parser.add_argument("--toolchain",dest="usr_tools",default="",help="path to the compiler tools",type=str)

# Parse arguments
args = parser.parse_args()

################################################################
# Paths
import os
import os.path

# Set cwd
print("Changing current working directory to the repository root")
repository_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(repository_dir)

softconsole_path = "./"
libraries_path = "./Libraries"

################################################################
# Environment variables
import utils

# if args.usr_tools != "":
#    os.environ['AR'] = os.path.join(args.usr_tools, 'msp430-elf-ar.exe')
#    os.environ['CC'] = os.path.join(args.usr_tools, 'msp430-elf-gcc.exe')
#    os.environ['CXX'] = os.path.join(args.usr_tools, 'msp430-elf-g++.exe')

# elif utils.is_windows:
import glob
stm_toolchain_glob = glob.glob("C:/ti/ccs1040/ccs/tools/compiler/msp430-gcc-9.3.1.11_win64/bin", recursive=True)
# stm_toolchain_glob = glob.glob("C:/Users/mckoyj/Downloads/msp430-gcc-9.3.1.11_win64/msp430-gcc-9.3.1.11_win64/bin", recursive=True)
print("HERE")
print(stm_toolchain_glob)
print("HERE")
if stm_toolchain_glob:
    stm_toolchain_path = os.path.normpath(stm_toolchain_glob[0])
    sys.path.extend(stm_toolchain_path)
    os.environ['AR'] = os.path.join(stm_toolchain_path, 'msp430-elf-ar.exe')
    os.environ['CC'] = os.path.join(stm_toolchain_path, 'msp430-elf-gcc.exe')
    os.environ['CXX'] = os.path.join(stm_toolchain_path, 'msp430-elf-g++.exe')
else:
    os.environ['AR'] = 'msp430-elf-ar.exe'
    os.environ['CC'] = 'msp430-elf-gcc.exe'
    os.environ['CXX'] = 'msp430-elf-g++.exe'
# else:
#    os.environ['AR'] = 'msp430-elf-ar'
#    os.environ['CC'] = 'msp430-elf-gcc'
#    os.environ['CXX'] = 'msp430-elf-g++'

################################################################
# Optimization Flags

optimization_flags = "-O3"  # Release build: enable all optimizations

if args.is_debug:
    optimization_flags = "-O0 -g"  # Debug: enable debug flags and disable all optimizations

################################################################
# Bootstrapping dependencies
import subprocess
################################################################
# Submodules
'''
print("Downloading submodules...")
subprocess.run("git submodule update --init --recursive", shell=True, check=True)

################################################################
import utils

# FreeRTOS
version_FREERTOS = "V9.0.0"
utils.download_git_branch(version_FREERTOS, "https://github.com/FreeRTOS/FreeRTOS-Kernel", libraries_path, "FreeRTOS-Kernel")

freertos_kernel_folder = os.path.join(libraries_path, 'FreeRTOS-Kernel')

# this is not needed for V10
if version_FREERTOS == "V9.0.0":
    # Move files to workaround the old folder structure of FreeRTOS-Kernel
    import shutil
    import pathlib
    source_folder = os.path.join(freertos_kernel_folder, 'FreeRTOS', 'Source')
    shutil.move(source_folder, libraries_path)     # move source content temporarily

    # Delete unneeded files
    shutil.rmtree(freertos_kernel_folder, ignore_errors=True)
    shutil.rmtree(os.path.join(freertos_kernel_folder, 'FreeRTOS'), ignore_errors=True)
    shutil.rmtree(os.path.join(freertos_kernel_folder, 'FreeRTOS-Plus'), ignore_errors=True)
    pathlib.Path(freertos_kernel_folder).mkdir(parents=True, exist_ok=True)

    # move content of source to freertos kernel folder
    source_temp_dir = os.path.join(libraries_path, 'Source')
    source_content = os.listdir(source_temp_dir)
    for file in source_content:
        shutil.move(os.path.join(source_temp_dir, file), os.path.join(freertos_kernel_folder, file))
    os.rmdir(source_temp_dir)


# remove portable directories not needed for Arm CORTEX M3
portable_folder = os.path.join(freertos_kernel_folder, 'portable')
portable_content = os.listdir(portable_folder)
for file in portable_content:
    if file != "portable" and file != "CCS" and file != "MemMang":
        shutil.rmtree(os.path.join(portable_folder, file), ignore_errors=True)

gcc_folder = os.path.join(portable_folder, "CCS")
gcc_content = os.listdir(gcc_folder)
for file in gcc_content:
    if file != "CCS" and file != "MSP430X":
        shutil.rmtree(os.path.join(gcc_folder, file), ignore_errors=True)


# Use heap_4.c from MemMang
memmang_folder = os.path.join(portable_folder, "MemMang")
memmang_content = os.listdir(memmang_folder)
for file in memmang_content:
    if file != "MemMang" and file != "heap_4.c":
        os.remove(os.path.join(memmang_folder, file))

################################################################
# libcsp
print("Downloading libcsp...")
version_libcsp = "track-4a9d" # tracks 49ad commit which adds endian selection needed to build for stm32 hard floating point without error.
utils.download_git_branch(version_libcsp, " https://github.com/IrisSat/libcsp", libraries_path, "libcsp")
'''
print("Building libcsp...")
try:
    # project_path = "../.."
    project_path = "C:/Users/mckoyj/Desktop/IrisSat-Software/IrisSat-Power-Software/iris-powctrl-ccs"
    #INCLUDES = f"{project_path},"
    INCLUDES = f"{project_path}/driverlib/MSP430FR2xx_4xx,"
    INCLUDES += f"{project_path}/driverlib/MSP430FR2xx_4xx/inc,"
    INCLUDES += f"{project_path}/driverlib/MSP430FR2xx_4xx/rom_headers,"
    INCLUDES += "C:/ti/ccs1040/ccs/ccs_base/msp430/include,"
    INCLUDES += "C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-msp430_20.2.5.LTS/include,"
    #INCLUDES += f"{project_path}/msp430/include,"
    INCLUDES += f"{project_path}/include,"
    INCLUDES += f"{project_path}/Libraries,"
    INCLUDES += f"{project_path}/Libraries/FreeRTOS-Kernel,"
    INCLUDES += f"{project_path}/Libraries/FreeRTOS-Kernel/portable,"
    INCLUDES += f"{project_path}/Libraries/FreeRTOS-Kernel/include,"
    INCLUDES += f"{project_path}/Libraries/FreeRTOS-Kernel/portable/CCS/MSP430X,"
    #INCLUDES += f"{project_path}/Full_Demo/FreeRTOS+CLI,"
    #INCLUDES += f"{project_path}/Full_Demo/Standard_Demo_Tasks/include,"

    os.environ["CFLAGS"] = f"-Wall -std=gnu11 {optimization_flags} -mmcu=msp430x"
    # os.environ["CFLAGS"] = f"-I $C:/ti/ccs1040/ccs/tools/compiler/msp430-gcc-9.3.1.11_win64/ -mmcu=msp430x -o2 -g"

    os.chdir(f"{libraries_path}/libcsp")
    print("configuring...")
    # subprocess.run(f'python waf configure --toolchain=msp430-elf- --endian=little --enable-if-can --with-os=freertos --includes="{INCLUDES}" -v', shell=True, check=True)
    subprocess.run(f'python3 waf2 configure --toolchain=msp430-elf- --endian=little --enable-if-can --with-os=freertos --includes="{INCLUDES}" -v', shell=True, check=True)
    print("building...")
    # subprocess.run('python waf build -v', shell=True, check=True)
    subprocess.run('python3 waf2 build -v', shell=True, check=True)

finally:
    os.chdir(repository_dir)


################################################################
# # littlefs
# print("Downloading littlefs...")
# version_libcsp = "v2.3.0"
# utils.download_git_branch(version_libcsp, "https://github.com/littlefs-project/littlefs", libraries_path, "littlefs")
