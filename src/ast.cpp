#include "ast.hpp"

namespace templa {
namespace ast {

char const program::symbol[] = "PROGRAM";
char const decl_func::symbol[] = "DECL_FUNC";
char const decl_params::symbol[] = "DECL_PARAMS";
char const list_match::symbol[] = "LIST_MATCH";
char const type_match::symbol[] = "TYPE_MATCH";
char const statement::symbol[] = "STMT";
char const let_statement::symbol[] = "LET_STMT";
char const if_statement::symbol[] = "IF_STMT";
char const case_statement::symbol[] = "CASE_STMT";
char const case_when::symbol[] = "CASE_WHEN";
char const expression::symbol[] = "EXPR";
char const formula::symbol[] = "FORM";
char const term::symbol[] = "TERM";
char const factor::symbol[] = "FACT";
char const relational_operator::symbol[] = "RELATIONAL_OP";
char const additive_operator::symbol[] = "ADDITIVE_OP";
char const mult_operator::symbol[] = "MULT_OP";
char const constant::symbol[] = "CONSTANT";
char const list::symbol[] = "LIST";
char const enum_list::symbol[] = "ENUM_LIST";
char const int_list::symbol[] = "INT_LIST";
char const char_list::symbol[] = "CHAR_LIST";
char const func_call::symbol[] = "FUNC_CALL";
char const call_args::symbol[] = "CALL_ARGS";

} // namespace ast
} // namespace templa
