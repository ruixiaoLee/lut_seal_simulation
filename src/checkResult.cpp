//202204@Richelle
//୧(๑•̀⌄•́๑)૭✧
#include "demo.hpp"
void show_memory_usage(pid_t pid){
  ostringstream path;
  path << "/proc/" << pid << "/status";
  ostringstream cmd;
  cmd << "grep -e 'VmHWM' -e 'VmRSS' -e 'VmSize' -e 'VmStk' -e 'VmData' -e 'VmExe' " << path.str();
  system(cmd.str().c_str());
  return;
}
void output_plaintext(const vector<vector <int64_t>> &a){
  int64_t row=a.size();
  int64_t col=a[0].size();

    for(int i=0 ; i<row ; i++){
        for(int j=0 ; j<col ; j++)
            cout<<a[i][j]<<" ";
        cout<<endl;
	cout<<i<<endl;
    }
}

void out_vector(const vector<int64_t> &a){
  for(int j=0 ; j<a.size() ; j++){
    cout<<a[j]<<" ";
  }
  cout<<endl;
}

int main(int argc, char *argv[]){
  auto startWhole=chrono::high_resolution_clock::now();
  //resetting FHE
  cout << "Setting FHE..." << flush;
  ifstream parmsFile("Key/Params");
  EncryptionParameters parms(scheme_type::bfv);
  parms.load(parmsFile);
  SEALContext context(parms);
  parmsFile.close();

  ifstream skFile("Key/SecretKey");
  SecretKey secret_key;
  secret_key.unsafe_load(context,skFile);
  skFile.close();

  // Encryptor encryptor(context, public_key);
  Evaluator evaluator(context);
  BatchEncoder batch_encoder(context);
  Decryptor decryptor(context, secret_key);

  size_t slot_count = batch_encoder.slot_count();
  size_t row_size = slot_count/2;
  cout << "Plaintext matrix row size: " << row_size << endl;
  cout << "Slot nums = " << slot_count << endl;

  // int64_t row_count = ceil(TABLE_SIZE/row_size);
/////////////////////////////////////////////////////////

  // string s1(argv[1]);
  ifstream result;
  Ciphertext ciphertext_query0,ciphertext_query1,ciphertext_query2;
  Plaintext plaintext_query0,plaintext_query1,plaintext_query2;
  // result.open(s1, ios::binary);
  result.open("Result/OutputResult", ios::binary);
  ciphertext_query0.load(context, result);
  // ciphertext_query1.load(context, result);
  // ciphertext_query2.load(context, result);
  result.close();
  cout << "End" << endl;

  auto startP = chrono::high_resolution_clock::now();
  cout << "The result from CS"<< endl;
  decryptor.decrypt(ciphertext_query0, plaintext_query0);

  vector<int64_t> plaintext_result0,plaintext_result1,plaintext_result2;
  batch_encoder.decode(plaintext_query0, plaintext_result0);
  auto endP=chrono::high_resolution_clock::now();

  cout<<"Noise budget in result:"
      << decryptor.invariant_noise_budget(ciphertext_query0)<<" bits"<<endl;
  cout<<"result is: "<<plaintext_result0[0]<<endl;
   // out_vector(plaintext_result0);
  auto endWhole=chrono::high_resolution_clock::now();
  chrono::duration<double> diffP = endWhole-startP;
  chrono::duration<double> diffWhole = endP-startWhole;
  cout << "Processing time is: " << diffP.count() << "s" << endl;
  cout << "Whole runtime is: " << diffWhole.count() << "s" << endl;
  show_memory_usage(getpid());

  return 0;
}
