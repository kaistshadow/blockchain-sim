#include "ArgsManager.h"
#include "Assert.h"
#include "Logger.h"
#include <sstream>

using namespace libBLEEP;

ArgsManager libBLEEP::gArgs;

/** Interpret string as boolean, for argument parsing */
static bool InterpretBool(const std::string& strValue)
{
    if (strValue.empty())
        return true;
    return (std::stoi(strValue) != 0);
}

/** Turn -noX into -X=0 */
static void InterpretNegativeSetting(std::string& strKey, std::string& strValue)
{
    if (strKey.length()>3 && strKey[0]=='-' && strKey[1]=='n' && strKey[2]=='o')
    {
        strKey = "-" + strKey.substr(3);
        strValue = InterpretBool(strValue) ? "0" : "1";
    }
}

static const int screenWidth = 79;
static const int optIndent = 2;
static const int msgIndent = 7;

static std::string FormatParagraph(const std::string& in, size_t width, size_t indent)
{
    std::stringstream out;
    size_t ptr = 0;
    size_t indented = 0;
    while (ptr < in.size())
    {
        size_t lineend = in.find_first_of('\n', ptr);
        if (lineend == std::string::npos) {
            lineend = in.size();
        }
        const size_t linelen = lineend - ptr;
        const size_t rem_width = width - indented;
        if (linelen <= rem_width) {
            out << in.substr(ptr, linelen + 1);
            ptr = lineend + 1;
            indented = 0;
        } else {
            size_t finalspace = in.find_last_of(" \n", ptr + rem_width);
            if (finalspace == std::string::npos || finalspace < ptr) {
                // No place to break; just include the entire word and move on
                finalspace = in.find_first_of("\n ", ptr);
                if (finalspace == std::string::npos) {
                    // End of the string, just add it and break
                    out << in.substr(ptr);
                    break;
                }
            }
            out << in.substr(ptr, finalspace - ptr) << "\n";
            if (in[finalspace] == '\n') {
                indented = 0;
            } else if (indent) {
                out << std::string(indent, ' ');
                indented = indent;
            }
            ptr = finalspace + 1;
        }
    }
    return out.str();
}

static std::string HelpMessageGroup(const std::string &message) {
    return std::string(message) + std::string("\n\n");
}

static std::string HelpMessageOpt(const std::string &option, const std::string &message) {
    return std::string(optIndent,' ') + std::string(option) +
           std::string("\n") + std::string(msgIndent,' ') +
           FormatParagraph(message, screenWidth - msgIndent, msgIndent) +
           std::string("\n\n");
}


std::string ArgsManager::HelpMessage() {
    std::string strUsage = HelpMessageGroup("General Options:");
    strUsage += HelpMessageOpt("-?", "Print this help message and exit");
    strUsage += HelpMessageOpt("-l=<level>", "log LEVEL above which to filter messages ('error' < 'critical' < 'message' < 'info' < 'debug') (default:debug)");

    strUsage += HelpMessageGroup("Blockchain Node General Options:");
    strUsage += HelpMessageOpt("-id=<id>", "Specify the peer id. (default:noid)");

    strUsage += HelpMessageGroup("Node Network Options:");
    strUsage += HelpMessageOpt("-connect=<id>", "Specify the neighbor id (ex. domain or ip) for connection. ");

    strUsage += HelpMessageGroup("Injector Options:");
    strUsage += HelpMessageOpt("-txgenstartat=<time>", "Specify the starttime for randomly generated tx. ");
    strUsage += HelpMessageOpt("-txgeninterval=<time>", "Specify the interval for randomly generated tx. ");

    strUsage += HelpMessageGroup("Blockchain Consensus Options (POW):");
    strUsage += HelpMessageOpt("-blocktxnum=<n>", "Number of transactions in one block. default:5");
    strUsage += HelpMessageOpt("-miningtime=<n>", "Emulated mean time for mining a block. default:10");    
    strUsage += HelpMessageOpt("-miningtimedev=<n>", "Standard deviation time for mining a block. default:2");    
    strUsage += HelpMessageGroup("Random Gossip Protocol Options:");
    strUsage += HelpMessageOpt("-fanout=<n>", "Number of fanout. default:7");
    strUsage += HelpMessageOpt("-outpeernum=<n>", "Number of outpeer connection. default:10");    

    return strUsage;
}

void ArgsManager::ParseParameters(int argc, const char* const argv[])
{
    mapArgs.clear();
    mapMultiArgs.clear();

    for (int i = 1; i < argc; i++)
    {
        std::string str(argv[i]);
        std::string strValue;
        size_t is_index = str.find('=');
        if (is_index != std::string::npos)
        {
            strValue = str.substr(is_index+1);
            str = str.substr(0, is_index);
        }

        if (str[0] != '-')
            break;

        // Interpret --foo as -foo.
        // If both --foo and -foo are set, the last takes effect.
        if (str.length() > 1 && str[1] == '-')
            str = str.substr(1);
        InterpretNegativeSetting(str, strValue);

        mapArgs[str] = strValue;
        mapMultiArgs[str].push_back(strValue);
    }

    if (IsArgSet("-l")) {
        std::string loglevel = GetArg("-l");
        if (loglevel == "error") 
            gLog.SetLogLevel(LOGLEVEL_ERROR);
        else if (loglevel == "critical")
            gLog.SetLogLevel(LOGLEVEL_CRITICAL);
        else if (loglevel == "message")
            gLog.SetLogLevel(LOGLEVEL_MESSAGE);
        else if (loglevel == "info")
            gLog.SetLogLevel(LOGLEVEL_INFO);
        else if (loglevel == "debug")
            gLog.SetLogLevel(LOGLEVEL_DEBUG);
        else 
            gLog.SetLogLevel(LOGLEVEL_UNSET);
    } else
        gLog.SetLogLevel(LOGLEVEL_UNSET);
}

std::vector<std::string> ArgsManager::GetArgs(const std::string& strArg) const
{
    auto it = mapMultiArgs.find(strArg);
    if (it != mapMultiArgs.end()) return it->second;
    return {};
}

std::string ArgsManager::GetArg(const std::string& strArg) const 
{
    auto it = mapArgs.find(strArg);
    if (it != mapArgs.end()) return it->second;
    
    // return default
    if (strArg == "-id") {
        return "noid";
    } else if (strArg == "-blocktxnum") {
        return "5";
    } else if (strArg == "-miningtime") {
        return "10";
    } else if (strArg == "-miningtimedev") {
        return "2";
    } else if (strArg == "-fanout") {
        return "7";
    } else if (strArg == "-outpeernum") {
        return "10";
    } else if (strArg == "-txgeninterval") {
        return "1";
    } else {
        M_Assert(0, "Invalid request for not existed argument. (Also, default value of the argument is not existed)");
    }

    return "";
}

bool ArgsManager::IsArgSet(const std::string& strArg) const
{
    return mapArgs.count(strArg);
}

std::string ArgsManager::GetArg(const std::string& strArg, const std::string& strDefault) const
{
    auto it = mapArgs.find(strArg);
    if (it != mapArgs.end()) return it->second;
    return strDefault;
}

int ArgsManager::GetArg(const std::string& strArg, int nDefault) const
{
    auto it = mapArgs.find(strArg);
    if (it != mapArgs.end()) return std::stoi(it->second);
    return nDefault;
}

bool ArgsManager::GetBoolArg(const std::string& strArg, bool fDefault) const
{
    auto it = mapArgs.find(strArg);
    if (it != mapArgs.end()) return InterpretBool(it->second);
    return fDefault;
}
