import sys
import clang.cindex
import clang
from ctypes.util import find_library
from os.path import join
from glob import glob

asm_keyword = ["__asm__"]
clang.cindex.Config.set_library_file(find_library('clang-10'))
index = clang.cindex.Index.create()
def traverse(n, i=0):
    for token in n.get_tokens():
        if token.kind is clang.cindex.TokenKind.KEYWORD:
            if token.spelling in asm_keyword:
                location = token.location
                print(location.file, end=" > ")
                print("line " + str(location.line))
                
def main(afile):
    translation_unit = index.parse(afile)
    traverse(translation_unit.cursor)

if __name__ == '__main__':
    files = []
    for ext in ('*.[cC][pP][pP]', '*.[cC]', '*.[hH][pP][pP]', '*.[hH]'):
        files.extend(glob(join(sys.argv[1] + "/**", ext), recursive=True))
    for file in files:
        try:
            main(file)
        except:
            print("PARSE ERROR : " + file)
