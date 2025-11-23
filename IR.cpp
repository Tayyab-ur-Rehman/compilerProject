#include "ir_gen.h"

using namespace std;

IRGenerator::IRGenerator() : temp_counter(0), label_counter(0) {}

void IRGenerator::generate(Program* program, const string& output_filename) {
    ofstream out_file(output_filename);
    if (!out_file.is_open()) {
        cerr << "Error: could not open output file " << output_filename << endl;
        return;
    }
    
    data_section.str("");
    code_section.str("");
    string_literal_labels.clear();

    visit(program);

    out_file << data_section.str() << endl;
    out_file << code_section.str();
    out_file.close();
}

string IRGenerator::new_temp() {
    return "%t" + to_string(temp_counter++);
}

string IRGenerator::new_label(const string& base) {
    return "@" + base + "_" + to_string(label_counter++);
}

string IRGenerator::get_qbe_type(const string& type) {
    if (type == "int" || type == "bool") return "w";
    if (type == "char") return "b";
    if (type == "double" || type == "float") return "d";
    if (type == "string" || type.back() == '*') return "l";
    if (type == "void") return "";
    return "l"; // Default to pointer/long
}

string IRGenerator::get_qbe_alloc_size(const string& type) {
    if (type == "double" || type == "string" || type.back() == '*') return "8";
    // int, bool, float, char, etc., are all allocated a 4-byte slot.
    return "4";
}

void IRGenerator::visit(Program* node) {
    for (auto* global : node->globals) {
        data_section << "data $" << global->name << " = { ";
        if (global->initializer) {
            if (auto* num = dynamic_cast<NumberLiteral*>(global->initializer)) {
                data_section << get_qbe_type(global->type) << " " << num->value;
            } else {
                data_section << "l 0";
            }
        } else {
            data_section << get_qbe_type(global->type) << " 0";
        }
        data_section << " }" << endl;
    }

    for (auto* func : node->functions) {
        visit(func);
        code_section << endl;
    }
}

void IRGenerator::visit(FunctionDeclaration* node) {
    var_map.clear();
    temp_counter = 0;
    current_func_end_label = new_label("end_" + node->name);

    code_section << "function " << get_qbe_type(node->returnType) << " $" << node->name << "(";
    for (size_t i = 0; i < node->params.size(); ++i) {
        code_section << (i > 0 ? ", " : "") << get_qbe_type(node->params[i].type) << " %p" << i;
    }
    code_section << ") {" << endl;
    code_section << "@start" << endl;

    for (size_t i = 0; i < node->params.size(); ++i) {
        const auto& param = node->params[i];
        string addr = "%addr_" + param.name;
        var_map[param.name] = addr;
        code_section << "    " << addr << " =l alloc" << get_qbe_alloc_size(param.type) << endl;
        code_section << "    sto" << get_qbe_type(param.type) << " %p" << i << ", " << addr << endl;
    }

    visit(node->body);

    code_section << current_func_end_label << endl;
    if (node->returnType == "void") {
        code_section << "    ret" << endl;
    }
    
    code_section << "}" << endl;
}

void IRGenerator::visit(BlockStatement* node) {
    for (auto* stmt : node->statements) {
        visit(stmt);
    }
}

void IRGenerator::visit(Statement* node) {
    if (!node) return;
    if (auto p = dynamic_cast<VariableDeclarationStatement*>(node)) visit(p);
    else if (auto p = dynamic_cast<ExpressionStatement*>(node)) visit(p);
    else if (auto p = dynamic_cast<IfStatement*>(node)) visit(p);
    else if (auto p = dynamic_cast<WhileStatement*>(node)) visit(p);
    else if (auto p = dynamic_cast<ForStatement*>(node)) visit(p);
    else if (auto p = dynamic_cast<ReturnStatement*>(node)) visit(p);
    else if (auto p = dynamic_cast<BlockStatement*>(node)) visit(p);
    else if (auto p = dynamic_cast<BreakStatement*>(node)) visit(p);
    else if (auto p = dynamic_cast<ContinueStatement*>(node)) visit(p);
}

void IRGenerator::visit(VariableDeclarationStatement* node) {
    string addr = "%addr_" + node->name;
    var_map[node->name] = addr;
    code_section << "    " << addr << " =l alloc" << get_qbe_alloc_size(node->type) << endl;
    if (node->initializer) {
        string init_val = visit_expr(node->initializer);
        code_section << "    sto" << get_qbe_type(node->type) << " " << init_val << ", " << addr << endl;
    }
}

void IRGenerator::visit(ExpressionStatement* node) {
    visit_expr(node->expression);
}

void IRGenerator::visit(IfStatement* node) {
    string true_label = new_label("if_true");
    string end_label = new_label("if_end");
    string false_label = node->elseBranch ? new_label("if_false") : end_label;

    string cond_val = visit_expr(node->condition);
    code_section << "    jnz " << cond_val << ", " << true_label << ", " << false_label << endl;

    code_section << true_label << endl;
    visit(node->thenBranch);
    code_section << "    jmp " << end_label << endl;

    if (node->elseBranch) {
        code_section << false_label << endl;
        visit(node->elseBranch);
    }

    code_section << end_label << endl;
}

void IRGenerator::visit(WhileStatement* node) {
    string cond_label = new_label("while_cond");
    string body_label = new_label("while_body");
    string end_label = new_label("while_end");

    string old_continue = current_loop_continue_label;
    string old_break = current_loop_break_label;
    current_loop_continue_label = cond_label;
    current_loop_break_label = end_label;

    code_section << "    jmp " << cond_label << endl;
    code_section << cond_label << endl;
    string cond_val = visit_expr(node->condition);
    code_section << "    jnz " << cond_val << ", " << body_label << ", " << end_label << endl;

    code_section << body_label << endl;
    visit(node->body);
    code_section << "    jmp " << cond_label << endl;

    code_section << end_label << endl;

    current_loop_continue_label = old_continue;
    current_loop_break_label = old_break;
}

void IRGenerator::visit(ForStatement* node) {
    string cond_label = new_label("for_cond");
    string body_label = new_label("for_body");
    string inc_label = new_label("for_inc");
    string end_label = new_label("for_end");

    string old_continue = current_loop_continue_label;
    string old_break = current_loop_break_label;
    current_loop_continue_label = inc_label;
    current_loop_break_label = end_label;

    if (node->initializer) visit(node->initializer);

    code_section << "    jmp " << cond_label << endl;
    code_section << cond_label << endl;
    if (node->condition) {
        string cond_val = visit_expr(node->condition);
        code_section << "    jnz " << cond_val << ", " << body_label << ", " << end_label << endl;
    } else {
        code_section << "    jmp " << body_label << endl;
    }

    code_section << body_label << endl;
    visit(node->body);
    code_section << "    jmp " << inc_label << endl;

    code_section << inc_label << endl;
    if (node->increment) visit_expr(node->increment);
    code_section << "    jmp " << cond_label << endl;

    code_section << end_label << endl;

    current_loop_continue_label = old_continue;
    current_loop_break_label = old_break;
}

void IRGenerator::visit(ReturnStatement* node) {
    if (node->returnValue) {
        string ret_val = visit_expr(node->returnValue);
        code_section << "    ret " << ret_val << endl;
    } else {
        code_section << "    ret" << endl;
    }
    code_section << "    jmp " << current_func_end_label << endl;
}

void IRGenerator::visit(BreakStatement* node) {
    if (!current_loop_break_label.empty()) {
        code_section << "    jmp " << current_loop_break_label << endl;
    }
}

void IRGenerator::visit(ContinueStatement* node) {
    if (!current_loop_continue_label.empty()) {
        code_section << "    jmp " << current_loop_continue_label << endl;
    }
}

string IRGenerator::visit_expr(Expression* node) {
    if (!node) return "";
    if (auto p = dynamic_cast<NumberLiteral*>(node)) return visit_expr(p);
    if (auto p = dynamic_cast<StringLiteral*>(node)) return visit_expr(p);
    if (auto p = dynamic_cast<CharLiteral*>(node)) return visit_expr(p);
    if (auto p = dynamic_cast<BoolLiteral*>(node)) return visit_expr(p);
    if (auto p = dynamic_cast<Identifier*>(node)) return visit_expr(p);
    if (auto p = dynamic_cast<BinaryOperation*>(node)) return visit_expr(p);
    if (auto p = dynamic_cast<Assignment*>(node)) return visit_expr(p);
    if (auto p = dynamic_cast<FunctionCall*>(node)) return visit_expr(p);
    if (auto p = dynamic_cast<UnaryOp*>(node)) return visit_expr(p);
    return "";
}

string IRGenerator::visit_expr(NumberLiteral* node) {
    string temp = new_temp();
    string type = (node->value.find('.') != string::npos) ? "d" : "w";
    code_section << "    " << temp << " =" << type << " " << node->value << endl;
    return temp;
}

string IRGenerator::visit_expr(StringLiteral* node) {
    if (string_literal_labels.count(node->value)) {
        string label = string_literal_labels.at(node->value);
        string temp = new_temp();
        code_section << "    " << temp << " =l $" << label << endl;
        return temp;
    }

    string label = "str_" + to_string(string_literal_labels.size());
    string_literal_labels[node->value] = label;
    data_section << "data $" << label << " = { b \"" << node->value << "\", b 0 }" << endl;
    
    string temp = new_temp();
    code_section << "    " << temp << " =l $" << label << endl;
    return temp;
}

string IRGenerator::visit_expr(CharLiteral* node) {
    string temp = new_temp();
    code_section << "    " << temp << " =w " << (int)node->value[0] << endl;
    return temp;
}

string IRGenerator::visit_expr(BoolLiteral* node) {
    string temp = new_temp();
    code_section << "    " << temp << " =w " << (node->value ? "1" : "0") << endl;
    return temp;
}

string IRGenerator::visit_expr(Identifier* node) {
    string temp = new_temp();
    string c_type = node->inferred_type;
    string qbe_suffix = get_qbe_type(c_type);
    string dest_type = qbe_suffix;

    if (qbe_suffix == "b" || qbe_suffix == "h") {
        dest_type = "w"; // loadb/h are promoted to word
    }

    string addr;
    if (var_map.count(node->name)) { // Local or parameter
        addr = var_map.at(node->name);
    } else { // Global
        addr = new_temp();
        code_section << "    " << addr << " =l $" << node->name << endl;
    }
    
    code_section << "    " << temp << " =" << dest_type << " load" << qbe_suffix << " " << addr << endl;
    return temp;
}

string IRGenerator::visit_expr(Assignment* node) {
    string val = visit_expr(node->value);
    string c_type = node->inferred_type;
    string qbe_suffix = get_qbe_type(c_type);
    
    string addr;
    if (var_map.count(node->identifier->name)) { // Local or parameter
        addr = var_map.at(node->identifier->name);
    } else { // Global
        addr = new_temp();
        code_section << "    " << addr << " =l $" << node->identifier->name << endl;
    }

    code_section << "    sto" << qbe_suffix << " " << val << ", " << addr << endl;
    return val;
}

string IRGenerator::visit_expr(FunctionCall* node) {
    vector<string> arg_temps;
    for (auto* arg : node->arguments) {
        arg_temps.push_back(visit_expr(arg));
    }

    string ret_type = get_qbe_type(node->inferred_type);
    string ret_temp;

    if (!ret_type.empty()) {
        ret_temp = new_temp();
        code_section << "    " << ret_temp << " =" << ret_type << " call $" << node->callee << "(";
    } else {
        code_section << "    call $" << node->callee << "(";
    }

    for (size_t i = 0; i < node->arguments.size(); ++i) {
        string arg_type = get_qbe_type(node->arguments[i]->inferred_type);
        code_section << (i > 0 ? ", " : "") << arg_type << " " << arg_temps[i];
    }
    code_section << ")" << endl;

    return ret_temp;
}

string IRGenerator::visit_expr(UnaryOp* node) {
    string right = visit_expr(node->right);
    string temp = new_temp();
    string type = get_qbe_type(node->inferred_type);

    if (node->op == "-") {
        string zero = new_temp();
        code_section << "    " << zero << " =" << type << " 0" << endl;
        code_section << "    " << temp << " =" << type << " sub " << zero << ", " << right << endl;
    } else if (node->op == "!") {
        string zero = new_temp();
        code_section << "    " << zero << " =w 0" << endl;
        code_section << "    " << temp << " =w ceqw " << right << ", " << zero << endl;
    }
    return temp;
}

string IRGenerator::visit_expr(BinaryOperation* node) {
    if (node->op == "&&" || node->op == "||") {
        string temp = new_temp();
        string res_addr = new_temp();
        code_section << "    " << res_addr << " =l alloc4" << endl;
        
        string eval_rhs_label = new_label("eval_rhs");
        string end_label = new_label("logical_op_end");

        string lhs_val = visit_expr(node->left);

        if (node->op == "&&") {
            string set_res_false_label = new_label("set_false");
            code_section << "    jnz " << lhs_val << ", " << eval_rhs_label << ", " << set_res_false_label << endl;
            
            code_section << set_res_false_label << endl;
            code_section << "    stow 0, " << res_addr << endl;
            code_section << "    jmp " << end_label << endl;
        } else { // ||
            string set_res_true_label = new_label("set_true");
            code_section << "    jnz " << lhs_val << ", " << set_res_true_label << ", " << eval_rhs_label << endl;

            code_section << set_res_true_label << endl;
            code_section << "    stow 1, " << res_addr << endl;
            code_section << "    jmp " << end_label << endl;
        }

        code_section << eval_rhs_label << endl;
        string rhs_val = visit_expr(node->right);
        code_section << "    stow " << rhs_val << ", " << res_addr << endl;
        code_section << "    jmp " << end_label << endl;

        code_section << end_label << endl;
        code_section << "    " << temp << " =w loadw " << res_addr << endl;
        return temp;
    }

    string left = visit_expr(node->left);
    string right = visit_expr(node->right);
    string temp = new_temp();
    string op_str;

    string operand_type = node->left->inferred_type;
    string type_suffix = (operand_type == "double" || operand_type == "float") ? "d" : "w";

    if (node->op == "+" || node->op == "-" || node->op == "*" || node->op == "/") {
        if (node->op == "+") op_str = "add";
        else if (node->op == "-") op_str = "sub";
        else if (node->op == "*") op_str = "mul";
        else if (node->op == "/") op_str = (type_suffix == "w") ? "div" : "divd";
        code_section << "    " << temp << " =" << type_suffix << " " << op_str << " " << left << ", " << right << endl;
    } else if (node->op == "%") {
        op_str = "rem"; // Only for integers
        code_section << "    " << temp << " =w " << op_str << " " << left << ", " << right << endl;
    } else if (node->op == "==" || node->op == "!=" || node->op == "<" || node->op == "<=" || node->op == ">" || node->op == ">=") {
        string cmp_op;
        if (node->op == "==") cmp_op = "ceq";
        else if (node->op == "!=") cmp_op = "cne";
        else if (node->op == "<") cmp_op = (type_suffix == "w") ? "cslt" : "clt";
        else if (node->op == "<=") cmp_op = (type_suffix == "w") ? "csle" : "cle";
        else if (node->op == ">") cmp_op = (type_suffix == "w") ? "csgt" : "cgt";
        else if (node->op == ">=") cmp_op = (type_suffix == "w") ? "csge" : "cge";
        
        op_str = cmp_op + type_suffix;
        code_section << "    " << temp << " =w " << op_str << " " << left << ", " << right << endl;
    }
    
    return temp;
}
