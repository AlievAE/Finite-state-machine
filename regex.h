#pragma once

#include <iostream>
#include <optional>
#include <vector>
#include <string>

struct RegEx {
    std::optional<std::vector<std::pair<int, int>>> BracketSequence() const;
    RegEx() {};
    RegEx(std::string A){
        for (char c : A) {
            if (c != ' ' && c != '\n' && c != '\t') {
                expression += c;
            }
        }
        if (!BracketSequence()) {
            throw std::runtime_error("bad bracket sequence, corrupted regex");
        }
    }
    RegEx(char a){
        expression = std::string(1, a);
    }

    RegEx(const char* a) {
        std::string A = a;
        for (char c : A) {
            if (c != ' ' && c != '\n' && c != '\t') {
                expression += c;
            }
        }
        if (!BracketSequence()) {
            throw std::runtime_error("bad bracket sequence, corrupted regex");
        }
    }

    bool Embraced() const {
        auto opt = BracketSequence();
        if (!opt) {
            throw std::runtime_error("bad bracket sequence, corrupted regex");
        }
        if (expression == "") return true;
        auto bracket = opt.value();
        return (!bracket.empty() && bracket[0].first == 0 && bracket[0].second == expression.size() - 1);
    }
    RegEx GetEmbraced() const {
        if (Embraced()) {
            return *this;
        }
        return RegEx("(" + expression + ")");
    }

    RegEx operator +(const RegEx& B) const {
        if (expression == "") {
            return B;
        } else if (B.expression == "") {
            return *this;
        }
        return RegEx(GetEmbraced().expression + "+" + B.GetEmbraced().expression);
    }

    RegEx operator *() const {
        if (expression == "") {
            return *this;
        }
        return RegEx(GetEmbraced().expression + "*");
    }

    RegEx operator *(const RegEx B) const {
        if (expression == "") {
            return B;
        } else if (B.expression == "") {
            return *this;
        }
        return RegEx(GetEmbraced().expression + B.GetEmbraced().expression);
    }

    std::string expression = "";
};

std::optional<std::vector<std::pair<int, int>>> RegEx::BracketSequence() const {
    std::vector<std::pair<int, int>> result;
    std::vector<int> open;
    for (size_t i = 0; i < expression.size(); ++i) {
        if (expression[i] == '(') {
            open.push_back(i);
        } else if (expression[i] == ')') {
            if (open.empty()) return std::nullopt;
            result.push_back({open.back(), i});
            open.pop_back();
        }
    }
    if (!open.empty()) return std::nullopt;
    return result;
}

std::ostream& operator << (std::ostream& out, const RegEx& A) {
    return (out << A.expression);
}

