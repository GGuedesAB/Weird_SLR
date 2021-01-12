#include "syntactic.hpp"

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
    result.heads = prod.heads;
    result.tails = prod.tails;
    result.H_T_map = prod.H_T_map;
}

void productions::add_production(std::string line) {
    size_t delimiter_position = line.find(delimiter);
    std::string head_creator = line.substr(0, delimiter_position);
    line.erase(0, delimiter_position+delimiter.length());
    
    std::string tail_creator = "";
    std::vector<std::string> tail;
    
    for (auto c : line) {
        if (c != ' ') {
            tail_creator.push_back(c);
        } else {
            tail.push_back(tail_creator);
            tail_creator = "";
        }
        if (std::find(all_symbols.begin(), all_symbols.end(), tail_creator) == all_symbols.end())
            all_symbols.push_back(tail_creator);
    }
    tail.push_back(tail_creator);
    
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

grammar::grammar(productions G)
{
    Grammar = G;
}

grammar::~grammar() {}

// void syntatic::create_states_to_G (std::string G) {

// }

// void syntatic::parse_tokens(std::vector<std::tuple<std::string, std::string>> tokens) {
//     for (auto &dup : tokens) {
//         std::string key = std::get<0>(dup);
//         std::string value = std::get<1>(dup);
//         std::string s = compilerStack.front();
//         if (ACTION[s, key] == "shift") {
//             compilerStack.push_back(key);
//         } else if (ACTION[s, key] == "reduce A->x") {
//             // pop |x| symbols
//             auto s = GOTO[t, A];
//             compilerStack.push_back(s);
//         } else if (ACTION == "accept") {
//             break;
//         } else {
//             std::cout << "Error on token (" << key << ", " << value << ")" << std::endl;
//         }
//     }
// }

state::state(std::vector<item> itms) {
    items_in_state = itms;
    calculate_symbols_that_must_be_checked_for_this_state();
}

state::state(unsigned int from_id, unsigned int id) {
    this->id = id;
    this->from_id = from_id;
}

//state state::operator= (const state &st) {
//    items_in_state = st.items_in_state;
//    id = st.id;
//    from_id = st.from_id;
//    symbols_that_must_be_checked = st.symbols_that_must_be_checked;
//}

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
    symbols_that_must_be_checked = already_visited;
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

int main () {
    std::ifstream grammar_def_file ("grammar.def");
    std::string line;
    productions P;
    while (std::getline(grammar_def_file, line))
    {
        P.add_production(line);
    }
    itemizer K;
    
    auto head = P.get_heads()[0];
    std::vector<std::vector<std::string>> itemized_tails;
    std::vector<std::vector<std::string>> prods = P.get_tail(head);
    for (auto prod : prods) {
         itemized_tails = K.itemize(prod);
    }
    item start_item (head, itemized_tails[0]);
    state S0 (0, 0);
    
    S0.closure(start_item, P);

    std::queue<state> state_queue;
    std::vector<state> state_stack;
    state_queue.push(S0);
    state_stack.push_back(S0);
    state current_state = state_queue.back();
    // Para cada símbolo nos não terminais nas produções de cada estado, pegue aqueles items com um ponto antes daquele símbolo, arreda o ponto pra direita do símbolo e coloca isso no GOTO(estado,símbolo)
    // Entender melhor GOTO, acho que já meio q to fazendo ele...
    // Simbolo + número do estado = goto(numero estado, simbolo)
    unsigned int global_id = 1;
    // Para cada símbolo que deve ser analisado nesse estado
    while(!state_queue.empty()){
        for (auto symbol : current_state.get_state_symbols()) {
            state new_s = GoTo_fun(current_state, global_id++, symbol, P);
            if (std::find(state_stack.begin(), state_stack.end(), new_s) == state_stack.end()){
                state_stack.push_back(new_s);
                state_queue.push(new_s);
            } else {
                global_id--;
            }
        }
        state_queue.pop();
        current_state=state_queue.front();
    }
    // for (auto each_itm : state_stack) {
    //     bool something_added = 0;
    //     for (auto symbol : P.get_all_symbols()){
    //         state new_s = GoTo_fun(each_itm, symbol, P);
    //         auto check_s = std::find(state_stack.begin(), state_stack.end(), new_s);
    //         if (!new_s.items_in_state.empty() && check_s == state_stack.end()) {
    //             something_added = 1;
    //             new_s.id = global_id++;
    //             state_stack.push_back(new_s);
    //         }
    //     }
    //     if (!something_added) {
    //         break;
    //     }
    // }

    P.print_all_productions();
}