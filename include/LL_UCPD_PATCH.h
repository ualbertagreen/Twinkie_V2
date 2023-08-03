#include "stm32g0xx_ll_ucpd.h"

#ifndef LL_UCPD_PATCH_H
#define LL_UCPD_PATCH_H

/**
  * @brief  Check if Rx error interrupt
  * @rmtoll SR          RXERR         LL_UCPD_IsActiveFlag_RxErr
  * @param  UCPDx UCPD Instance
  * @retval None
  */
__STATIC_INLINE uint32_t LL_UCPD_IsActiveFlag_RxErr(UCPD_TypeDef const * const UCPDx)
{
  return ((READ_BIT(UCPDx->SR, UCPD_SR_RXERR) == UCPD_SR_RXERR) ? 1UL : 0UL);
}

#endif
