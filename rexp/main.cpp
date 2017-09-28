#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <set>

#define MAX_STRING_LENGTH 50
#define MAX_STATES 50
#define NUM_ALPHABET 2
#define EPSILON NUM_ALPHABET

using state = int;

class NFA
{
    public:
        std::set<state> transition[MAX_STATES][NUM_ALPHABET+1];
        state initial_state;
        bool is_final[MAX_STATES];

        NFA()
        {
            for (state i = 0; i < MAX_STATES; i++)
                is_final[i] = false;
            initial_state = -1;
        }
};

void epsilon_closure(NFA const &nfa, std::set<state> &C);
std::string regexp_to_postfix(std::string regexp);
NFA postfix_to_nfa(std::string postfix);
std::string check(std::string input_string, NFA const &nfa);


int main()
{
    std::ios::sync_with_stdio(false);

    std::ifstream infile("input.txt");
    std::ofstream outfile("2017_19871.out");

    int T = 0;
    infile >> T;

    for (int i = 0; i < T; i++)
    {
        std::string regexp, input_string;
        std::string dummy;

        infile >> dummy >> regexp >> input_string;

        /* Part 1. constructing an NFA from a regular expression */

        std::string postfix = regexp_to_postfix(regexp);
        NFA nfa = postfix_to_nfa(postfix);

        /* Part 2. running an NFA with an input string */

        std::string result = check(input_string, nfa);
        outfile << result << '\n';
    }

    return 0;
}


void epsilon_closure(NFA const &nfa, std::set<state> &C)
{
    std::set<state> C_temp(C);
    do
    {
        C = C_temp;
        for(state i : C_temp)
            for (state j : nfa.transition[i][EPSILON])
                C_temp.insert(j);
    } while (C_temp != C);

}

std::string regexp_to_postfix(std::string regexp)
{
    std::stack<char> op_stack;

    std::string postfix = "";

    for (char &c : regexp)
    {
        switch (c)
        {
            case '(':
                break;
            case '0':
            case '1':
                postfix += c;
                break;
            case '+':
            case '*':
            case '.':
                op_stack.push(c);
                break;
            case ')':
                postfix += op_stack.top();
                op_stack.pop();
                break;
        }
    }

    return postfix;
}

NFA postfix_to_nfa(std::string postfix)
{
    NFA nfa;
    state j = 0;
    std::stack<std::pair<state, state> > state_pair_stack;

    for (char &c : postfix)
    {
        switch (c)
        {
            case '0':
            case '1':
                {
                    int alph = c - '0';

                    nfa.transition[j][alph].insert(j+1); // state j to j+1

                    nfa.is_final[j+1] = true;

                    state_pair_stack.push(std::make_pair(j, j+1));
                    j += 2; // for next iteration
                    break;
                }

            case '+':
                {
                    std::pair<state, state> state_pair2 = state_pair_stack.top();
                    state_pair_stack.pop();
                    std::pair<state, state> state_pair1 = state_pair_stack.top();
                    state_pair_stack.pop();

                    nfa.transition[j][EPSILON].insert(state_pair1.first);
                    nfa.transition[j][EPSILON].insert(state_pair2.first);
                    nfa.transition[state_pair1.second][EPSILON].insert(j+1);
                    nfa.transition[state_pair2.second][EPSILON].insert(j+1);

                    nfa.is_final[state_pair1.second] = false;
                    nfa.is_final[state_pair2.second] = false;
                    nfa.is_final[j+1] = true;

                    state_pair_stack.push(std::make_pair(j, j+1));
                    j += 2;
                    break;
                }

            case '.':
                {
                    std::pair<state, state> state_pair2 = state_pair_stack.top();
                    state_pair_stack.pop();
                    std::pair<state, state> state_pair1 = state_pair_stack.top();
                    state_pair_stack.pop();

                    nfa.transition[state_pair1.second][EPSILON].insert(state_pair2.first);

                    nfa.is_final[state_pair1.second] = false;

                    state_pair_stack.push(std::make_pair(state_pair1.first, state_pair2.second));
                    break;
                }

            case '*':
                {
                    std::pair<state, state> state_pair = state_pair_stack.top();
                    state_pair_stack.pop();

                    nfa.transition[j][EPSILON].insert(state_pair.first);
                    nfa.transition[j][EPSILON].insert(j+1);
                    nfa.transition[state_pair.second][EPSILON].insert(state_pair.first);
                    nfa.transition[state_pair.second][EPSILON].insert(j+1);

                    nfa.is_final[state_pair.second] = false;
                    nfa.is_final[j+1] = true;

                    state_pair_stack.push(std::make_pair(j, j+1));
                    j += 2;
                    break;
                }
        }
    }
    std::pair<state, state> state_pair = state_pair_stack.top();
    nfa.initial_state = state_pair.first;

    return nfa;
}

std::string check(std::string input_string, NFA const &nfa)
{
    std::set<state> C, C_temp;

    C.insert(nfa.initial_state);
    epsilon_closure(nfa, C);

    for (char &c : input_string)
    {
        int alph = c - '0';
        C_temp.clear();

        for (state j : C)
            for (state k : nfa.transition[j][alph])
                C_temp.insert(k);

        C = C_temp;
        epsilon_closure(nfa, C);
    }

    for (state i : C)
        if (nfa.is_final[i]) goto yes;

    return "No";

    yes:
        return "Yes";
}
