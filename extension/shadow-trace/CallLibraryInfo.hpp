#include <iostream>
class CallLibraryInfo
{
public:
    uint64_t instruction_pointer;
    std::string function_name;
    std::string file_name;
    int file_line = -1;
    uint64_t function_offset;

    CallLibraryInfo(uint64_t instruction_pointer, std::string function_name, uint64_t function_offset)
    {
        this->instruction_pointer = instruction_pointer;
        this->function_name = function_name;
        this->function_offset = function_offset;
    };

    CallLibraryInfo(uint64_t instruction_pointer, std::string function_name, uint64_t function_offset, std::string file_name, int file_line)
    {
        this->instruction_pointer = instruction_pointer;
        this->function_name = function_name;
        this->function_offset = function_offset;
        this->file_name = file_name;
        this->file_line = file_line;
    };
};