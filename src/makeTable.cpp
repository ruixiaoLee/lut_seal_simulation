#include "demo.hpp"
//for 16-bit input

unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
std::mt19937 generator(seed);

void out_vector(const vector<int64_t> &a){
  for(int j=0 ; j<a.size() ; j++){
    cout<<a[j]<<" ";
  }
  cout<<endl;
}

int main(int argc, char const *argv[]) {
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
  size_t row_size = slot_count/2; //row_size = slot_count
  cout << "Plaintext matrix row size: " << row_size << endl;
  cout << "Slot nums = " << slot_count << endl;
  cout << "===========================" << endl;

//   /*18-bit integer table*/
//     vector<int64_t> inputTable18, outputTable18;
//     for(int64_t i = -131072 ; i < 131072 ; i++){ //262,144
//         inputTable18.push_back(i);
//         outputTable18.push_back(i);
//     }
//     cout<<"18 bit table size is:"<<inputTable18.size()<<endl;
//     //encrypt
//     ofstream input_18bit;
//     input_18bit.open("Table/InputTable_18bit", ios::binary);// Attention: row size
//
//     int64_t row_input18=ceil(double(inputTable18.size())/double(row_size));
//     cout<<"row_input18:"<<row_input18<<", row_size:"<<row_size<<endl;
//
//     vector<int64_t> input_row18;
//     for(int64_t s=0 ; s<row_input18 ; s++){
//       for(int64_t k=0 ; k<row_size; k++){
//         if((s*row_size+k)<=inputTable18.size()){
//           input_row18.push_back(inputTable18[s*row_size+k]);
//         }else{
//           input_row18.push_back(131072);
//         }
//       }
//       input_row18.resize(slot_count);
//       Plaintext temp_pla_in;
//       Ciphertext temp_enc_in;
//       batch_encoder.encode(input_row18, temp_pla_in);
//       encryptor.encrypt(temp_pla_in, temp_enc_in);
//       temp_enc_in.save(input_18bit);
//       input_row18.clear();
//     }
//     input_18bit.close();
//     //output
//     ofstream output_18bit;
//     output_18bit.open("Table/OutputTable_18bit", ios::binary);// Attention: row size
//     int64_t row_output18=ceil(double(outputTable18.size())/double(row_size));
//     cout<<"row_output18:"<<row_output18<<", rwo_size:"<<row_size<<endl;
//     vector<int64_t> output_row18;
//     for(int64_t s=0 ; s<row_output18 ; s++){
//       for(int64_t k=0 ; k<row_size; k++){
//         if((s*row_size+k)<=outputTable18.size()){
//           output_row18.push_back(outputTable18[s*row_size+k]);
//         }else{
//           output_row18.push_back(0);
//         }
//       }
//       output_row18.resize(slot_count);
//       Plaintext temp_pla_out;
//       Ciphertext temp_enc_out;
//       batch_encoder.encode(output_row18, temp_pla_out);
//       encryptor.encrypt(temp_pla_out, temp_enc_out);
//       temp_enc_out.save(output_18bit);
//       output_row18.clear();
//     }
//     output_18bit.close();
//     cout << "===========================" << endl;

// /*16-bit integer table*/
//   vector<int64_t> inputTable16, outputTable16;
//   for(int64_t i = -32768 ; i < 32768 ; i++){ //65,536
//       inputTable16.push_back(i);
//       outputTable16.push_back(i);
//       // cout<<"\033[32m input/output:"<<i<<"\033[0m"<<endl;
//   }
//   cout<<"16 bit table size is:"<<inputTable16.size()<<endl;
//   //encrypt
//   ofstream input_16bit;
//   input_16bit.open("Table/InputTable_16bit", ios::binary);// Attention: row size
//
//   int64_t row_input16=ceil(double(inputTable16.size())/double(row_size));
//   cout<<"row_input16:"<<row_input16<<", row_size:"<<row_size<<endl;
//
//   vector<int64_t> input_row16;
//   for(int64_t s=0 ; s<row_input16 ; s++){
//     for(int64_t k=0 ; k<row_size; k++){
//       if((s*row_size+k)<=inputTable16.size()){
//         input_row16.push_back(inputTable16[s*row_size+k]);
//       }else{
//         input_row16.push_back(32768);
//       }
//     }
//     input_row16.resize(slot_count);
//     Plaintext temp_pla_in;
//     Ciphertext temp_enc_in;
//     batch_encoder.encode(input_row16, temp_pla_in);
//     encryptor.encrypt(temp_pla_in, temp_enc_in);
//     temp_enc_in.save(input_16bit);
//     input_row16.clear();
//   }
//   input_16bit.close();
//   //output
//   ofstream output_16bit;
//   output_16bit.open("Table/OutputTable_16bit", ios::binary);// Attention: row size
//   int64_t row_output16=ceil(double(outputTable16.size())/double(row_size));
//   cout<<"row_output16:"<<row_output16<<", rwo_size:"<<row_size<<endl;
//   vector<int64_t> output_row16;
//   for(int64_t s=0 ; s<row_output16 ; s++){
//     for(int64_t k=0 ; k<row_size; k++){
//       if((s*row_size+k)<=outputTable16.size()){
//         output_row16.push_back(outputTable16[s*row_size+k]);
//       }else{
//         output_row16.push_back(0);
//       }
//     }
//     output_row16.resize(slot_count);
//     Plaintext temp_pla_out;
//     Ciphertext temp_enc_out;
//     batch_encoder.encode(output_row16, temp_pla_out);
//     encryptor.encrypt(temp_pla_out, temp_enc_out);
//     temp_enc_out.save(output_16bit);
//     output_row16.clear();
//   }
//   output_16bit.close();
//   cout << "===========================" << endl;

//   /*14-bit integer table*/
//     vector<int64_t> inputTable14, outputTable14;
//     for(int64_t i = -8192 ; i < 8192 ; i++){ //16,384
//         inputTable14.push_back(i);
//         outputTable14.push_back(i);
//     }
//     cout<<"14 bit table size is:"<<inputTable14.size()<<endl;
//     //encrypt
//     ofstream input_14bit;
//     input_14bit.open("Table/InputTable_14bit", ios::binary);// Attention: row size
//
//     int64_t row_input14=ceil(double(inputTable14.size())/double(row_size));
//     cout<<"row_input14:"<<row_input14<<", row_size:"<<row_size<<endl;
//
//     vector<int64_t> input_row14;
//     for(int64_t s=0 ; s<row_input14 ; s++){
//       for(int64_t k=0 ; k<row_size; k++){
//         if((s*row_size+k)<=inputTable14.size()){
//           input_row14.push_back(inputTable14[s*row_size+k]);
//         }else{
//           input_row14.push_back(8192);
//         }
//       }
//       input_row14.resize(slot_count);
//       Plaintext temp_pla_in;
//       Ciphertext temp_enc_in;
//       batch_encoder.encode(input_row14, temp_pla_in);
//       encryptor.encrypt(temp_pla_in, temp_enc_in);
//       temp_enc_in.save(input_14bit);
//       input_row14.clear();
//     }
//     input_14bit.close();
//     //output
//     ofstream output_14bit;
//     output_14bit.open("Table/OutputTable_14bit", ios::binary);// Attention: row size
//     int64_t row_output14=ceil(double(outputTable14.size())/double(row_size));
//     cout<<"row_input14:"<<row_output14<<", rwo_size:"<<row_size<<endl;
//     vector<int64_t> output_row14;
//     for(int64_t s=0 ; s<row_output14 ; s++){
//       for(int64_t k=0 ; k<row_size; k++){
//         if((s*row_size+k)<=outputTable14.size()){
//           output_row14.push_back(outputTable14[s*row_size+k]);
//         }else{
//           output_row14.push_back(0);
//         }
//       }
//       output_row14.resize(slot_count);
//       Plaintext temp_pla_out;
//       Ciphertext temp_enc_out;
//       batch_encoder.encode(output_row14, temp_pla_out);
//       encryptor.encrypt(temp_pla_out, temp_enc_out);
//       temp_enc_out.save(output_14bit);
//       output_row14.clear();
//     }
//     output_14bit.close();
//     cout << "===========================" << endl;

//   /*12-bit integer table*/
//     vector<int64_t> inputTable12, outputTable12;
//     for(int64_t i = -2048 ; i < 2048 ; i++){ //4,096
//         inputTable12.push_back(i);
//         outputTable12.push_back(i);
//     }
//     cout<<"12 bit table size is:"<<inputTable12.size()<<endl;
//     //encrypt
//     ofstream input_12bit;
//     input_12bit.open("Table/InputTable_12bit", ios::binary);// Attention: row size
//
//     int64_t row_input12=ceil(double(inputTable12.size())/double(row_size));
//     cout<<"row_input12:"<<row_input12<<", row_size:"<<row_size<<endl;
//
//     vector<int64_t> input_row12;
//     for(int64_t s=0 ; s<row_input12 ; s++){
//       for(int64_t k=0 ; k<row_size; k++){
//         if((s*row_size+k)<=inputTable12.size()){
//           input_row12.push_back(inputTable12[s*row_size+k]);
//         }else{
//           input_row12.push_back(2048);
//         }
//       }
//       input_row12.resize(slot_count);
//       Plaintext temp_pla_in;
//       Ciphertext temp_enc_in;
//       batch_encoder.encode(input_row12, temp_pla_in);
//       encryptor.encrypt(temp_pla_in, temp_enc_in);
//       temp_enc_in.save(input_12bit);
//       input_row12.clear();
//     }
//     input_12bit.close();
//     //output
//     ofstream output_12bit;
//     output_12bit.open("Table/OutputTable_12bit", ios::binary);// Attention: row size
//     int64_t row_output12=ceil(double(outputTable12.size())/double(row_size));
//     cout<<"row_output12:"<<row_output12<<", rwo_size:"<<row_size<<endl;
//     vector<int64_t> output_row12;
//     for(int64_t s=0 ; s<row_output12 ; s++){
//       for(int64_t k=0 ; k<row_size; k++){
//         if((s*row_size+k)<=outputTable12.size()){
//           output_row12.push_back(outputTable12[s*row_size+k]);
//         }else{
//           output_row12.push_back(0);
//         }
//       }
//       output_row12.resize(slot_count);
//       Plaintext temp_pla_out;
//       Ciphertext temp_enc_out;
//       batch_encoder.encode(output_row12, temp_pla_out);
//       encryptor.encrypt(temp_pla_out, temp_enc_out);
//       temp_enc_out.save(output_12bit);
//       output_row12.clear();
//     }
//     output_12bit.close();
//     cout << "===========================" << endl;

  // 2-input function, 6-bit
  /*6-bit integer table*/// 2^6 = 64
    vector<int64_t> inputTable6, outputTable6;
    for(int64_t i = -32 ; i < 32 ; i++){ //64
        inputTable6.push_back(i);
    }
    for(int64_t i = -32 ; i < 32 ; i++){ // 2^12=4096
      for(int64_t j = -32 ; j < 32 ; j++){
        outputTable6.push_back(i+j);
      }
    }
    cout<<"6-bit input table size is:"<<inputTable6.size()<<endl;
    cout<<"2-input 6-bit output table size is:"<<outputTable6.size()<<endl;
    //encrypt
    ofstream input_6bit;
    input_6bit.open("Table/two/InputTable_6bit", ios::binary);// Attention: row size

    int64_t row_input6=ceil(double(inputTable6.size())/double(row_size));
    cout<<"row_input6:"<<row_input6<<endl;

    vector<int64_t> input_row6;
    for(int64_t s=0 ; s<row_input6 ; s++){
      for(int64_t k=0 ; k<row_size; k++){
        if((s*row_size+k)<=inputTable6.size()){
          input_row6.push_back(inputTable6[s*row_size+k]);
        }else{
          input_row6.push_back(0);
        }
      }
      input_row6.resize(slot_count);
      Plaintext temp_pla_in;
      Ciphertext temp_enc_in;
      batch_encoder.encode(input_row6, temp_pla_in);
      encryptor.encrypt(temp_pla_in, temp_enc_in);
      temp_enc_in.save(input_6bit);
      input_row6.clear();
    }
    input_6bit.close();
    //output
    ofstream output_6bit;
    output_6bit.open("Table/two/OutputTable_6bit", ios::binary);// Attention: row size
    int64_t row_output6=ceil(double(outputTable6.size())/double(row_size));
    cout<<"row_output6:"<<row_output6<<endl;
    vector<int64_t> output_row6;
    for(int64_t s=0 ; s<row_output6 ; s++){
      for(int64_t k=0 ; k<row_size; k++){
        if((s*row_size+k)<=outputTable6.size()){
          output_row6.push_back(outputTable6[s*row_size+k]);
        }else{
          output_row6.push_back(0);
        }
      }
      output_row6.resize(slot_count);
      Plaintext temp_pla_out;
      Ciphertext temp_enc_out;
      batch_encoder.encode(output_row6, temp_pla_out);
      encryptor.encrypt(temp_pla_out, temp_enc_out);
      temp_enc_out.save(output_6bit);
      output_row6.clear();
    }
    output_6bit.close();

  // 2-input function, 8-bit
  /*8-bit integer table*/// 2^8 = 256
    vector<int64_t> inputTable8, outputTable8;
    for(int64_t i = -128 ; i < 128 ; i++){ //256
        inputTable8.push_back(i);
    }
    for(int64_t i = -128 ; i < 128 ; i++){ // 2^16=65536
      for(int64_t j = -128 ; j < 128 ; j++){
        outputTable8.push_back(i+j);
      }
    }
    cout<<"8-bit input table size is:"<<inputTable8.size()<<endl;
    cout<<"2-input 8-bit output table size is:"<<outputTable8.size()<<endl;
    //encrypt
    ofstream input_8bit;
    input_8bit.open("Table/two/InputTable_8bit", ios::binary);// Attention: row size

    int64_t row_input8=ceil(double(inputTable8.size())/double(row_size));
    cout<<"row_input8:"<<row_input8<<endl;

    vector<int64_t> input_row8;
    for(int64_t s=0 ; s<row_input8 ; s++){
      for(int64_t k=0 ; k<row_size; k++){
        if((s*row_size+k)<=inputTable8.size()){
          input_row8.push_back(inputTable8[s*row_size+k]);
        }else{
          input_row8.push_back(0);
        }
      }
      input_row8.resize(slot_count);
      Plaintext temp_pla_in;
      Ciphertext temp_enc_in;
      batch_encoder.encode(input_row8, temp_pla_in);
      encryptor.encrypt(temp_pla_in, temp_enc_in);
      temp_enc_in.save(input_8bit);
      input_row8.clear();
    }
    input_8bit.close();
    //output
    ofstream output_8bit;
    output_8bit.open("Table/two/OutputTable_8bit", ios::binary);// Attention: row size
    int64_t row_output8=ceil(double(outputTable8.size())/double(row_size));
    cout<<"row_output8:"<<row_output8<<endl;
    vector<int64_t> output_row8;
    for(int64_t s=0 ; s<row_output8 ; s++){
      for(int64_t k=0 ; k<row_size; k++){
        if((s*row_size+k)<=outputTable8.size()){
          output_row8.push_back(outputTable8[s*row_size+k]);
        }else{
          output_row8.push_back(0);
        }
      }
      output_row8.resize(slot_count);
      Plaintext temp_pla_out;
      Ciphertext temp_enc_out;
      batch_encoder.encode(output_row8, temp_pla_out);
      encryptor.encrypt(temp_pla_out, temp_enc_out);
      temp_enc_out.save(output_8bit);
      output_row8.clear();
    }
    output_8bit.close();

  // 2-input function, 10-bit
  /*10-bit integer table*/// 2^10 = 1024
    vector<int64_t> inputTable10, outputTable10;
    for(int64_t i = -512 ; i < 512 ; i++){ //1024
        inputTable10.push_back(i);
    }
    for(int64_t i = -512 ; i < 512 ; i++){ // 2^20=1048576
      for(int64_t j = -512 ; j < 512 ; j++){
        outputTable10.push_back(i+j);
      }
    }
    cout<<"10-bit input table size is:"<<inputTable10.size()<<endl;
    cout<<"2-input 10-bit output table size is:"<<outputTable10.size()<<endl;
    //encrypt
    ofstream input_10bit;
    input_10bit.open("Table/two/InputTable_10bit", ios::binary);// Attention: row size

    int64_t row_input10=ceil(double(inputTable10.size())/double(row_size));
    cout<<"row_input10:"<<row_input10<<endl;

    vector<int64_t> input_row10;
    for(int64_t s=0 ; s<row_input10 ; s++){
      for(int64_t k=0 ; k<row_size; k++){
        if((s*row_size+k)<=inputTable10.size()){
          input_row10.push_back(inputTable10[s*row_size+k]);
        }else{
          input_row10.push_back(0);
        }
      }
      input_row10.resize(slot_count);
      Plaintext temp_pla_in;
      Ciphertext temp_enc_in;
      batch_encoder.encode(input_row10, temp_pla_in);
      encryptor.encrypt(temp_pla_in, temp_enc_in);
      temp_enc_in.save(input_10bit);
      input_row10.clear();
    }
    input_10bit.close();

    ofstream output_10bit;
    output_10bit.open("Table/two/OutputTable_10bit", ios::binary);// Attention: row size
    int64_t row_output10=ceil(double(outputTable10.size())/double(row_size));
    cout<<"row_output10:"<<row_output10<<endl;
    vector<int64_t> output_row10;
    for(int64_t s=0 ; s<row_output10 ; s++){
      for(int64_t k=0 ; k<row_size; k++){
        if((s*row_size+k)<=outputTable10.size()){
          output_row10.push_back(outputTable10[s*row_size+k]);
        }else{
          output_row10.push_back(0);
        }
      }
      output_row10.resize(slot_count);
      Plaintext temp_pla_out;
      Ciphertext temp_enc_out;
      batch_encoder.encode(output_row10, temp_pla_out);
      encryptor.encrypt(temp_pla_out, temp_enc_out);
      temp_enc_out.save(output_10bit);
      output_row10.clear();
    }
    output_10bit.close();

  // 2-input function, 12-bit
  /*12-bit integer table*/// 2^24 = 16777216
    vector<int64_t> inputTable12, outputTable12;
    for(int64_t i = -2048 ; i < 2048 ; i++){ //4096
        inputTable12.push_back(i);
    }
    for(int64_t i = -2048 ; i < 2048 ; i++){ // 2^24=16777216
      for(int64_t j = -2048 ; j < 2048 ; j++){
        outputTable12.push_back(i+j);
      }
    }
    cout<<"12-bit input table size is:"<<inputTable12.size()<<endl;
    cout<<"2-input 12-bit output table size is:"<<outputTable12.size()<<endl;
    //encrypt
    ofstream input_12bit;
    input_12bit.open("Table/two/InputTable_12bit", ios::binary);// Attention: row size

    int64_t row_input12=ceil(double(inputTable12.size())/double(row_size));
    cout<<"row_input12:"<<row_input12<<endl;

    vector<int64_t> input_row12;
    for(int64_t s=0 ; s<row_input12 ; s++){
      for(int64_t k=0 ; k<row_size; k++){
        if((s*row_size+k)<=inputTable12.size()){
          input_row12.push_back(inputTable12[s*row_size+k]);
        }else{
          input_row12.push_back(0);
        }
      }
      input_row12.resize(slot_count);
      Plaintext temp_pla_in;
      Ciphertext temp_enc_in;
      batch_encoder.encode(input_row12, temp_pla_in);
      encryptor.encrypt(temp_pla_in, temp_enc_in);
      temp_enc_in.save(input_12bit);
      input_row12.clear();
    }
    input_12bit.close();
    //output
    ofstream output_12bit;
    output_12bit.open("Table/two/OutputTable_12bit", ios::binary);// Attention: row size
    int64_t row_output12=ceil(double(outputTable12.size())/double(row_size));
    cout<<"row_output12:"<<row_output12<<endl;
    vector<int64_t> output_row12;
    for(int64_t s=0 ; s<row_output12 ; s++){
      for(int64_t k=0 ; k<row_size; k++){
        if((s*row_size+k)<=outputTable12.size()){
          output_row12.push_back(outputTable12[s*row_size+k]);
        }else{
          output_row12.push_back(0);
        }
      }
      output_row12.resize(slot_count);
      Plaintext temp_pla_out;
      Ciphertext temp_enc_out;
      batch_encoder.encode(output_row12, temp_pla_out);
      encryptor.encrypt(temp_pla_out, temp_enc_out);
      temp_enc_out.save(output_12bit);
      output_row12.clear();
    }
    output_12bit.close();

  // 3-input function, 4-bit
  /*4-bit integer table*/// 2^12 = 4096
    vector<int64_t> inputTable4, outputTable4;
    for(int64_t i = -8 ; i < 8 ; i++){ //2^4=16
        inputTable4.push_back(i);
    }
    for(int64_t i = -8 ; i < 8 ; i++){ // 2^12=4096
      for(int64_t j = -8 ; j < 8 ; j++){
        for(int64_t k = -8 ; k < 8 ; k++){
          outputTable4.push_back(i+j+k);
        }
      }
    }
    cout<<"4-bit input table size is:"<<inputTable4.size()<<endl;
    cout<<"3-input 4-bit output table size is:"<<outputTable4.size()<<endl;
    //encrypt
    ofstream input_4bit;
    input_4bit.open("Table/three/InputTable_4bit", ios::binary);// Attention: row size

    int64_t row_input4=ceil(double(inputTable4.size())/double(row_size));
    cout<<"row_input4:"<<row_input4<<endl;

    vector<int64_t> input_row4;
    for(int64_t s=0 ; s<row_input4 ; s++){
      for(int64_t k=0 ; k<row_size; k++){
        if((s*row_size+k)<=inputTable4.size()){
          input_row4.push_back(inputTable4[s*row_size+k]);
        }else{
          input_row4.push_back(0);
        }
      }
      input_row4.resize(slot_count);
      Plaintext temp_pla_in;
      Ciphertext temp_enc_in;
      batch_encoder.encode(input_row4, temp_pla_in);
      encryptor.encrypt(temp_pla_in, temp_enc_in);
      temp_enc_in.save(input_4bit);
      input_row4.clear();
    }
    input_4bit.close();
    //output
    ofstream output_4bit;
    output_4bit.open("Table/three/OutputTable_4bit", ios::binary);// Attention: row size
    int64_t row_output4=ceil(double(outputTable4.size())/double(row_size));
    cout<<"row_output4:"<<row_output4<<endl;
    vector<int64_t> output_row4;
    for(int64_t s=0 ; s<row_output4 ; s++){
      for(int64_t k=0 ; k<row_size; k++){
        if((s*row_size+k)<=outputTable4.size()){
          output_row4.push_back(outputTable4[s*row_size+k]);
        }else{
          output_row4.push_back(0);
        }
      }
      output_row4.resize(slot_count);
      Plaintext temp_pla_out;
      Ciphertext temp_enc_out;
      batch_encoder.encode(output_row4, temp_pla_out);
      encryptor.encrypt(temp_pla_out, temp_enc_out);
      temp_enc_out.save(output_4bit);
      output_row4.clear();
    }
    output_4bit.close();

  // 3-input function, 6-bit
  /*4-bit integer table*/// 2^18 = 262144
    vector<int64_t> inputTable63, outputTable63;
    for(int64_t i = -32 ; i < 32 ; i++){ //2^6=64
        inputTable63.push_back(i);
    }
    for(int64_t i = -32 ; i < 32 ; i++){ // 2^18=262144
      for(int64_t j = -32 ; j < 32 ; j++){
        for(int64_t k = -32 ; k < 32 ; k++){
          outputTable63.push_back(i+j+k);
        }
      }
    }
    cout<<"4-bit input table size is:"<<inputTable63.size()<<endl;
    cout<<"3-input 4-bit output table size is:"<<outputTable63.size()<<endl;

    ofstream input_63bit;
    input_63bit.open("Table/three/InputTable_6bit", ios::binary);// Attention: row size

    int64_t row_input63=ceil(double(inputTable63.size())/double(row_size));
    cout<<"row_input63:"<<row_input63<<endl;

    vector<int64_t> input_row63;
    for(int64_t s=0 ; s<row_input63 ; s++){
      for(int64_t k=0 ; k<row_size; k++){
        if((s*row_size+k)<=inputTable63.size()){
          input_row63.push_back(inputTable63[s*row_size+k]);
        }else{
          input_row63.push_back(0);
        }
      }
      input_row63.resize(slot_count);
      Plaintext temp_pla_in;
      Ciphertext temp_enc_in;
      batch_encoder.encode(input_row63, temp_pla_in);
      encryptor.encrypt(temp_pla_in, temp_enc_in);
      temp_enc_in.save(input_63bit);
      input_row63.clear();
    }
    input_63bit.close();
    //output
    ofstream output_63bit;
    output_63bit.open("Table/three/OutputTable_6bit", ios::binary);// Attention: row size
    int64_t row_output63=ceil(double(outputTable63.size())/double(row_size));
    cout<<"row_output63:"<<row_output63<<endl;

    vector<int64_t> output_row63;
    for(int64_t s=0 ; s<row_output63 ; s++){
      for(int64_t k=0 ; k<row_size; k++){
        if((s*row_size+k)<=outputTable63.size()){
          output_row63.push_back(outputTable63[s*row_size+k]);
        }else{
          output_row63.push_back(0);
        }
      }
      output_row63.resize(slot_count);
      Plaintext temp_pla_out;
      Ciphertext temp_enc_out;
      batch_encoder.encode(output_row63, temp_pla_out);
      encryptor.encrypt(temp_pla_out, temp_enc_out);
      temp_enc_out.save(output_63bit);
      output_row63.clear();
    }
    output_63bit.close();

  // 3-input function, 8-bit
  /*4-bit integer table*/// 2^24 = 16777216
    vector<int64_t> inputTable83, outputTable83;
    for(int64_t i = -128 ; i < 128 ; i++){ //2^8=256
        inputTable83.push_back(i);
    }
    for(int64_t i = -128 ; i < 128 ; i++){ // 2^18=262144
      for(int64_t j = -128 ; j < 128 ; j++){
        for(int64_t k = -128 ; k < 128 ; k++){
          outputTable83.push_back(i+j+k);
        }
      }
    }
    cout<<"8-bit input table size is:"<<inputTable83.size()<<endl;
    cout<<"3-input 8-bit output table size is:"<<outputTable83.size()<<endl;
    //encrypt
    ofstream input_83bit;
    input_83bit.open("Table/three/InputTable_8bit", ios::binary);// Attention: row size

    int64_t row_input83=ceil(double(inputTable83.size())/double(row_size));
    cout<<"row_input83:"<<row_input83<<endl;

    vector<int64_t> input_row83;
    for(int64_t s=0 ; s<row_input83 ; s++){
      for(int64_t k=0 ; k<row_size; k++){
        if((s*row_size+k)<=inputTable83.size()){
          input_row83.push_back(inputTable83[s*row_size+k]);
        }else{
          input_row83.push_back(0);
        }
      }
      input_row83.resize(slot_count);
      Plaintext temp_pla_in;
      Ciphertext temp_enc_in;
      batch_encoder.encode(input_row83, temp_pla_in);
      encryptor.encrypt(temp_pla_in, temp_enc_in);
      temp_enc_in.save(input_83bit);
      input_row83.clear();
    }
    input_83bit.close();
    //output
    ofstream output_83bit;
    output_83bit.open("Table/three/OutputTable_8bit", ios::binary);// Attention: row size
    int64_t row_output83=ceil(double(outputTable83.size())/double(row_size));
    cout<<"row_output83:"<<row_output83<<endl;

    vector<int64_t> output_row83;
    for(int64_t s=0 ; s<row_output83 ; s++){
      for(int64_t k=0 ; k<row_size; k++){
        if((s*row_size+k)<=outputTable83.size()){
          output_row83.push_back(outputTable83[s*row_size+k]);
        }else{
          output_row83.push_back(0);
        }
      }
      output_row83.resize(slot_count);
      Plaintext temp_pla_out;
      Ciphertext temp_enc_out;
      batch_encoder.encode(output_row83, temp_pla_out);
      encryptor.encrypt(temp_pla_out, temp_enc_out);
      temp_enc_out.save(output_83bit);
      output_row83.clear();
    }
    output_83bit.close();

  return 0;
}
