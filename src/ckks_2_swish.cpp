#include "demo.hpp"
// range -3 ~ 3

double swish(double x){
    double z = exp(-x);
    double swi = 1 / (1 + z);
    return (swi*x);
}

vector<double> readTable(const string &filename){
  vector<double> ori_table;
  string lineStr;
  ifstream readData(filename);

  cout<<"read file:"<<filename<<endl;
  while(getline(readData, lineStr)){
    // cout<<lineStr<<" ";
    double temp_num = stod(lineStr);
    // cout<<temp_num<<" ";
    ori_table.push_back(temp_num);
  }
  cout<<endl;
  // cout<<"the table size (# of numbers) is "<<ori_table.size()<<endl;
  return ori_table;
}

void ErrorCom(vector<double> a, vector<double> b){
  double sum_error=0.0;
  double sum_error2=0.0;
  for(int64_t i=0 ; i<a.size() ; i++){
    double temp = abs((a[i]-b[i])/a[i]);
    double temp2 = abs(a[i]-b[i]);
    sum_error += temp;
    sum_error2 += temp2;
  }
  cout<<"number of slot:"<<a.size()<<endl;
  double avg_error = sum_error / double(a.size());
  double abs_error = sum_error2 / double(a.size());

  cout<<"Avg abs Error:"<<abs_error<<endl;
  cout<<"Avg Error Percentage(ct):"<<(avg_error*100.0)<<endl;
}

int main(int argc, char const *argv[]) {
  auto startWhole=chrono::high_resolution_clock::now();

  EncryptionParameters parms(scheme_type::ckks);

  size_t poly_modulus_degree = 16384;
  parms.set_poly_modulus_degree(poly_modulus_degree);
  parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, { 50, 30, 30, 50 }));

  double scale = pow(2.0, 30);

  SEALContext context(parms);
  print_parameters(context);
  cout << endl;

  KeyGenerator keygen(context);
  auto secret_key = keygen.secret_key();
  PublicKey public_key;
  keygen.create_public_key(public_key);
  RelinKeys relin_keys;
  keygen.create_relin_keys(relin_keys);
  GaloisKeys gal_keys;
  keygen.create_galois_keys(gal_keys);

  Encryptor encryptor(context, public_key);
  Evaluator evaluator(context);
  Decryptor decryptor(context, secret_key);

  CKKSEncoder encoder(context);
  size_t slot_count = encoder.slot_count();
  cout << "Number of slots: " << slot_count << endl;
  cout << "Reading query" << flush;
  vector<double> input, tempv;
  /* read the test data */
  input=readTable("randomInput/test_normal.txt");
  input.resize(slot_count);

  cout << "Evaluating polynomial 0.1576 * x^2 + 0.5002 * x + 0.06811 ..." << endl;
  double coeff2 = 0.1576;
  double coeff1 = 0.5002;
  double coeff0 = 0.06811;
  vector<double> v_coeff2, v_coeff1, v_coeff0;
  Plaintext plain_coeff2, plain_coeff1, plain_coeff0;
  Ciphertext ct_coeff2, ct_coeff1, ct_coeff0;

  cout << "Encode vectors." << endl;
  for(int64_t i=0 ; i<slot_count ; i++){
    v_coeff2.push_back(coeff2);
    v_coeff1.push_back(coeff1);
    v_coeff0.push_back(coeff0);
  }

  encoder.encode(v_coeff2, scale, plain_coeff2);
  encryptor.encrypt(plain_coeff2, ct_coeff2);
  encoder.encode(v_coeff1, scale, plain_coeff1);
  encryptor.encrypt(plain_coeff1, ct_coeff1);
  encoder.encode(v_coeff0, scale, plain_coeff0);
  encryptor.encrypt(plain_coeff0, ct_coeff0);

  Plaintext x_plain;
  encoder.encode(input, scale, x_plain);
  Ciphertext x_encrypted;
  encryptor.encrypt(x_plain, x_encrypted);

  /* start to eva here */
  auto starteva=chrono::high_resolution_clock::now();

  Ciphertext x2_encrypted;
  cout << "Compute x^2 and relinearize:" << endl;
  evaluator.square(x_encrypted, x2_encrypted);
  evaluator.relinearize_inplace(x2_encrypted, relin_keys);
  cout << "    + Scale of x^2 before rescale: " << log2(x2_encrypted.scale()) << " bits" << endl;
  cout << "Rescale x^2." << endl;
  evaluator.rescale_to_next_inplace(x2_encrypted);
  cout << "    + Scale of x^2 after rescale: " << log2(x2_encrypted.scale()) << " bits" << endl;

  cout << "Compute 0.5002 *x." << endl;// 0.1576 * x^2 + 0.5002 * x + 0.06811
  evaluator.multiply_inplace(x_encrypted, ct_coeff1);
  cout << "Compute 0.1576 * x^2." << endl;
  Ciphertext x2_encrypted_coeff2 = x2_encrypted;
  evaluator.mod_switch_to_next_inplace(ct_coeff2);
  evaluator.multiply_inplace(x2_encrypted_coeff2, ct_coeff2);

  cout << "    + Modulus chain index for x2_encrypted_coeff2: " // check the modulus
        << context.get_context_data(x2_encrypted_coeff2.parms_id())->chain_index() << endl;
  cout << "    + Modulus chain index for x_encrypted: "
        << context.get_context_data(x_encrypted.parms_id())->chain_index() << endl;

  cout << "Normalize encryption parameters to the lowest level." << endl; // switch the modulus to same
  evaluator.mod_switch_to_next_inplace(x_encrypted);

  cout << "The exact scales of all three terms are different:" << endl;
  ios old_fmt(nullptr);
  old_fmt.copyfmt(cout);
  cout << fixed << setprecision(10);
  cout << "    + Exact scale in 0.1576 * x^2: " << x2_encrypted_coeff2.scale() << endl;
  cout << "    + Exact scale in   0.5002 * x: " << x_encrypted.scale() << endl;
  cout.copyfmt(old_fmt);

  cout << "Normalize scales to 2^30." << endl;
  x2_encrypted_coeff2.scale() = pow(2.0, 30);
  x_encrypted.scale() = pow(2.0, 30);

  cout << "Compute 0.1576 * x^2 + 0.5002 * x " << endl;
  Ciphertext encrypted_result;
  evaluator.add(x2_encrypted_coeff2, x_encrypted, encrypted_result);
  evaluator.relinearize_inplace(encrypted_result, relin_keys);
  cout << "    + Scale of 0.1576 * x^2 + 0.5002 * x before rescale: " << log2(encrypted_result.scale()) << " bits" << endl;
  evaluator.rescale_to_next_inplace(encrypted_result);
  cout << "    + Scale of 0.1576 * x^2 + 0.5002 * x rescale: " << log2(encrypted_result.scale()) << " bits" << endl;

  cout << "    + Modulus chain index for encrypted_result: " // check the modulus
        << context.get_context_data(encrypted_result.parms_id())->chain_index() << endl;
  cout << "    + Modulus chain index for ct_coeff0: "
        << context.get_context_data(ct_coeff0.parms_id())->chain_index() << endl;

  cout << "Normalize encryption parameters to the lowest level." << endl; // switch the modulus to same
  evaluator.mod_switch_to_next_inplace(ct_coeff0);
  evaluator.mod_switch_to_next_inplace(ct_coeff0);

  cout << "The exact scales of all three terms are different:" << endl;
  ios old_fmt2(nullptr);
  old_fmt2.copyfmt(cout);
  cout << fixed << setprecision(10);
  cout << "    + Exact scale in 0.1576 * x^2 + 0.5002 * x : " << encrypted_result.scale() << endl;
  cout << "    + Exact scale in 0.06811 : " << ct_coeff0.scale() << endl;
  cout.copyfmt(old_fmt2);

  cout << "Normalize scales to 2^30." << endl;
  encrypted_result.scale() = pow(2.0, 30);
  evaluator.add_inplace(encrypted_result, ct_coeff0);

  /* Evaluation Finish*/
  cout << "Eva finish." << endl;
  auto endeva=chrono::high_resolution_clock::now();
  /* Check the result */
  vector<double> true_result, poly_result;
  for(int i=0; i<slot_count ; i++){
    double temp = swish(input[i]); // function
    true_result.push_back(temp);
  }
  cout<<"Real result >>"<<endl;
  print_vector(true_result, 3, 7);

  Plaintext plain_result;
  cout << "Decrypt and decode 0.344125*x^2 + 0.5x + 0.0851505" << endl;
  decryptor.decrypt(encrypted_result, plain_result);
  vector<double> result;
  encoder.decode(plain_result, result);
  cout<<"Result by poly approximation >>"<<endl;
  print_vector(result, 3, 7);

  cout<<"Avg Error (ct)"<<endl;
  ErrorCom(true_result, result);

  // cout << "Save in a file." << endl;
  // ofstream inputValue;
  // inputValue.open("Result/result_ckks", ios::binary);
  // encrypted_result.save(inputValue);
  // inputValue.close();

  // chrono::duration<double> diffWhole = endWhole-startWhole;
  chrono::duration<double> diffeva = endeva-starteva;
  cout << "Eva function time is: " << diffeva.count() << "s" << endl;

  return 0;
}
