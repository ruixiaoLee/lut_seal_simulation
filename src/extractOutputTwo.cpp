//********* for two-input functions *********
/* Construct PIR queries and extract output */
#include "demo.hpp"

void show_memory_usage(pid_t pid){
  ostringstream path;
  path << "/proc/" << pid << "/status";
  ostringstream cmd;
  cmd << "grep -e 'VmHWM' -e 'VmRSS' -e 'VmSize' -e 'VmStk' -e 'VmData' -e 'VmExe' " << path.str();
  system(cmd.str().c_str());
  return;
}

int64_t sqrtN(int64_t a, int64_t b){
  int64_t n=0;
  while(pow(b,n)<a){
    n++;
  }
  return n;
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

  int64_t row_count = ceil((double)TABLE_SIZE_OUT/(double)row_size);
  cout<<"# of row:"<<row_count<<endl;

  int64_t n_query = sqrtN(TABLE_SIZE_OUT, row_size);
  cout<<"# of query:"<<n_query<<endl;

  //Read OutputTable from file
  vector<Ciphertext> fun_tab_out;
  ifstream readtable;
  readtable.open("Table/two/OutputTable_12bit");
  for(int w = 0; w < row_count ; w++) {
    Ciphertext tep;
    tep.load(context, readtable);
    fun_tab_out.push_back(tep);
  }

  vector<Ciphertext> query_rec;
  for(int i=0; i<row_count ; i++){
    Ciphertext temp_rec;
    query_rec.push_back(temp_rec);
  }

  cout << "Reading query from DS..." << flush;
  fstream PIRqueryFile("Result/queryPIR", fstream::in);
  Ciphertext new_query0, new_query1;
  new_query0.load(context, PIRqueryFile);
  if(n_query==2)
    new_query1.load(context, PIRqueryFile);
  PIRqueryFile.close();
  cout<<"query ok"<<endl;

  ifstream readNum;
  Ciphertext ciphertext_num;
  readNum.open("Result/RandomNum", ios::binary);
  ciphertext_num.load(context, readNum);
  readNum.close();

  auto startEva = chrono::high_resolution_clock::now();
  if(n_query==1){
  omp_set_num_threads(NF);
  #pragma omp parallel for
    for(int64_t i=0 ; i<row_count ; i++){
      // cout<<"No."<<i<<endl;
      Ciphertext temp = new_query0;
      evaluator.multiply_inplace(temp, fun_tab_out[i]);
      evaluator.relinearize_inplace(temp, relin_keys);
      query_rec[i] = temp;
    }
  }
  if(n_query==2){
  omp_set_num_threads(NF);
  #pragma omp parallel for
    for(int64_t i=0 ; i<row_count ; i++){
      // cout<<"No."<<i<<endl;
      Ciphertext temp = new_query1;
      evaluator.rotate_rows_inplace(temp, -i, gal_keys);
      evaluator.multiply_inplace(temp, new_query0);
      evaluator.relinearize_inplace(temp, relin_keys);

      evaluator.multiply_inplace(temp, fun_tab_out[i]);
      evaluator.relinearize_inplace(temp, relin_keys);
      query_rec[i] = temp;
    }
  }

  cout<<"result size:"<<query_rec.size()<<endl;

  Ciphertext sum_result;
  sum_result=query_rec[0];
  if(query_rec.size()>0){
    for(int i=1 ; i<row_count ; i++){
      evaluator.add_inplace(sum_result, query_rec[i]);
    }
  }

  // totalSum
  for(int64_t i=0 ; i<log2(row_size) ; i++){
       Ciphertext ct = sum_result;
       evaluator.rotate_rows_inplace(ct, -pow(2,i), gal_keys);
       // evaluator.relinearize_inplace(ct, relin_keys);
       evaluator.add_inplace(sum_result, ct);
  }

  evaluator.add_inplace(sum_result, ciphertext_num);

  auto endEva=chrono::high_resolution_clock::now();

  //write Final_result in a file
  cout << "Saving final result..." << flush;
  ofstream Final_result;
  Final_result.open("Result/OutputResult", ios::binary);
  //Final_result << fin_res ;
  sum_result.save(Final_result);
  Final_result.close();
  cout << "OK" << endl;

  auto endWhole=chrono::high_resolution_clock::now();
  chrono::duration<double> diffEva = endEva-startEva;
  chrono::duration<double> diffWhole = endWhole-startWhole;
  cout << "Processing time is: " << diffEva.count() << "s" << endl;
  cout << "Whole runtime is: " << diffWhole.count() << "s" << endl;
  show_memory_usage(getpid());

  return 0;
}
