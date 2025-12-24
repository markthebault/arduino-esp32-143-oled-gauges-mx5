Import("env")
import os
import shutil

def remove_helium_files(source, target, env):
    """Remove ARM Helium assembly files that are incompatible with ESP32"""
    libdeps_dir = env.get("PROJECT_LIBDEPS_DIR")
    pioenv = env.get("PIOENV")

    if libdeps_dir and pioenv:
        helium_dir = os.path.join(libdeps_dir, pioenv, "lvgl", "src", "draw", "sw", "blend", "helium")
        if os.path.exists(helium_dir):
            print(f"Removing ARM Helium directory: {helium_dir}")
            shutil.rmtree(helium_dir)

# Run before build
env.AddPreAction("$BUILD_DIR/${PROGNAME}.elf", remove_helium_files)
