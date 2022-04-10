// Author: Gustavo Diaz Galeas (Incapamentum)
//
// Last revision: April 10th, 2022

#include <algorithm>
#include <Ed25519.h>
#include <SHA256.h>
#include <SHA512.h>
#include <string.h>

#include "Transactions.h"

// On invocation, sets the ownerKey_ to the public key of the
// device that created the Transaction
Transaction::Transaction(uint8_t key[KEY_LENGTH])
{
    std::copy(key, key + KEY_LENGTH, ownerKey_);
}

// Prints the signature value of the Transaction. Used for debugging purposes
void Transaction::printSignature(void)
{
    int i;

    for (i = 0; i < SIGN_LENGTH; i++)
        i != SIGN_LENGTH - 1 ? Serial.printf("%08X ", signature_[i]) : Serial.printf(" %08X\n", signature_[i]);

}

// Sets temperature and humidity data
void Transaction::setData(uint8_t temp, uint8_t hum)
{
    data_.temperature = temp;
    data_.humidity = hum;
}

// Hashes the timestamp_, ownerKey_, and data_ of the block
void Transaction::hashTransaction(uint8_t ts[SHA256_SIZE])
{
    SHA512 h;
    unsigned int ptr = 0;
    unsigned char d_buffer[transaction_size];

    // Packing data to the buffer
    memcpy(d_buffer + ptr, ts, SHA256_SIZE);
    ptr += SHA256_SIZE;
    memcpy(d_buffer + ptr, ownerKey_, KEY_LENGTH);
    ptr += KEY_LENGTH;
    memcpy(d_buffer + ptr, &data_, DATA_LENGTH);

    // Setting data to be hashed
    h.update(d_buffer, transaction_size);

    // Hashing
    h.finalize(transactionHash_, SHA512_SIZE);
}

// Signs the hash of the Transaction with the device private key
void Transaction::sign(uint8_t deviceKey[KEY_LENGTH])
{
    Ed25519::sign(signature_, deviceKey, ownerKey_, transactionHash_, SHA512_SIZE);
}

// Verifies the hash of the Transaction with the device public key
int Transaction::verify(void)
{
    bool verified;

    verified = Ed25519::verify(signature_, ownerKey_, transactionHash_, SHA512_SIZE);

    if (verified)
        return 1;

    return 0;
}