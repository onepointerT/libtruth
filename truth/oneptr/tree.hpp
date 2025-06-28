
#pragma once

#include <set>
#include <vector>


namespace oneptr {


/// @brief A simple balanced or imbalanced tree
/// @tparam ContentT The content of the nodes
/// @tparam LeafT The content of the leafs, this thus means the nodes with no sub-nodes are leafs
template< class ContentT, class LeafT >
class Tree {
public:
    // Forward declaration
    template< typename LeafContentT, typename NodeContentT >
    class Node;

    /// @brief The class of all last sub-nodes, that cannot have a sub-node
    /// @tparam LeafContentT The content type of all leaf nodes
    /// @tparam NodeContentT The content type oof all parent nodes of a leaf
    template< typename LeafContentT = ContentT, typename NodeContentT = ContentT >
    class Leaf
    {
    public:
        /// @brief The pre-last parent node pointer
        Node< LeafContentT, NodeContentT >* parent;
        /// @brief The content pointer of this leaf
        LeafContentT* content;

        /// @brief Constructor
        /// @param leaf_content A reference to the content of this leaf
        /// @param parent_node A reference to the parent node
        Leaf( LeafContentT& leaf_content, Node< LeafContentT, NodeContentT >& parent_node )
            :   parent( &parent_node )
            ,   content( &leaf_content )
        {}
    };

    /// @brief The class of all nodes, that can have sub-nodes, leafs, content and parent nodes
    /// @tparam LeafContentT The content type oof all leaf nodes
    /// @tparam NodeContentT The content type of all parent
    template< typename LeafContentT, typename NodeContentT = ContentT >
    class Node
    {
    public:
        /// @brief The parent node of the current node. The root node has no parent node
        Node< LeafContentT, NodeContentT >* parent;
        /// @brief All sub-nodes that are a leaf without sub-nodes of this node. Typically empty for each level of nodes but the last
        std::vector< Leaf< LeafContentT, NodeContentT >* > leafs;
        /// @brief All sub-nodes of this node
        std::vector< Node< LeafContentT, NodeContentT >* > subs;
        /// @brief A pointer to the content of this node
        NodeContentT* content;

        /// @brief Constructor
        /// @param node_content A reference to the node's content
        /// @param parent_node A pointer to the parent node. Typically never a `nullptr` but for `tree.root`
        Node( NodeContentT& node_content, Node< LeafContentT, NodeContentT >* parent_node = nullptr )
            :   parent( parent_node )
            ,   leafs()
            ,   subs()
            ,   content( &node_content )
        {}
        /// @brief Copy Constructor
        /// @param leaf The leaf to promote to a full node
        Node( Leaf< LeafContentT, NodeContentT >& leaf )
            :   parent( leaf.parent )
            ,   leafs()
            ,   subs()
            ,   content( nullptr )
        {}
        /// @brief Copy Constructor
        /// @param leaf The leaf to promote to a full node
        Node( Leaf< NodeContentT, NodeContentT >& leaf )
            :   parent( leaf.parent )
            ,   leafs()
            ,   subs()
            ,   content( leaf.content )
        {}

        /// @brief Add a sub node to this node
        /// @param node_sub A referene to the sub-node
        void add_sub( Node< LeafContentT, NodeContentT >& node_sub ) {
            node_sub.parent = this;
            this->subs.push_back( &node_sub );
        }
    
        /// @brief Add a sub node to this node
        /// @param node_sub A referene to the sub-node
        void add_leaf( Leaf< LeafContentT, NodeContentT >& node_leaf ) {
            node_leaf.parent = this;
            this->leafs.push_back( &node_leaf );
        }

        /// @brief Remove a sub-node of this node
        /// @param node_sub A reference to the sub-node to remove
        /// @return True on success
        bool remove_sub( Node< LeafContentT, NodeContentT >& node_sub ) {
            unsigned int pos = 0;
            for ( Node< LeafContentT, NodeContentT >* node : this->subs ) {
                if ( *node == node_sub ) {
                    node->parent = nullptr;
                    this->subs.erase( pos );
                    return true;
                } else ++pos;
                
            }
            return false;
        }
        
        /// @brief Remove a sub-leaf of this node
        /// @param node_leaf A reference to the leaf to remove
        /// @return True on success
        bool remove_sub( Leaf< LeafContentT, NodeContentT >& node_leaf ) {
            unsigned int pos = 0;
            for ( Leaf< LeafContentT, NodeContentT >* leaf : this->leafs ) {
                if ( *leaf == node_leaf ) {
                    this->leafs.erase( pos );
                    return true;
                } else ++pos;
                
            }
            return false;
        }
    };

    /// @brief The leaf's type in this tree
    typedef Leaf< LeafT, ContentT > leaf_t;
    /// @brief The node's type in this tree
    typedef Node< LeafT, ContentT > node_t;
    /// @brief The type of this tree
    typedef Tree< ContentT, LeafT > tree_t;
    /// @brief The most upper top node in this tree, this thus means `root.parent == nullptr`
    node_t* root;

    /// @brief All nodes of this tree collected
    std::set< node_t* > nodes;
    /// @brief All leafs of this tree collected
    std::set< leaf_t* > leafs;

    /// @brief The iterator for the `nodes` collector data structure
    typedef std::set< node_t* >::iterator node_iterator;
    /// @brief The iterator for the `leafs` collector data structure
    typedef std::set< leaf_t* >::iterator leaf_iterator;

    /// @brief Constructor
    /// @param root_content The content of the most upper parent node, the root node of the tree
    Tree( ContentT& root_content )
        :   root( root_content, nullptr )
        ,   nodes()
        ,   leafs()
    {}
    /// @brief Constructor
    Tree()
        :   root( nullptr )
        ,   nodes()
        ,   leafs()
    {}

    /// @brief Add a node as sub-node of another node
    /// @param node A reference to the node to add
    /// @param behind A reference to the node that get the new sub-node
    void add( node_t& node, node_t& behind ) {
        this->nodes.push_back( &node );
        behind.add_sub( node );
    }
    /// @brief Add a leaf as sub-node of another node
    /// @param node A reference to the leaf to add
    /// @param behind A reference to the node that get the new sub-node
    void add( leaf_t& leaf, node_t& behind ) {
        this->leafs.push_back( &leaf );
        behind.add_leaf( leaf );
    }
    /// @brief Add a node as sub-node of a leaf and promote the leaf to a node
    /// @param node A reference to the node to add
    /// @param behind A reference to the leaf that get the new sub-node and to be promoted to a fully-featured node
    void add( node_t& node, leaf_t& behind ) {
        this->leafs.erase( &behind );
        node_t* ex_leaf_node = new node_t( behind );
        this->nodes.push_back( ex_leaf_node );
        this->add( node, ex_leaf_node );
    }
    /// @brief Add a complete sub-tree as sub-node of another node
    /// @param at A reference to the node that get the root of `another` as new sub-node
    /// @param another A reference to another tree to add as sub-nodes of `at`
    void add( node_t& at, tree_t& another ) {
        this->add( another.root, at );
        for ( typename std::set< node_t& >::iterator it = another.nodes.begin()
            ; it != another.nodes.end(); ++it
        ) {
            this->nodes.insert( it );
        }
        for ( typename std::set< leaf_t& >::iterator it = another.leafs.begin()
            ; it != another.leafs.end(); ++it
        ) {
            this->leafs.insert( it );
        }
    }
    /// @brief Add a node between two nodes of this tree
    /// @param node The node to add between `behind` and `before`
    /// @param behind The node to add `node` as sub-node before `before`
    /// @param before The node that shall have `node` as parent node between itself and `behind`
    /// @note After calling of this function, the following is concluded
    ///     `behind != before.parent && node.parent == behind && before.parent == node`
    void add_between( node_t& node, node_t& behind, node_t& before ) {
        this->nodes.push_back( &node );
        behind.add_sub( node );
        behind.remove_sub( before );
        before.parent = &node;
        node.add_sub( before );
    }
    /// @brief Add a node between a node and one of is leafs of this tree
    /// @param node The node to add between `behind` and `before`
    /// @param behind The node to add `node` as sub-node before `before`
    /// @param before The leaf that shall have `node` as parent node between itself and `behind`
    /// @note After calling of this function, the following is concluded
    ///     `behind != before.parent && node.parent == behind && before.parent == node`
    void add_between( node_t& node, node_t& behind, leaf_t& before ) {
        before.parent = &node;
        behind.add_sub( node );
        behind.remove_sub( before );
        node.add_leaf( before );
    }

    /// @brief Remove a node from another's sub-nodes
    /// @param node The node, that will not have the node ´from` as parent node
    /// @param from The node that should loose a sub-node
    /// @note The node `node` stays in  the set `nodes` of this tree
    void remove_from( node_t& node, node_t& from ) {
        node.parent = nullptr;
        from.remove_sub( node );
    }
    /// @brief Remove a leaf from another's sub-nodes
    /// @param leaf The leaf, that will not have the node ´from` as parent node
    /// @param from The node that should loose a sub-node leaf
    /// @note The node `node` stays in  the set `leafs` of this tree
    void remove_from( leaf_t& leaf, node_t& from ) {
        leaf.parent = nullptr;
        from.remove_leaf( leaf );
    }

    /// @brief Get a leaf inside of this tree by only knowing it's content
    /// @param leaf_content A reference to the leaf's content
    /// @return A pointer to the leaf, if a leaf was found with the content `leaf_content`
    leaf_t* findLeaf( LeafT& leaf_content ) {
        for ( leaf_t* leaf : this->leafs ) {
            if ( leaf->content == &leaf_content )
                return leaf;
        }
        return nullptr;
    }
    /// @brief Get a node inside of this tree by only knowing it's content
    /// @param node_content A reference to the node's content
    /// @return A pointer to the node, if a leaf was found with the content `node_content`
    node_t* find( ContentT& node_content ) {
        for ( node_t* node : this->nodes ) {
            if ( node->content == &node_content )
                return node;
        }
        return nullptr;
    }

    /// @brief Calculate the depth to a node, beginning and starting at and with `root`
    /// @param node The node to calculate the depth since `root` of
    /// @return The number of nodes between root and node plus two
    const unsigned int depthTo( node_t* node ) {
        if ( node == this->root ) return 0;
        else return this->depthTo(node->parent) + 1;
    }

    /// @brief Calculate the depth to a leaf, beginning and starting at and with `root`
    /// @param leaf The leaf to calculate the depth since `root` of
    /// @return The number of nodes between root and leaf plus two
    const unsigned int depthTo( leaf_t* leaf ) {
        return this->depthTo(leaf->parent) + 1;
    }
};

} // namespace oneptr
