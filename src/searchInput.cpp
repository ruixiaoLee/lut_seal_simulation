//********* for single input functions *********
/* Search the matched data points in the input LUT and generate intermediate result */
#include "demo.hpp"

void show_memory_usage(pid_t pid){
  ostringstream path;
  path << "/proc/" << pid << "/status";
  ostringstream cmd;
  cmd << "grep -e 'VmHWM' -e 'VmRSS' -e 'VmSize' -e 'VmStk' -e 'VmData' -e 'VmExe' " << path.str();
  system(cmd.str().c_str());
  return;
}

int main(int argc, char *argv[]){
  auto startWhole=chrono::high_resolution_clock::now();
  cout << "Setting FHE..." << flush;
  ifstream parmsFile("Key/Params");
  EncryptionParameters parms(scheme_type::bfv);
  parms.load(parmsFile);
  SEALContext context(parms);
  parmsFile.close();

  ifstream pkFile("Key/PublicKey");
  PublicKey public_key;
  public_key.unsafe_load(context,pkFile);
  pkFile.close();

  ifstream galFile("Key/GaloisKey");
  GaloisKeys gal_keys;
  gal_keys.load(context,galFile);
  galFile.close();

  ifstream relinFile("Key/RelinKey");
  RelinKeys relin_keys;
  relin_keys.load(context,relinFile);
  relinFile.close();

  Encryptor encryptor(context, public_key);
  Evaluator evaluator(context);
  BatchEncoder batch_encoder(context);

  size_t slot_count = batch_encoder.slot_count();
  size_t row_size = slot_count/2;
  cout << "Plaintext matrix row size: " << row_size << endl;
  cout << "Slot nums = " << slot_count << endl;

  int64_t row_count = ceil((double)TABLE_SIZE/(double)row_size);

  //Read InputTable from file
  vector<Ciphertext> fun_tab;
  ifstream readtable;
  readtable.open("Table/InputTable_12bit");
  for(int w = 0; w < row_count ; w++) {
    Ciphertext temp;
    temp.load(context, readtable);
    fun_tab.push_back(temp);
  }

  vector<Ciphertext> Result;
  for(int i=0; i<row_count ; i++){
    Ciphertext tep;
    Result.push_back(tep);
  }

  //Read query from file
  cout << "Reading query" << flush;
  ifstream readQuery;
  // string s1(argv[1]);
  Ciphertext ciphertext_query;
  readQuery.open("Result/Input", ios::binary);
  ciphertext_query.load(context, readQuery);
  readQuery.close();
  cout << "End" << endl;

  auto startEva =  chrono::high_resolution_clock::now();
//thread work
omp_set_num_threads(NF);
#pragma omp parallel for
  for(int64_t i=0 ; i<row_count ; i++){
    Ciphertext res = ciphertext_query;
    evaluator.sub_inplace(res, fun_tab[i]);
    // evaluator.relinearize_inplace(res, relin_keys);
    Result[i]=res;
  }

  auto endEva=chrono::high_resolution_clock::now();

  cout << "Saving..." << flush;
  //save in a file
  ofstream outResult;
  outResult.open("Result/IntermediateRes", ios::binary);
  for(int i=0 ; i<row_count ; i++){
    Result[i].save(outResult);
  }
  outResult.close();
  cout << "OK" << endl;

  auto endWhole=chrono::high_resolution_clock::now();
  chrono::duration<double> diffEva = endEva-startEva;
  chrono::duration<double> diffWhole = endWhole-startWhole;
  cout << "Processing time is: " << diffEva.count() << "s" << endl;
  cout << "Whole runtime is: " << diffWhole.count() << "s" << endl;
  show_memory_usage(getpid());

  return 0;
}
