#include "lsystem.h"

void Lsystem::addRule(char in, string out, float probability) {
    rules.push_back({in, out, probability});
}

void Lsystem::setAxiom(string s) {
    product = s;
}

void Lsystem::iterate(int n) {
    if (n == 0) return;
    string newProduct = "";
    for (int i = 0; i < product.length(); ++i) {
        char c = product[i];
        string replacement;
        replacement += c;
        float randomValue = ((float)rand()/(float)RAND_MAX);
        float baseValue = 0.0f;
        for (Production p : rules) {
            if (p.in == c) {
                if (randomValue < baseValue + p.probability) {
                    replacement = p.out;
                    break;
                }
                baseValue += p.probability;
            }
        }
        newProduct += replacement;
    }
    product = newProduct;
    iterate(n-1);
}