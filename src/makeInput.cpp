//********* for single input functions *********
/* Make the input value and random noise for LUT processing */
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

  Encryptor encryptor(context, public_key);
  Evaluator evaluator(context);
  BatchEncoder batch_encoder(context);

  size_t slot_count = batch_encoder.slot_count();
  size_t row_size = slot_count/2;
  cout << "Plaintext matrix row size: " << row_size << endl;
  cout << "Slot nums = " << slot_count << endl;

  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::mt19937 generator(seed);
  //make query

  cout<<"Get query: "; // make a input for simulate
  int64_t LUT_query=2;
  //encrypt the LUT query
  cout << "Encrypt and save your query..." << flush;
  vector<int64_t> query,num;
  for(int i=0 ; i<row_size ; i++){
    query.push_back(LUT_query);
    int64_t LUT_num=(generator()%100+1); // generate random noise for simulate
    num.push_back(LUT_num);
  }
  query.resize(slot_count);
  num.resize(slot_count);

  auto startEva=chrono::high_resolution_clock::now();

  /* encrypt the input value and random noise */
  Plaintext plaintext_query;
  batch_encoder.encode(query, plaintext_query);
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
  print_matrix(query);
  print_matrix(num);

  Ciphertext ciphertext_query;
  cout << "Encrypting: ";
  encryptor.encrypt(plaintext_query, ciphertext_query);
  Ciphertext ciphertext_num;
  encryptor.encrypt(plaintext_num, ciphertext_num);
  cout << "Done" << endl;
  auto endEva=chrono::high_resolution_clock::now();

  //save in a file
  cout << "Save in a file." << endl;
  ofstream queryFile;
  queryFile.open("Result/Input", ios::binary);
  ciphertext_query.save(queryFile);
  queryFile.close();

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
