// "Copyright [2021] <kaistshadow>"

#ifndef BLEEPLIB_SRC_UTILITY_ARGSMANAGER_H_
#define BLEEPLIB_SRC_UTILITY_ARGSMANAGER_H_

#include <map>
#include <string>
#include <vector>

namespace libBLEEP {

class ArgsManager {
 protected:
    std::map<std::string, std::string> mapArgs;
    std::map<std::string, std::vector<std::string>> mapMultiArgs;


 public:
    std::string HelpMessage();

    void ParseParameters(int argc, const char*const argv[]);

    /**
     * Return a vector of strings of the given argument
     *
     * @param strArg Argument to get (e.g. "-foo")
     * @return command-line arguments
     */
    std::vector<std::string> GetArgs(const std::string& strArg) const;

    /**
     * Return argument or default value
     * The argument should have been set as command-line argument or have a default value.
     *
     * @param Argument to get (e.g. "-foo")
     * @return command-line argument or default value
     */
    std::string GetArg(const std::string& strArg) const;

    /**
     * Return string argument or default value
     *
     * @param strArg Argument to get (e.g. "-foo")
     * @param strDefault (e.g. "1")
     * @return command-line argument or default value
     */
    std::string GetArg(const std::string& strArg, const std::string& strDefault) const;

    /**
     * Return integer argument or default value
     *
     * @param strArg Argument to get (e.g. "-foo")
     * @param nDefault (e.g. 1)
     * @return command-line argument (0 if invalid number) or default value
     */
    int GetArg(const std::string& strArg, int nDefault) const;

    /**
     * Return boolean argument or default value
     *
     * @param strArg Argument to get (e.g. "-foo")
     * @param fDefault (true or false)
     * @return command-line argument or default value
     */
    bool GetBoolArg(const std::string& strArg, bool fDefault) const;

    /**
     * Return true if the given argument has been manually set
     *
     * @param strArg Argument to get (e.g. "-foo")
     * @return true if the argument has been set
     */
    bool IsArgSet(const std::string& strArg) const;
};

extern ArgsManager gArgs;
} // namespace libBLEEP

#endif // BLEEPLIB_SRC_UTILITY_ARGSMANAGER_H_
