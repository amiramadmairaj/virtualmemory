# Please use python3 to run this script
import os
import subprocess

cdir = os.getcwd()
hwDir = cdir+"/hw/"

if not os.path.exists('hw'):
    os.makedirs('hw')

os.system("cp hw.zip ./hw")
os.chdir(cdir+"/hw")
os.system("unzip hw.zip")

os.system("gcc -fprofile-arcs -ftest-coverage hw.c -o hwX")

for fname in os.listdir("./"):
    if fname.endswith(".run"):
        ## We will grab the first line, the command-line args
        ## Then we will read all of the remaining lines.

        rfile = open(fname, "r")
        cmdArgs = rfile.readline().rstrip()
        stdinArgs = rfile.read()
        rfile.close()

        ## Now we run the program with its command-line args, and we
        ## pass it the stdin args from the file.

        cmdList = [hwDir+"hwX"] + cmdArgs.split()
        result = subprocess.run(cmdList, input=str.encode(stdinArgs))

os.system("gcov hwX-hw.gcno")


