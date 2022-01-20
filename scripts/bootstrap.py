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

ccsrtosdemo_path = "./iris-powctrl-freertos-ccs"
libraries_path = "./iris-powctrl-freertos-ccs/Libraries"

################################################################
# Environment variables
import utils

if args.usr_tools != "":
    os.environ['AR'] = os.path.join(args.usr_tools, 'arm-none-eabi-ar.exe')
    os.environ['CC'] = os.path.join(args.usr_tools, 'arm-none-eabi-gcc.exe')
    os.environ['CXX'] = os.path.join(args.usr_tools, 'arm-none-eabi-g++.exe')

elif utils.is_windows:
    import glob
    #print(os.listdir('C:/ST/STM32CubeIDE_1.5.1/STM32CubeIDE/plugins'))
    stm_toolchain_glob = glob.glob("C:/ST/STM32CubeIDE_1.7.0/STM32CubeIDE/plugins/*mcu.externaltools.gnu-tools-for-stm32*/tools/bin",recursive=True)
    print(stm_toolchain_glob)

    if stm_toolchain_glob:
        stm_toolchain_path = os.path.normpath(stm_toolchain_glob[0])
        sys.path.extend(stm_toolchain_path)
        os.environ['AR'] = os.path.join(stm_toolchain_path, 'arm-none-eabi-ar.exe')
        os.environ['CC'] = os.path.join(stm_toolchain_path, 'arm-none-eabi-gcc.exe')
        os.environ['CXX'] = os.path.join(stm_toolchain_path, 'arm-none-eabi-g++.exe')
    else:
        os.environ['AR'] = 'arm-none-eabi-ar.exe'
        os.environ['CC'] = 'arm-none-eabi-gcc.exe'
        os.environ['CXX'] = 'arm-none-eabi-g++.exe'
else:
    os.environ['AR'] = 'arm-none-eabi-ar'
    os.environ['CC'] = 'arm-none-eabi-gcc'
    os.environ['CXX'] = 'arm-none-eabi-g++'

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
print("Downloading submodules...")
subprocess.run("git submodule update --init --recursive", shell=True, check=True)

################################################################
import utils

# # FreeRTOS
# version_FREERTOS = "V9.0.0"
# utils.download_git_branch(version_FREERTOS, "https://github.com/FreeRTOS/FreeRTOS-Kernel", libraries_path, "FreeRTOS-Kernel")

# freertos_kernel_folder = os.path.join(libraries_path, 'FreeRTOS-Kernel')

# # this is not needed for V10
# if version_FREERTOS == "V9.0.0":
    # # Move files to workaround the old folder structure of FreeRTOS-Kernel
    # import shutil
    # import pathlib
    # source_folder = os.path.join(freertos_kernel_folder, 'FreeRTOS', 'Source')
    # shutil.move(source_folder, libraries_path)     # move source content temporarily

    # # Delete unneeded files
    # shutil.rmtree(freertos_kernel_folder, ignore_errors=True)
    # shutil.rmtree(os.path.join(freertos_kernel_folder, 'FreeRTOS'), ignore_errors=True)
    # shutil.rmtree(os.path.join(freertos_kernel_folder, 'FreeRTOS-Plus'), ignore_errors=True)
    # pathlib.Path(freertos_kernel_folder).mkdir(parents=True, exist_ok=True)

    # # move content of source to freertos kernel folder
    # source_temp_dir = os.path.join(libraries_path, 'Source')
    # source_content = os.listdir(source_temp_dir)
    # for file in source_content:
        # shutil.move(os.path.join(source_temp_dir, file), os.path.join(freertos_kernel_folder, file))
    # os.rmdir(source_temp_dir)


# # remove portable directories not needed for Arm CORTEX M3
# portable_folder = os.path.join(freertos_kernel_folder, 'portable')
# portable_content = os.listdir(portable_folder)
# for file in portable_content:
    # if file != "portable" and file != "GCC" and file != "MemMang":
        # shutil.rmtree(os.path.join(portable_folder, file), ignore_errors=True)

# gcc_folder = os.path.join(portable_folder, "GCC")
# gcc_content = os.listdir(gcc_folder)
# for file in gcc_content:
    # if file != "GCC" and file != "ARM_CM3":
        # shutil.rmtree(os.path.join(gcc_folder, file), ignore_errors=True)


# # Use heap_4.c from MemMang
# memmang_folder = os.path.join(portable_folder, "MemMang")
# memmang_content = os.listdir(memmang_folder)
# for file in memmang_content:
    # if file != "MemMang" and file != "heap_4.c":
        # os.remove(os.path.join(memmang_folder, file))

################################################################
# libcsp
print("Downloading libcsp...")
version_libcsp = "track-8faf353" # tracks c9c177e commit of upstream with an additional bug fix for get_csp_can_queue track-8faf353
utils.download_git_branch(version_libcsp, " https://github.com/IrisSat/libcsp", libraries_path, "libcsp")

print("Building libcsp...")
try:
    project_path = "../.."
    INCLUDES = f"{project_path}/include,"
    INCLUDES += f"{project_path}/Middlewares/Third_Party/FreeRTOS,"
    INCLUDES += f"{project_path}/Middlewares/Third_Party/FreeRTOS/Source,"
    INCLUDES += f"{project_path}/Middlewares/Third_Party/FreeRTOS/Source/include,"
    INCLUDES += f"{project_path}/Middlewares/Third_Party/FreeRTOS/Source/portable,"
    INCLUDES += f"{project_path}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1,"

    os.environ["CFLAGS"] = f"-Wall -std=gnu11 {optimization_flags} -mcpu=cortex-m7 -mthumb -mfloat-abi=hard"

    os.chdir(f"{libraries_path}/libcsp")
    print("configuring...")
    subprocess.run(f'python waf configure --toolchain=arm-none-eabi- --enable-if-can --with-os=freertos --includes="{INCLUDES}"', shell=True, check=True)
    print("building...")
    subprocess.run('python waf build', shell=True, check=True)

finally:
    os.chdir(repository_dir)


################################################################
# # littlefs
# print("Downloading littlefs...")
# version_libcsp = "v2.3.0"
# utils.download_git_branch(version_libcsp, "https://github.com/littlefs-project/littlefs", libraries_path, "littlefs")
