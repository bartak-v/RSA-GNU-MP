/* 
    Projekt: RSA and integer factorization
    Autor:   Vít Barták
    Datum:   10.03.2022
*/

#include <iostream>
#include <string>
#include <gmpxx.h>

using namespace std;

bool miller_rabin_test(mpz_class d, mpz_class n)
{
    
}


bool is_number_prime(mpz_class n) {
    if (n <= 1 || n == 4) return false;
    if (n <= 3) return true;
 
    // Find r such that n = 2^d * r + 1 for some r >= 1
    mpz_class d = n - 1;
    mpz_class d_mod_two_remainder;
    mpz_class two = 2;
    
    mpz_mod(d_mod_two_remainder.get_mpz_t(), d.get_mpz_t(), two.get_mpz_t());
    
    while (d_mod_two_remainder == 0){
        mpz_mod(d_mod_two_remainder.get_mpz_t(), d.get_mpz_t(), two.get_mpz_t());
        d /= 2;
    }
        
 
    for (int i = 0; i < 100; i++)
         if (!miller_rabin_test(d, n))
              return false;

    return true;
}

mpz_class generate_prime_number(mp_bitcnt_t prime_length){
    gmp_randclass random = gmp_randclass(gmp_randinit_mt); // GMP randomness initialization 
    mpz_class prime_number = random.get_z_bits(prime_length);
    // TODO: Nastavení bitu na 1?
    mpz_setbit(prime_number.get_mpz_t(),prime_length); // Nastavění Most sig bit  na 1?
	while (!is_number_prime(prime_number))prime_number++;

	return prime_number;
}

void rsa_generate(string num_of_bits){
    // Generate public and private RSA key with the given length
    // Output P Q N E D
    int nbits = stoi(num_of_bits);
    mp_bitcnt_t p_prime_length = nbits / 2, q_prime_length = nbits - p_prime_length; // lengths of prime numbers so that the public modulus N has n-bit size
    mpz_class P,Q,N;
    // Generating the prime numbers
    while(true){
            P = generate_prime_number(p_prime_length);
			Q = generate_prime_number(q_prime_length);
			N = P * Q;
            // If we find number (the public modulus), that is equal to the specified number of bits we end the generating.
			if (mpz_sizeinbase(N.get_mpz_t(), 2) == nbits) break;
    }

    return;
}

void rsa_encrypt(string public_exponent, string public_modulus, string message){
    // Encrypt message M with given E and N
    return;
}

void rsa_decrypt(string private_exponent, string public_modulus, string ciphertext){
    return;
}
void rsa_break(string public_modulus){
    return;
}

// Modes of RSA operation
enum RSA_Operation { gen, encr, decr, br};
RSA_Operation current_op;

int main(int argc, char** argv) {
    // Get the option
    string opt="";
    if(argc >= 2) opt = argv[1]; 
    // Set the option to be compared later
    if(opt.compare("-g")  == 0) {current_op = gen;}
    else if(opt.compare("-e")  == 0) {current_op = encr;}
    else if(opt.compare("-d")  == 0) {current_op = decr;}
    else if(opt.compare("-b")  == 0) {current_op = br;}
    else{ cout << "You need to set the correct mode of operation (-g -e -d -b)" << endl; return 0;}
    

    // Check arguments and call the proper functions (expecting user to know what inputs to give us).
    switch(current_op) {
    
        case RSA_Operation::gen:
            if(argc < 3){cout<<"You need to set the number of bits (B) to be generated (./kry -g B)" <<endl; break;}
            else rsa_generate(argv[2]);
            break;
    
        case RSA_Operation::encr:
            if(argc < 5){cout<<"You need to set the public exponent (E), public modulus (N), and message (M) to be encrypted (./kry -e E N M)" << endl ; break;}
            else {rsa_encrypt(argv[2],argv[3],argv[4]);}
            break;
    
        case RSA_Operation::decr:
            if(argc < 5){cout<<"You need to set the private exponent (D), public modulus (N), and ciphertext (C) to be decrypted (./kry -d D N C)" << endl ; break;}
            else {rsa_decrypt(argv[2],argv[3],argv[4]);}
            break;
    
        case RSA_Operation::br:
            if(argc < 3){cout<<"You need to set public modulus (N) to be factorized" <<"(./kry -b N)" << endl ; break;}
            else {rsa_break(argv[2]);}
            break;
    
        default:
            break;
    }
    return 0;
}