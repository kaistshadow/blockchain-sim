#include <iostream>
#include <vector>
#include <algorithm>
class InputParser
{
public:
    InputParser(int &argc, char **argv)
    {
        for (int i = 1; i < argc; ++i)
            this->tokens.push_back(std::string(argv[i]));
    }

    const std::string &getCmdOption(const std::string &option) const
    {
        std::vector<std::string>::const_iterator itr;
        itr = std::find(this->tokens.begin(), this->tokens.end(), option);
        if (itr != this->tokens.end() && ++itr != this->tokens.end())
        {
            return *itr;
        }
        static const std::string empty_string("");
        return empty_string;
    }

    char *getCmdOption(char **begin, char **end, const std::string &option)
    {
        char **itr = std::find(begin, end, option);
        if (itr != end && ++itr != end)
        {
            return *itr;
        }
        return 0;
    }

    bool cmdOptionExists(const std::string &option) const
    {
        return std::find(this->tokens.begin(), this->tokens.end(), option) != this->tokens.end();
    }

    bool cmdOptionExists(char **begin, char **end, const std::string &option)
    {
        return std::find(begin, end, option) != end;
    }

private:
    std::vector<std::string> tokens;
};