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
    // num_of_bits = "NOT IMPLEMENTED";
    cout << "\n";
    return;
}

// Break the RSA by prime factorization
// Output D
void rsa_break(string public_modulus)
{
    // public_modulus = "NOT IMPLEMENTED";
    cout << "\n";
    return;
}

/*  Encrypt or decrypt the message by the RSA equation C = M ^ E mod N | M = C ^ D mod N
    We use the same function since RSA is symmetric in this way.
        Encrypt message M with given E and N
        Output C

        Decrypt cryptogram C with given D and N
        Output M
*/
void rsa_encrypt_decrypt(string exponent_str, string modulus_str, string message_str)
{
    mpz_t p_c, exponent, modulus, message;
    mpz_init(p_c); // Plaintext or cryptogram, based on which operation we need at the moment.
    mpz_init_set_str(exponent, exponent_str.c_str(), 0);
    mpz_init_set_str(modulus, modulus_str.c_str(), 0);
    mpz_init_set_str(message, message_str.c_str(), 0);

    mpz_powm(p_c, message, exponent, modulus);

    gmp_printf("%#Zx\n", p_c);

    mpz_clear(p_c);
    mpz_clear(exponent);
    mpz_clear(modulus);
    mpz_clear(message);
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
            rsa_encrypt_decrypt(argv[2], argv[3], argv[4]);
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
            rsa_encrypt_decrypt(argv[2], argv[3], argv[4]);
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