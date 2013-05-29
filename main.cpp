#include "Charm.hpp"
#include "CSet.hpp"
#include "ResultSaver.hpp"
#include "DatabaseReader.hpp"
#include "Typedefs.hpp"

#include <stdexcept>

#ifndef TREE_TEST

void print_usage();

/*!
 * \brief main
 * \param argc
 * \param argv
 * \return
 */
int main( int argc, const char * argv[] )
{
    if ( argc != 4 ) {
        print_usage();
        return -1;
    }
    std::vector < std::string > argv_vector( argc - 1 );
    auto argv_index = argc - 1;
    while ( argv_index -- ) {
        argv_vector[ argv_index ] = std::string( argv[ argv_index + 1 ] );
    }
    std::cout << "Input args: \n";
    std::for_each ( argv_vector.cbegin(), argv_vector.cend(), []( const std::string & str ) {
        std::cout << str << ' ';
    } );
    std::cout << std::endl;
    // Read support
    unsigned int min_sup = 0;
    try {
        min_sup = std::stoi( argv_vector.at( 0 ) );
    }
    catch ( const std::invalid_argument & ia ) {
        std::cerr << "Invalid argument: " << ia.what() << '\n';
        print_usage();
        return -1;
    }
    // Read database
    Database database;
    {
        std::ifstream data_stream;
        const std::string & database_filename = argv_vector.at( 1 );
        data_stream.open( database_filename );
        if ( data_stream.is_open() ) {
            DatabaseReader< n_of_fields >::read_database( data_stream, database );
            std::cerr << "Database was read" << std::endl;
        }
        else {
            std::cerr << "Cannot open file: " << database_filename << std::endl;
            print_usage();
            return -1;
        }
        std::cerr << "Database size: " << database.size() << std::endl;
    }

    const auto c_set = Charm::charm( database, min_sup );
    std::cout << "Number of frequent itemsets: " << c_set.size() << std::endl;
    // Save results
    {
        std::ofstream c_set_stream;
        const std::string & result_filename( argv_vector.at( 2 ) );
        c_set_stream.open( result_filename );
        if ( c_set_stream.is_open() ) {
            ResultSaver::save(  c_set_stream, c_set );
            std::cout << "Results was saved" << std::endl;
        }
        else {
            std::cerr << "Cannot open file: " << result_filename << std::endl;
            print_usage();
            return -1;
        }
    }
    return 0;
}

/*!
 * \brief print_usage
 */
void print_usage()
{
    std::cerr << "Usage: min_sup input.dat output.res" << std::endl;
}

#endif
