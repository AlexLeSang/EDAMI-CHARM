#ifndef TREE_HPP
#define TREE_HPP

#include "Typedefs.hpp"

#include <memory>
#include <cassert>

#include "Node.hpp"

/*!
 * \brief The Tree class
 */
class Tree
{
public:
    Tree();
    Tree(const Node & node);
    void add(const Itemset &itemset, const Tidset & tidset);
    std::weak_ptr < Node > root_node();
    void print_tree() const;
//    void remove( const Itemset & itemset );
    void replace( const Itemset & itemset, const Itemset & itemset_to );

private:
    static void replace_item(Node &node_ref, const Itemset &itemset, const Itemset &itemset_to);
//    static void remove_node(Node& node_ref, const Itemset &itemset);
    static void print_node(const std::weak_ptr<Node> &node_ptr);

private:
    std::shared_ptr < Node > _root_node;
};


#endif // TREE_HPP
