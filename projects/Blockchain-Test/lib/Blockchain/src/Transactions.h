#ifndef TRANSACTIONS_H
#define TRANSACTIONS_H

#include <Arduino.h>
#include <algorithm>
#include <stdint.h>

// Double check that the Data struct is actually 2 bytes
// long
#define DATA_LENGTH 2

// Sensor data is collected and packed here
typedef struct
{
    byte temperature;
    byte humidity;
} Data;

// A data transaction that is digitally signed by the device
// that generated it
class Transaction
{
    private:
        Data data;
        uint8_t serverKey[32]; // Is this a private or a public key?
        uint8_t signature[64];

        Transaction(uint8_t key[32])
        {
            std::copy(key, key + 32, serverKey);
        }

    // Will this be needing the verify() function?
    // Why won't the verify() function be needed again?
    //     I think the reason why the verify() function may
    //     not be needed is simply because the Ed25519 library
    //     already handles it in a much nicer way than what I
    //     could ever do
    //
    // Think more about how to approach this before you start
    // implementing things
    public:
        // This might end up needing an associated key as a param
        int sign(void);
        int verify();
};


#endif