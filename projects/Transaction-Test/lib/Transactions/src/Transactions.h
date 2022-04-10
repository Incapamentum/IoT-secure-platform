// Author: Gustavo Diaz Galeas (Incapamentum)
// 
// Last revision: April 10th, 2022

#ifndef TRANSACTIONS_H
#define TRANSACTIONS_H

#include <Arduino.h>
#include <Crypto.h>
#include <stdint.h>

#define DATA_LENGTH 2
#define KEY_LENGTH 32
#define SHA256_SIZE 32
#define SHA512_SIZE 64
#define SIGN_LENGTH 64

// Sensor data is collected and packed here
typedef struct
{
    uint8_t temperature;
    uint8_t humidity;
} Data;

// A data transaction that is digitally signed by the device
// that generated it
class Transaction
{
    private:
        Data data_;

        uint8_t transactionHash_[SHA512_SIZE]; // SHA512 hash between timestamp, server key, and data
        uint8_t ownerKey_[KEY_LENGTH];
        uint8_t signature_[SIGN_LENGTH]; // Signature of the transactionHash_

    public:
        Transaction(uint8_t key[KEY_LENGTH]);

        static const size_t transaction_size = 66;

        void printSignature(void);
        void setData(uint8_t temp, uint8_t hum);

        void hashTransaction(uint8_t ts[SHA256_SIZE]);
        void sign(uint8_t deviceKey[KEY_LENGTH]);
        int verify(void);
};

#endif