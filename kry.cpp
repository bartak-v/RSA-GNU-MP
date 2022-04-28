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
// Output P
void rsa_break(string public_modulus)
{
    // Initializations
    mpz_t P, R, N, Np1, X, Y, XmY, C, D, ABS, two;
    mpz_init(P);
    mpz_init(R);
    mpz_init(X);
    mpz_init(Y);
    mpz_init(C);
    mpz_init(D);
    mpz_init(XmY);
    mpz_init(ABS);
    mpz_init(Np1); // N+1
    mpz_init_set_ui(two, 2);
    mpz_init_set_str(N, public_modulus.c_str(), 0);
    mpz_add_ui(Np1, N, 1);

    // If we can divide N by 2, 2 is our P
    mpz_mod(R, N, two);
    // If N is 0x1, return N
    if (mpz_cmp_ui(N, 1) == 0)
    {
        mpz_set_ui(P, 1);
        gmp_printf("%#Zx\n", P);
    }
    else if (mpz_sgn(R) == 0) // If we can divide by 2 P is two
    {
        mpz_set_ui(P, 2);
        gmp_printf("%#Zx\n", P);
    }
    else
    { // Check for 1 milion numbers - the trivial division method
        for (int i = 3; i < 1000000; i++)
        {
            mpz_set_ui(P, i);
            mpz_cdiv_r(R, N, P);
            if (mpz_sgn(R) == 0)
            {
                // P divides N without remainder -> P is our prime.
                gmp_printf("%#Zx\n", P);
                mpz_clear(P);
                mpz_clear(R);
                mpz_clear(X);
                mpz_clear(Y);
                mpz_clear(C);
                mpz_clear(D);
                mpz_clear(N);
                mpz_clear(Np1);
                mpz_clear(two);
                mpz_clear(ABS);
                mpz_clear(XmY);
                return;
            }
        }

        // Pollard rho

        // Safe random initialization
        gmp_randstate_t rstate;
        gmp_randinit_mt(rstate);
        FILE *fp;
        fp = fopen("/dev/urandom", "r");
        int seed;
        fread(const_cast<int *>((&seed)), sizeof(seed), 1, fp);
        fclose(fp);
        gmp_randseed_ui(rstate, seed);

    pollard_rho:
        mpz_urandomm(X, state, Np1); // Get random in range 0 n+1
        mpz_urandomm(C, state, Np1);
        mpz_set(Y, X);    // Y = X
        mpz_set_ui(D, 1); // D is DIVISOR

        while (mpz_cmp_ui(D, 1) == 0)
        {

            mpz_mul(X, X, X); // X= ((X*X)%N + C )% N
            mpz_mod(X, X, N);
            mpz_add(X, X, C);
            mpz_mod(X, X, N);

            mpz_mul(Y, Y, Y); // Y= ((Y*Y)%N + C )% N
            mpz_mod(Y, Y, N);
            mpz_add(Y, Y, C);
            mpz_mod(Y, Y, N);

            mpz_mul(Y, Y, Y); // Y = f(f(Y))
            mpz_mod(Y, Y, N);
            mpz_add(Y, Y, C);
            mpz_mod(Y, Y, N);

            // check gcd of |x-y| and n
            mpz_sub(XmY, X, Y); // x-y
            mpz_abs(ABS, XmY);  // |x-y|
            mpz_gcd(D, ABS, N); // nesmim použít MPZ GCD!!!!
            if (mpz_cmp(D, N) == 0)
                goto pollard_rho; // Evil hack that works because we work with pointers.
        }
        gmp_printf("%#Zx\n", D);
    }

    mpz_clear(P);
    mpz_clear(R);
    mpz_clear(X);
    mpz_clear(Y);
    mpz_clear(C);
    mpz_clear(D);
    mpz_clear(N);
    mpz_clear(Np1);
    mpz_clear(two);
    mpz_clear(ABS);
    mpz_clear(XmY);
    gmp_randclear(state);
}

void pollard_rho(mpz_t N)
{
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