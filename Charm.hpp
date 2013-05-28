#ifndef CHARM_HPP
#define CHARM_HPP

#include "Tree.hpp"
#include "CSet.hpp"

#include "Database.hpp"

/*!
 * \brief The Charm class
 */
class Charm
{
public:
    static CSet charm( const Database & database, const unsigned int min_sup );

private:
    static void charm_extend(Tree & p_tree, CSet &c_set, const unsigned int min_sup);
    static void charm_property(Tree & p_i_tree, Tree & p_tree, const Node &test_node, Node & Xi, Node & Xj, const unsigned int min_sup);
    static bool is_subsumed(const CSet &c_set, const Node & node);
    static void itemset_union(Itemset & X, Node & Xj);
    static void tidset_intersection(Node& Xj, Tidset & Y);
    static void property_1(Tree & p_i_tree, Tree & p_tree, Node & Xj, Node & Xi, const Node &test_node);
    static void property_2(Tree & p_tree, Tree & p_i_tree, const Node &test_node, Node & Xi);
    static void property_3(Tree & p_i_tree, Node & Xj, const Node &test_node);
    static void property_4(Tree & p_i_tree, const Node &test_node);
    static bool tidset_equal(const Node& Xi, const Node& Xj);
    static bool is_subset(const Node& subset, const Node& superset);
};

#endif // CHARM_HPP
