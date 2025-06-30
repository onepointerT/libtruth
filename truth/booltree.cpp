// Copyright 2025 The OnePointer Authors.
//

#include <cstring>
#include <string>
#include <string_view>

#include "bool_base.hpp"

#include "booltree.hpp"





namespace truth {


TreeNode::TreeNode( AlgebraicBind& algebraic_term, TreeNode* lneightbour, TreeNode* rneightbour )
    :   m_term( algebraic_term )
    ,   m_expr( *(new BoolExpr(algebraic_term.value.first.first + BoolOperator::string(algebraic_term.value.second) + algebraic_term.value.first.second)) )
    ,   m_value( *(new BoolValue(BoolValue::Unknown)) )
    ,   op( algebraic_term.value.second )
    ,   deepness( 0 )
    ,   neighbour_left( lneightbour )
    ,   neighbour_right( rneightbour )
    ,   leaf( nullptr )
{}


TreeNode::TreeNode( const BoolExpr& algebraic_term, BoolOperator::Type logic_operator, bool left_term, TreeNode* lneightbour, TreeNode* rneightbour )
    :   m_term( *(new AlgebraicBind( left_term ? algebraic_term : *(new BoolExpr("")), ! left_term ? *(new BoolExpr("")) : algebraic_term 
                        , logic_operator ))  )
    ,   m_expr( algebraic_term )
    ,   m_value( *(new BoolValue(BoolValue::Unknown)) )
    ,   op( logic_operator )
    ,   deepness( 0 )
    ,   neighbour_left( lneightbour )
    ,   neighbour_right( rneightbour )
    ,   leaf( nullptr )
{}

const BoolValue& TreeNode::value() const {
    return this->m_value;
}
const BoolExpr& TreeNode::expr() const {
    return this->m_expr;
}

const AlgebraicBind& TreeNode::term() const {
    return this->m_term;
}

AlgebraicBind::truth_t& TreeNode::terms_truth() {
    return this->m_term.truth;
}

TreeNode::operator bool() const {
    return this->m_value;
}
TreeNode::operator BoolValue() const {
    return this->m_value;
}

TreeNode::node_children_pair_t* TreeNode::split() const {

    BoolExpr::expr_pair_t exprpair
        = this->m_expr.splitBeforeLastOpeningTermBracket();

    if ( exprpair.first.first == nullptr && exprpair.first.second == nullptr )
        exprpair = this->m_expr.splitAtFirstOperator();

    TreeNode* node1 = new TreeNode( *exprpair.first.first, exprpair.second, true );
    TreeNode* node2 = new TreeNode( *exprpair.first.second, exprpair.second );
    
    node1->neighbour_right = node2;
    node2->neighbour_left = node1;

    return new node_children_pair_t{ node1, node2 };
}


TreeLeaf* TreeNode::new_leaf() const {
    TreeLeaf* tleaf = new TreeLeaf(
        &(AlgebraicSmallTerm::new_small_term( this->m_expr ))
    );
    return tleaf;
}




TreeLeaf::TreeLeaf( AlgebraicSmallTerm* ast )
    :   term( ast )
    ,   result( *(new BoolValue(BoolValue::Unknown)) )
{}



Tree* Tree::get_new( const BoolExpr* expr ) {
    Tree* tree = new Tree( expr );
    return tree;
}

void Tree::build_leafs_of_node( node_t* node ) {
    TreeLeaf* nleaf = node->content->new_leaf();
    leaf_t* tleaf = new leaf_t( *nleaf, *node );
    node->add_leaf( *tleaf );

    if ( most_left_leaf == nullptr ) most_left_leaf = tleaf;
    most_right_leaf = tleaf;
}

void Tree::build_node( node_t* node ) {

    BoolExpr::expr_pair_t ept;
    if ( node->content->expr().contains(")") ) {
        BoolType::expression_t expt = BoolType::atomic_bracket_find( node->content->expr() );
        atomic_expr_t* expt_queue = expt.first;
        if ( expt_queue->size() == 0 ) {
            ept = node->content->expr().splitBehindFirstClosingTermBracket();
        } else {

            BoolExpr* expr1 = &((*expt_queue)[0]->get()->first);
            std::string reststr = node->content->expr().find_suffix_behind(*expr1);
            BoolExpr tmp = reststr;
            size_t pos_op = tmp.find_first_of_all( {"&&", "||", ">>"});
            BoolOperator::Type lop = BoolOperator::UNKNOWN;
            if ( pos_op != tmp.npos ) {
                lop = BoolOperator::is( tmp.substr( pos_op, 2 ) );
            }

            ept = { { expr1
                    , new BoolExpr( tmp.substr(pos_op+3) )
            }, lop };

            size_t pos1 = node->content->expr().find_first_of( *ept.first.first );
            size_t pos2 = node->content->expr().find_first_of( *ept.first.second );

            if ( pos1 > pos2 ) {
                std::string opstr = node->content->expr().substr( pos2+ept.first.second->length()+1, pos1-pos2+ept.first.second->length()-2 );
                ept.second = BoolOperator::is( opstr );
            } else {
                std::string opstr = node->content->expr().substr( pos1+ept.first.first->length()+1, pos2-pos1+ept.first.first->length()-2 );
                ept.second = BoolOperator::is( opstr );
            }
        }
        
    } else {
        ept = node->content->expr().splitAtFirstOperator();
    }

    if ( ept.first.first == nullptr || ept.first.second == nullptr ) {
        this->build_leafs_of_node( node );
        return;
    }

    node->content->op = ept.second;
    TreeNode* tn1 = new TreeNode( *(ept.first.first), ept.second, true );
    TreeNode* tn2 = new TreeNode( *(ept.first.second), ept.second );
    tn1->neighbour_right = tn2;
    tn2->neighbour_left = tn1;

    node_t* snode1 = new node_t( *tn1, node );
    node_t* snode2 = new node_t( *tn2, node );

    node->add_sub( *snode1 );
    node->add_sub( *snode2 );

    for ( node_t* cnode : node->subs ) {
        this->build_node( node );
    }
}

Tree::node_t* Tree::build( const BoolExpr& logical_expr, node_t* node ) {
    
    node_t* cnode = node;
    if ( this->root == nullptr ) {
        this->build_root( logical_expr );
        cnode = this->root;
        this->build_node( this->root );
    } else {
        this->build_node( node );
    }
    
    return cnode;
}


void Tree::build_root( const BoolExpr& logical_expr ) {
    TreeNode* tnode = new TreeNode( logical_expr, BoolOperator::UNKNOWN );
    node_t* cnode = new node_t( *tnode, nullptr );
    this->root = cnode;
}


Tree::Tree( const AtomicBoolTerm* atomic_bt )
    :   oneptr::Tree< TreeNode, TreeLeaf >()
    ,   expr( &atomic_bt->atomic_expr )
    ,   most_left_leaf( nullptr )
    ,   most_right_leaf( nullptr )
{
    this->build( *this->expr );
}

Tree::Tree( const BoolExpr* atomic_expr )
    :   oneptr::Tree< TreeNode, TreeLeaf >()
    ,   expr( atomic_expr )
    ,   most_left_leaf( nullptr )
    ,   most_right_leaf( nullptr )
{
    this->build( *this->expr );
}


void Tree::reset( const AtomicBoolTerm* abt ) {
    this->nodes.clear();
    this->leafs.clear();
    this->root = nullptr;

    if ( abt != nullptr ) this->build( abt->atomic_expr );
}


void Tree::reset( const AlgebraicBind* ab ) {
    this->nodes.clear();
    this->leafs.clear();
    this->root = nullptr;

    if ( ab != nullptr ) this->build( ab->string() );
}


BoolValue Tree::eval( const leaf_t* leaf, const BoolDict<BoolValue>* bd ) const {
    if ( leaf->content->result != BoolValue::None && leaf->content->result != BoolValue::Unknown ) {
        return leaf->content->result;
    }
    
    bool leaf_result = leaf->content->term->eval( *bd );
    leaf->content->result.value = leaf_result;
    leaf->parent->content->terms_truth().first = leaf->content->result;
    return leaf_result;
}


BoolValue Tree::eval( const node_t* node, const BoolDict<BoolValue>* bd ) const {

    // Elemental unary bool term, only one variable
    if ( node->content->op == BoolOperator::UNKNOWN /*&& node == this->root*/ )  {
        if ( node->content->expr() == "True" || node->content->expr() == "False" 
          || node->content->expr() == "true" || node->content->expr() == "false"
          || node->content->expr() == "None" || node->content->expr() == "none"
        ) {
            return BoolValue::parse( node->content->expr().c_str() );
        } else return bd->at( node->content->expr() ); 
    }

    BoolValue lchld, rchld, thisnode = BoolValue::Unknown;

    if ( node->subs.size() > 0 ) {
        node_t* cnode = node->subs[0];
        lchld = this->eval( cnode, bd );
    }
    if ( node->subs.size() > 1 ) {
        node_t* cnode = node->subs[1];
        rchld = this->eval( cnode, bd );
    }
    if ( node->leafs.size() > 0 ) {
        for ( const leaf_t* leaf : node->leafs ) {
            this->eval( leaf, bd );
        }
        thisnode = BoolOperator::eval( 
                        node->content->terms_truth().first
                      , node->content->op
                      , node->content->terms_truth().second  
        );
    }

    if ( lchld != BoolValue::Unknown && rchld != BoolValue::Unknown )
        return BoolOperator::eval( lchld, node->content->op, rchld );
    else if ( lchld != BoolValue::Unknown )
        return BoolOperator::eval( lchld, node->content->op, node->content->terms_truth().second );
    else if ( rchld != BoolValue::Unknown )
        return BoolOperator::eval( node->content->terms_truth().first, node->content->op, rchld );
    else if ( thisnode != BoolValue::Unknown && thisnode != BoolValue::None )
        return thisnode;
    return BoolValue::None;
}

BoolValue Tree::eval( const BoolDict<BoolValue>* bd ) const {
    return this->eval( this->root, bd );
}

Tree::operator BoolValue() const {
    return this->root->content->value();
}
Tree::operator bool() const {
    return this->root->content->value();
}


} // namespace truth




