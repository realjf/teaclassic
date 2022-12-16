#!/usr/bin/env python

import platform
import os
import sys
import io
import subprocess
import json
import getopt


CEND = '\33[0m'
CBOLD = '\33[1m'
CITALIC = '\33[3m'
CURL = '\33[4m'
CBLINK = '\33[5m'
CBLINK2 = '\33[6m'
CSELECTED = '\33[7m'

CBLACK = '\33[30m'
CRED = '\33[31m'
CGREEN = '\33[32m'
CYELLOW = '\33[33m'
CBLUE = '\33[34m'
CVIOLET = '\33[35m'
CBEIGE = '\33[36m'
CWHITE = '\33[37m'

CBLACKBG = '\33[40m'
CREDBG = '\33[41m'
CGREENBG = '\33[42m'
CYELLOWBG = '\33[43m'
CBLUEBG = '\33[44m'
CVIOLETBG = '\33[45m'
CBEIGEBG = '\33[46m'
CWHITEBG = '\33[47m'

CGREY = '\33[90m'
CRED2 = '\33[91m'
CGREEN2 = '\33[92m'
CYELLOW2 = '\33[93m'
CBLUE2 = '\33[94m'
CVIOLET2 = '\33[95m'
CBEIGE2 = '\33[96m'
CWHITE2 = '\33[97m'

CGREYBG = '\33[100m'
CREDBG2 = '\33[101m'
CGREENBG2 = '\33[102m'
CYELLOWBG2 = '\33[103m'
CBLUEBG2 = '\33[104m'
CVIOLETBG2 = '\33[105m'
CBEIGEBG2 = '\33[106m'
CWHITEBG2 = '\33[107m'


DEBUG_OUTPUT = False


TOOL_COMMAND_PYTHON = "python"


if platform.system() == "Linux":
    TOOL_COMMAND_PYTHON = "python3"


def log(string, color=CWHITE):
    print(color + "--- " + string + color)


def dlog(string, color=CWHITE):
    if DEBUG_OUTPUT:
        print(color + "*** " + string + color)


def executeCommand(command, printCommand=False, quiet=False):
    printCommand = printCommand or DEBUG_OUTPUT
    out = None
    err = None

    if quiet:
        out = open(os.devnull, 'w')
        err = subprocess.STDOUT

    if printCommand:
        if DEBUG_OUTPUT:
            dlog(">>> " + command)
        else:
            log(">>> " + command)

    return subprocess.call(command, shell=True, stdout=out, stderr=err)


def dieIfNonZero(res):
    if res != 0:
        raise ValueError("Command returned non-zero status: " + str(res))


def findToolCommand(command, paths_to_search, required=False):
    command_res = command
    found = False

    for path in paths_to_search:
        command_abs = os.path.join(path, command)
        if os.path.exists(command_abs):
            command_res = command_abs
            found = True
            break

    if required and not found:
        log("WARNING: command " + command +
            " not found, but required by script", CYELLOW)

    dlog("Found '" + command + "' as " + command_res)
    return command_res


def readJSONData(filename):
    try:
        json_data = open(filename).read()
    except:
        log("ERROR: Could not read JSON file " + filename, CRED)
        return None

    try:
        data = json.loads(json_data)
    except:
        log("ERROR: Could not parse JSON document", CRED)
        return None

    return data


def listLibraries(data):
    for library in data:
        name = library.get('name', None)
        if name is not None:
            print(name)


def installLibrary(name, commands):
    ret = 0
    for command in commands:
        ret = executeCommand(command)
        if ret > 0:
            return ret
    return ret


def pp(string):
    print(CVIOLET + string + CVIOLET)


def printOptions():
    pp("--------------------------------------------------------------------------------")
    pp("Install required libraries.")
    pp("")
    pp("Options:")
    pp("  --list, -l                  List all required libraries")
    pp("  --file, -f                  Specifies the required libraries file to be")
    pp("                              installed")
    pp("  --debug-output, -d          Enables extra debugging output")
    pp("  --break-on-first-error, -b  Terminate script once the first error is encountered")
    pp("--------------------------------------------------------------------------------")


def main(argv):
    global DEBUG_OUTPUT
    global TOOL_COMMAND_PYTHON

    try:
        opts, args = getopt.getopt(
            argv,  # argument list
            "lf:dbh",  # short options
            ["list", "file=", "debug-output", "help", "break-on-first-error"])  # long options
    except getopt.GetoptError:
        printOptions()
        return 0

    list_libraries = False
    opt_file = ""

    break_on_first_error = False

    for opt, arg in opts:
        if opt in ("-h", "--help"):
            printOptions()
            return 0
        if opt in ("-l", "--list"):
            list_libraries = True
        if opt in ("-f", "--file"):
            opt_file = os.path.abspath(arg)
        if opt in ("-b", "--break-on-first-error",):
            break_on_first_error = True
        if opt in ("-d", "--debug-output",):
            DEBUG_OUTPUT = True

    if platform.system() != "Windows":
        # Unfortunately some IDEs do not have a proper PATH environment variable set,
        # so we search manually for the required tools in some obvious locations.
        paths_to_search = os.environ["PATH"].split(
            ":") + ["/usr/local/bin", "/opt/local/bin", "/usr/bin"]
        TOOL_COMMAND_PYTHON = findToolCommand(
            TOOL_COMMAND_PYTHON, paths_to_search, required=True)

    if len(opt_file) == 0 or not os.path.exists(os.path.abspath(opt_file)):
        log("ERROR: The libraries.json file not found", CRED)
        return -1

    # read canonical libraries data
    data = readJSONData(opt_file)
    if data is None:
        return -1

    # some sanity checking
    for library in data:
        if library.get('name', None) is None:
            log("ERROR: Invalid schema: library object does not have a 'name'", CRED)
            return -1
        if library.get('commands', None) is None:
            log("ERROR: Invalid schema: library object does not have a 'commands'", CRED)
            return -1

    if list_libraries:
        listLibraries(data)
        return 0

    failed_libraries = []

    for library in data:
        name = library.get('name', None)
        commands = library.get('commands', None)

        dlog("********** LIBRARY " + name + " **********")

        # install library
        ret = installLibrary(name, commands)
        if ret > 0:
            failed_libraries.append(name)
            if break_on_first_error:
                break
        else:
            log(name + " is installed successfully", CGREEN)

    if failed_libraries:
        log("***************************************", CRED)
        log("FAILURE to install the following libraries:", CRED)
        log(', '.join(failed_libraries), CRED)
        log("***************************************", CRED)
        return -1

    log("Finished", CGREEN)

    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
