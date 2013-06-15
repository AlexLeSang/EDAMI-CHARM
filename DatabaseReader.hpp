#ifndef DATABASEREADER_HPP
#define DATABASEREADER_HPP

#include "Typedefs.hpp"

#include "Itemset.hpp"
#include "Database.hpp"

#include <sstream>

/*!
 * \brief The DatabaseReader class
 */

template < unsigned int nfields >
class DatabaseReader
{
private:
    static constexpr unsigned int number_of_fields = nfields;

public:
    /*!
     * \brief operator ()
     * \param data_stream
     * \param database
     */
//    /* Test database
#ifdef SIMPLE_TEST
    void operator ()(std::ifstream & data_stream, Database & database) const
    {
        std::string s;
        while ( ! data_stream.eof() ) {
            std::getline( data_stream, s );
            if ( s.size() ) {
                Itemset itemset( s.size() );
                std::copy( s.cbegin(), s.cend(), itemset.begin() );
                database.push_back( itemset );
            }
        }
    }
//    */
#else
    // Chess
    /*
    void operator ()(std::ifstream & data_stream, Database & database) const
    {
        std::string s;
        while ( ! data_stream.eof() ) {
            std::getline( data_stream, s );

            if ( !s.empty() ) {
                //            std::cerr << "Read string: \n" << s << std::endl;
                std::istringstream stringstream( s );

                Itemset itemset( number_of_fields );
                for ( unsigned int i = 0; i < number_of_fields; ++ i ) {
                    stringstream >> itemset[ i ];
                }
                //            std::cerr << "Itemset: \n" << itemset << std::endl;
                //            exit( -1 );
                database.push_back( itemset );
            }
        }
    }
    */
    void operator ()(std::ifstream & data_stream, Database & database) const
    {
        const char delim = ';';
        std::string s;
        Itemset itemset;
        Item item;
        std::string item_stirng;
        while ( ! data_stream.eof() ) {
            std::getline( data_stream, s );
            if ( !s.empty() ) {
                std::istringstream stringstream( s );
                itemset.clear();
                item_stirng.clear();
                bool first_skipped = false;
                while ( std::getline( stringstream, item_stirng, delim ) ) {
                    if ( ! first_skipped ) {
                        first_skipped = true;
                        continue;
                    }
                    std::istringstream buff( item_stirng );
                    buff >> item;
                    itemset.push_back( item );
                }
                database.push_back( itemset );
            }
        }
    }


#endif
    /*!
     * \brief read_database
     * \param data_stream
     * \param database
     */
    static void read_database(std::ifstream & data_stream, Database & database)
    {
        DatabaseReader reader;
        reader( data_stream, database );
    }
};

#endif // DATABASEREADER_HPP
