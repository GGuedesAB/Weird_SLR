#include "syntactic.hpp"
#include "lex.hpp"

std::vector<std::string> string_to_vector (std::string str) {
    std::vector<std::string> parsed_inp;
    std::string token = "";
    for (auto c : str) {
        if (c != ' ') {
            token.push_back(c);
        } else {
            parsed_inp.push_back(token);
            token = "";
        }
    }
    parsed_inp.push_back(token);
    return parsed_inp;
}

item::item(std::string head, std::vector<std::string> itemized_tail) {
    this->head = head;
    i[head] = itemized_tail;
}

item::~item() {}

bool item::operator ==(const item &itm) const{
    return (i == itm.i);
}

std::map<std::string, std::vector<std::string>> item::get_map() {
    return i;
}

std::string item::get_head_of_item() {
    return head;
}

std::vector<std::string> item::get_tail_of_item() {
    return i[head];
}

itemizer::itemizer() {}

itemizer::~itemizer() {}

// Gets a tail and itemizes it
// Input: T -> a B c (vector of strings)
// Output: T -> .a B c, T -> a. B c, T -> a B. c, T -> a B c. (vector of vectors of strings)
std::vector<std::vector<std::string>> itemizer::itemize(std::vector<std::string> a_production) {
    std::vector<std::vector<std::string>> result;
    std::vector<std::string> aux = a_production;
    auto it = aux.begin();
    for (int i = 0; i < aux.size(); ++i) {
        aux.insert(it+i, ".");
        result.push_back(aux);
        it = aux.begin();
        aux.erase(it+i);
    }
    aux = a_production;
    aux.push_back(".");
    result.push_back(aux);
    return result;
}

productions::productions() {}

productions::~productions() {}

productions productions::operator= (const productions& prod) {
    productions result;
    result.created_using_operator = true;
    result.all_rules = prod.all_rules;
    result.all_symbols = prod.all_symbols;
    result.heads = prod.heads;
    result.tails = prod.tails;
    result.H_T_map = prod.H_T_map;
    return result;
}

void productions::add_production(std::string line) {
    size_t delimiter_position = line.find(delimiter);
    std::string head_creator = line.substr(0, delimiter_position);
    line.erase(0, delimiter_position+delimiter.length());
    
    std::vector<std::string> tail = string_to_vector(line);
    
    for (auto t : tail) {
        
        if (std::find(all_symbols.begin(), all_symbols.end(), t) == all_symbols.end())
            all_symbols.push_back(t);
    }
    std::string full_tail;
    for (auto t : tail) {
        full_tail += t;
        full_tail += " ";
    }
    full_tail.pop_back();
    std::pair<std::string, std::string> rule = {head_creator, full_tail};
    if (std::find(all_rules.begin(), all_rules.end(), rule) == all_rules.end())
        all_rules.push_back(rule);
    
    if (std::find(heads.begin(), heads.end(), head_creator) == heads.end())
        heads.push_back(head_creator);
    
    if (std::find(tails.begin(), tails.end(), tail) == tails.end())
        tails.push_back(tail);

    if (std::find(all_symbols.begin(), all_symbols.end(), head_creator) == all_symbols.end())
        all_symbols.push_back(head_creator);
    H_T_map[head_creator].push_back(tail);
}

std::vector<std::vector<std::string>> productions::get_tail(std::string head) {
    return H_T_map[head];
}

std::vector<std::string> productions::get_heads() {
    return heads;
}

std::vector<std::string> productions::get_all_symbols() {
    return all_symbols;
}

std::vector<std::pair<std::string, std::string>> productions::get_all_rules() {
    return all_rules;
}

bool productions::is_non_terminal(std::string symbol) {
    if (std::find(heads.begin(), heads.end(), symbol) == heads.end()) {
        return false;
    } else {
        return true;
    }
}

void productions::print_all_productions() {
    for (auto &h : heads) {
        for (auto &t : H_T_map[h]){
            std::cout << h << " -> ";
            for (auto &s : t) {
                std::cout << s;
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}

state::state(std::vector<item> itms) {
    items_in_state = itms;
    calculate_symbols_that_must_be_checked_for_this_state();
}

state::state(unsigned int from_id, unsigned int id) {
    this->id = id;
    this->from_id = from_id;
}

bool state::operator ==(const state &st) const {
    return (items_in_state == st.items_in_state);
}

void state::set_items(std::vector<item> itms) {
    items_in_state = itms;
    calculate_symbols_that_must_be_checked_for_this_state();
}

state::~state() {}

void state::calculate_symbols_that_must_be_checked_for_this_state() {
    for (auto itm : items_in_state) {
        auto tail = itm.get_tail_of_item();
        auto it = std::find(tail.begin(), tail.end(), ".");
        if (it != tail.end() && it+1 != tail.end()) {
            if (std::find(symbols_that_must_be_checked.begin(), symbols_that_must_be_checked.end(), *(it+1)) == symbols_that_must_be_checked.end()) {
                symbols_that_must_be_checked.push_back(*(it+1));
            }
        }
    }
}

// This must be started with an item that is the axiom of a Grammar
void state::closure (item I, productions P) {
    
    // Can I make this a b list?
    // I = {S -> .expr_list}
    items_in_state.push_back(I);
    item aux = items_in_state.back();
    std::vector<std::string> item_tail = I.get_tail_of_item();
    auto it = std::find(item_tail.begin(), item_tail.end(), ".");
    // Non-terminal B is right after the dot
    std::string first_B = *(++it);
    // B = expr_list
    std::vector<std::string> next_Bs;
    std::vector<std::string> already_visited;
    next_Bs.push_back(first_B);

    while (!next_Bs.empty()) {
        auto B = next_Bs.back();
        if (std::find(already_visited.begin(), already_visited.end(), B) == already_visited.end()) {
            already_visited.push_back(B);
        }
        next_Bs.pop_back();
        for (auto prod : P.get_tail(B)) {
            // Add the prod with a dot before it to J
            prod.insert(prod.begin(), ".");
            item I1(B, prod);
            item_tail = I1.get_tail_of_item();
            items_in_state.push_back(I1);
            // Search next non_terminal
            it = std::find (item_tail.begin(), item_tail.end(), ".");
            std::string next_symbol = *(++it);
            // Add the non_terminal to the list of next Bs if not already visited and not already in list of next ones to visit
            if (std::find(next_Bs.begin(), next_Bs.end(), next_symbol) == next_Bs.end() && std::find(already_visited.begin(), already_visited.end(), next_symbol) == already_visited.end()) {
                next_Bs.push_back(next_symbol);
            }
        }
    }
    for (auto sym : already_visited) {
        if (std::find(symbols_that_must_be_checked.begin(), symbols_that_must_be_checked.end(), sym) == symbols_that_must_be_checked.end()){
            symbols_that_must_be_checked.push_back(sym);
        }
    }
}

std::vector<std::string> state::get_state_symbols () {
    return symbols_that_must_be_checked;
}

std::vector<item> state::get_items () {
    return items_in_state;
}

unsigned int state::get_self_id() {
    return id;
}

std::vector<item> state::get_all_items_with_dot_before_symbol_with_dot_moved_after(std::string symbol, unsigned int for_state, productions P) {
    std::vector<item> new_items;
    for (auto itm : items_in_state) {
        auto itm_tail = itm.get_tail_of_item();
        auto it = std::find(itm_tail.begin(), itm_tail.end(), ".");
        if (it != itm_tail.end() && it+1 != itm_tail.end() && *(it+1) == symbol) {
            itm_tail.insert(it+2, ";");
            it = std::find(itm_tail.begin(), itm_tail.end(), ".");
            itm_tail.erase(it);
            std::replace(itm_tail.begin(), itm_tail.end(), ";", ".");
            auto it_check_non_Terminal = std::find(itm_tail.begin(), itm_tail.end(), ".");
            // Se a nova produção possuir um .E (ou seja, um ponto e um terminal) faça o fechamento de .E, a menos que seja o estado inicial!!!
            if (for_state != 0 && it_check_non_Terminal != itm_tail.end() && it_check_non_Terminal+1 != itm_tail.end() && P.is_non_terminal(*(it_check_non_Terminal+1))) {
                std::string head = itm.get_head_of_item();
                std::vector<std::string> tail = itm_tail;
                item I(head, tail);
                state compute_closure (0,0);
                compute_closure.closure(I, P);
                std::vector<item> closure_items = compute_closure.get_items();
                for (auto i : closure_items) {
                    new_items.push_back(i);
                }
            } else {
                std::string head = itm.get_head_of_item();
                std::vector<std::string> tail = itm_tail;
                item I(head, tail);
                new_items.push_back(I);
            }
        }
    }
    return new_items;
}

state GoTo_fun (state S, unsigned int id, std::string symbol, productions P) {
    std::vector<item> all_items_generated_from_latest_S = S.get_all_items_with_dot_before_symbol_with_dot_moved_after(symbol, id, P);
    state k (S.get_self_id(), id);
    k.set_items(all_items_generated_from_latest_S);
    return k;
}

std::map<std::string, std::vector<std::string>> calculate_first_for_grammar (productions P) {
    std::map<std::string, std::vector<std::string>> first;
    for (auto symbol : P.get_all_symbols()) {
        if (!P.is_non_terminal(symbol)) {
            if (first[symbol].size() < 1)
                first[symbol].push_back(symbol);
        }
    }
    bool something_added = false;
    // This count stuff is to count the number of loops nothing is added to the FIRST
    int count = P.get_all_symbols().size();
    do {
        for (auto symbol : P.get_all_symbols()) {
            if (P.is_non_terminal(symbol)) {
                // Para cada produção desse símbolo na gramática P
                for (auto this_non_T_productions : P.get_tail(symbol)) {
                    for (auto it = this_non_T_productions.begin(); it != this_non_T_productions.end(); ++it){
                        // Se for o primeiro não terminal, ou se no first do n_T anterior tinha epslon
                        bool had_epslon = false;
                        if (it > this_non_T_productions.begin()) {
                            auto last_first_had_epslon = first.find(*(it-1));
                            // Encontrei o first do item anterior, checar se tinha epslon
                            if (last_first_had_epslon != first.end()) {
                                auto last_first = last_first_had_epslon->second;
                                for (auto epslon_searcher = last_first.begin(); epslon_searcher != last_first.end(); ++epslon_searcher) {
                                    if (*epslon_searcher == "") {
                                        had_epslon = true;
                                    }
                                }
                                had_epslon = false;
                            }
                        }
                        if (it == this_non_T_productions.begin() || had_epslon) {
                            if (first.find(*it) != first.end()) {
                                for (auto item : first[*it]) {
                                    if ((std::find(first[symbol].begin(), first[symbol].end(), item) == first[symbol].end())) {
                                        first[symbol].push_back(item);
                                        count += P.get_all_symbols().size();
                                    } else {
                                        count--;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    } while (count > 0);
    return first;
}

std::map<std::string, std::vector<std::string>> calculate_follow_for_grammar (productions P) {
    auto head = P.get_heads()[0];
    std::vector<std::vector<std::string>> prods = P.get_tail(head);
    std::vector<std::vector<std::string>> itemized_tails;
    itemizer K;
    for (auto prod : prods) {
         itemized_tails = K.itemize(prod);
    }
    item start_item (head, itemized_tails[0]);
    state S0 (0, 0);
    S0.closure(start_item, P);
    std::vector<item> S0_items = S0.get_items();
    item first_P_production = S0_items.front();
    std::string start_symbol = first_P_production.get_head_of_item();

    std::map<std::string, std::vector<std::string>> first = calculate_first_for_grammar(P);

    std::map<std::string, std::vector<std::string>> follow;
    follow[start_symbol].push_back("$");
    
    int count = P.get_all_symbols().size();

    do {
        for (auto symbol : P.get_all_symbols()) {
            if (P.is_non_terminal(symbol)) {
                // Olhando as broduções de symbol
                for (auto symbol_productions : P.get_tail(symbol)) {
                    for (auto symbol_being_avaliated = symbol_productions.begin(); symbol_being_avaliated != symbol_productions.end(); ++symbol_being_avaliated) {
                        // Encontrando follow imediatos E -> F id (id está em follow(F))
                        if (!P.is_non_terminal(*symbol_being_avaliated) && symbol_being_avaliated != symbol_productions.begin() && P.is_non_terminal(*(symbol_being_avaliated-1))) {
                            if (std::find(follow[*(symbol_being_avaliated-1)].begin(),follow[*(symbol_being_avaliated-1)].end(), *symbol_being_avaliated) == follow[*(symbol_being_avaliated-1)].end()) {
                                follow[*(symbol_being_avaliated-1)].push_back(*symbol_being_avaliated);
                                count += P.get_all_symbols().size();
                            } else {
                                count --;
                            }
                        } else if (P.is_non_terminal(*symbol_being_avaliated) && symbol_being_avaliated + 1 != symbol_productions.end()) {
                            // Add everything (except epslon) of first(symbol_being_avaliated + 1) to follow(symbol_being_avaliated)
                            std::vector<std::string> first_of_next_symbol = first[*(symbol_being_avaliated+1)];
                            for (auto sym_first : first_of_next_symbol) {
                                if (sym_first != "") {
                                    if (std::find(follow[*symbol_being_avaliated].begin(),follow[*symbol_being_avaliated].end(), sym_first) == follow[*symbol_being_avaliated].end()) {
                                        follow[*symbol_being_avaliated].push_back(sym_first);
                                        count += P.get_all_symbols().size();
                                    } else {
                                        count--;
                                    }
                                }
                            }
                        } else if (P.is_non_terminal(*symbol_being_avaliated)) {
                            // A -> @B append follow (A) to follow(B)
                            if (symbol_being_avaliated + 1 == symbol_productions.end()) {
                                for (auto s : follow[symbol]) {
                                    if (std::find(follow[*symbol_being_avaliated].begin(),follow[*symbol_being_avaliated].end(), s) == follow[*symbol_being_avaliated].end()) {
                                        follow[*symbol_being_avaliated].push_back(s);
                                        count += P.get_all_symbols().size();
                                    } else {
                                        count --;
                                    }
                                }
                            // A -> @B# and first(#) has epslon append follow (A) to follow (B)
                            } else if (symbol_being_avaliated + 1 != symbol_productions.end()) {
                                // check: first(#) has epslon append follow (A) to follow (B)
                                // to check first (ABC), check each one and make sure epslon exists in all
                                bool rest_of_prod_contains_epslon = true;
                                for (auto s = symbol_being_avaliated + 1; s != symbol_productions.end(); ++s) {
                                    if (std::find(first[*s].begin(), first[*s].end(), "") == first[*s].end()) {
                                        rest_of_prod_contains_epslon = false;
                                        break;
                                    }
                                }
                                if (rest_of_prod_contains_epslon) {
                                    // append follow (A) to follow (B)
                                    for (auto s : follow[symbol]) {
                                        if (std::find(follow[*symbol_being_avaliated].begin(),follow[*symbol_being_avaliated].end(), s) == follow[*symbol_being_avaliated].end()) {
                                            follow[*symbol_being_avaliated].push_back(s);
                                            count += P.get_all_symbols().size();
                                        } else {
                                            count --;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    } while (count > 0);
    return follow;
}

std::tuple<std::map<unsigned int, std::map<std::string, unsigned int>>, std::map<unsigned int, std::map<std::string, std::string>>> calculate_canonical_collection (productions P) {
    auto head = P.get_heads()[0];
    std::vector<std::vector<std::string>> prods = P.get_tail(head);
    std::vector<std::vector<std::string>> itemized_tails;
    itemizer K;
    for (auto prod : prods) {
         itemized_tails = K.itemize(prod);
    }
    item start_item (head, itemized_tails[0]);
    state S0 (0, 0);

    S0.closure(start_item, P);
    std::vector<item> S0_items = S0.get_items();
    item first_P_production = S0_items.front();
    std::string start_symbol = first_P_production.get_head_of_item();

    std::map<std::string, std::vector<std::string>> follow_map;
    follow_map = calculate_follow_for_grammar(P);

    std::queue<state> state_queue;
    std::vector<state> state_stack;
    state_queue.push(S0);
    state_stack.push_back(S0);
    state current_state = state_queue.back();
    unsigned int global_id = 1;
    // Para cada símbolo nos não terminais nas produções de cada estado, pegue aqueles items com um ponto antes daquele símbolo, arreda o ponto pra direita do símbolo e coloca isso no GOTO(estado,símbolo)
    // Entender melhor GOTO, acho que já meio q to fazendo ele...
    // Simbolo + número do estado = goto(numero estado, simbolo)
    // Para cada símbolo que deve ser analisado nesse estado
    std::map<unsigned int, std::map<std::string, unsigned int>> GOTO_table;
    std::map<unsigned int, std::map<std::string, std::string>> ACTION_table;
    while(!state_queue.empty()){
        for (auto symbol : P.get_all_symbols()) {
            state new_s = GoTo_fun(current_state, global_id++, symbol, P);
            if (!new_s.items_in_state.empty()) {
                std::string new_state_number = std::to_string (new_s.id);
                auto state_iterator = std::find(state_stack.begin(), state_stack.end(), new_s);
                if (state_iterator == state_stack.end()){
                    GOTO_table[current_state.id][symbol]=new_s.id;
                    state_stack.push_back(new_s);
                    state_queue.push(new_s);
                } else {
                    // Revogar o item e seu id
                    state repeated_state = *(state_iterator);
                    GOTO_table[current_state.id][symbol]=repeated_state.id;
                    global_id--;
                }
            } else {
                global_id--;
            }
        }
        state_queue.pop();
        if (!state_queue.empty()){
            current_state=state_queue.front();
        } else {
            break;
        }
    }

    std::vector<state> canonical_state_stack;
    for (auto stt : state_stack) {
        // Delete non-kernel items
        if (stt.id != 0) {
            auto &stt_items = stt.items_in_state;
            auto itm_it = stt_items.begin();
            while(itm_it != stt_items.end()) {
                item I = *itm_it;
                if (I.get_tail_of_item()[0] == ".") {
                    itm_it = stt_items.erase(itm_it);
                } else {
                    ++itm_it;
                }
            }
        }
        canonical_state_stack.push_back(stt);
    }

    for (auto curr_state : state_stack) {
        for (auto symbol : P.get_all_symbols()){
            for (auto current_state_item : curr_state.get_items()) {
                std::vector<std::string> tail = current_state_item.get_tail_of_item();
                std::string head = current_state_item.get_head_of_item();
                auto dot = std::find(tail.begin(), tail.end(), ".");
                // if [A -> @.a#]
                if (!P.is_non_terminal(symbol) && dot != tail.end() && dot+1 != tail.end() && *(dot+1) == symbol) {
                    std::string new_state_number = std::to_string(GOTO_table[curr_state.id][symbol]);
                    ACTION_table[curr_state.id][symbol] = "shift " + new_state_number;
                //Not sure if this is enough, have it tested
                } else if (head == start_symbol && dot+1 == tail.end()) {
                    ACTION_table[curr_state.id]["$"] = "acc 0";
                } else if (dot+1 == tail.end()) {
                    for (auto prod : follow_map[head]) {
                        std::string tail_as_str;
                        for (auto s : tail) {
                            if (s != ".") {
                                tail_as_str += s;
                                tail_as_str += " ";
                            }
                        }
                        tail_as_str.pop_back();
                        int i = 0;
                        for (auto grammar_head : P.get_heads()) {
                            for (auto grammar_tail : P.get_tail(grammar_head)) {
                                std::string grammar_tail_as_str;
                                for (auto s : grammar_tail) {
                                        grammar_tail_as_str += s;
                                        grammar_tail_as_str += " ";
                                }
                                grammar_tail_as_str.pop_back();
                                if (tail_as_str == grammar_tail_as_str) {
                                    ACTION_table[curr_state.id][prod] = "red " + std::to_string(i);
                                }
                                i++;
                            }
                        }
                    }
                }
            }
        }
    }
    return {GOTO_table, ACTION_table};
}

syntatic::syntatic (std::map<unsigned int, std::map<std::string, unsigned int>> GOTO_table, std::map<unsigned int, std::map<std::string, std::string>> ACTION_table) {
    this->GOTO_table = GOTO_table;
    this->ACTION_table = ACTION_table;
}

syntatic::~syntatic() {}

void syntatic::parse_input(std::vector<std::tuple<std::string, std::string>> inp, productions P) {
    std::vector<std::string> parsed_inp;
    std::string parsed_as_str;
    std::string keys_as_str;
    std::vector<std::string> aux;
    for (auto tup : inp) {
        auto value = std::get<0>(tup);
        aux.push_back(value);
    }
    for (auto val : aux) {
        parsed_inp.push_back(val);
        parsed_as_str+=(val + " ");
    }
    parsed_as_str.pop_back();
    while (!aux.empty()){
        aux.pop_back();
    }
    for (auto tup : inp) {
        auto value = std::get<1>(tup);
        aux.push_back(value);
    }
    for (auto val : aux) {
        keys_as_str+=(val + " ");
    }
    keys_as_str.pop_back();
    keys_as_str.pop_back();
    std::vector<std::pair<std::string, std::string>> Grammar_rules = P.get_all_rules();
    const std::string delimiter = " ";

    state_stack.push_back(0);
    auto inp_it = parsed_inp.begin();
    unsigned int steps = 0;
    while(1) {
        steps++;
        if (inp_it == parsed_inp.end()) {
            std::cout << "PARSER - Error on input -> " << keys_as_str << std::endl;
            break;
        }
        unsigned int top_state = state_stack.back();
        std::string action = ACTION_table[top_state][*inp_it];
        if (action == "") {
            std::cout << "PARSER - Error on input -> " << keys_as_str << std::endl;
            break;
        }
        size_t delimiter_position = action.find(" ");
        std::string command = action.substr(0, delimiter_position);
        action.erase(0, delimiter_position+delimiter.length());
        std::string specifier = action;
        if (command == "shift") {
            unsigned int shifted_state = std::stoi(specifier);
            state_stack.push_back(shifted_state);
            inp_it++;
        } else if (command == "red") {
            unsigned int rule_number = std::stoi(specifier);
            auto rule = Grammar_rules[rule_number];
            auto head = std::get<0>(rule);
            auto tail = std::get<1>(rule);
            std::vector<std::string> tokenized_tail = string_to_vector(tail);
            for (auto s : tokenized_tail) {
                state_stack.pop_back();
            }
            auto new_state = GOTO_table[state_stack.back()][head];
            state_stack.push_back(new_state);
        } else if (command == "acc") {
            std::cout << "PARSER - Parsing was successful in " << std::to_string(steps) << " steps." << std::endl;
            std::cout << "PARSER - Input          : " << keys_as_str << std::endl;
            std::cout << "PARSER - Interpreted as : " << parsed_as_str << std::endl;
            break;
        } else {
            std::cout << "PARSER - Error on input -> " << keys_as_str  << std::endl;
            break;
        }
    }
}

int main () {
    std::ifstream grammar_def_file ("grammar.def");
    std::string line;
    productions P;
    while (std::getline(grammar_def_file, line))
    {
        P.add_production(line);
    }
    
    // Example: GOTO_table[0, E] = 2;
    std::tuple<std::map<unsigned int, std::map<std::string, unsigned int>>, std::map<unsigned int, std::map<std::string, std::string>>> SLR_tables;
    std::map<unsigned int, std::map<std::string, unsigned int>> GOTO_table;
    std::map<unsigned int, std::map<std::string, std::string>> ACTION_table;
    SLR_tables = calculate_canonical_collection (P);
    GOTO_table = std::get<0>(SLR_tables);
    ACTION_table = std::get<1>(SLR_tables);
    syntatic parser (GOTO_table, ACTION_table);
    while (1)
    {
        std::string input;
        std::cout << "Enter an expression: " << std::endl;
        std::getline(std::cin, input);
        lex l_parser (input);
        std::vector<std::tuple<std::string, std::string>> list_of_tokens = l_parser.parse_input();
        parser.parse_input(list_of_tokens, P);
    }
}