#ifndef CSET_HPP
#define CSET_HPP

#include <unordered_map>

#include "Tidset.hpp"
#include "Itemset.hpp"

/*!
 * \brief CSet
 */
typedef std::unordered_multimap< Tidset, Itemset, tidset_hash > CSet;

/*!
 * \brief operator <<
 * \param os
 * \param c_set
 * \return
 */
inline std::ostream & operator << ( std::ostream & os, const CSet & c_set )
{
    std::for_each( c_set.cbegin(), c_set.cend(), [&]( CSet::const_reference entries ) {
        os << entries.second << ' ' /*<< entries.first << ' '*/ << entries.first.size() << '\n';
    } );

    return os;
}


#endif // CSET_HPP
