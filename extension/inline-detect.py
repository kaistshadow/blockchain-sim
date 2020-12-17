import signal
import sys
import time
import threading
import clang.cindex
import clang
from ctypes.util import find_library
from os.path import join
from glob import glob
from multiprocessing import Pool, cpu_count
import argparse

asm_keyword = ["__asm__"]
clang.cindex.Config.set_library_file(find_library('clang-10'))
index = clang.cindex.Index.create()

global CPU_CORE
global AFTER_LINE
global BEFORE_LINE
global EXIT_SIGNAL

def traverse(n, i=0):
    for token in n.get_tokens():
        if token.kind is clang.cindex.TokenKind.KEYWORD:
            if token.spelling in asm_keyword:
                output_str = None
                location = token.location
                file = open(str(location.file), 'r')
                lines = file.readlines()
                output_str = str(location.file) + " > LINE : " +  str(location.line) + "\n"
                
                START_LINE = int(location.line-BEFORE_LINE)
                if START_LINE < 0:
                    START_LINE = 0
                END_LINE = int(location.line+AFTER_LINE + 1)
                if END_LINE >= len(lines):
                    START_LINE = len(lines)
                count = START_LINE
                for line in lines[START_LINE:END_LINE]:
                    output_str += "[" + str(count) + "] " + line
                    count+=1
                print(output_str)
                
def main(file):
    translation_unit = index.parse(file)
    traverse(translation_unit.cursor)

if __name__ == '__main__':
    EXIT_SIGNAL = False
    CPU_CORE = 1
    AFTER_LINE = 5
    BEFORE_LINE = 5
    parser = argparse.ArgumentParser(description='inline ASM Detect Python Script')
    parser.add_argument('-A', '--after-context',
                        type=int, 
                        metavar='NUM',
                        dest='A',
                        default=5,
                        help='print NUM lines of trailing context')

    parser.add_argument('-B', '--before-context',
                    type=int, 
                    metavar='NUM',
                    dest='B',
                    default=5,
                    help='print NUM lines of leading context')
                    
    parser.add_argument('-C', '--CORE',
                    type=int,
                    metavar='NUM',
                    dest='C',
                    default=-1,
                    help='Thread Pool Core Number')

    parser.add_argument("PATH", help="Dir path", type=str)

    args = parser.parse_args()

    if args.C is -1:
        CPU_CORE = cpu_count()

    files = []
    
    for ext in ('*.[cC][pP][pP]', '*.[cC]', '*.[hH][pP][pP]', '*.[hH]'):
        files.extend(glob(join(sys.argv[len(sys.argv) - 1] + "/**", ext), recursive=True))

    with Pool(CPU_CORE) as p:
        for file in files:
            if EXIT_SIGNAL:
                break
            try:
                p.apply(main, [file])
            except KeyboardInterrupt:
                EXIT_SIGNAL = True
                break
            except:
                print("PARSE ERROR : " + file)
