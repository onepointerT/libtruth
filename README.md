### OnePointer C++ LibTruth
##### v0.1.2
###### Basic and general concepts for truth evaluation with C++ from OnePointer UG


This library helps evaluating truth values of algebraic truth terms corresponding to
their real value of coherent variables. Under development.

The documentation can be found in the folder `doc/`.

#### Usage

You may easily create your own logical expressions and evaluate its substantial parts down
until atomic logical terms and algebraic operands consisting of only two variables and and a operator
or even two logical terms and a operator.

```C++
#include <truth/boolexpr.hpp>
#include <truth/bool_base.hpp>
#include <truth/bool_atomic_term.hpp>
#include <truth/bool_value_dict.hpp>

using namespace truth;

BoolExpr expr = "(a && b) >> c";
BoolType* type = new BoolType( expr );
BoolDict<BoolValue>* bd = new BoolDict<BoolValue>();
(*bd)["a"] = True;
(*bd)["b"] = True;
(*bd)["c"] = False;
BoolValue value = BoolType::evaluate_bool( expr, *bd );
type->results[expr] = value;
```

Also you may inspect terms and their result in shape of a b-tree.

```C++
#include <truth/booltree.hpp>

truth::Tree* btree = Tree::get_new( expr );
btree->eval( *bd );
```

Lastly you may get an evaluation pair for inspection and evaluate it later.

```C++
#include <truth/bool_lazy.hpp>

truth_pair_t& tp = truth::get_new( new AtomicBoolTerm(expr) );
tp.second->set("a", false);
tp.second->set("c", true);
tp.second->set("b", []() { return btree->eval(*bd) } );
truth::eval( tp );
```

* Numeric comparison operators in logical expressions will be implemented
* Interaction for getting the realistic values of bool dicts is under development as well as logging functionality.
