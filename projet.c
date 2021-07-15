#include <stdio.h>
#include <memory.h>
#include <limits.h>
#include <assert.h>
#include "sha256.h"
#include "picnic3_impl.h"

#define WORD_SIZE_BITS 32


int projet_sign(uint32_t* witness)
{
    int numWitBytes = 8;
    int numWitBits = 8*numWitBytes;

    unsigned char publicHash[32];  //256 bits

    sha256(publicHash, numWitBits);

    printHex("publicHash", (uint8_t*)publicHash, 32);
    printf("Wt; %s", (char*)witness);

    // paramset_t* params = malloc(60);
    paramset_SHA256_t* params = (paramset_SHA256_t*)malloc(sizeof(paramset_SHA256_t));
    params->stateSizeBits = 83 * 32;
    params->numMPCRounds = 3;
    params->numOpenedRounds = 2;
    params->numMPCParties = 16;
    params->digestSizeBytes = 32;
    // params->andSizeBytes = 5312; // d'après calcul sur feuille
    params->andSizeBytes = 5824; 
    params->stateSizeBytes = 4;
    params->seedSizeBytes = 1;
    params->stateSizeWords = (params->stateSizeBits + WORD_SIZE_BITS - 1)/ WORD_SIZE_BITS;
    params->transform = 255;
    params->saltSizeBytes = 32; /* same for all parameter sets */
    params->inputSizeBits = 512;
    params->wordSizeBits = 32;
    params->andSizeBits = 46592;   // nombre de and gates


    signature2_t* sig = (signature2_t*)malloc(sizeof(signature2_t));
    sig->salt = (uint8_t*)malloc(params->saltSizeBytes);
    sig->iSeedInfo = NULL;
    sig->iSeedInfoLen = 0;
    sig->cvInfo = NULL;       // Sign/verify code sets it
    sig->cvInfoLen = 0;
    sig->challengeC = (uint16_t*)malloc(params->numOpenedRounds * sizeof(uint16_t));
    sig->challengeP = (uint16_t*)malloc(params->numOpenedRounds * sizeof(uint16_t));
    sig->challengeHash = (uint8_t*)malloc(params->digestSizeBytes);
    sig->proofs = calloc(params->numMPCRounds, sizeof(proof2_t));

    printf("sign_picnic3: \n");
    fflush(stdout);

    int ret = sign_picnic3(witness, (uint32_t*)publicHash, sig, params);

    ret = verify_picnic3(sig, (uint32_t*)publicHash, params);
    return ret;
}