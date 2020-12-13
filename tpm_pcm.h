#ifndef TPM_PCM_H
#define TPM_PCM_H

#include "frdm_bsp.h"

/**
 * @brief TPM0 initialization. PCM.
 */
void TPM0_Init_PCM(void);
/**
 * @brief Play sine wave len many times.
 *
 */
void TPM0_PCM_Play(uint8_t len);

#endif /* TPM_PCM_H */
