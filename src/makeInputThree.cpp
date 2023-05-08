//********* for three-input functions *********
/* Make the input value and random noise for LUT processing */
#include "demo.hpp"

void out(vector<int64_t> ss){
  for(int i=0 ; i<ss.size() ; i++){
    cout << ss[i] << " ";
  }cout << endl;
}
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

  Encryptor encryptor(context, public_key);
  Evaluator evaluator(context);
  BatchEncoder batch_encoder(context);

  size_t slot_count = batch_encoder.slot_count();
  size_t row_size = slot_count/2;
  cout << "Plaintext matrix row size: " << row_size << endl;
  cout << "Slot nums = " << slot_count << endl;

  auto startP = chrono::high_resolution_clock::now();
  //make query
  cout<<"Get query!: ";
  int64_t LUT_query_1=1;
  int64_t LUT_query_2=2;
  int64_t LUT_query_3=3;
  int64_t LUT_num=0;

  //encrypt the LUT query
  cout << "Encrypt and save your query..." << flush;
  vector<int64_t> query1, query2, query3, num;
  for(int i=0 ; i<row_size ; i++){
    query1.push_back(LUT_query_1);
    query2.push_back(LUT_query_2);
    query3.push_back(LUT_query_3);
    num.push_back(LUT_num);
  }
  query1.resize(slot_count);
  query2.resize(slot_count);
  query3.resize(slot_count);
  num.resize(slot_count);

  auto startEva=chrono::high_resolution_clock::now();
  Plaintext plaintext_query1;
  batch_encoder.encode(query1, plaintext_query1);
  Plaintext plaintext_query2;
  batch_encoder.encode(query2, plaintext_query2);
  Plaintext plaintext_query3;
  batch_encoder.encode(query3, plaintext_query3);
  Plaintext plaintext_num;
  batch_encoder.encode(num, plaintext_num);
  /*
  Printing the matrix is a bit of a pain.
  */
  auto print_matrix = [row_size](auto &matrix)
  {
      cout << endl;
      size_t print_size = 5;

      cout << "    [";
      for (size_t i = 0; i < print_size; i++)
      {
          cout << setw(3) << matrix[i] << ",";
      }
      cout << setw(3) << " ...,";
      for (size_t i = row_size - print_size; i < row_size; i++)
      {
          cout << setw(3) << matrix[i] << ((i != row_size - 1) ? "," : " ]\n");
      }
      cout << "    [";
      for (size_t i = row_size; i < row_size + print_size; i++)
      {
        //cout<<i<<endl<<row_size<<endl<<print_size<<endl;
          cout << setw(3) << matrix[i] << ",";
      }
      cout << setw(3) << " ...,";
      for (size_t i = 2 * row_size - print_size; i < 2 * row_size; i++)
      {
          cout << setw(3) << matrix[i] << ((i != 2 * row_size - 1) ? "," : " ]\n");
      }
      cout << endl;
  };
  print_matrix(query1);
  print_matrix(query2);
  print_matrix(query3);
  cout << "Encrypting: ";
  Ciphertext ciphertext_query1;
  encryptor.encrypt(plaintext_query1, ciphertext_query1);
  Ciphertext ciphertext_query2;
  encryptor.encrypt(plaintext_query2, ciphertext_query2);
  Ciphertext ciphertext_query3;
  encryptor.encrypt(plaintext_query3, ciphertext_query3);
  Ciphertext ciphertext_num;
  encryptor.encrypt(plaintext_num, ciphertext_num);
  cout << "Done" << endl;
  auto endEva=chrono::high_resolution_clock::now();

  //save in a file
  cout << "Save in a file." << endl;
  ofstream queryFile1;
  queryFile1.open("Result/Input_1", ios::binary);
  ciphertext_query1.save(queryFile1);
  queryFile1.close();
  ofstream queryFile2;
  queryFile2.open("Result/Input_2", ios::binary);
  ciphertext_query2.save(queryFile2);
  queryFile2.close();
  ofstream queryFile3;
  queryFile3.open("Result/Input_3", ios::binary);
  ciphertext_query3.save(queryFile3);
  queryFile3.close();

  ofstream numFile;
  numFile.open("Result/RandomNum", ios::binary);
  ciphertext_num.save(numFile);
  numFile.close();

  cout << "End" << endl;
  auto endWhole=chrono::high_resolution_clock::now();
  chrono::duration<double> diffEva=endEva-startEva;
  chrono::duration<double> diffWhole = endWhole-startWhole;
  cout << "Processing time is: " << diffEva.count() << "s" << endl;
  cout << "Whole runtime is: " << diffWhole.count() << "s" << endl;
  show_memory_usage(getpid());
  return 0;
}
