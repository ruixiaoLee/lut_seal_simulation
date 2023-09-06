#include "binfhecontext.h"
// #include "gen-cryptocontext.h"
#include <chrono>
#include "omp.h"

using namespace lbcrypto;
using namespace std;

// use multithreading
vector<LWECiphertext> bit_lut_parallel(vector<LWECiphertext> ctQuery, vector<LWECiphertext> ctRes, LWECiphertext ctZero, LWECiphertext ctOne, vector<vector<LWECiphertext> > ctTableInput,
                              vector<vector<LWECiphertext> > ctTableOutput, const BinFHEContext cc, size_t binarySize, size_t num){
    vector<vector<LWECiphertext> > interRes=ctTableOutput;
    size_t inBinary = binarySize * num;
    size_t w = pow(2,inBinary);
    #pragma omp parallel for
    for(size_t i=0 ; i<w ; i++){
      cout<<"i:"<<i<<endl;
      auto tempBit = ctOne;
      for(size_t j=0 ; j<inBinary ; j++){
        auto temp = cc.EvalBinGate(XOR, ctQuery[j], ctTableInput[i][j]);
        temp = cc.EvalBinGate(XOR, temp, ctOne);
        tempBit = cc.EvalBinGate(AND, tempBit, temp);
      }
      vector<LWECiphertext> interResRow(binarySize);
      for(size_t k=0 ; k<binarySize ; k++){
        auto temp2 = cc.EvalBinGate(AND, tempBit, ctTableOutput[i][k]);
        interResRow[k] = temp2;
      }
      interRes[i] = interResRow;
    }
    cout<<"size:"<<interRes.size()<<", "<<interRes[0].size()<<endl;
    size_t zz=interRes.size();
    for(size_t i=0 ; i<binarySize ; i++){
      for(size_t j=0 ; j<zz ; j++)
        ctRes[i] = cc.EvalBinGate(XOR, ctRes[i], interRes[j][i]);
    }

    return ctRes;
}

// use one thread
vector<LWECiphertext> bit_lut(vector<LWECiphertext> ctQuery, vector<LWECiphertext> ctRes, LWECiphertext ctZero, LWECiphertext ctOne, vector<vector<LWECiphertext> > ctTableInput,
                              vector<vector<LWECiphertext> > ctTableOutput, const BinFHEContext cc, size_t binarySize, size_t num){
    size_t inBinary = binarySize * num;
    size_t w = pow(2,inBinary);
    for(size_t i=0 ; i<w ; i++){
      cout<<"i:"<<i<<endl;
      auto tempBit = ctOne;
      for(size_t j=0 ; j<inBinary ; j++){
        auto temp = cc.EvalBinGate(XOR, ctQuery[j], ctTableInput[i][j]);
        temp = cc.EvalBinGate(XOR, temp, ctOne);
        tempBit = cc.EvalBinGate(AND, tempBit, temp);
      }
      for(size_t k=0 ; k<binarySize ; k++){
        auto temp2 = cc.EvalBinGate(AND, tempBit, ctTableOutput[i][k]);
        ctRes[k] = cc.EvalBinGate(XOR, ctRes[k], temp2);
      }
    }
    return ctRes;
}

void printVector(vector<int64_t> v){
  cout<<"Query vector: ";
  for(size_t i=0 ; i<v.size() ; i++){
    cout<<v[i]<<" ";
  }
  cout<<endl;
}
vector<int64_t> switchToBinary(int64_t input, size_t binarySize){
  vector<int64_t> query;
  while(input>1){
    auto temp = input % 2;
    input = input / 2;
    query.push_back(temp);
  }
  query.push_back(input);
  if(query.size()<binarySize){
    query.resize(binarySize);
    reverse(query.begin(), query.end());
  }else{
    reverse(query.begin(), query.end());
    query.resize(binarySize);
  }
  return query;
}

int64_t func(int64_t x){
  return x+1;
}

int main() {
    auto cc = BinFHEContext();
    cc.GenerateBinFHEContext(STD128);
    auto sk = cc.KeyGen();
    std::cout << "Generating the bootstrapping keys..." << std::endl;
    cc.BTKeyGen(sk);
    std::cout << "Completed the key generation." << std::endl;

    size_t binarySize; //3bit:0~7
    size_t num;

    cout<<"Give 1. the bit size, 2. the number of inputs: ";
    cin>>binarySize>>num;
    cout<<"Give input"<<endl;
    vector<int64_t> input;
    for(size_t i=0 ; i<num ; i++){
      int64_t t_input;
      cout<<"No."<<i<<": ";
      cin >>t_input;
      vector<int64_t> binInput = switchToBinary(t_input, binarySize);
      input.insert(input.end(),binInput.begin(), binInput.end());
    }
    printVector(input);
    size_t comBinarysize = binarySize * num;
    // Encrypt the query
    std::vector<LWECiphertext> ctQuery;
    for(size_t i=0 ; i<comBinarysize; i++){
      auto temp = cc.Encrypt(sk, input[i]);
      ctQuery.push_back(temp);
    }
    // Make LUT
    std::vector<vector<LWECiphertext> > ctTableInput, ctTableOutput;
    for(size_t i=0 ; i<pow(2,comBinarysize); i++){
      int64_t output = func(i);
      vector<int64_t> rowIn = switchToBinary(i,comBinarysize);
      vector<int64_t> rowOut = switchToBinary(output,binarySize);
      cout<<"input:";
      printVector(rowIn);
      cout<<", output:";
      printVector(rowOut);

      vector<LWECiphertext> ctrowIn, ctrowOut;
      for(size_t i=0 ; i<comBinarysize; i++){
        auto tempI = cc.Encrypt(sk, rowIn[i]);
        ctrowIn.push_back(tempI);
      }
      ctTableInput.push_back(ctrowIn);

      for(size_t i=0 ; i<binarySize; i++){
        auto tempO = cc.Encrypt(sk, rowOut[i]);
        ctrowOut.push_back(tempO);
      }
      ctTableOutput.push_back(ctrowOut);
    }
    // LUT processing
    std::vector<LWECiphertext> ctRes;
    for(size_t i=0 ; i<binarySize ; i++){
      auto temp = cc.Encrypt(sk, 0);
      ctRes.push_back(temp);
    }
    auto ctZero = cc.Encrypt(sk, 0);
    auto ctOne = cc.Encrypt(sk, 1);

    cout<<"LUT processing..."<<flush;
    auto startWhole=chrono::high_resolution_clock::now();
    // ctRes = bit_lut(ctQuery, ctRes, ctZero, ctOne, ctTableInput, ctTableOutput, cc, binarySize, num);
    ctRes = bit_lut_parallel(ctQuery, ctRes, ctZero, ctOne, ctTableInput, ctTableOutput, cc, binarySize, num);
    auto endWhole=chrono::high_resolution_clock::now();
    cout<<"Finish"<<flush;
    chrono::duration<double> diffWhole = endWhole-startWhole;
    cout << "Whole runtime is: " << diffWhole.count() << "s" << endl;
    // Decryption
    // std::vector<LWEPlaintext> ptResult;
    std::cout << "Result: ";
    for(size_t i=0 ; i<binarySize; i++){
      LWEPlaintext result;
      cc.Decrypt(sk, ctRes[i], &result);
      std::cout << result <<" ";
      // ptResult.push_back(result);
    }cout<<endl;

    return 0;
}
