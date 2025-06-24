// Copyright 2025 The OnePointer Authors.
//

#include <cstring>
#include <string>
#include <string_view>

#include "bool_atomic_term.hpp"

#include "bool_base.hpp"



namespace truth {


BoolType::BoolType( const BoolType::arguments_t args, bool value, BoolDict< BoolValue >* tvalues )
    :   BoolValue( value )
    ,   result_in( args )
    ,   truth_values( tvalues != nullptr ? *tvalues : *(new BoolDict<BoolValue>()) )
{}

BoolType::BoolType( const bool value )
    :   BoolValue( value )
    ,   result_in()
    ,   truth_values( *(new BoolDict<BoolValue>()) )
{}

BoolType::BoolType( const BoolType::Value value )
    :   BoolValue( value )
    ,   result_in()
    ,   truth_values( *(new BoolDict<BoolValue>()) )
{}


bool BoolType::boolval( const BoolType::Value val ) {
    return ( val == True ? true : false );
}

bool BoolType::boolval( const arguments_t args, const BoolOperator::Type op ) {

    if ( op == BoolOperator::Type::AND ) {
        for ( typename BoolType::arguments_iter_t val : args ) {
            if ( val.second != True ) return false;
        }
    } else if ( op == BoolOperator::OR ) {
        for ( typename BoolType::arguments_iter_t val : args ) {
            if ( val.second != False ) return true;
        }
    }

    return true;
}


BoolType::term_t BoolType::find_terms( expr_t expr, bool_expression_t& restexpr ) {

    oneptr::queue< atomic_term_t >* atq = new oneptr::queue< atomic_term_t >();
    atomic_terms_t* atx = new atomic_terms_t{ expr, atq };

    bool term_begin = false;
    std::string bte;
    for ( char e : expr ) {
        if ( e == ' ' && ! term_begin ) {
            term_begin = true;
        } else if ( term_begin && e == ' ' ) {
            term_begin = false;
            BoolExpr be( bte );
            BoolTerm bt = be.is();
            atq->push_back( { be, bt } );
            bte = "";
        } else {
            bte += e;
        }
    }

    restexpr.first = expr.find_prefix_before( bte );
    restexpr.second = expr.find_suffix_behind( bte );

    return { atx, BoolExpr( bte ).is() };
}


BoolType::expression_t BoolType::find_exprs( expr_t expr, bool_expression_t& restexpr ) {

    std::string exprstr = expr;

    oneptr::queue< atomic_expr_t >* aet
        = new oneptr::queue< atomic_expr_t >();
    BoolExpr reststring_brackets = exprstr;
    size_t aet_size_before = 0;
    while ( reststring_brackets.length() != 0
         && aet_size_before < aet->size()
    ) {
        expression_t exp = BoolType::atomic_bracket_find( exprstr, 1 );
        aet->emplace_back( exp.first );
        reststring_brackets -= exp.second;
    }

    restexpr.first = expr.find_prefix_before( reststring_brackets );
    restexpr.second = expr.find_suffix_behind( reststring_brackets );

    
    atomic_expr_t* aex = new atomic_expr_t();
    for ( oneptr::queue_element<atomic_expr_t>* qeae : { aet->front(), aet->back() } ) {

        bool_expression_t bet{ "", "" };

        for ( oneptr::queue_element<atomic_terms_t>* qeat : { (*qeae)()->front(), (*qeae)()->back() } ) {
            aex->emplace_back( (*qeat)() );
        }
    }

    BoolExpr evaluated_expr = ( expr - restexpr.first ) - restexpr.second;

    return { aex, evaluated_expr };
}


BoolType::expr_vec_t BoolType::atomic_find( expr_t expr, size_t max_terms ) {

    BoolExpr new_expr;
    std::string exprstr = expr;
    size_t epos = 0;
    unsigned int tfound = 0;

    expr_vec_t exprvec;

    std::string estr = "";
    for ( unsigned int e = epos; e < exprstr.length(); e = epos ) {
        char ec = exprstr[e];

        if ( tfound >= max_terms && max_terms > 0 ) break;
        else if ( ec == '(' ) {
            ++tfound;

            expression_t bracket_expr = BoolType::atomic_bracket_find( exprstr.substr( epos ).c_str(), 1 );
            estr += bracket_expr.second;

            BoolExpr bexpr = estr;

            exprvec.emplace_back( bool_expr_t{ bool_expression_t{ bexpr, expr }, truth::BRACKETTERM } );

            if ( BoolType::atomic_expr_count( bracket_expr.second ) + tfound <= max_terms && max_terms > 0 ) {
                expr_vec_t bexprvec = BoolType::atomic_find( bracket_expr.second, max_terms == 0 ? 0 : max_terms - tfound );

                if ( bexprvec.size() > 0 ) exprvec.append_range( std::ranges::subrange{ bexprvec.begin(), bexprvec.end() } );
            }

            epos = exprstr.find_first_not_of( bracket_expr.second, e );

        } else if ( ec == ' ' ) {

            size_t vpos_end = exprstr.find_first_of( ' ', e+1 );

            BoolExpr var_expr = exprstr.substr( e+1, vpos_end-e );
            estr += var_expr;

            ++tfound;

            epos = vpos_end + 1;

            exprvec.emplace_back( bool_expr_t{ bool_expression_t{ var_expr, expr }, truth::VARIABLE } );

        } else if ( ec == '&' || ec == '|' || ec == '>' || ec == '!' ) {

            std::string opstr;
            if ( e+1 < exprstr.length() ) {
                if ( exprstr[e+1] == '&' || exprstr[e+1] == '|' ) {
                    opstr += ec + exprstr[e+1];

                    epos = e+2;

                    ++tfound;
                }
            } else {
                opstr += ec;

                epos = e + 1;

                ++tfound;
            }

            estr += opstr;

            if ( opstr.length() == 2 ) {
                BoolExpr opexpr = opstr;

                exprvec.emplace_back( bool_expr_t{ bool_expression_t{ opexpr, expr }, truth::OPERATOR } );
            } else if ( opstr.length() == 1 && opstr == "!" ) {
                BoolExpr opexpr = opstr;

                exprvec.emplace_back( bool_expr_t{ bool_expression_t{ opexpr, expr }, truth::NEGATION } );
            }
        }
    }
    
    return exprvec;
}


BoolType::expression_t BoolType::atomic_bracket_find( expr_t expr, size_t max_bracket_pairs ) {

    BoolExpr* new_expr = nullptr;
    std::string exprstr = expr;
    size_t bpos = exprstr.find_first_of( "(" );

    if ( bpos == exprstr.npos ) return { nullptr, expr };

    atomic_expr_t* aet = new atomic_expr_t();

    unsigned int bopen = 1;
    unsigned int bmatch = 0;
    while ( bopen > 0 && ( max_bracket_pairs > bmatch || max_bracket_pairs == 0 ) ) {

        std::string bstr;
        for ( unsigned int b = bpos; b < exprstr.length(); b++ ) {
            char cpos = exprstr[b];

            bstr += cpos;

            if ( cpos == ')' && bopen == 1 ) { ++bmatch; break; }
            else if ( cpos == ')' ) --bopen;
            else if ( cpos == '(' ) ++bopen;
            else if ( bmatch == max_bracket_pairs ) break;
        }
        bpos += exprstr.length();

        new_expr = new BoolExpr( bstr );
        atomic_term_t* att = new atomic_term_t{ *new_expr, new_expr->is() };
        oneptr::queue<atomic_term_t>* qat = new oneptr::queue<atomic_term_t>();
        qat->insert( qat->cbegin(), { att } );
        bool_expression_t be{ "", "" };
        term_t tt = BoolType::find_terms( *new_expr, be );
        qat->append_range( std::ranges::subrange{ tt.first->second->begin(), tt.first->second->end() } );
        atomic_terms_t* ats = new atomic_terms_t{ *new_expr, qat };
        aet->insert( aet->cbegin(), { ats } );
    }
    
    return expression_t{ aet, *new_expr };
}


BoolType::bool_expression_t BoolType::atomic_restexpr_find( expr_t expr, expr_t expr_match ) {

    std::string reststr = expr;
    size_t mpos = reststr.find_first_of( expr_match );
    size_t rpos = reststr.find_first_not_of( expr_match, mpos );

    return { expr, reststr.substr( rpos ).c_str() };
}


size_t BoolType::atomic_expr_count( expr_t expr ) {

    std::string exprstr = expr;
    unsigned int aec = 0;

    for ( unsigned int epos = 0; epos < exprstr.length(); ) {
        char e = exprstr[epos];

        if ( e == '(' || e == '&' || e == '|' || e == ' ' ) {

            if ( e == ' ' ) {
                size_t tpos = exprstr.find_first_of( ' ', epos+1 );

                if ( tpos != exprstr.npos ) ++aec;
                else {
                    tpos = exprstr.find_last_of( ' ', exprstr.npos );

                    std::string term = exprstr.substr( tpos ).c_str();
                    if ( term != "&&" && term != "||" && term != "(" && term != ")" ) {
                        return aec;
                    } else ++aec;
                }

                epos = tpos;
                continue;

            } else ++aec;

            ++aec;

            epos = exprstr.find_first_of( ' ', epos );

        } else epos++;
    }

    return aec;
}


BoolType::atomic_bool_t& BoolType::expr( expr_t expr ) {

    bool_expression_t bet{ "", "" };
    term_t tt = BoolType::find_terms( expr, bet );
    AtomicBoolTerm* abt = new AtomicBoolTerm( *tt.first );

    return *(new atomic_bool_t{ abt, expr });
}



BoolType::small_term_queue_t* BoolType::find_operator_in_expr( expr_t expr ) {

    std::string str = expr;

    small_term_queue_t* stqt = new small_term_queue_t();
    expr_vec_t exprvec = BoolType::atomic_find( expr, 0 );

    size_t pos = 0;
    uint8_t setpos = 0;
    small_term_t* stt = nullptr;
    std::string v1, v2, o = "";
    for ( bool_expr_t& bet : exprvec ) {
        if ( bet.second == truth::VARIABLE ) {
            if ( v1.length() == 0 ) {
                v1 = bet.first.first;
                ++setpos;
            } else {
                v2 = bet.first.first;
                ++setpos;
            }
        } else if ( bet.second == truth::OPERATOR ) {
            if ( o.length() == 0 ) {
                o = bet.first.first;
                ++setpos;
            }
        }

        if ( v2.length() > 0 || ( o == "!" && v1.length() > 0 ) ) {
            stt = new small_term_t{ { v1, v2 }, BoolOperator::is( o ) };
            stqt->push_back( stt );

            stt = nullptr;
            v1 = "", v2 = "", o = "";
        }
        
        if ( setpos >= 3 ) {
            setpos = 0;
            stt = nullptr;
            v1 = "", v2 = "", o = "";
        }
    }

    return stqt;
}


BoolType::bindval_t* BoolType::find_first_expr_bind( expr_t expr, expr_t& restexpr ) {

    std::string str = restexpr.length() > 0 ? restexpr : expr;

    BoolExpr* e1, *e2 = nullptr;
    BoolOperator::Type o = BoolOperator::UNKNOWN;
    bool pre_op = true;
    for ( unsigned int i = 0; i < str.length(); i++ ) {

        char s = str[i];
        std::string ops;
        if ( i+1 < str.length() )
            ops = str.substr( i, 2 );
        else ops = str.substr( i, 1 );

        if ( s == '&' || s == '|' || ops == "&&" || ops == "||" ) {
            std::string opsstr = ops;
            size_t gpos = str.find_first_of( ' ', i + opsstr.length() + 2 );
            o = BoolOperator::is( str.substr( gpos ) );
            pre_op = false;
        } else if ( s == '!' ) {
            o = BoolOperator::NEGATION;
            pre_op = false;
        } else if ( pre_op ) {
            if ( s != ' ' ) *e1 += s;
        } else {
            if ( s != ' ' ) *e2 += s;
        }
    }

    if ( e1->length() == 0 ) return nullptr;

    if ( o == BoolOperator::NEGATION ) {
        size_t pos_e1 = str.find_first_not_of( *e1 );
        size_t pos_o = str.rfind( '!', pos_e1 );

        std::string rst = str.substr( 0, pos_o - 1 );
        rst += str.substr( pos_e1 );

        restexpr.assign( rst );
    } else {
        size_t pos_e1 = str.find_first_of( *e1 );
        size_t pos_e2 = str.find_first_not_of( *e2 );

        std::string rst = str.substr( 0, pos_e1 );
        rst += str.substr( pos_e2 );

        restexpr.assign( rst );
    }

    return new bindval_t{ { *e1, *e2 }, o };
}


BoolType::bindval_queue_t* BoolType::find_operators( expr_t expr, bindval_queue_t& bindvals  ) {

    BoolExpr* restexpr = new BoolExpr( expr );
    bindval_t* bs = nullptr;

    while ( (bs = BoolType::find_first_expr_bind( expr.c_str(), *restexpr )) != nullptr ) {

        bool found = false;
        for ( oneptr::queue_element< bindval_t >* qebv : bindvals ) {
            if ( (*qebv)() == bs ) { found = true; break; }
        }
        
        if ( ! found ) {
            bindvals.insert( bindvals.begin(), { bs } );
        }
    }

    return &bindvals;
}

BoolType::bindval_queue_t* BoolType::find_bindvals( expr_t expr, expr_t& restexpr ) {

    bindval_queue_t* bvqt = new bindval_queue_t();

    return BoolType::find_operators( expr, *bvqt );
}


BoolType::Value BoolType::evaluate_bool( expr_t expr, const BoolDict<BoolValue> res ) {

    AtomicBoolTerm* abt = new AtomicBoolTerm( expr );
    return abt->eval( res );
}


void BoolType::evaluate_bool_atomic( const atomic_expr_t expr, BoolType& res ) {
    
    for ( oneptr::queue_element< atomic_terms_t >* qeat : expr ) {

        const atomic_terms_t* att = qeat->get();

        Value bl = BoolType::evaluate_bool( att->first, res.truth_values );

        res.result_in[ att->first ] = bl;
    }
}


BoolType::Value BoolType::evaluate_bool_term( const atomic_terms_t term, const BoolType& res ) {

    bool result = true;

    for ( const oneptr::queue_element<atomic_term_t>* att : { term.second->front(), term.second->back() } ) {
        atomic_term_t atomic_term = *(att->get());

        result = result && BoolType::evaluate_bool( atomic_term.first, res.truth_values );
    }

    return result;
}

BoolType::Value BoolType::eval_bool( const atomic_terms_t* expr, const BoolDict<BoolValue> args ) {

    BoolDict<>* bd = new BoolDict<>( args );
    BoolType* bt = new BoolType( {}, false, bd );
    
    Value eval = evaluate_bool_term( *expr, *bt );
    
    if ( eval ) return BoolType::TRUE;
    return BoolType::FALSE;
}


} // namespace truth

