import subprocess
import re

def nm(path, sharedType = 0):
    result = []
    cmd = ['nm', '-D', path]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE)
    out, err = p.communicate()
    typeString = 'U'
    if sharedType is 1:
        typeString = 'T'
    if err is '':
        for line in out.splitlines():
            if ' ' + typeString + ' ' in line:
                nmParse = line.split(typeString + ' ')
                if len(nmParse) is 2 :
                    result.append(nmParse[1])
    return result

def ldd(path):
    cmd = ['ldd', path]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE)
    out, err = p.communicate()
    result = []
    if err is '':
        p = re.compile("(.*) => (.*) (.*)", re.MULTILINE)
        parse = p.findall(out)
        for line in parse:
            if len(line) is 3 :
                result.append(line[1])
    return result

def main(path):
    targetSharedFunctions = nm(path)
    sharedLibs = ldd(path)
    for lib in sharedLibs:
        sharedLibFunction = nm(lib)
        for targetFunction in targetSharedFunctions:
            if targetFunction in sharedLibFunction :
                print(lib)

if __name__ == '__main__':
    libPath = '/home/arcadia/Workspace/blockchain-sim/tests/regtest/shadow-ripple/librippled.so'
    main(libPath)