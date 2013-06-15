#ifndef ITEM_HPP
#define ITEM_HPP

#include <iostream>

#include "Typedefs.hpp"

/*!
 * \brief The item_hash class
 */
class item_hash {
public:
    inline int operator()( const Item & item ) const
    {
        return (int)( item );
    }
};

/* For more advanced Item struct
inline std::ostream & operator << ( std::ostream & os, const Item & item )
{
    os << item;
    return os;
}
*/

#endif // ITEM_HPP
