#ifndef TRANSACTIONS_H
#define TRANSACTIONS_H

#include <algorithm>
#include <stdint.h>

// Sensor data is collected and packed here
typedef struct
{
    float temp;
    float humidity;
} Data;

// A data transaction that is digitally signed by the device
// that generated it
class Transaction
{
    private:
        Data data;
        uint8_t serverKey[32];
        uint8_t signature[64];

        Transaction(uint8_t key[32])
        {
            std::copy(key, key + 32, serverKey);
        }

    // Will this be needing the verify() functionality?
    public:
        int sign(void);
        int verify();
};

#endif