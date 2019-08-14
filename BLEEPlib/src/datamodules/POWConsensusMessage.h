#ifndef POW_CONSENSUS_MESSAGE_H
#define POW_CONSENSUS_MESSAGE_H


#include <boost/serialization/vector.hpp>

#include "../datamodules/POWBlock.h"

namespace libBLEEP {
    typedef std::string POWConsensusMessageType;

    class POWConsensusMessage {
    private:
        POWConsensusMessageType _type;
        std::string _hash;
        std::vector<std::string> _hashlist;
        std::shared_ptr<POWBlock> _blkptr;
        std::vector<POWBlock> _POWblocks;
    public:
        POWConsensusMessage() {}
        POWConsensusMessage(POWConsensusMessageType type) { _type = type; }
        POWConsensusMessage(POWConsensusMessageType type, std::string hash) { _type = type; _hash = hash; }
        POWConsensusMessage(POWConsensusMessageType type, std::vector<std::string> hashlist) { _type = type; _hashlist = hashlist; }
        POWConsensusMessage(POWConsensusMessageType type, std::shared_ptr<POWBlock> blkptr) { _type = type; _blkptr = blkptr; }
        POWConsensusMessage(POWConsensusMessageType type, std::vector<POWBlock> POWblocks) { _type = type; _POWblocks = POWblocks; }

        POWConsensusMessageType GetType() const { return _type; }
        std::string GetHash() { return _hash; }
        std::vector<std::string> GetHashlist() { return _hashlist; }     
        std::shared_ptr<POWBlock> GetPOWBlockPtr() { return _blkptr; }
        std::vector<POWBlock>& GetPOWBlocks() { return _POWblocks; }
    private: // boost serialization
        friend class boost::serialization::access;
        // When the class Archive corresponds to an output archive, the
        // & operator is defined similar to <<.  Likewise, when the class Archive
        // is a type of input archive the & operator is defined similar to >>
        template<class Archive>
            void serialize(Archive & ar, const unsigned int version) {
            ar & _type;
            ar & _hash;
            ar & _hashlist;
            ar & _blkptr;
            ar & _POWblocks;
        }

    };












}






#endif
