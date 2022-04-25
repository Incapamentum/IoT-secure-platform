// Author: Gustavo Diaz Galeas (Incapamentum)
//
// Last revision: April 20th, 2022

#include <algorithm>
#include <Ed25519.h>
#include <SHA256.h>
#include <SHA512.h>
#include <string.h>

#include "Transactions.h"

// On invocation, sets the ownerKey_ to the public key of the
// device that created the Transaction
Transaction::Transaction(uint8_t key[KEY_LENGTH], const char stamp[STAMP_LENGTH])
{
    std::copy(key, key + KEY_LENGTH, ownerKey_);
    std::copy(stamp, stamp + STAMP_LENGTH, stamp_);
}

uint8_t *Transaction::genHash(void)
{
    SHA256 h256;
    uint8_t *t_hash;

    t_hash = (uint8_t *)calloc(SHA256_SIZE, sizeof(uint8_t));

    h256.update(stamp_, STAMP_LENGTH);
    h256.finalize(t_hash, SHA256_SIZE);

    return t_hash;
}

// Prints the timestamp of when the Transaction was created
void Transaction::printStamp(void)
{
    Serial.println(stamp_);
}

// Prints the signature value of the Transaction. Used for debugging purposes
void Transaction::printSignature(void)
{
    int i;

    for (i = 0; i < SIGN_LENGTH; i++)
        i != SIGN_LENGTH - 1 ? Serial.printf("%08X ", signature_[i]) : Serial.printf(" %08X\n", signature_[i]);
}

// Returns the transaction as a byte string
uint8_t *Transaction::toByteArray(void)
{
    int i, j, k = 0;
    size_t total_size;
    uint8_t *buffer;

    total_size = sizeof(Data) + STAMP_LENGTH + KEY_LENGTH + SIGN_LENGTH;

    buffer = (uint8_t *)calloc(total_size, sizeof(uint8_t));

    // Clearing of any garbage data
    memset(buffer, 0, total_size);

    for (i = 0; i < TOTAL_ITEMS; i++)
    {
        // Handles Data
        if (i == 0)
        {
            buffer[k++] = data_.temperature;
            buffer[k++] = data_.humidity;
        }

        // Handles the stamp
        else if (i == 1)
        {
            for (j = 0; j < STAMP_LENGTH; j++)
            {
                buffer[k++] = stamp_[j];                
            }
        }

        // Handles the key
        else if (i == 2)
        {
            for (j = 0; j < KEY_LENGTH; j++)
            {
                buffer[k++] = ownerKey_[j];
            }
        }

        // Handles the signature
        else
        {
            for (j = 0; j < SIGN_LENGTH; j++)
            {
                buffer[k++] = signature_[j];
            }
        }
    }

    return buffer;
}

// Sets temperature and humidity data
void Transaction::setData(uint8_t temp, uint8_t hum)
{
    data_.temperature = temp;
    data_.humidity = hum;
}

// Hashes the contents of the Transaction before signing
void Transaction::sign(uint8_t deviceKey[KEY_LENGTH])
{
    SHA256 h256;
    uint8_t *t_hash;

    t_hash = genHash();
    
    Ed25519::sign(signature_, deviceKey, ownerKey_, t_hash, SHA256_SIZE);

    free(t_hash);
}

// // Verifies the hash of the Transaction with the device public key
int Transaction::verify(void)
{
    bool verified;
    uint8_t *t_hash;

    t_hash = genHash();

    verified = Ed25519::verify(signature_, ownerKey_, t_hash, SHA256_SIZE);

    free(t_hash);

    if (verified)
        return 1;

    return 0;
}