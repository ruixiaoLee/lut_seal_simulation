//********* for three-input functions *********
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
  //resetting FHE
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
  cout<<"Input table has "<<row_count<<" rows."<<endl;

  //Read InputTable from file
  vector<Ciphertext> fun_tab1;
  ifstream readtable1;
  readtable1.open("Table/three/InputTable_6bit"); /* Please change the LUT path you need */
  for(int w = 0; w < row_count ; w++) {
    Ciphertext temp1;
    temp1.load(context, readtable1);
    fun_tab1.push_back(temp1);
  }
  //Read InputTable from file
  vector<Ciphertext> fun_tab2;
  ifstream readtable2;
  readtable2.open("Table/three/InputTable_6bit"); /* Please change the LUT path you need */
  for(int w = 0; w < row_count ; w++) {
    Ciphertext temp2;
    temp2.load(context, readtable2);
    fun_tab2.push_back(temp2);
  }

  vector<Ciphertext> fun_tab3;
  ifstream readtable3;
  readtable3.open("Table/three/InputTable_6bit"); /* Please change the LUT path you need */
  for(int w = 0; w < row_count ; w++) {
    Ciphertext temp3;
    temp3.load(context, readtable3);
    fun_tab3.push_back(temp3);
  }

  //Make Threads to compute every row of table
  vector<Ciphertext> Result1, Result2, Result3;
  for(int i=0; i<row_count ; i++){
    Ciphertext tep;
    Result1.push_back(tep);
    Result2.push_back(tep);
    Result3.push_back(tep);
  }

  //Read query from file
  cout << "Reading query" << flush;
  ifstream readQuery1;
  // string s1(argv[1]);
  Ciphertext ciphertext_query1;
  readQuery1.open("Result/Input_1", ios::binary);
  ciphertext_query1.load(context, readQuery1);
  readQuery1.close();

  ifstream readQuery2;
  // string s1(argv[1]);
  Ciphertext ciphertext_query2;
  readQuery2.open("Result/Input_2", ios::binary);
  ciphertext_query2.load(context, readQuery2);
  readQuery2.close();

  ifstream readQuery3;
  // string s1(argv[1]);
  Ciphertext ciphertext_query3;
  readQuery3.open("Result/Input_3", ios::binary);
  ciphertext_query3.load(context, readQuery3);
  readQuery3.close();
  cout << "End" << endl;

//thread work
  auto startEva =  chrono::high_resolution_clock::now();
  // omp_set_num_threads(NF);
  // #pragma omp parallel for
  for(int64_t i=0 ; i<row_count ; i++){
    Ciphertext res1 = ciphertext_query1;
    Ciphertext res2 = ciphertext_query2;
    Ciphertext res3 = ciphertext_query3;
    evaluator.sub_inplace(res1, fun_tab1[i]);
    evaluator.sub_inplace(res2, fun_tab2[i]);
    evaluator.sub_inplace(res3, fun_tab3[i]);
    // evaluator.relinearize_inplace(res1, relin_keys);
    // evaluator.relinearize_inplace(res2, relin_keys);
    // evaluator.relinearize_inplace(res3, relin_keys);
    Result1[i]=res1;
    Result2[i]=res2;
    Result3[i]=res3;
  }
  auto endEva=chrono::high_resolution_clock::now();

  cout << "Saving..." << flush;
  //save in a file
  ofstream outResult1, outResult2, outResult3;
  outResult1.open("Result/IntermediateRes1", ios::binary);
  outResult2.open("Result/IntermediateRes2", ios::binary);
  outResult3.open("Result/IntermediateRes3", ios::binary);
  for(int i=0 ; i<row_count ; i++){
    Result1[i].save(outResult1);
    Result2[i].save(outResult2);
    Result3[i].save(outResult3);
  }
  outResult1.close();
  outResult2.close();
  outResult3.close();
  cout << "OK" << endl;

  auto endWhole=chrono::high_resolution_clock::now();
  chrono::duration<double> diffEva = endEva-startEva;
  chrono::duration<double> diffWhole = endWhole-startWhole;
  cout << "Processing time is: " << diffEva.count() << "s" << endl;
  cout << "Whole runtime is: " << diffWhole.count() << "s" << endl;
  show_memory_usage(getpid());

  return 0;
}
