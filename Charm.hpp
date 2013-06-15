#ifndef CHARM_HPP
#define CHARM_HPP

#include "CSet.hpp"

#include "Database.hpp"
#include "Node.hpp"

/*!
 * \brief The Charm class
 */
class Charm
{
public:
    static CSet charm( const Database & database, const unsigned int min_sup );

private:
    static void charm_extend(Node & p_tree, CSet &c_set, const unsigned int min_sup);
    static void charm_property(Node & p_i_tree, Node & p_tree, const Node &test_node, Node & Xi, Node & Xj, const unsigned int min_sup);
    static bool is_subsumed(const CSet &c_set, const Node & node);
    static void itemset_union(Itemset & X, Node & Xj);
    static void tidset_intersection(const Node &Xj, Tidset & Y);

    static void replace_item(Node& node_ref, const Itemset &itemset, const Itemset &itemset_to);
};

#endif // CHARM_HPP
