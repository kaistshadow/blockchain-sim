#ifndef INVENTORY_H
#define INVENTORY_H

#include <memory>
#include <iostream>
#include <string>
#include <list>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/list.hpp>
#include <boost/property_tree/ptree.hpp>


#include "Transaction.h"

namespace libBLEEP {
    class Inventory {
    private:
        std::list<std::string> _hashlist;
    public:
        Inventory() {};


        const std::list<std::string >& GetHashlist() const { return _hashlist; }
        
        void AppendHash(std::string hash) { _hashlist.push_back(hash); }

        // This overloaded operator<< 
        // Defines a non-member function, and makes it a friend of this class at the same time. 
        friend std::ostream& operator<<(std::ostream &out, const Inventory &t) 
        {
            return t.print(out);
        }
        // We'll rely on member function print() to do the actual printing
        // Because print is a normal member function, it can be virtualized
        virtual std::ostream& print(std::ostream&out) const ;

    private:
        friend class boost::serialization::access;
        // When the class Archive corresponds to an output archive, the
        // & operator is defined similar to <<.  Likewise, when the class Archive
        // is a type of input archive the & operator is defined similar to >>
    
        template<class Archive>
            void serialize(Archive & ar, const unsigned int version) {
            ar & _hashlist;
        }
    };

    /* std::ostream& operator<<(std::ostream&, const Block&); // in order to overload the << operator */

}
#endif
