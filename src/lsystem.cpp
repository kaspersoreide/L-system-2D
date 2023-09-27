#include "lsystem.h"
#include "loadshaders.h"

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

void Lsystem::loadProductionsBuffer() {
    /* buffer is laid out like this:
    {
        uint inputs[N_PRODUCTIONS];
        uint offsets[N_PRODUCTIONS];
        uint lengths[N_PRODUCTIONS];
        float probabilities[N_PRODUCTIONS];
        uint string[];
    }; 
    */
    glCreateBuffers(1, &productionsBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, productionsBuffer);
    uint count = rules.size();
    uint totalStringLength = 0;
    vector<uint32_t> inputs, offsets, lengths, allProductions;
    vector<float> probabilities;
    for (Production p : rules) {
        inputs.push_back(p.in);
        offsets.push_back(totalStringLength);
        lengths.push_back(p.out.size());
        probabilities.push_back(p.probability);
        for (char c : p.out) allProductions.push_back(c);
        totalStringLength += p.out.size();
    }
    uint totalBufferSize = count * (3 * sizeof(uint32_t) + sizeof(float)) + totalStringLength * sizeof(uint32_t);
    glBufferData(GL_SHADER_STORAGE_BUFFER, totalBufferSize, NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(uint32_t) * count, inputs.data());
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, count * sizeof(uint32_t), count * sizeof(uint32_t), offsets.data());
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 2 * count * sizeof(uint32_t), count * sizeof(uint32_t), lengths.data());
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 3 * count * sizeof(uint32_t), count * sizeof(float), probabilities.data());
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, count * (3 * sizeof(uint32_t) + sizeof(float)), totalStringLength * sizeof(uint32_t), allProductions.data());

    /*
    //printing
    vector<uint32_t> bufferdata;
    bufferdata.resize(totalBufferSize);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, totalBufferSize, bufferdata.data());
    cout << "productions buffer: \n";
    for (uint32_t c : bufferdata) {
        if (c < 1000) cout << c << ", ";
        else cout << *(float*)&c << ", ";
    } 
    cout << "\n";
    */
}

void Lsystem::iterateParallel(int n) {
    vector<uint32_t> test;
    for (char c : product) test.push_back(c);
    loadProductionsBuffer();
    glDeleteBuffers(1, &inputBuffer);
    glDeleteBuffers(1, &outputBuffer);
    glCreateBuffers(1, &inputBuffer);
    glCreateBuffers(1, &outputBuffer);
    
    GLuint stringAssignShader = loadComputeShader("shaders/compute/stringassign.glsl");
    GLuint sumShader = loadComputeShader("shaders/compute/scansum.glsl");
    GLuint productShader = loadComputeShader("shaders/compute/genproduct.glsl");
    glUseProgram(stringAssignShader);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, inputBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, test.size() * sizeof(uint32_t), test.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, inputBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 2 * test.size() * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, outputBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, productionsBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, productionsBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glDispatchCompute(test.size() / 32 + 1, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    //scansum
    GLuint tmp = outputBuffer;
    outputBuffer = inputBuffer;
    inputBuffer = tmp;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, inputBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, inputBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 2 * test.size() * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, outputBuffer);
    
    glUseProgram(sumShader);
    glDispatchCompute(test.size() / 1024 + 1, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    //genproduct
    tmp = outputBuffer;
    outputBuffer = inputBuffer;
    inputBuffer = tmp;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, inputBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, inputBuffer);
    uvec2 lastBufferEntry; 
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, (2 * test.size() - 2) * sizeof(uint32_t), 2 * sizeof(uint32_t), &lastBufferEntry);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputBuffer);
    //resize output buffer to (last offset + length of last string)
    glBufferData(GL_SHADER_STORAGE_BUFFER, (lastBufferEntry.y + rules[lastBufferEntry.x].out.size()) * sizeof(uint32_t), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, outputBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, productionsBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, productionsBuffer);
    glUseProgram(productShader);
    glDispatchCompute((2 * test.size()) / 32 + 1, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    //printing
    
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputBuffer);
    int outputSize;
    glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_SIZE, &outputSize);
    vector<uint32_t> testSum;
    testSum.resize(outputSize / sizeof(uint32_t));
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, outputSize, testSum.data());
    /*
    cout << "buffer data:\n";
    for (int i = 0; i < testSum.size(); i++) {
        cout << ", " << testSum[i];
    }
    */

    product = "";
    for (auto c : testSum) {
        product += c;
    }

}