#include "demo.hpp"
double relu(double x){
    if(x>0)
        return x;
    else
        return 0;
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
  EncryptionParameters parms(scheme_type::ckks);

  size_t poly_modulus_degree = 16384;
  cout << CoeffModulus::MaxBitCount(poly_modulus_degree);
  parms.set_poly_modulus_degree(poly_modulus_degree);
  parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, { 50, 30, 30, 30, 50 }));

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

  cout << "Evaluating polynomial 0.003017 * x^6 + 0.00003482 * x^5 - 0.05222 * x^4 - 0.0002848 * x^3 + 0.3845 * x^2 + 0.5004 * x + 0.1282 ..." << endl;
  double coeff6 = 0.003017;
  double coeff5 = 0.00003482;
  double coeff4 = -0.05222;
  double coeff3 = -0.0002848;
  double coeff2 = 0.3845;
  double coeff1 = 0.5004;
  double coeff0 = 0.1282;
  vector<double> v_coeff6, v_coeff5, v_coeff4, v_coeff3, v_coeff2, v_coeff1, v_coeff0;
  Plaintext plain_coeff6, plain_coeff5, plain_coeff4, plain_coeff3, plain_coeff2, plain_coeff1, plain_coeff0;
  Ciphertext ct_coeff6, ct_coeff5, ct_coeff4, ct_coeff3, ct_coeff2, ct_coeff1, ct_coeff0;

  cout << "Encode vectors." << endl;
  for(int64_t i=0 ; i<slot_count ; i++){
    v_coeff6.push_back(coeff6);
    v_coeff5.push_back(coeff5);
    v_coeff4.push_back(coeff4);
    v_coeff3.push_back(coeff3);
    v_coeff2.push_back(coeff2);
    v_coeff1.push_back(coeff1);
    v_coeff0.push_back(coeff0);
  }

  encoder.encode(v_coeff6, scale, plain_coeff6);
  encryptor.encrypt(plain_coeff6, ct_coeff6);
  encoder.encode(v_coeff5, scale, plain_coeff5);
  encryptor.encrypt(plain_coeff5, ct_coeff5);
  encoder.encode(v_coeff4, scale, plain_coeff4);
  encryptor.encrypt(plain_coeff4, ct_coeff4);
  encoder.encode(v_coeff3, scale, plain_coeff3);
  encryptor.encrypt(plain_coeff3, ct_coeff3);
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
  evaluator.square(x_encrypted, x2_encrypted); // compute square of x and store in x2=======================>  x2_encrypted => x^2
  evaluator.relinearize_inplace(x2_encrypted, relin_keys);
  cout << "    + Scale of x^2 before rescale: " << log2(x2_encrypted.scale()) << " bits" << endl;
  evaluator.rescale_to_next_inplace(x2_encrypted);
  cout << "    + Scale of x^2 after rescale: " << log2(x2_encrypted.scale()) << " bits" << endl;

  cout << "Compute 0.5*x." << endl;
  Ciphertext x1_encrypted = x_encrypted;
  evaluator.multiply_inplace(x1_encrypted, ct_coeff1); // compute x * 0.5 and store to x1===========>  x_encrypted => 0.5 * x
  Ciphertext x5_encrypted = x_encrypted;
  evaluator.multiply_inplace(x5_encrypted, ct_coeff5); // compute x * 0.5 and store to x5===========>  x5_encrypted => 0.5002 * x
  evaluator.relinearize_inplace(x5_encrypted, relin_keys);
  cout << "    + Scale of x5_encrypted before rescale: " << log2(x5_encrypted.scale()) << " bits" << endl;
  evaluator.rescale_to_next_inplace(x5_encrypted);
  cout << "    + Scale of x5_encrypted after rescale: " << log2(x5_encrypted.scale()) << " bits" << endl;
  Ciphertext x3_encrypted = x_encrypted;
  evaluator.multiply_inplace(x3_encrypted, ct_coeff3); // compute x * -0.0.0001641 and store to x1===========>  x3_encrypted => 0.0.0001641 * x
  evaluator.relinearize_inplace(x3_encrypted, relin_keys);
  cout << "    + Scale of x^2 before rescale: " << log2(x3_encrypted.scale()) << " bits" << endl;
  evaluator.rescale_to_next_inplace(x3_encrypted);
  cout << "    + Scale of x^2 after rescale: " << log2(x3_encrypted.scale()) << " bits" << endl;
  evaluator.multiply_inplace(x3_encrypted, x2_encrypted); // ==========================================> 3_encrypted => 0.0.0001641 * x^3

  cout << "Compute 0.3729 * x^2." << endl;
  Ciphertext x2_encrypted_coeff2 = x2_encrypted; // compute 0.3729 * x^2 =========================> x2_encrypted_coeff2 => 0.3729 * x^2
  evaluator.mod_switch_to_next_inplace(ct_coeff2);
  evaluator.multiply_inplace(x2_encrypted_coeff2, ct_coeff2);

  cout<<"Compute x^4."<<endl;
  Ciphertext x4_encrypted;
  evaluator.square(x2_encrypted, x4_encrypted);
  evaluator.relinearize_inplace(x4_encrypted, relin_keys); // compute x^4 =======================================>   x4_encrypted => x^4
  cout << "    + Scale of x^4 before rescale: " << log2(x4_encrypted.scale()) << " bits" << endl;
  evaluator.rescale_to_next_inplace(x4_encrypted);
  cout << "    + Scale of x^4 after rescale: " << log2(x4_encrypted.scale()) << " bits" << endl;

  cout << "Parameters used by all four terms are different." << endl; // =======================================> x5_encrypted => n * x * x^4
  cout << "    + Modulus chain index for x4_encrypted: "
       << context.get_context_data(x4_encrypted.parms_id())->chain_index() << endl; //
  cout << "    + Modulus chain index for x5_encrypted: "
       << context.get_context_data(x5_encrypted.parms_id())->chain_index() << endl; //
  evaluator.mod_switch_to_next_inplace(x5_encrypted);
  evaluator.multiply_inplace(x5_encrypted, x4_encrypted);

  Ciphertext x6_encrypted = x2_encrypted;
  evaluator.mod_switch_to_next_inplace(ct_coeff6);
  evaluator.multiply_inplace(x6_encrypted, ct_coeff6);
  evaluator.relinearize_inplace(x6_encrypted, relin_keys); // compute x^2 * coeff6 ===============================>   x6_encrypted => 0.016 * x^2
  cout << "    + Scale of 0.016 * x^2 before rescale: " << log2(x6_encrypted.scale()) << " bits" << endl;
  evaluator.rescale_to_next_inplace(x6_encrypted);
  cout << "    + Scale of 0.016 * x^2 after rescale: " << log2(x6_encrypted.scale()) << " bits" << endl;

  cout<<"Compute -0.041 * x^4."<<endl;
  Ciphertext x4_encrypted_coeff4 = x4_encrypted; // compute -0.041 * x^4 =========================> x4_encrypted_coeff4 => -0.041 * x^4
  evaluator.mod_switch_to_next_inplace(ct_coeff4);
  evaluator.mod_switch_to_next_inplace(ct_coeff4);
  evaluator.multiply_inplace(x4_encrypted_coeff4, ct_coeff4);

  cout<<"Compute 0.016 * x^2 * x^4."<<endl;
  Ciphertext x6_encrypted_coeff6 = x4_encrypted; // compute 0.016 * x^2 * x^4 =========================> x6_encrypted_coeff6 => 0.016 * x^2 * x^4
  evaluator.multiply_inplace(x6_encrypted_coeff6, x6_encrypted);

  cout << "Parameters used by all four terms are different." << endl;
  cout << "    + Modulus chain index for x6_encrypted_coeff6: "
       << context.get_context_data(x6_encrypted_coeff6.parms_id())->chain_index() << endl; //
  cout << "    + Modulus chain index for x5_encrypted: "
       << context.get_context_data(x5_encrypted.parms_id())->chain_index() << endl; //
  cout << "    + Modulus chain index for x4_encrypted_coeff4: "
       << context.get_context_data(x4_encrypted_coeff4.parms_id())->chain_index() << endl; // 0.5x
  cout << "    + Modulus chain index for x3_encrypted: "
       << context.get_context_data(x3_encrypted.parms_id())->chain_index() << endl; // 0.119782
  cout << "    + Modulus chain index for x2_encrypted_coeff2: "
       << context.get_context_data(x2_encrypted_coeff2.parms_id())->chain_index() << endl; // 0.119782
  cout << "    + Modulus chain index for x1_encrypted: "
       << context.get_context_data(x1_encrypted.parms_id())->chain_index() << endl; // 0.119782

  cout << "The exact scales of all four terms are different:" << endl;
  ios old_fmt1(nullptr);
  old_fmt1.copyfmt(cout);
  cout << fixed << setprecision(10);
  cout << "    + Exact scale in 0.016 * x^6 : " << x6_encrypted_coeff6.scale() << endl;
  cout << "    + Exact scale in -0.041 * x^5: " << x5_encrypted.scale() << endl;
  cout << "    + Exact scale in -0.041 * x^3: " << x3_encrypted.scale() << endl;
  cout << "    + Exact scale in -0.041 * x^4: " << x4_encrypted_coeff4.scale() << endl;
  cout << "    + Exact scale in 0.3729 * x^2: " << x2_encrypted_coeff2.scale() << endl;
  cout << "    + Exact scale in 0.5 * x: " << x1_encrypted.scale() << endl;
  cout.copyfmt(old_fmt1);

  cout << "Normalize scales to 2^30." << endl;
  x6_encrypted_coeff6.scale() = pow(2.0, 30);
  x5_encrypted.scale() = pow(2.0, 30);
  x4_encrypted_coeff4.scale() = pow(2.0, 30);
  x3_encrypted.scale() = pow(2.0, 30);
  x2_encrypted_coeff2.scale() = pow(2.0, 30);
  x1_encrypted.scale() = pow(2.0, 30);

  cout << "Normalize encryption parameters to the lowest level." << endl;
  evaluator.mod_switch_to_next_inplace(x3_encrypted);
  evaluator.mod_switch_to_next_inplace(x2_encrypted_coeff2);
  evaluator.mod_switch_to_next_inplace(x1_encrypted);
  evaluator.mod_switch_to_next_inplace(x1_encrypted);

  cout << "Compute 0.0016 * x^6 - 0.041 * x^4 + 0.3729 * x^2 + 0.5 * x." << endl;
  Ciphertext encrypted_result=x6_encrypted_coeff6;
  evaluator.add_inplace(encrypted_result, x5_encrypted);
  evaluator.add_inplace(encrypted_result, x4_encrypted_coeff4);
  evaluator.add_inplace(encrypted_result, x3_encrypted);
  evaluator.add_inplace(encrypted_result, x2_encrypted_coeff2);
  evaluator.add_inplace(encrypted_result, x1_encrypted);
  evaluator.relinearize_inplace(encrypted_result, relin_keys); // ===========================>  encrypted_result => 0.0016 * x^6 - 0.041 * x^4 + 0.3729 * x^2 + 0.5 * x
  cout << "    + Scale of 0.0016 * x^6 - 0.041 * x^4 + 0.3729 * x^2 + 0.5 * x before rescale: " << log2(encrypted_result.scale()) << " bits" << endl;
  evaluator.rescale_to_next_inplace(encrypted_result);
  cout << "    + Scale of 0.0016 * x^6 - 0.041 * x^4 + 0.3729 * x^2 + 0.5 * x after rescale: " << log2(encrypted_result.scale()) << " bits" << endl;

  cout << "Parameters used by all two terms are different." << endl;
  cout << "    + Modulus chain index for encrypted_result: "
       << context.get_context_data(encrypted_result.parms_id())->chain_index() << endl; //
  cout << "    + Modulus chain index for ct_coeff0: "
       << context.get_context_data(ct_coeff0.parms_id())->chain_index() << endl; //

  cout << "The exact scales of all four terms are different:" << endl;
  ios old_fmt2(nullptr);
  old_fmt2.copyfmt(cout);
  cout << fixed << setprecision(10);
  cout << "    + Exact scale in encrypted_result: " << encrypted_result.scale() << endl;
  cout << "    + Exact scale in 0.1249: " << ct_coeff0.scale() << endl;
  cout.copyfmt(old_fmt2);

  cout << "Normalize scales to 2^30." << endl;
  encrypted_result.scale() = pow(2.0, 30);

  cout << "Normalize encryption parameters to the lowest level." << endl;
  parms_id_type last_parms_id = encrypted_result.parms_id();
  evaluator.mod_switch_to_inplace(ct_coeff0, last_parms_id);

  evaluator.add_inplace(encrypted_result, ct_coeff0);

  /* Evaluation Finish*/
  cout << "Eva finish." << endl;
  auto endeva=chrono::high_resolution_clock::now();
  /* Check the result */
  vector<double> true_result, poly_result;
  for(int i=0; i<slot_count ; i++){
    double temp = relu(input[i]); // function
    double temp1 = input[i]*input[i]; //0.003017 * x^6 + 0.00003482 * x^5 - 0.05222 * x^4 - 0.0002848 * x^3 + 0.3845 * x^2 + 0.5004 * x + 0.1282
    double temp2 = 0.003017 * pow(input[i],6) + 0.00003482 * pow(input[i],5) - 0.05222 * pow(input[i],4) - 0.0002848 * pow(input[i],3) + 0.3845 * pow(input[i],2) + 0.5004 *input[i] + 0.1282;
    true_result.push_back(temp);
    poly_result.push_back(temp2);
  }
  cout<<"Real result >>"<<endl;
  print_vector(true_result, 3, 7);
  cout<< "Poly result >>" << endl;
  ErrorCom(true_result, poly_result);

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
  // cout << "Whole runtime is: " << diffWhole.count() << "s" << endl;

  return 0;
}
