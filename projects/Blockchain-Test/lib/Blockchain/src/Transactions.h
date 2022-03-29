// Author: Gustavo Diaz Galeas (Incapamentum)
// 
// Last revision: March 29th, 2022

#ifndef TRANSACTIONS_H
#define TRANSACTIONS_H

#include <Arduino.h>
#include <Crypto.h>
#include <stdint.h>

#define BLOCK_SIZE 64
#define DATA_LENGTH 2
#define HASH_SIZE 32
#define KEY_LENGTH 32
#define SIGN_LENGTH 64

// Sensor data is collected and packed here
typedef struct
{
    uint8_t temperature;
    uint8_t humidity;
} Data;

// A data transaction that is digitally signed by the device
// that generated it
//
// =========================
//           NOTES
// =========================
//
//      The Transaction class must contain the following:
//          + a 'timestamp' hash using chip ID and actual timestamp
//          + the PK of the next/new owner (is always the server)
//          + the data included in the transaction
//          + a signature generated by current owner signing the hash of 'timestamp', PK, and data
class Transaction
{
    private:
        Data data_;
        uint8_t transactionHash_[HASH_SIZE];
        uint8_t timestamp_[HASH_SIZE];
        uint8_t serverKey_[KEY_LENGTH];
        uint8_t signature_[SIGN_LENGTH];

    // Will this be needing the verify() function?
    // Why won't the verify() function be needed again?
    //     I think the reason why the verify() function may
    //     not be needed is simply because the Ed25519 library
    //     already handles it in a much nicer way than what I
    //     could ever do
    //
    //      Also, the devices may not have to verify the transactions,
    //      as that is something the server may have to do
    //
    // Think more about how to approach this before you start
    // implementing things
    public:
        Transaction(uint8_t key[KEY_LENGTH]);

        void setData(uint8_t temp, uint8_t hum);
        void setTimestamp(uint8_t ts[HASH_SIZE]);
        void hashTransaction(void);
        int sign(uint8_t deviceKey[KEY_LENGTH]);
        int verify(void);
};


#endif