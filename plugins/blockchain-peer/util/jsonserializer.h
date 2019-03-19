#ifndef JSONSERIALIZER_H
#define JSONSERIALIZER_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "../blockchain/powblock.h"
#include "../blockchain/block.h"

class JSONSerializer {
 public:
    void DumpToJson(boost::property_tree::ptree& root, POWBlock &blk);
    void DumpToJson(boost::property_tree::ptree& root, Block &blk);
};


#endif // JSONSERIALIZER_H
