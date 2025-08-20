import sys
import os
import shutil
import subprocess

def clean(super_c):
    try:
        shutil.rmtree("build")
    except:
        pass
    
    try:
        if super_c:
            shutil.rmtree("sys/hakkun/sail/build")
    except:
        pass

    try:
        if super_c:
            shutil.rmtree("lib/std")
    except:
        pass

def build(flags):
    if os.path.isdir("build") == False or os.path.isdir("lib/std") == False or os.path.isdir("sys/hakkun/sail/build") == False:
        add = ""
        print("SETUP IS BROKEN AND MUST BE PREFORMED AGAIN")
        while True:
            option = input("1 - normal\n2 - prepackaged\n")
            if option == "1":
                break
            elif option == "2":
                add = "+_prepackaged"
                break
            else:
                print("INVALID OPTION")

        setup(add, flags)

    subprocess.run(["cmake", "flags", ".."], cwd="build")
    subprocess.run(["cp",  "compile_commands.json", ".."], cwd="build")
    subprocess.run(["make", "-j8"], cwd="build")

def setup(add, flags):
    clean(True)

    subprocess.run(["python", "sys/tools/setup_libcxx" + add + ".py"])
    subprocess.run(["python", "sys/tools/setup_sail.py"])

    os.mkdir("build")

    subprocess.run(["cmake", "flags", ".."], cwd="build")
    subprocess.run(["cp",  "compile_commands.json", ".."], cwd="build")

if len(sys.argv) == 1:
    print("usage:")
    print("build - build a debug build")
    print("rbuild - build a release build")
    print("setup - setup a debug build")
    print("psetup - setup a debug build using the prepackaged cxx libs")
    print("rsetup - setup a release build")
    print("rpsetup - setup a release build using the prepackaged cxx libs")
    print("clean - clean the main build directory")
    print("sclean - clean the main build directory sail build directory and cxx lib directory")
else:
    if sys.argv[1] == "build":
        print("BUILDING IN DEBUG MODE")
        build("")

    elif sys.argv[1] == "rbuild":
        print("BUILDING IN RELEASE MODE")
        build("-DCMAKE_BUILD_TYPE=Release")

    elif sys.argv[1] == "rsetup":
        print("SETTING UP IN RELEASE MODE")
        setup("", "-DCMAKE_BUILD_TYPE=Release")

    elif sys.argv[1] == "rpsetup":
        print("SETTING UP IN RELEASE MODE USING PREPACKAGED CXX LIBS")
        setup("_prepackaged", "-DCMAKE_BUILD_TYPE=Release")

    elif sys.argv[1] == "setup":
        print("SETTING UP IN DEBUG MODE")
        setup("", "")

    elif sys.argv[1] == "psetup":
        print("SETTING UP IN DEBUG MODE USING PREPACKAGED CXX LIBS")
        setup("_prepackaged", "")

    elif sys.argv[1] == "clean":
        print("CLEANING MAIN BUILD DIRECTORY")
        clean(False)

    elif sys.argv[1] == "sclean":
        print("CLEANING MAIN BUILD DIRECTORY SAIL BUILD DIRECTORY AND CXX LIB DIRECTORY")
        clean(True)