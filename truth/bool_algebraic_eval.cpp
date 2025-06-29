// Copyright 2025 The OnePointer Authors.
//

#include "bool_atomic_term.hpp"

#include "bool_algebraic_eval.hpp"





namespace truth {

EvaluationGroup::EvaluationGroup( std::initializer_list< AlgebraicBind* > logic_terms )
    :   oneptr::queue< AlgebraicBind >()
    ,   exam( nullptr )
{
    this->append_range( std::ranges::subrange{ *(logic_terms.begin()), *(logic_terms.end()) } );

    exam = get_evaluation_exam();
}

EvaluationGroup::EvaluationGroup( const BoolExpr& expr )
    :   oneptr::queue< AlgebraicBind >()
    ,   exam( nullptr )
{
    BoolExpr restexpr = "";
    BoolType::bindval_queue_t* bqt = BoolType::find_bindvals( expr, restexpr );
    
    for ( oneptr::queue_element< BoolType::bindval_t >* qe : *bqt ) {
        AlgebraicBind* ab = new AlgebraicBind( *(qe->get()) );
        this->push_back( ab );
    }

    exam = get_evaluation_exam();
}

EvaluationGroup::EvaluationGroup( const std::string expr )
    :   oneptr::queue< AlgebraicBind >()
    ,   exam( nullptr )
{
    BoolExpr restexpr = "";
    BoolType::bindval_queue_t* bqt = BoolType::find_bindvals( expr, restexpr );
    
    for ( oneptr::queue_element< BoolType::bindval_t >* qe : *bqt ) {
        AlgebraicBind* ab = new AlgebraicBind( *(qe->get()) );
        this->push_back( ab );
    }

    exam = get_evaluation_exam();
}

EvaluationExam* EvaluationGroup::get_evaluation_exam() {
    return new EvaluationExam( *this );
}


EvaluationExam::EvaluationExam( const EvaluationGroup& eval_group )
    :   evaluation_group( eval_group )
    ,   evaluated( false )
    ,   small_terms()
{}

EvaluationExam::EvaluationExam( EvaluationGroup& eval_group )
    :   evaluation_group( eval_group )
    ,   evaluated( false )
    ,   small_terms()
{}

EvaluationExam::EvaluationExam( const EvaluationExam& evaluation_exam )
    :   evaluation_group( evaluation_exam.evaluation_group )
    ,   evaluated( evaluation_exam.evaluated )
    ,   small_terms( evaluation_exam.small_terms )
{}

bool EvaluationExam::is_evaluated() const {
    return evaluated;
}

void EvaluationExam::evaluate_all_groups() {

    small_terms.clear();
    evaluated = false;

    for ( const oneptr::queue_element< AlgebraicBind >* qe : { evaluation_group.front(), evaluation_group.back() } ) {
        std::vector< AlgebraicSmallTerm* > astvec_left = qe->get()->small_terms_left();
        std::vector< AlgebraicSmallTerm* > astvec_right =  qe->get()->small_terms_right();
        
        auto add = [this](AlgebraicSmallTerm* term) {
            this->small_terms.push_back( term );
        };

        for ( AlgebraicSmallTerm* ast : astvec_left ) { add( ast ); }
        for ( AlgebraicSmallTerm* ast : astvec_right ) { add( ast ); }
    }
    
    evaluated = true;
}

void EvaluationExam::evaluate_exam( const BoolDict<BoolValue>& bd ) {
    if ( ! evaluated ) { this->evaluate_all_groups(); }

    for ( oneptr::queue_element< AlgebraicSmallTerm >* qe : small_terms ) {
        bool result = (*qe)()->eval( bd );
    }

    // TODO:
}


void EvaluationExam::evaluate_group( const BoolDict<BoolValue>& bd ) {
    this->value = this->evaluation_group.eval_if_complete( bd );
}


Evaluator::Evaluator()
    :   exams()
    ,   groups()
    ,   type( *(new BoolType()) )
    ,   value( *(new BoolValue(BoolValue::Unknown)) )
    ,   expr()
{}

Evaluator::Evaluator( BoolType& bool_type )
    :   exams()
    ,   groups()
    ,   type( bool_type )
    ,   value( *(new BoolValue(BoolValue::Unknown)) )
    ,   expr()
{}

Evaluator& Evaluator::operator<<( const std::string algebraic_logic_expression ) {
    this->expr.append( algebraic_logic_expression );
    return *this;
}

void Evaluator::set_expr( const std::string algebraic_logic_expression ) {
    this->expr = algebraic_logic_expression;
}

void Evaluator::clear_expr() {
    this->expr.clear();
}

bool Evaluator::eval( const BoolDict< BoolValue >& bd ) {
    if ( expr.size() == 0 ) return false;

    expr_t restexpr = "";
    BoolType::bindval_queue_t* bqt = BoolType::find_bindvals( expr, restexpr );

    for ( oneptr::queue_element< BoolType::bindval_t >* qe : *bqt ) {
        groups.emplace_front(
            new EvaluationGroup(
                { new AlgebraicBind( *(qe->get()) ) }
            )
        );
    }

    BoolValue eval_group_result = BoolValue::None;
    for ( oneptr::queue_element< EvaluationGroup >* qe_eg : groups ) {
        qe_eg->get()->exam->evaluate_exam( bd );
        
        BoolValue result, tmp = BoolValue::None;
        oneptr::queue_element< AlgebraicSmallTerm >* ast_tmp;
        for ( oneptr::queue_element< AlgebraicSmallTerm >* qe_ast : qe_eg->get()->exam->small_terms ) {
            AlgebraicSmallTerm* ast = qe_ast->get();
            if ( ast->term.first.first.length() == 0 ) {
                result = ast->eval( bd, qe_ast->prev->get()->value );
            } else if ( ast->term.first.second.length() == 0 ) {
                ast_tmp = qe_ast;
            } else {
                result = ast->eval( bd );
            }
            
            if ( ast_tmp != nullptr ) {
                result = ast->eval( bd, ast_tmp->get()->value );
                ast_tmp = nullptr;
            }

            if ( ast->from == nullptr ) {
                for ( oneptr::queue_element< AlgebraicBind >* qe_ab : *(qe_eg->get())  ) {
                    if ( qe_ab->get()->value.first.first.contains(ast->string())
                      || qe_ab->get()->value.first.second.contains(ast->string())
                    ) {
                        ast->from = qe_ab->get();
                        break;
                    }
                }
            }
            if ( ast->from == nullptr ) {
                qe_eg->get()->exam->value = BoolValue::None;
                return false;
            } else {
                ast->from->push_evaluated_small_term( ast );
            }
            
        }

        qe_eg->get()->exam->value = result;

        qe_eg->get()->eval_if_complete( bd );
    }

    AtomicBoolTerm* abt = new AtomicBoolTerm( this->expr );
    bool result = abt->eval( bd );
    type.results[ this->expr.c_str() ] = result;

    return result;
}

BoolDict< BoolValue >& Evaluator::getDict() const {
    BoolDict< BoolValue >* bd = new BoolDict< BoolValue >();

    for ( oneptr::queue_element< EvaluationGroup >* qe_group : groups ) {
        for ( oneptr::queue_element< AlgebraicBind >* ab : *(*qe_group)() ) {
            AlgebraicBind::fillDict( ab->get()->value, *bd );
        }
    }

    return *bd;
}
const oneptr::queue< EvaluationExam > Evaluator::get_exams() const {
    return this->exams;
}

const oneptr::queue< EvaluationGroup > Evaluator::get_groups() const {
    return this->groups;
}

void Evaluator::clear() {
    this->clear_expr();
    this->exams.clear();
    this->groups.clear();
}


} // namespace truth


