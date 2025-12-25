#include <bits/stdc++.h>
#include <random>
using namespace std;

// Слова из языка
// (aa^+b)
string gen_block() {
    int k = rand() % 4 + 2; 
    return string(k, 'a') + "b";
}

string gen_w(int max_blocks=3) {
    int blocks_count = rand() % max_blocks + 1;
    string w;
    for (int i = 0; i < blocks_count; ++i) {
         w += gen_block();
    }
    if (rand() % 2) {
        w += "a";
    }
    return w;
}

string gen_language_word() {
    string w = gen_w();
    string v = gen_w();
    int n = rand() % 5;
    string middle = "b" + string(n+1, 'a') + "b" + string(rand()%5, 'a');
    string rev_w = w;
    reverse(rev_w.begin(), rev_w.end());
    return w + middle + rev_w + v;
}

// Слова скорее всего не из языка
string gen_random_word(int max_len=20) {
    int len = rand() % max_len + 1;
    string w;
    for (int i = 0; i < len; ++i) {
        if (rand() % 2){
            w += 'a';
        }else{
            w += 'b';
        }
    }
    return w;
}

vector<string> generate_words(int n) {
    vector<string> data;
    for (int i = 0; i < n/2; ++i)
        data.push_back(gen_language_word());

    for (int i = n/2; i < n; ++i)
        data.push_back(gen_random_word());

    return data;
}

struct DFA {
    set<string> states;
    string start_state;
    set<string> final_states;
    map<pair<string,char>, string> transitions;

    bool accepts(const string &word) const {
        string current = start_state;
        for (char c : word) {
            auto it = transitions.find({current, c});
            if (it == transitions.end()) {
                return false; 
            }
            current = it->second;
        }
        return final_states.count(current) > 0;
    }
};

// Чтение ДКА из файла
DFA read_dfa(const string &filename) {
    DFA dfa;
    ifstream fin(filename);
    string line;
    while (getline(fin, line)) {
        line = regex_replace(line, regex("^\\s+|\\s+$"), ""); 
        if (line.empty() || line[0] == '/' || line.find("digraph") != string::npos || line[0] == '}') 
            continue;

        smatch m;
        //Начальное состояние
        if (regex_match(line, m, regex(R"(start\s*->\s*(\d+))"))) {
            dfa.start_state = m[1];
            dfa.states.insert(m[1]);
            continue;
        }

        // Финальные состояния
        if (regex_match(line, m, regex(R"((\d+)\s*\[shape=doublecircle\])"))) {
            dfa.final_states.insert(m[1]);
            dfa.states.insert(m[1]);
            continue;
        }

        // Обычные состояния
        if (regex_match(line, m, regex(R"((\d+)\s*\[shape=circle\])"))) {
            dfa.states.insert(m[1]);
            continue;
        }

        // Переходы
        if (regex_match(line, m, regex(R"((\d+)\s*->\s*(\d+)\s*\[label=([ab])\])"))) {
            string from = m[1];
            string to = m[2];
            char symbol = m[3].str()[0];
            dfa.states.insert(from);
            dfa.states.insert(to);
            dfa.transitions[{from, symbol}] = to;
            continue;
        }
    }
    return dfa;
}

bool check_S(const string &s);
bool check_P(const string &s);
bool check_P1(const string &s);
bool check_P2(const string &s);
bool check_P3(const string &s);
bool check_P4(const string &s);
bool check_P5(const string &s);
bool check_A1(const string &s);
bool check_V(const string &s);
bool check_B(const string &s);
bool check_C(const string &s);
bool check_E(const string &s);

// Проверка исходной LL(1)-грамматики
bool check_S(const string &s) {
    for (size_t i = 0; i <= s.size(); ++i) {
        if (check_P(s.substr(0,i)) && check_V(s.substr(i))) return true;
    }
    return false;
}

bool check_P(const string &s) {
    if (s.size() < 4) return false;
    if (s.substr(0,2) != "aa" || s.substr(s.size()-2) != "aa") return false;
    return check_P1(s.substr(2,s.size()-4));
}

bool check_P1(const string &s) {
    if (s.size() < 2) return false;
    if (s.front() == 'a' && s.back() == 'a') return check_P1(s.substr(1,s.size()-2));
    if (s.front() == 'b' && s.back() == 'b') return check_P2(s.substr(1,s.size()-2));
    return false;
}

bool check_P2(const string &s) {
    if (!s.empty() && s.front() == 'a') return check_P5(s.substr(1));
    return check_P4(s);
}

bool check_P5(const string &s) {
    if (s.size() >= 3 && s.front() == 'a' && s.substr(s.size()-2) == "aa") 
        return check_P1(s.substr(1,s.size()-3));
    return check_P3(s);
}

bool check_P3(const string &s) {
    if (s.size() < 2 || s.substr(0,2) != "ba") return false;
    string rest = s.substr(2);
    for (size_t i = 0; i <= rest.size(); ++i) {
        if (check_A1(rest.substr(0,i))) {
            string r2 = rest.substr(i);
            if (r2.size() >= 2 && r2.substr(0,2) == "ba" && check_A1(r2.substr(2))) return true;
        }
    }
    return false;
}

bool check_P4(const string &s) {
    if (s.size() < 2 || s.substr(0,2) != "ba") return false;
    string rest = s.substr(2);
    for (size_t i = 0; i <= rest.size(); ++i) {
        if (check_A1(rest.substr(0,i))) {
            string r2 = rest.substr(i);
            if (!r2.empty() && r2.front() == 'b' && check_A1(r2.substr(1))) return true;
        }
    }
    return false;
}

bool check_A1(const string &s) {
    return s.empty() || (s.front() == 'a' && check_A1(s.substr(1)));
}

bool check_V(const string &s) {
    if (s.size() < 2 || s.substr(0,2) != "aa") return false;
    return check_B(s.substr(2));
}

bool check_B(const string &s) {
    if (!s.empty() && s.front() == 'a') return check_B(s.substr(1));
    if (!s.empty() && s.front() == 'b') return check_C(s.substr(1));
    return false;
}

bool check_C(const string &s) {
    return s.empty() || (!s.empty() && s.front() == 'a' && check_E(s.substr(1)));
}

bool check_E(const string &s) {
    return s.empty() || (!s.empty() && s.front() == 'a' && check_B(s.substr(1)));
}

// Загрузка грамматик пересечения из файла
pair<map<char,set<string>>, map<pair<string,string>,set<string>>> laod_inters(const string &filename) {
    map<char,set<string>> term_rules;
    map<pair<string,string>,set<string>> bin_rules;

    ifstream fin(filename);
    string line;
    int s = 0;
    while (getline(fin, line)) {
        if (line.find("->") == string::npos){
             continue;
        }
        string lhs = line.substr(0,line.find("->"));
        lhs.erase(remove(lhs.begin(), lhs.end(),' '), lhs.end());

        string rhs = line.substr(line.find("->")+2);
        istringstream iss(rhs);
        vector<string> symbols;
        string sym;
        while (iss >> sym) {
            symbols.push_back(sym);
        }
        if (symbols.size() == 1 && (symbols[0]=="a" || symbols[0]=="b")){
            term_rules[symbols[0][0]].insert(lhs);
            s++;
        }
        else if (symbols.size() == 2){
            s++;
            bin_rules[{symbols[0], symbols[1]}].insert(lhs);
        }
    }
    return {term_rules, bin_rules};
}

// Проверка грамматик пересечений
bool cyk(const string &word, const map<char,set<string>> &term_rules, const map<pair<string,string>,set<string>> &bin_rules, const set<string> &start_symbols) {

    int n = word.size();
    if (n == 0) {
        return false;
    }

    vector<vector<set<string>>> table(n, vector<set<string>>(n+1));
    for (int i = 0; i < n; ++i) {
        if (term_rules.count(word[i])) {
            table[i][1] = term_rules.at(word[i]);
        }
    }

    for (int l = 2; l <= n; ++l) {
        for (int i = 0; i <= n - l; ++i) {
            set<string> cell;
            for (int k = 1; k < l; ++k) {
                for (auto &B : table[i][k]){
                    for (auto &C : table[i+k][l-k]) {
                        auto it = bin_rules.find({B,C});
                        if (it != bin_rules.end()){
                            cell.insert(it->second.begin(), it->second.end());
                        }
                    }
                }
            }
            table[i][l] = cell;
        }
    }

    for (auto &s : table[0][n]){
        if (start_symbols.count(s)){
            return true;
        }
    }
    return false;
}

using Key = tuple<string,char,char>;
using Value = pair<string,string>; 
// Проверка DPDA
bool dpda_accept(const string &word) {
    string state = "1";           
    vector<char> stack = {'z'}; 

    set<string> final_states = {"12","13"};

    map<Key, Value> trans;
    trans[{"1",'z','a'}] = {"2","Az"};
    trans[{"2",'A','a'}] = {"3","AA"};
    trans[{"3",'A','a'}] = {"3","AA"};
    trans[{"3",'A','b'}] = {"4","BA"};
    trans[{"4",'B','a'}] = {"5","AB"};
    trans[{"5",'A','a'}] = {"3","AA"};
    trans[{"5",'\0','b'}] = {"6",""};
    trans[{"4",'\0','b'}] = {"6",""};
    trans[{"6",'\0','a'}] = {"7",""};
    trans[{"7",'\0','a'}] = {"7",""};
    trans[{"7",'\0','b'}] = {"8",""};
    trans[{"8",'B','b'}] = {"9",""};
    trans[{"8",'A','a'}] = {"8",""};
    trans[{"8",'B','a'}] = {"8","B"};
    trans[{"9",'A','a'}] = {"9",""};
    trans[{"9",'B','b'}] = {"9",""};
    trans[{"9",'z','a'}] = {"10","z"};
    trans[{"10",'\0','a'}] = {"11",""};
    trans[{"11",'\0','a'}] = {"11",""};
    trans[{"11",'\0','b'}] = {"12",""};
    trans[{"12",'\0','a'}] = {"13",""};
    trans[{"13",'\0','a'}] = {"11",""};

    for(char c : word) {
        char top = stack.back();
        auto it = trans.find({state, top, c});
        if(it == trans.end()) {
            it = trans.find({state, '\0', c});
            if(it == trans.end()) {
                return false;
            }
        }
        state = it->second.first;

        if(get<1>(it->first) != '\0' && !stack.empty())
            stack.pop_back();

        string push = it->second.second;
        for(auto rit = push.rbegin(); rit != push.rend(); ++rit) {
            if(*rit != '\0') stack.push_back(*rit);
        }
    }

    return final_states.count(state) > 0;
}


int main() {
    srand(time(0));

    int N = 10;
    auto words = generate_words(N);

    DFA lr0dfa = read_dfa("dfalr0.dot");
    auto [term_rules1, bin_rules1] = laod_inters("intersection_grammar_lr0.txt");
    string q01 = "21"; 
    set<string> final_states1 = {"1","3","10","12"}; 
    set<string> start_symbols1;
    for (auto &qf1 : final_states1) start_symbols1.insert("<" + q01 + ",S," + qf1 + ">");

    DFA ll1dfa = read_dfa("dfa_ll1.dot");
    auto [term_rules2, bin_rules2] = laod_inters("intersection_grammar_ll1.txt");
    string q02 = "26"; 
    set<string> final_states2 = {"1","3","9","12"}; 
    set<string> start_symbols2;
    for (auto &qf2 : final_states2) start_symbols2.insert("<" + q02 + ",S," + qf2 + ">");

    for (auto &w : words) {
        bool g1 = check_S(w);
        bool d = dpda_accept(w);
        bool lr0  = lr0dfa.accepts(w);
        bool g_inter1 = cyk(w, term_rules1, bin_rules1, start_symbols1);
        bool ll1  = ll1dfa.accepts(w);
        bool g_inter2 = cyk(w, term_rules2, bin_rules2, start_symbols2);

        if ((g1 != d) || (g1 != g_inter1) || (g1 != g_inter2) || ((lr0 == 0) && (g1 == 1)) || ((ll1 == 0) && (g1 == 1))) {
            cout << "Слово: " << w << " Исходная грамматика: " << g1 << "  " << lr0 << "  " << ll1 << " Пересечение lr0: " << g_inter1 << " Пересечение ll1: " << g_inter2 << " dpda: " << d << endl;
            return 0;
        }
    }
    cout << "Работют эквивалентно" << endl;
}
