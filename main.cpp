#include "regex.h"
#include "regex_automaton.h"

int main() {
    Automaton A({'a', 'b'});
    A.add(0, 1, 'a');
    A.add(1, 1, 'b');
    A.add(1, 2, 'a');
    A.add(2, 4, 'a');
    A.add(4, 2, 'b');
    A.add(2, 3, 'a');
    A.add(3, 5, 'a');
    A.add(5, 3, 'b');
    A.add(3, 1, '1');
    A.MakeTerminal(1);
    std::cout << A.GetDFA();
}