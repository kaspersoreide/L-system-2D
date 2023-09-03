#pragma once
#include <string>
#include <vector>
#include <random>

using namespace std;

class Lsystem {
public:
    struct Production {
        // for productions with same in, probabilities must add up to 1
        char in;
        string out;
        float probability;
    };

    Lsystem() {};
    void addRule(char in, string out, float probability);
    void setAxiom(string s);
    void iterate(int n);
    
    vector<Production> rules;
    string product;
};