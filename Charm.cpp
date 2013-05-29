#include "Charm.hpp"

#include <iostream>
#include <cassert>
#include "Itemset.hpp"

typedef std::unordered_map< Item, Tidset, item_hash > ItemMap;

/*!
 * \brief Charm::charm
 * \param database
 * \param min_sup
 * \return
 */
CSet Charm::charm(const Database &database, const unsigned int min_sup)
{
    Node p;
    // Create a P tree
    {
        ItemMap item_map;
        // For each itemset
        TID transaction_counter = 1;
        std::for_each( database.cbegin(), database.cend(), [&] ( const Itemset & itemset ) {
            // For each item in itemset
            std::for_each( itemset.cbegin(), itemset.cend(), [&]( const Item & item ) {
                // If there is such item increase support
                const auto got = item_map.find( item );
                if ( item_map.cend() != got ) {
                    got->second.push_back( transaction_counter );
                }
                // Else add item with sup == 1
                else {
                    Tidset new_tidset( 1 );
                    new_tidset.at( 0 ) = transaction_counter;
                    item_map.insert( std::make_pair( item, new_tidset ) );
                }
            } );

            ++ transaction_counter;
        } );

        // Fill the tree
        {
            std::for_each( item_map.cbegin(), item_map.cend(), [&]( ItemMap::const_reference key_value ) {
                if ( min_sup <= key_value.second.size() ) {
                    Itemset itemset;
                    itemset.push_back( key_value.first );
                    p.add_child( Node( itemset, key_value.second ) );
                }
            } );
        }

    }
    auto c_set = CSet();
    charm_extend( p, c_set, min_sup );
#ifdef SIMPLE_TEST
    std::cerr << "c_set: \n" << c_set << std::endl;
#endif
    return c_set;
}

/*!
 * \brief Charm::charm_extend
 * \param p_tree
 * \param c_set
 * \param min_sup
 */
void Charm::charm_extend(Node &p_tree, CSet &c_set, const unsigned int min_sup)
{
    static unsigned int recursion_level = 0;
    ++ recursion_level;

//    std::cerr << "\nrecursion_level = " << recursion_level << std::endl;

//    std::cerr << "p_tree: " << p_tree << std::endl;
//    std::for_each( p_tree.children().cbegin(), p_tree.children().cend(), []( const std::shared_ptr<Node> & val_ref ) { std::cerr << (*val_ref) << std::endl; } );

    for ( auto it = p_tree.children().cbegin(); it != p_tree.children().cend(); ++ it ) {
        Node & current_child = (*(*it));
        if ( current_child.is_erased() ) continue;
//        std::cerr << "\ncurrent_child = " << current_child << std::endl;
        for ( auto int_it = it + 1; int_it != p_tree.children().cend(); ++ int_it ) {
            Node & internal_child = (*(*int_it));
            if ( internal_child.is_erased() ) continue;
//            std::cerr << "\n  internal_child = " << internal_child << std::endl;
            Itemset X = current_child.itemset();
            Tidset Y = current_child.tidset();

            itemset_union( X, internal_child );
            tidset_intersection( internal_child, Y );
            Node test_node( X, Y );
//            std::cerr << "  test_node = " << test_node << std::endl;

            if ( test_node.sup() >= min_sup ) {
                if ( std::equal( internal_child.tidset().cbegin(), internal_child.tidset().cend(), current_child.tidset().cbegin() ) ) {
//                    std::cerr << "   property_1" << std::endl;
//                    std::cerr << "Erase " << internal_child << std::endl;
                    internal_child.set_erased();
                    const Itemset replased_itemset = current_child.itemset();
                    replace_item( current_child, replased_itemset, test_node.itemset() );
                    //                    property_1( p_i_tree, p_tree, Xj, Xi, test_node );

                }
                else {
                    if ( std::includes( internal_child.tidset().cbegin(), internal_child.tidset().cend(), current_child.tidset().cbegin(), current_child.tidset().cend() ) ) {
//                        std::cerr << "   property_2" << std::endl;
                        const Itemset replased_itemset = current_child.itemset();
                        replace_item( current_child, replased_itemset, test_node.itemset() );
                        //                        property_2( p_tree, p_i_tree, test_node, Xi );
                    }
                    else {
                        if ( std::includes( current_child.tidset().cbegin(), current_child.tidset().cend(), internal_child.tidset().cbegin(), internal_child.tidset().cend() ) ) {
//                            std::cerr << "   property_3" << std::endl;
//                            std::cerr << "Erase " << internal_child << std::endl;
                            internal_child.set_erased();
                            current_child.add_child( test_node );
                            //                            property_3( p_i_tree, Xj, test_node );
                        }
                        else {
//                            std::cerr << "   property_4" << std::endl;
                            current_child.add_child( test_node );
                            //                            property_4( p_i_tree, test_node );
                        }
                    }
                }
            }
            else{
//                std::cerr << "   rejected" << std::endl;
            }

        }

        charm_extend( current_child, c_set, min_sup );
    }

    if ( ! p_tree.itemset().empty() ) {
        if ( ! is_subsumed( c_set, p_tree ) ) {
//            std::cerr << "\nInsert: " << p_tree << std::endl;
            c_set.insert( CSet::value_type( p_tree.tidset(), p_tree.itemset() ) );
        }
    }

    recursion_level -- ;
}

/*!
 * \brief Charm::is_subsumed
 * \param c_set
 * \param node
 * \return
 */
bool Charm::is_subsumed(const CSet &c_set, const Node & node)
{
    const Itemset & X = node.itemset();
    const Tidset & Y = node.tidset();

    bool is_subsumed = false;
    const auto range = c_set.equal_range( Y );
    for ( auto it = range.first; it != range.second; ++ it ) {
        const Tidset & tidset = (*it).first;
        if ( tidset.size() == Y.size() ) {
            const Itemset & C = (*it).second;
            if ( std::includes( C.cbegin(), C.cend(), X.cbegin(), X.cend() ) ) {
                is_subsumed = true;
                break;
            }
        }
    }
    return is_subsumed;
}

/*!
 * \brief Charm::itemset_union
 * \param X
 * \param Xi
 * \param Xj
 */
void Charm::itemset_union(Itemset &X, Node &Xj)
{
    Itemset union_itemset( X.size() + Xj.itemset().size() );
    auto it_union = std::set_union( X.cbegin(), X.cend(), Xj.itemset().cbegin(), Xj.itemset().cend(), union_itemset.begin() );
//    union_itemset.resize( it_union - union_itemset.begin() );
    union_itemset.resize( std::distance(union_itemset.begin(), it_union) );
    X = union_itemset;
}

/*!
 * \brief Charm::tidset_intersection
 * \param Xj
 * \param Y
 */
void Charm::tidset_intersection(Node &Xj, Tidset &Y)
{
    const auto it = std::set_intersection( Y.cbegin(), Y.cend(), Xj.tidset().cbegin(), Xj.tidset().cend(), Y.begin() );
    Y.resize( it - Y.begin() );
}

void Charm::replace_item(Node &node_ref, const Itemset &itemset, const Itemset &itemset_to)
{
//    std::cerr << "Replace " << itemset << " to " << itemset_to << " in " << node_ref << std::endl;
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
    // Apply for each children
    std::for_each( node_ref.children_ref().begin(), node_ref.children_ref().end(), [&]( std::shared_ptr<Node> & shared_node ) {
        replace_item( (*shared_node), itemset, itemset_to );
    } );
}
