#include "Charm.hpp"

/*!
 * \brief Charm::charm
 * \param database
 * \param min_sup
 * \return
 */
CSet Charm::charm(const Database &database, const unsigned int min_sup)
{
    Tree p;
    // Create a P tree
    {
        std::unordered_map< Item, Tidset, item_hash > item_map;
        // For each itemset
        TID transaction_counter = 1;
        std::for_each( database.cbegin(), database.cend(), [&] ( const Itemset & itemset ) {
            // For each item in itemset
            std::for_each( itemset.cbegin(), itemset.cend(), [&]( const Item & item ) {
                // If there is such item increase support
                auto got = item_map.find( item );
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
        // TODO remove debug output
        /*
            {
                std::cerr << "item_map = \n";
                std::for_each( item_map.cbegin(), item_map.cend(), []( decltype(item_map)::const_reference key_value ) {
                    std::cerr << "(" << key_value.first << ":<";
                    unsigned int index = key_value.second.size();
                    std::for_each( key_value.second.cbegin(), key_value.second.cend(), [&]( const TID & tid ) {
                        std::cerr << tid << ( --index ? ',' : '>' );
                    } );
                    std::cerr << ")\n";
                } );
            }
            */
        // Fill the tree
        {
            std::for_each( item_map.cbegin(), item_map.cend(), [&]( decltype(item_map)::const_reference key_value ) {
                if ( min_sup < key_value.second.size() ) {
                    Itemset itemset;
                    itemset.push_back( key_value.first );
                    p.add( itemset, key_value.second );
                }
            } );
            //            P.print_tree(); // TODO remove debug output
        }

    }
    auto c_set = CSet();
    charm_extend( p, c_set, min_sup );
    //        std::cerr << "c_set = \n" << c_set << std::endl; // TODO remove debug output
    return c_set;
}

/*!
 * \brief Charm::charm_extend
 * \param p_tree
 * \param c_set
 * \param min_sup
 */
void Charm::charm_extend(Tree &p_tree, CSet &c_set, const unsigned int min_sup)
{
    auto root = p_tree.root_node().lock();
    for ( auto it = root->children().begin(); it != root->children().end(); ++ it ) {
        Node & Xi = (*(*it));
        if ( Xi.is_erased() ) continue;
        Tree p_i_tree;
        Itemset X = Xi.itemset();
        Tidset Y = Xi.tidset();
        // For each Xj
        for (  auto internal_it = it + 1; internal_it != root->children().end(); ++ internal_it ) {
            Node & Xj = (*(*internal_it));
            if ( Xj.is_erased() ) continue;
            X = Xi.itemset();
            itemset_union( X, Xj );
            Y = Xi.tidset();
            tidset_intersection( Xj, Y );
            Node test_node( X, Y );
            charm_property( p_i_tree, p_tree, test_node, Xi, Xj, min_sup );
        }
        if ( ! ( root->children().empty() ) ) {
            charm_extend( p_i_tree, c_set, min_sup );
        }
        if ( ! is_subsumed( c_set, X, Y ) ) {
            c_set.insert( CSet::value_type(Y, X) );
        }
    }
}

/*!
 * \brief Charm::charm_property
 * \param p_i_tree
 * \param p_tree
 * \param test_node
 * \param Xi
 * \param Xj
 * \param min_sup
 */
void Charm::charm_property(Tree &p_i_tree, Tree &p_tree, Node &test_node, Node &Xi, Node &Xj, const unsigned int min_sup)
{
    if ( test_node.tidset().size() >= min_sup ) {
        if ( tidset_equal(Xi, Xj) ) {
            property_1( p_i_tree, p_tree, Xj, Xi, test_node );
        }
        else {
            if ( is_subset( Xi, Xj ) ) {
                property_2( p_tree, p_i_tree, test_node, Xi );
            }
            else {
                if ( is_subset( Xj, Xi ) ) {
                    property_3( p_i_tree, p_tree, Xj, test_node );
                }
                else {
                    property_4( p_i_tree, test_node );
                }
            }
        }
    }
}

/*!
 * \brief Charm::is_subsumed
 * \param c_set
 * \param X
 * \param Y
 * \return
 */
bool Charm::is_subsumed(const CSet &c_set, const Itemset &X, const Tidset &Y)
{
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
    union_itemset.resize( it_union - union_itemset.begin() );
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

/*!
 * \brief Charm::property_1
 * \param p_i_tree
 * \param p_tree
 * \param Xj
 * \param Xi
 * \param test_node
 */
void Charm::property_1(Tree &p_i_tree, Tree &p_tree, Node &Xj, Node &Xi, Node &test_node)
{
    p_tree.remove( Xj.itemset() );
    const auto itemset_to_replace = Xi.itemset();
    p_i_tree.replace( itemset_to_replace, test_node.itemset() );
    p_tree.replace( itemset_to_replace, test_node.itemset() );
}

/*!
 * \brief Charm::property_2
 * \param p_tree
 * \param p_i_tree
 * \param test_node
 * \param Xi
 */
void Charm::property_2(Tree &p_tree, Tree &p_i_tree, Node &test_node, Node &Xi)
{
    const auto itemset_to_replace = Xi.itemset();
    p_i_tree.replace( itemset_to_replace, test_node.itemset() );
    p_tree.replace( itemset_to_replace, test_node.itemset() );
}

/*!
 * \brief Charm::property_3
 * \param p_i_tree
 * \param p_tree
 * \param Xj
 * \param test_node
 */
void Charm::property_3(Tree &p_i_tree, Tree &p_tree, Node &Xj, Node &test_node)
{
    p_tree.remove( Xj.itemset() );
    p_i_tree.add( test_node.itemset(), test_node.tidset() );
}

/*!
 * \brief Charm::property_4
 * \param p_i_tree
 * \param test_node
 */
void Charm::property_4(Tree &p_i_tree, Node &test_node)
{
    p_i_tree.add( test_node.itemset(), test_node.tidset() );
}

/*!
 * \brief Charm::tidset_equal
 * \param Xi
 * \param Xj
 * \return
 */
bool Charm::tidset_equal(const Node &Xi, const Node &Xj)
{
    return ( ( Xi.tidset().size() == Xj.tidset().size() ) && std::equal( Xi.tidset().cbegin(), Xi.tidset().cend(), Xj.tidset().cbegin() ) );
}

/*!
 * \brief Charm::is_subset
 * \param subset
 * \param superset
 * \return
 */
bool Charm::is_subset(const Node &subset, const Node &superset)
{
    return std::includes( superset.tidset().cbegin(), superset.tidset().cend(), subset.tidset().cbegin(), subset.tidset().cend() );
}
