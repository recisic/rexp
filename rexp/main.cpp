#include <iostream>
#include <cstdio>
#include <vector>
#include <stack>
#include <set>

#define MAX_STRING_LENGTH 50
#define MAX_STATES 50
#define NUM_ALPHABET 2
#define EPSILON NUM_ALPHABET

using namespace std;

class NFA
{
    public:
        set<int> transition[MAX_STATES][NUM_ALPHABET+1];
        bool is_final[MAX_STATES];
        int initial_state;

        NFA()
        {
            for(int i = 0; i < MAX_STATES; i++)
                is_final[i] = false;
            initial_state = -1;
        }
} nfa;

void epsilon_closure(NFA const &nfa, set<int> &C);

int main()
{
    /* Part 1. constructing an NFA from a regular expression */

    stack<char> op_stack;

    char regexp[MAX_STRING_LENGTH];
    scanf("%s", regexp);

    printf("input: %s\n", regexp);

    char postfix[MAX_STRING_LENGTH] = "";
    int i = 0, j = 0;

    while(regexp[i++])
    {
        switch(regexp[i])
        {
            case '(':
                break;
            case '0':
            case '1':
                postfix[j++] = regexp[i];
                break;
            case '+':
            case '*':
            case '.':
                op_stack.push(regexp[i]);
                break;
            case ')':
                postfix[j++] = op_stack.top();
                op_stack.pop();
                break;
        }
    }

    printf("postfix: %s\n", postfix);

    i = 0;
    j = 0;
    stack<pair<int, int> > state_pair_stack;

    while(postfix[i])
    {
        switch(postfix[i])
        {
            case '0':
            case '1':
                {
                    int alph = postfix[i] - '0';
                    nfa.transition[j][alph].insert(j+1); // state j to j+1
                    nfa.is_final[j+1] = true;
                    state_pair_stack.push(make_pair(j, j+1));
                    j += 2; // for next iteration
                    break;
                }

            case '+':
                {
                    pair<int, int> state_pair2 = state_pair_stack.top();
                    state_pair_stack.pop();
                    pair<int, int> state_pair1 = state_pair_stack.top();
                    state_pair_stack.pop();

                    nfa.transition[j][EPSILON].insert(state_pair1.first);
                    nfa.transition[j][EPSILON].insert(state_pair2.first);
                    nfa.transition[state_pair1.second][EPSILON].insert(j+1);
                    nfa.transition[state_pair2.second][EPSILON].insert(j+1);

                    nfa.is_final[state_pair1.second] = false;
                    nfa.is_final[state_pair2.second] = false;
                    nfa.is_final[j+1] = true;

                    state_pair_stack.push(make_pair(j, j+1));
                    j += 2;
                    break;
                }

            case '.':
                {
                    pair<int, int> state_pair2 = state_pair_stack.top();
                    state_pair_stack.pop();
                    pair<int, int> state_pair1 = state_pair_stack.top();
                    state_pair_stack.pop();

                    nfa.transition[state_pair1.second][EPSILON].insert(state_pair2.first);

                    nfa.is_final[state_pair1.second] = false;

                    state_pair_stack.push(make_pair(state_pair1.first, state_pair2.second));
                    break;
                }

            case '*':
                {
                    pair<int, int> state_pair = state_pair_stack.top();
                    state_pair_stack.pop();

                    nfa.transition[j][EPSILON].insert(state_pair.first);
                    nfa.transition[j][EPSILON].insert(j+1);
                    nfa.transition[state_pair.second][EPSILON].insert(state_pair.first);
                    nfa.transition[state_pair.second][EPSILON].insert(j+1);

                    nfa.is_final[state_pair.second] = false;
                    nfa.is_final[j+1] = true;

                    state_pair_stack.push(make_pair(j, j+1));
                    j += 2;
                    break;
                }
        }
        i++;
    }
    pair<int, int> state_pair = state_pair_stack.top();
    nfa.initial_state = state_pair.first;

    /* Part 2. running an NFA with an input string */

    set<int> C, C_temp;
    i = 0;

    char input_string[30] = "0011000";
    printf("string: %s\n", input_string);

    C.insert(nfa.initial_state); // C <- E(q0)
    epsilon_closure(nfa, C);

    /*
    for (int i = 0; i < MAX_STATES; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k : nfa.transition[i][j])
                printf("%d ", k);
            printf(" / ");
        }
        printf("%d", nfa.is_final[i]);
        printf("\n");
    }
    */

    while(input_string[i])
    {
        int alph = input_string[i] - '0';
        C_temp.clear();
        for (int j : C)
        {
            for (int k : nfa.transition[j][alph])
                C_temp.insert(k);
        }
        C = C_temp;
        epsilon_closure(nfa, C);
        i++;
    }

    for (int i : C)
    {
        if (nfa.is_final[i]) goto end;
    }

    printf("No");

    return 0;

    end:
        printf("Yes");
        return 0;
}

void epsilon_closure(NFA const &nfa, set<int> &C)
{
    set<int> C_temp(C);
    do
    {
        C = C_temp;
        for(int i : C_temp)
            for (int j : nfa.transition[i][EPSILON])
                C_temp.insert(j);
    } while(C_temp != C);

}
