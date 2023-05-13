// generate keys
#include "demo.hpp"

int main(int argc, char *argv[]){
    //FHE setting, generate public key and secret key
    EncryptionParameters parms(scheme_type::bfv);
    parms.set_poly_modulus_degree(8192);//8192
    //parms.set_coeff_modulus(CoeffModulus::BFVDefault(8192));
    parms.set_coeff_modulus(CoeffModulus::Create(8192, {50,30,30,50}));
    //parms.set_coeff_modulus(CoeffModulus::BFVDefault(16384));
    parms.set_plain_modulus(786433);//1769473 //319489 //114689 //786433

    SEALContext context(parms);
    print_parameters(context);
    cout << "Parameter validation (success): " << context.parameter_error_message() << endl;

    KeyGenerator keygen(context);
    SecretKey secret_key = keygen.secret_key();
    PublicKey public_key;
    keygen.create_public_key(public_key);

    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);

    RelinKeys relin_keys;
    keygen.create_relin_keys(relin_keys);
    GaloisKeys gal_keys;
    keygen.create_galois_keys(gal_keys);

    BatchEncoder batch_encoder(context);
    size_t slot_count = batch_encoder.slot_count();
    size_t row_size = slot_count / 2;

    cout << "Save keys..." << flush;
    ofstream pkFile("Key/PublicKey",ios::binary);
    public_key.save(pkFile);
    pkFile.close();

    ofstream skFile("Key/SecretKey",ios::binary);
    secret_key.save(skFile);
    skFile.close();

    ofstream parmsFile("Key/Params",ios::binary);
    parms.save(parmsFile);
    parmsFile.close();

    ofstream galFile("Key/GaloisKey",ios::binary);
    gal_keys.save(galFile);
    galFile.close();

    ofstream relinFile("Key/RelinKey",ios::binary);
    relin_keys.save(relinFile);
    relinFile.close();

    cout << "End" << endl;
    return 0;
}
