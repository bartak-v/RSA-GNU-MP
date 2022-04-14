/*
    Projekt: RSA and integer factorization
    Autor:   Vít Barták
    Datum:   10.03.2022
*/
#include <cstddef> 
#include <stddef.h> 
#include <iostream>
#include <string>
#include <gmpxx.h>

using namespace std;

gmp_randstate_t state;

// Generate public and private RSA key with the given length
// Output P Q N E D
void rsa_generate(string num_of_bits)
{
    num_of_bits = "NOT IMPLEMENTED";
    cout << num_of_bits << "\n";
    return;
}

// Break the RSA by prime factorization
// Output D
void rsa_break(string public_modulus)
{
    public_modulus = "NOT IMPLEMENTED";
    cout << public_modulus << "\n";
    return;
}

// Encrypts or decrypts the message by the RSA equation C = M ^ E mod N | M = C ^ D mod N
char *rsa_enc_dec(mpz_t exponent, mpz_t modulus, mpz_t message)
{
    mpz_t p_c; // Plaintext or cryptogram, based on which operation we need
    mpz_init(p_c);
    mpz_powm(p_c, message, exponent, modulus);
    return mpz_get_str(0, 16, p_c);
}

// Encrypt message M with given E and N
// Output C
void rsa_encrypt(string const public_exponent, string public_modulus, string message)
{
    // Set the inner representation of E,N,M in GNU MP
    mpz_t exponent, modulus, plaintext;
    mpz_init_set_str(exponent, public_exponent.c_str(), 0);
    mpz_init_set_str(modulus, public_modulus.c_str(), 0);
    mpz_init_set_str(plaintext, message.c_str(), 0);
    cout << "0x" << rsa_enc_dec(exponent, modulus, plaintext) << "\n";
    return;
}

// Decrypt cryptogram C with given D and N
// Output M
void rsa_decrypt(string private_exponent, string public_modulus, string ciphertext)
{
    // Set the inner representation of D,N,C in GNU MP
    mpz_t exponent, modulus, cryptogram;
    mpz_init_set_str(exponent, private_exponent.c_str(), 0);
    mpz_init_set_str(modulus, public_modulus.c_str(), 0);
    mpz_init_set_str(cryptogram, ciphertext.c_str(), 0);
    cout << "0x" << rsa_enc_dec(exponent, modulus, cryptogram) << "\n";
    return;
}

// Modes of RSA operation
enum RSA_Operation
{
    gen,
    encr,
    decr,
    br
};
RSA_Operation current_op;

int main(int argc, char **argv)
{
    gmp_randinit_default(state); // Initialize the GNU MP rand state
    string opt = "";             // Get the option
    if (argc >= 2)
        opt.assign(argv[1]);
    // Set the option to be compared
    if (opt.compare("-g") == 0)
        current_op = gen;
    else if (opt.compare("-e") == 0)
        current_op = encr;
    else if (opt.compare("-d") == 0)
        current_op = decr;
    else if (opt.compare("-b") == 0)
        current_op = br;
    else
    {
        cout << "You need to set the correct mode of operation (-g -e -d -b)\n";
        return EXIT_FAILURE;
    }

    // Check arguments and call the proper functions (expecting user to know what inputs to give us).
    switch (current_op)
    {

    case RSA_Operation::gen:
        if (argc < 3)
        {
            cout << "You need to set the number of bits (B) to be generated (./kry -g B)\n";
            break;
        }
        else
            rsa_generate(argv[2]);
        break;

    case RSA_Operation::encr:
        if (argc < 5)
        {
            cout << "You need to set the public exponent (E), public modulus (N), and message (M) to be encrypted (./kry -e E N M)\n";
            break;
        }
        else
        {
            rsa_encrypt(argv[2], argv[3], argv[4]);
        }
        break;

    case RSA_Operation::decr:
        if (argc < 5)
        {
            cout << "You need to set the private exponent (D), public modulus (N), and ciphertext (C) to be decrypted (./kry -d D N C)\n";
            break;
        }
        else
        {
            rsa_decrypt(argv[2], argv[3], argv[4]);
        }
        break;

    case RSA_Operation::br:
        if (argc < 3)
        {
            cout << "You need to set public modulus (N) to be factorized"
                 << "(./kry -b N)\n";
            break;
        }
        else
        {
            rsa_break(argv[2]);
        }
        break;

    default:
        break;
    }
    return EXIT_SUCCESS;
}