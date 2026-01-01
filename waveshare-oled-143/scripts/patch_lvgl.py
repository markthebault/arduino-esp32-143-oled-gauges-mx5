Import("env")
import os

def patch_lvgl_rotation(source, target, env):
    """
    Patches LVGL library to fix rotation rendering issues.
    Adds 'height &= ~0x1UL;' to ensure height is always even for QSPI displays.
    """
    # Find the LVGL library path
    libdeps_dir = env.subst("$PROJECT_LIBDEPS_DIR")
    env_name = env.subst("$PIOENV")
    lvgl_path = os.path.join(libdeps_dir, env_name, "lvgl", "src", "core", "lv_refr.c")

    if not os.path.exists(lvgl_path):
        print(f"Warning: LVGL lv_refr.c not found at {lvgl_path}")
        return

    # Read the file
    with open(lvgl_path, 'r') as f:
        content = f.read()

    # Check if patch is already applied
    if 'height &= ~0x1UL;' in content:
        print("LVGL rotation patch already applied.")
        return

    # Apply the patch - find the line and add the fix
    search_string = "lv_coord_t height = LV_MIN(max_row, area_h - row);"
    replacement = "lv_coord_t height = LV_MIN(max_row, area_h - row);\n            height &= ~0x1UL;"

    if search_string in content:
        content = content.replace(search_string, replacement)

        # Write back the patched file
        with open(lvgl_path, 'w') as f:
            f.write(content)

        print("✓ Successfully patched LVGL lv_refr.c for rotation support")
    else:
        print("Warning: Could not find target line in lv_refr.c - LVGL version may have changed")

# Register the post-action to run after library installation
env.AddPostAction("$BUILD_DIR/${PROGNAME}.elf", patch_lvgl_rotation)

# Also try to patch immediately if libraries are already installed
try:
    patch_lvgl_rotation(None, None, env)
except Exception as e:
    print(f"Note: Will patch LVGL after library download. ({e})")
