#pragma once

#include "regex.h"
#include "automaton.h"

class RegEx_Automaton {
public:
    RegEx_Automaton(const std::vector<char>& alphabet) :
        alphabet_(alphabet),
        G_({{}}) {}

    RegEx_Automaton(const Automaton& A) {
        alphabet_ = A.GetAlphabet();
        terminal_ = A.GetTerminal();
        auto graph = A.GetGraph();
        for (int i = 0; i < graph.size(); ++i) {
            for (auto& p : graph[i]) {
                add(i, p.first, RegEx(p.second));
            }
        }
    }

    void add(int a, int b, RegEx c);
    void erase(int a); // makes equivalent FA w/o a-th vertex
    void MakeTerminal(int a);
    RegEx toRegEx() const;

    std::vector<std::vector<std::pair<int, RegEx>>> GetGraph() const {
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
    std::vector<std::vector<std::pair<int, RegEx>>> G_;
    std::vector<int> terminal_;
};

std::ostream& operator << (std::ostream& out, const RegEx_Automaton& A) {
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

void RegEx_Automaton::add(int a, int b, RegEx c) {
    //std::cout << c << std::endl;
    int sz = G_.size();
    for (int i = 0; i < std::max(a, b) - sz + 1; ++i) {
        G_.push_back({});
    }
    for (std::pair<int, RegEx>& p : G_[a]) {
        if (p.first == b) {
            p.second = p.second + c;
            return;
        }
    }
    G_[a].push_back({b, c});
}

void RegEx_Automaton::erase(int a) {
    //std::cout << a << " tada" << std::endl;
    if (a == 0) return;
    if (std::find(terminal_.begin(), terminal_.end(), a) != terminal_.end()) return;
    RegEx loop = "";
    for (auto& p : G_[a]) {
        if (p.first == a) {
            loop = p.second;
            break;
        } 
    }
    for (int i = 0; i < G_.size(); ++i) {
        if (i == a) continue;
        for (auto p : G_[i]) {
            std::cout << p.first << " " << p.second << std::endl;
            if (p.first != a) continue;
            for (auto edge : G_[a]) {
                // std::cout << p.second << " ! " << loop << " ! " << *loop << " ! " << edge.second << std::endl;
                // std::cout << p.second + (*loop) + edge.second << std::endl;
                std::cout << i << " ? " << edge.first << std::endl;
                add(i, edge.first, p.second * (*loop) * edge.second);
                std::cout << i << " ! " << a << std::endl;
            }
        }
        //while (1);
    }
    std::cout << a << " bambam " << std::endl;
    std::vector<std::vector<std::pair<int, RegEx>>> new_G;
    for (int i = 0; i < G_.size(); ++i) {
        new_G.push_back({});
        if (i == a) continue;
        for (auto& p : G_[i]) {
            if (p.first == a) continue;
            new_G[i].push_back(p);
        }
    }
    G_ = std::move(new_G);
}

RegEx RegEx_Automaton::toRegEx() const {
    auto A = *this;
    int n = A.G_.size() + 1;
    for (int t : A.terminal_) {
        A.add(t, n - 1, "1");
    }
    A.terminal_ = {n - 1};
    std::cout << A;
    //std::cout << A;
    for (int i = 1; i < n - 1; ++i) {
        A.erase(i);
        //std::cout << i << " ############## \n\n" << A;
    }
    RegEx a, b, c, d;
    for (auto& p : A.G_[0]) {
        if (p.first == 0) a = p.second;
        if (p.first == n - 1) b = p.second;
    }
    for (auto& p : A.G_[n - 1]) {
        if (p.first == 0) d = p.second;
        if (p.first == n - 1) c = p.second;
    }
    //std::cout << a << " ! " << b << " ! " << c << " ! " << d << std::endl;
    if (d.expression != "") {
        return *((*a) * b * (*c) * d);
    }
    return (*a) * b * c;
} 

void RegEx_Automaton::MakeTerminal(int a) {
    int sz = G_.size();
    for (int i = 0; i < a - sz + 1; ++i) {
        G_.push_back({});
    }
    for (auto el : terminal_) {
        if (el == a) return;
    }
    terminal_.push_back(a);
}

RegEx toRegEx(const Automaton& A) {
    RegEx_Automaton B(A);
    //std::cout << B;
    return B.toRegEx();
}