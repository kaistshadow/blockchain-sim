import os

if __name__ == '__main__':
    os.system("rm -rf ~/.shadow")
    os.system("rm -rf ./build")
    os.system("python setup.py --install")
    os.system("python setup.py --test")
