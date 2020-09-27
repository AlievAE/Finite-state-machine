#pragma once

#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <algorithm>

#include "regex.h"
//#include "regex_automaton.h"

class Automaton { // one-letter edges only, 1 = empty symbol
public:
    Automaton(std::vector<char> alphabet) :
        alphabet_(alphabet),
        G_({{}}) {}

    void add(int a, int b, char c);
    void MakeTerminal(int a);
    void ClearTerminal() {
        terminal_.clear();
    }

    Automaton GetComplement() const; // works only with DFA

    Automaton DeleteEmpty() const;

    Automaton GetDFA() const {
        return DeleteEmpty().DFA();
    }

    bool IsComplete() const; // works only with DFA

    Automaton GetComplete() const; // works only with DFA

    std::vector<std::vector<std::pair<int, char>>> GetGraph() const {
        return G_;
    }
    std::vector<int> GetTerminal() const {
        return terminal_;
    }
    std::vector<char> GetAlphabet() const {
        return alphabet_;
    }

private:
    std::vector<char> alphabet_;
    std::vector<std::vector<std::pair<int, char>>> G_;
    std::vector<int> terminal_;

    std::vector<int> GetSon(int ind) const;
    Automaton DFA() const;

};

std::ostream& operator << (std::ostream& out, const Automaton& A) {
    auto G = A.GetGraph();
    out << "total vertexes: " << G.size() << "\n" << "start: " << 0 << "\n";
    for (int i = 0; i < G.size(); ++i) {
        for (auto p : G[i]) {
            out << i << " ---> " << p.first << " via " << p.second << "\n";
        }
    }
    out << "\nterminal vertexes:\n";
    auto term = A.GetTerminal();
    for (int el : term) {
        out << el << " ";
    }
    out << std::endl;
    return out;
}

void Automaton::add(int a, int b, char c) {
    int sz = G_.size();
    for (int i = 0; i < std::max(a, b) - sz + 1; ++i) {
        G_.push_back({});
    }
    for (std::pair<int, char>& p : G_[a]) {
        if (p == std::make_pair(b, c)) {
            return;
        }
    }
    G_[a].push_back({b, c});
}

void Automaton::MakeTerminal(int a) {
    int sz = G_.size();
    for (int i = 0; i < a - sz + 1; ++i) {
        G_.push_back({});
    }
    for (auto el : terminal_) {
        if (el == a) return;
    }
    terminal_.push_back(a);
}

Automaton Automaton::DeleteEmpty() const {
    Automaton res(alphabet_);
    for (int i = 0; i < G_.size(); ++i) {
        std::vector<int> son = GetSon(i);
        for (int j : son) {
            for (auto p : G_[j]) {
                if (p.second != '1') {
                    res.add(i, p.first, p.second);
                }
            }
            for (int t : terminal_) {
                if (t == j) {
                    res.MakeTerminal(i);
                }
            }
        }
    }
    return res;
}

std::vector<int> Automaton::GetSon(int ind) const {
    std::vector<int> res;
    std::vector<bool> used;
    used.assign(G_.size(), false);
    std::queue<int> q;
    q.push(ind);
    res.push_back(ind);
    used[ind] = true;
    while (!q.empty()) {
        int a = q.front();
        q.pop();
        for (auto p : G_[a]) {
            if (p.second != '1') continue;
            if (!used[p.first]) {
                used[p.first] = true;
                res.push_back(p.first);
                q.push(p.first);
            }
        }
    }
    return res;
}

Automaton Automaton::DFA() const {
    Automaton res(alphabet_);

    std::map<std::set<int>, int> index_of_subset;
    std::vector<std::set<int>> subset;
    std::queue<int> q;

    q.push(0);
    subset.push_back({0});
    index_of_subset[{0}] = 0;

    while (!q.empty()) {
        auto a = q.front();
        q.pop();
        for (char c: alphabet_) {
            std::set<int> vertex;
            for (int v : subset[a]) {
                for (auto p : G_[v]) {
                    if (p.second == c) {
                        vertex.insert(p.first);
                    }
                }
            }  
            if (vertex.empty()) continue;
            if (index_of_subset.find(vertex) != index_of_subset.end()) {
                res.add(a, index_of_subset[vertex], c);
            } else {
                subset.push_back(vertex);
                index_of_subset[vertex] = subset.size() - 1;
                q.push(subset.size() - 1);
                res.add(a, index_of_subset[vertex], c);
            }
        }
    }

    for (int i = 0; i < subset.size(); ++i) {
        for (int el : subset[i]) {
            if (std::find(terminal_.begin(), terminal_.end(), el) != terminal_.end()) {
                res.MakeTerminal(i);
                break;
            }
        }
    }
    return res;
}

bool Automaton::IsComplete() const {
    for (int i = 0; i < G_.size(); ++i) {
        for (char c : alphabet_) {
            bool flag = false;
            for (auto p : G_[i]) {
                if (p.second == c) {
                    flag = true;
                    break;
                }
            }
            if (!flag) {
                return false;
            }
        }
    }
    return true;
}

Automaton Automaton::GetComplete() const {
    if (IsComplete()) {
        return *this;
    }
    Automaton res = *this;
    for (int i = 0; i < G_.size() + 1; ++i) {
        for (char c : alphabet_) {
            bool flag = false;
            for (auto p : res.G_[i]) {
                if (p.second == c) {
                    flag = true;
                    break;
                }
            }
            if (!flag) {
                res.add(i, G_.size(), c);
            }
        }
    }
    return res;
}


Automaton Automaton::GetComplement() const {
    auto A = GetComplete();
    A.ClearTerminal();
    for (int i = 0; i < A.G_.size(); ++i) {
        bool flag = false;
        for (int t : terminal_) {
            if (t == i) {
                flag = true;
                break;
            }
        }
        if (!flag) {
            A.MakeTerminal(i);
        }
    }
    return A;
}