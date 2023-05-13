//********* for single input functions *********
/* Decrypt and find the matched data point, generate PIR queries */
#include "demo.hpp"

void show_memory_usage(pid_t pid){
  ostringstream path;
  path << "/proc/" << pid << "/status";
  ostringstream cmd;
  cmd << "grep -e 'VmHWM' -e 'VmRSS' -e 'VmSize' -e 'VmStk' -e 'VmData' -e 'VmExe' " << path.str();
  system(cmd.str().c_str());
  return;
}

vector<int64_t> shift_work(const vector<int64_t> &query, const int64_t &index, const int64_t &num_slots){
  vector<int64_t> new_index;
  int64_t size = query.size();
  int64_t temp;

  for(int64_t i=0 ; i<num_slots ; i++){
    if((i+index) >= size) temp = query[i+index-size];
    else temp=query[i+index];
    new_index.push_back(temp);
  }
  return new_index;
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

  ifstream skFile("Key/SecretKey");
  SecretKey secret_key;
  secret_key.unsafe_load(context,skFile);
  skFile.close();

  ifstream pkFile("Key/PublicKey");
  PublicKey public_key;
  public_key.unsafe_load(context,pkFile);
  pkFile.close();

  Encryptor encryptor(context, public_key);
  Evaluator evaluator(context);
  BatchEncoder batch_encoder(context);
  Decryptor decryptor(context, secret_key);

  size_t slot_count = batch_encoder.slot_count();
  size_t row_size = slot_count/2;
  cout << "Plaintext matrix row size: " << row_size << endl;
  cout << "Slot nums = " << slot_count << endl;

  int64_t row_count = ceil((double)TABLE_SIZE/(double)row_size);

  vector<Ciphertext> ct_result1;
  vector<vector<int64_t> > dec_result1(row_count);
  vector<Plaintext> poly_dec_result1;
  for(int s=0 ; s<row_count ; s++){
    Plaintext ex;
    poly_dec_result1.push_back(ex);
  }
  cout << "Reading intermediate result..." << flush;
  ifstream mid_result1("Result/IntermediateRes", ios::binary);

  Ciphertext temp1;
  for(int w = 0; w < row_count ; w++) {
    temp1.load(context, mid_result1);
    ct_result1.push_back(temp1);
  }
  mid_result1.close();

  auto startEva = chrono::high_resolution_clock::now();

  omp_set_num_threads(NF);
  #pragma omp parallel for
  for (int z=0 ; z<row_count ; z++){
    decryptor.decrypt(ct_result1[z], poly_dec_result1[z]);
    batch_encoder.decode(poly_dec_result1[z], dec_result1[z]);

  }
  cout << "OK" << endl;
  cout << "Making PIR-query..." << endl;
  int64_t index_row_x,index_col_x;
  int64_t flag1=0;

  for(int i=0 ; i<row_count ; i++){
    for(int j=0 ; j<row_size ; j++){
      if(dec_result1[i][j] == 0 && flag1 == 0){ // find the matched data point. In this simulation, we using full-point LUT
        index_row_x = i;
        index_col_x = j;
        flag1 = 1;
        break;
      }
    }
  }

  if(flag1 == 0) cout<<"ERROR 1: NO FIND INPUT NUMBER!"<<endl;

  cout << "OK" << endl;
  cout<<"x[row,col]=["<<index_row_x<<", "<<index_col_x<<"]"<<endl;

  vector<int64_t> new_query0, new_query1;
  Ciphertext new_PIR_query0, new_PIR_query1;
  Plaintext plaintext_new_PIR_query0, plaintext_new_PIR_query1;

  int64_t n_query = sqrtN(TABLE_SIZE_OUT,row_size);
  cout<<"the number of query:"<<n_query<<endl;
  if(n_query==1){
    cout<<"When the number of query is 1"<<endl;
    int64_t index = (index_row_x * row_size + index_col_x);
    int64_t index0 = index % row_size;
    cout<<"index0="<<index0<<endl;
    for(int kk=0 ; kk<row_size ; kk++){
      if(kk==index0) new_query0.push_back(1);
      else new_query0.push_back(0);
    }
    new_query0.resize(slot_count);

    batch_encoder.encode(new_query0, plaintext_new_PIR_query0);
    encryptor.encrypt(plaintext_new_PIR_query0, new_PIR_query0);
  }

  if(n_query==2){
    cout<<"When the number of query is 2"<<endl;
    int64_t index = (index_row_x * row_size + index_col_x);
    int64_t index0 = index % row_size;
    int64_t index1 = (index/row_size) % row_size;
    for(int kk=0 ; kk<row_size ; kk++){
      if(kk==index0) new_query0.push_back(1);
      else new_query0.push_back(0);
    }
    new_query1 = shift_work(new_query0 ,index1, row_size);
    cout << "index0 is "<<index0 << endl;
    cout << "index1 is "<<index1 << endl;
    new_query0.resize(slot_count);
    new_query1.resize(slot_count);

    batch_encoder.encode(new_query0, plaintext_new_PIR_query0);
    encryptor.encrypt(plaintext_new_PIR_query0, new_PIR_query0);
    batch_encoder.encode(new_query1, plaintext_new_PIR_query1);
    encryptor.encrypt(plaintext_new_PIR_query1, new_PIR_query1);
  }
  auto endEva=chrono::high_resolution_clock::now();

  if(n_query==1){
    cout << "Saving query...";
    ofstream queryFile;
    queryFile.open("Result/queryPIR", ios::binary);
    new_PIR_query0.save(queryFile);
    queryFile.close();
    cout << "OK" << endl;
  }
  if(n_query==2){
    cout << "Saving query...";
    ofstream queryFile;
    queryFile.open("Result/queryPIR", ios::binary);
    new_PIR_query0.save(queryFile);
    new_PIR_query1.save(queryFile);
    queryFile.close();
    cout << "OK" << endl;
  }

  auto endWhole=chrono::high_resolution_clock::now();
  chrono::duration<double> diffWhole = endWhole-startWhole;
  chrono::duration<double> diffEva = endEva-startEva;
  cout << "Processing time is: " << diffEva.count() << "s" << endl;
  cout << "Whole runtime is: " << diffWhole.count() << "s" << endl;
  show_memory_usage(getpid());

  return 0;
}
