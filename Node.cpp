#include "Node.hpp"

#include <cassert>

#include "Typedefs.hpp"

/*!
 * \brief Node::Node
 */
Node::Node() :
    _itemset( Itemset() ),
    _tidset( Tidset() ),
    _is_erased( false )
{}

/*!
 * \brief Node::Node
 * \param itemset
 * \param tidset
 */
Node::Node(const Itemset &itemset, const Tidset &tidset) :
    _itemset( itemset ),
    _tidset( tidset ),
    _is_erased( false )
{}

/*!
 * \brief Node::Node
 * \param r_node
 */
Node::Node(const Node &r_node) :
    _itemset( r_node.itemset() ),
    _tidset( r_node.tidset() ),
    _parent( r_node.parent() ),
    _children( r_node.children() ),
    _is_erased( r_node.is_erased() )
{}

/*!
 * \brief Node::operator =
 * \param r_node
 * \return
 */
Node &Node::operator =(const Node &r_node)
{
    if ( this != &r_node ) {
        _itemset = r_node.itemset();
        _tidset = r_node.tidset();
        _children = r_node.children();
        _parent = r_node.parent();
        _is_erased = r_node.is_erased();
    }
    return *this;
}

/*!
 * \brief Node::is_erased
 * \return
 */
bool Node::is_erased() const
{
    return _is_erased;
}

/*!
 * \brief Node::set_erased
 */
void Node::set_erased()
{
    _is_erased = true;
}

/*!
 * \brief Node::equal
 * \param r_node
 * \return
 */
bool Node::equal(const Node &r_node) const
{
    return ( ( sup() == r_node.sup() ) && std::equal( _tidset.cbegin(), _tidset.cend(), r_node.tidset().cbegin() ) );
}

/*!
 * \brief Node::is_subset
 * \param r_node
 * \return
 */
bool Node::is_superset_of( const Node r_node ) const
{
    return std::includes( _tidset.cbegin(), _tidset.cend(), r_node.tidset().cbegin(), r_node.tidset().cend() );
}

/*!
 * \brief Node::sup
 * \return
 */
unsigned int Node::sup() const
{
    return _tidset.size();
}

/*!
 * \brief Node::add_child
 * \param node_ref
 */
void Node::add_child(const Node &node_ref)
{
    std::shared_ptr< Node > node( new Node( node_ref ) );
    node->set_parent( this );
    _children.push_back( node );
    /* Lexicographic orded
    std::sort( _children.begin(), _children.end(), [] ( std::shared_ptr< Node > ch1, std::shared_ptr< Node > ch2 ) {
        return ( sup( ch1 ) < sup( ch2 ) );
    } );
    */
    //    /* Increasing order of sup
    std::sort( _children.begin(), _children.end(), [] ( std::shared_ptr< Node > ch1, std::shared_ptr< Node > ch2 ) {
        return ( ch1->sup() < ch2->sup() ); // Sup
    } );
    //    */
    /* Increasing order of sup and lexicograph
    std::sort( _children.begin(), _children.end(), [] ( std::shared_ptr< Node > ch1, std::shared_ptr< Node > ch2 ) {
        if ( sup( ch1 ) != sup( ch2 ) ) {
            return ( sup( ch1 ) < sup( ch2 ) ); // Sup
        }
        else {
            return ( ch1->itemset() < ch2->itemset() ); // Lexicograph
        }
    } );
    */
}

/*!
 * \brief Node::parent
 * \return
 */
Node *Node::parent() const
{
    return _parent;
}

/*!
 * \brief Node::set_parent
 * \param parent_ptr
 */
void Node::set_parent(Node * parent_ptr)
{
    assert( nullptr != parent_ptr );
    _parent = parent_ptr;
}

/*!
 * \brief Node::children
 * \return
 */
const std::vector<std::shared_ptr<Node> > &Node::children() const
{
    return _children;
}

/*!
 * \brief Node::children_ref
 * \return
 */
std::vector<std::shared_ptr<Node> > &Node::children_ref()
{
    return _children;
}

/*!
 * \brief Node::itemset
 * \return
 */
const Itemset &Node::itemset() const
{
    return _itemset;
}

/*!
 * \brief Node::itemset
 * \return
 */
Itemset &Node::itemset()
{
    return _itemset;
}

/*!
 * \brief Node::tidset
 * \return
 */
const Tidset &Node::tidset() const
{
    return _tidset;
}
