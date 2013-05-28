#include "Tree.hpp"

#include <functional>
#include <list>
#include <thread>

/*!
 * \brief Tree::Tree
 */
Tree::Tree() : _root_node( new Node() )
{}

/*!
 * \brief Tree::Tree
 * \param node
 */
Tree::Tree(const Node &node) : _root_node( new Node( node ) )
{}

/*!
 * \brief Tree::add
 * \param itemset
 * \param tidset
 */
void Tree::add(const Itemset &itemset, const Tidset &tidset)
{
    _root_node->add_child( Node( itemset, tidset ) );
}

/*!
 * \brief Tree::root_node
 * \return
 */
std::weak_ptr<Node> Tree::root_node()
{
    return std::weak_ptr< Node > ( _root_node );
}

/*!
 * \brief Tree::print_node
 * \param node_ptr
 */
void Tree::print_node( const std::weak_ptr< Node > & node_ptr )
{
    const auto node_shared_ptr = node_ptr.lock();
    std::cerr << (*node_shared_ptr) << std::endl;
    std::for_each( node_shared_ptr->children().cbegin(), node_shared_ptr->children().cend(), []( const std::shared_ptr< Node > & node_wk_ptr ) {
        print_node( node_wk_ptr );
    } );
}

/*!
 * \brief Tree::print_tree
 */
void Tree::print_tree() const
{
    print_node( std::weak_ptr< Node > ( _root_node ) );
}

//void Tree::remove_node(Node& node_ref, const Itemset &itemset)
//{
//    bool found = false;
//    for ( auto it = node_ref.children_ref().begin(); it != node_ref.children_ref().end(); ++ it ) {
//        auto & child = (*(*it));
//        if ( itemset == child.itemset() ) {
//            found = true;
//            child.set_erased();
//            break;
//        }
//    }
//    if ( ! found ) {
//        std::for_each( node_ref.children_ref().begin(), node_ref.children_ref().end(), [&]( std::shared_ptr<Node> & shared_node ) {
//            remove_node( (*shared_node), itemset );
//        } );
//    }
//}

//void Tree::remove(const Itemset &itemset)
//{
//    Node & root_node = (*_root_node);
//    remove_node( root_node, itemset );
//}

/*!
 * \brief Tree::replace_item
 * \param node_ref
 * \param itemset
 * \param itemset_to
 */
void Tree::replace_item(Node& node_ref, const Itemset &itemset, const Itemset &itemset_to)
{
    assert( itemset.size() != 0 );
    assert( itemset_to.size() != 0 );
    if ( node_ref.itemset().size() >= itemset.size() ) {
        if ( ! std::includes( node_ref.itemset().cbegin(), node_ref.itemset().cend(), itemset_to.cbegin(), itemset_to.cend() ) ) {
            // Look for all items
            if ( std::includes( node_ref.itemset().cbegin(), node_ref.itemset().cend(), itemset.cbegin(), itemset.cend() ) ) {
                // Remove all occurence of itemset
                std::for_each( itemset.cbegin(), itemset.cend(), [&]( const Item & item ) {
                    node_ref.itemset().erase( std::find( node_ref.itemset().begin(), node_ref.itemset().end(), item ) );
                } );
                // Insert new sequence
                std::for_each( itemset_to.cbegin(), itemset_to.cend(), [&]( const Item & item_to ) {
                    node_ref.itemset().push_back( item_to );
                } );
                // Sort it
                std::sort( node_ref.itemset().begin(), node_ref.itemset().end() );
            }
        }
    }
//    /* Sequential version
    // Apply for each children
    std::for_each( node_ref.children_ref().begin(), node_ref.children_ref().end(), [&]( std::shared_ptr<Node> & shared_node ) {
        replace_item( (*shared_node), itemset, itemset_to );
    } );
//    */
    /* Parallel version
    std::vector< std::thread > thread_vector( node_ref.children().size() );
    unsigned int index = 0;
    std::for_each( node_ref.children_ref().begin(), node_ref.children_ref().end(), [&]( std::shared_ptr<Node> & shared_node ) {
        thread_vector[ index ] = std::thread ( replace_item, std::ref(*shared_node), itemset, itemset_to );
        ++index;
    } );

    for( auto && th : thread_vector ) {
        th.join();
    }
    */
}

/*!
 * \brief Tree::replace
 * \param itemset
 * \param itemset_to
 */
void Tree::replace(const Itemset &itemset, const Itemset &itemset_to)
{
    Node & root_node = (*_root_node);
    replace_item( root_node, itemset, itemset_to  );
}

#ifdef TREE_TEST

#include <iostream>
#include <ctime>
#include <cstdlib>

int main()
{

    //    const std::string s = "abcde";
    //    Itemset itemset( s.size() );
    //    std::copy( s.cbegin(), s.cend(), itemset.begin() );

    //    const std::string s1 = "abd";
    //    Itemset itemset1( s1.size() );
    //    std::copy( s1.cbegin(), s1.cend(), itemset1.begin() );

    //    const std::string s2 = "de";
    //    Itemset itemset2( s2.size() );
    //    std::copy( s2.cbegin(), s2.cend(), itemset2.begin() );

    //    const std::string s3 = "gh";
    //    Itemset itemset3( s3.size() );
    //    std::copy( s3.cbegin(), s3.cend(), itemset3.begin() );


    //    const Tidset tidset = { 1, 2, 3, 4, 5 };
    //    const Tidset tidset1 = { 1, 2, 3 };
    //    const Tidset tidset2 = { 1, 2 };
    //    const Tidset tidset3 = { 1, 2, 3, 4, 5, 6 };


    //    std::cerr << "tidset = " << tidset << std::endl;
    //    std::cerr << "tidset1 = " << tidset1 << std::endl;
    //    Tidset intersect( tidset.size() + tidset1.size() );

    //    auto it = std::set_intersection( tidset.begin(), tidset.end(), tidset1.begin(), tidset1.end(), intersect.begin() );
    //    intersect.resize( it - intersect.begin() );
    //    std::cerr << "intersect = " << intersect << std::endl;

    //    Node node( itemset, tidset );

    //    Node node1( itemset, tidset1 );
    //    Node node2( itemset, tidset2 );
    //    Node node3( itemset1, tidset3 );

    //    node.add_child( node1 );
    //    node.add_child( node2 );
    //    node.add_child( node3 );


    //    std::cout << node << std::endl;

    //    std::for_each( node.children().cbegin(), node.children().cend(), [&] ( const std::shared_ptr< Node > & node_ptr ) {
    //        std::cout << (*node_ptr) << std::endl;
    //    } );
    //    Tree P;
    //    auto root = P.root_node().lock();
    //    root->add_child( node );

    //    P.print_tree();
    //    P.remove( itemset1 );
    //    P.print_tree();
    //    P.replace( itemset2, itemset3 );
    //    P.print_tree();

    //    srand( time(NULL) );
    std::vector< int > v( 10 );
    std::for_each( v.begin(), v.end(), []( int & val ) {
        val = rand() % 20;
    } );

    std::for_each( v.cbegin(), v.cend(), [] ( const int & val ) {
        std::cerr << val << ' ';
    } );
    std::cerr << std::endl;

    std::cerr << '\n';
    for ( auto it = v.begin(); it != v.end(); ++ it ) {
        std::cerr << (*it) << ": ";
        auto internal_it = it;
        std::advance( internal_it, 1 );
        for ( ; internal_it != v.end(); ++ internal_it ) {
            std::cerr << (*internal_it) << ' ';
            if ( (*internal_it) % 3 == 0 )
                v.erase( internal_it );
        }
        std::cerr << '\n';
    }
    std::cerr << std::endl;

    return 0;
}

#endif
