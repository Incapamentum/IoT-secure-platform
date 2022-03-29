// Author: Gustavo Diaz Galeas (Incapamentum)
//
// Last revision: March 29th, 2022

#include <algorithm>
#include <Ed25519.h>

#include "Transactions.h"

// On invocation, sets the server public key to the passed
// key param
Transaction::Transaction(uint8_t key[KEY_LENGTH])
{
    std::copy(key, key + KEY_LENGTH, serverKey_);
}

// Sets temperature and humidity data
void Transaction::setData(uint8_t temp, uint8_t hum)
{
    data_.temperature = temp;
    data_.humidity = hum;
}

// Sets the timestamp_ variable to the value of ts, which is the result
// of hashing the timestamp at the time of generation with the ID of
// the chip
void Transaction::setTimestamp(uint8_t ts[HASH_SIZE])
{
    std::copy(ts, ts + HASH_SIZE, timestamp_);
}

// Signs the hash of the Transaction with the device private key
int Transaction::sign(uint8_t deviceKey[KEY_LENGTH])
{
    // =======================
    //          NOTES
    // =======================
    //
    //      Ed25519::sign() takes in five params:
    //          uint8_t signture[64] - signature value
    //          uint8_t privateKey[32] - private key used to sign message
    //          uint8_t publicKey[32] - public key to the corresponding private key
    //          void *message - message to be signed
    //          size_t len - length of the message to be signed

    // Ed25519::sign(signature_, )
    return 0;
}

// WiP
int Transaction::verify(void)
{
    return 0;
}