// Author: Gustavo Diaz Galeas (Incapamentum)
//
// Last revision: April 25th, 2022

#include <algorithm>
#include <Base64.h>
#include <Ed25519.h>
#include <SHA256.h>
#include <SHA512.h>

#include "Transactions.h"

// Converts from bytes/uint8s to char array of double the size
void byteToHex(uint8_t *src, char *dst, int length)
{
    int i, j;
    uint8_t b, upper, lower;

    for (i = 0, j = 0; i < length; i++)
    {
        b = src[i];
        upper = b >> 4;
        lower = b & 0xF;

        if (upper < 10)
            upper += '0';
        else
            upper += 55;

        if (lower < 10)
            lower += '0';
        else
            lower += 55;

        dst[j++] = upper;
        dst[j++] = lower;
    }
}

// On invocation, sets the ownerKey_ to the public key of the
// device that created the Transaction
Transaction::Transaction(uint8_t key[KEY_LENGTH], const char timestamp[STAMP_LENGTH])
{
    std::copy(key, key + KEY_LENGTH, ownerKey_);
    std::copy(timestamp, timestamp + STAMP_LENGTH, stamp_);
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

char *Transaction::getDataHex(void)
{
    static char hexData[(sizeof(Data) * 2) + 1];

    // Clearing garbage data
    memset(hexData, '\0', (sizeof(Data) * 2) + 1);

    byteToHex((uint8_t *)&data_, hexData, sizeof(Data));

    return hexData;
}

// Returns the base address of the stamp
const char *Transaction::getStamp(void)
{
    return stamp_;
}

// Returns the owner key of the Transaction as a
// hexadecimal value
char *Transaction::getOwnerKeyHex(void)
{
    static char hexKey[(KEY_LENGTH * 2) + 1];

    // Clearing garbage data
    memset(hexKey, '\0', (KEY_LENGTH * 2) + 1);

    byteToHex(ownerKey_, hexKey, KEY_LENGTH);

    return hexKey;
}

char *Transaction::getSignatureHex(void)
{
    static char hexSign[(SIGN_LENGTH * 2) + 1];

    // Clearing garbage data
    memset(hexSign, '\0', (SIGN_LENGTH * 2) + 1);

    byteToHex(signature_, hexSign, SIGN_LENGTH);

    return hexSign;
}

// // Prints the timestamp of when the Transaction was created
// void Transaction::printStamp(void)
// {
//     Serial.println(stamp_);
// }

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