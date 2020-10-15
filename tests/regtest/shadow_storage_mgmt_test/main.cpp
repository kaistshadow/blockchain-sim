#include "CanDDaGae.h"
#include <iostream>
#include <cassert>
#include <cstring>

int main() {
    // tracker configuration setting: in-memory file size max: 0x800, start swapout on 2 in-memory files until 1 in-memory file
    CanDDaGae::config(0x800, 2, 1);
    FILE* file;

    // TEST 1: read-only fopen on really & virtually non-existent file
    file = CanDDaGae::fopen("BrandonKuzma.txt", "r");
    assert(file==NULL);
    if (file)
        CanDDaGae::fclose(file);

    // TEST 2: file creation & write
    file = CanDDaGae::fopen("BrandonKuzma.txt", "w");
    char nintendo[] = "Nintendo Sixty-foooour!!!\n";
    fwrite(nintendo, sizeof(char), strlen(nintendo)+1, file);
    CanDDaGae::fclose(file);
    CanDDaGae::debug();

    // TEST 3: read created file
    file = CanDDaGae::fopen("BrandonKuzma.txt", "r");
    char readResult[100];
    fread(readResult, sizeof(char), strlen(nintendo)+1, file);
    std::cout<<"read result: "<<readResult<<"\n";
    CanDDaGae::fclose(file);

    // TEST 4: error test on double fopen
    file = CanDDaGae::fopen("BrandonKuzma.txt", "r");
    bool err = false;
    try {
        FILE* file2 = CanDDaGae::fopen("BrandonKuzma.txt", "r");
    } catch (const std::exception& e) {
        // throw std::runtime_error("CanDDaGae::fopen called twice on same time without CanDDaGae::fclose called");
        std::cout << e.what() << "\n";
        err = true;
    }
    assert(err);
    err = false;
    try {
        FILE* file2 = CanDDaGae::fopen("BrandonKuzma.txt", "w");
    } catch (const std::exception& e) {
        // throw std::runtime_error("CanDDaGae::fopen called twice on same time without CanDDaGae::fclose called");
        std::cout << e.what() << "\n";
        err = true;
    }
    assert(err);
    CanDDaGae::fclose(file);

    // TEST 5: disk file test
    file = CanDDaGae::fopen("RachelKuzma.txt", "w");
    fwrite(nintendo, sizeof(char), strlen(nintendo)+1, file);
    assert(CanDDaGae::debug_fileloc("BrandonKuzma.txt") == STORAGE_PERSISTENT);
    assert(CanDDaGae::debug_fileloc("RachelKuzma.txt") == STORAGE_NONPERSISTENT);
    CanDDaGae::fclose(file);
    file = CanDDaGae::fopen("BrandonKuzma.txt", "r");
    fread(readResult, sizeof(char), strlen(nintendo)+1, file);
    std::cout<<"read result: "<<readResult<<"\n";
    assert(CanDDaGae::debug_fileloc("BrandonKuzma.txt") == STORAGE_PERSISTENT);
    assert(CanDDaGae::debug_fileloc("RachelKuzma.txt") == STORAGE_NONPERSISTENT);
    CanDDaGae::fclose(file);
    file = CanDDaGae::fopen("BrandonKuzma.txt", "r+");
    assert(CanDDaGae::debug_fileloc("BrandonKuzma.txt") == STORAGE_NONPERSISTENT);
    assert(CanDDaGae::debug_fileloc("RachelKuzma.txt") == STORAGE_PERSISTENT);
    CanDDaGae::fclose(file);
}