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

// Count the GCD of two mpz_t big integers (Euclid's algorithm)
void e_gcd(mpz_t result, mpz_t X, mpz_t Y)
{
    mpz_t a, b, divisor;
    mpz_init(a);
    mpz_init(b);
    mpz_init(divisor);
    mpz_set(a, X);
    mpz_set(b, Y);
    while (mpz_sgn(a) > 0)
    {
        mpz_set(divisor, a);
        mpz_mod(a, b, a);
        mpz_set(b, divisor);
    }
    mpz_set(result, divisor); // "return"

    mpz_clear(a);
    mpz_clear(b);
    mpz_clear(divisor);
}

// Break the RSA by integer factorization
// Output one of the two primes P (or Q) - thanks to randomness, it gets back one of them.
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


    mpz_mod(R, N, two);
    // If N = 1, return N
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
    { // Check for first 1 milion numbers - the trivial division method
        for (int i = 3; i < 1000000; i++)
        {
            mpz_set_ui(P, i);
            mpz_cdiv_r(R, N, P);
            if (mpz_sgn(R) == 0)
            {
                // P divides N without remainder -> P is our prime
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

        // Pollard Rho factorization algorithm

        // Safe random initialization
        gmp_randstate_t rstate;
        gmp_randinit_default(rstate);

        FILE *fp;
        fp = fopen("/dev/urandom", "r"); // Get our randomness from /dev/urandom (more secure than getting it from time)
        int seed;
        fread(const_cast<int *>((&seed)), sizeof(seed), 1, fp);
        fclose(fp);
        gmp_randseed_ui(rstate, seed);

    pollard_rho:
        mpz_urandomm(X, rstate, Np1); // Get random number in range 0 n+1
        mpz_urandomm(C, rstate, Np1);
        mpz_set(Y, X);    // Y = X
        mpz_set_ui(D, 1); // D is DIVISOR

        while (mpz_cmp_ui(D, 1) == 0)
        {
            mpz_mul(X, X, X); // X = ((X*X)%N + C )% N == f(X) where f() is polynomial mod n
            mpz_mod(X, X, N);
            mpz_add(X, X, C);
            mpz_mod(X, X, N);

            mpz_mul(Y, Y, Y); // Y = ((Y*Y)%N + C )% N == f(Y)
            mpz_mod(Y, Y, N);
            mpz_add(Y, Y, C);
            mpz_mod(Y, Y, N);

            mpz_mul(Y, Y, Y); // Y = f(f(Y))
            mpz_mod(Y, Y, N);
            mpz_add(Y, Y, C);
            mpz_mod(Y, Y, N);

            // Get D as gcd of |x-y| and n
            mpz_sub(XmY, X, Y); // x-y
            mpz_abs(ABS, XmY);  // |x-y|
            e_gcd(D, ABS, N);   // call my own GCD function
            if (mpz_cmp(D, N) == 0)
                goto pollard_rho; // Nasty trick that works, because we work with pointers. (the CPU doesn't care, nor do the sanitizers)
        }
        gmp_printf("%#Zx\n", D);
        gmp_randclear(rstate);
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
    string opt = ""; // Get the option
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
            cout << "\n";
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