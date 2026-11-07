/* SPDX-License-Identifier: Apache-2.0 */
#include <assert.h>
#include <stdio.h>
#include "py32f0xx.h"

/* Use the real device register layout with RAM-backed registers. */
static RCC_TypeDef test_rcc;
static EXTI_TypeDef test_exti;
#undef RCC
#undef EXTI
#define RCC (&test_rcc)
#define EXTI (&test_exti)

#include "py32f0xx_ll_exti.h"
#include "py32f0xx_ll_exti.h" /* LL headers must be repeatable. */
#include "py32f0xx_ll_rcc.h"
#include "py32f0xx_ll_bus.h"
#include "../py32/py32f0xx/soc/system_py32f0xx.c"

static void test_clocks(void)
{
  static const uint32_t frequencies[] = {
    4000000, 8000000, 16000000, 22120000, 24000000, 4000000, 4000000, 4000000
  };
  static const unsigned shifts[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};

  for (unsigned fs = 0; fs < 8; ++fs) {
    RCC->ICSCR = fs << RCC_ICSCR_HSI_FS_Pos;
    for (unsigned div = 0; div < 8; ++div) {
      RCC->CR = div << RCC_CR_HSIDIV_Pos;
      for (unsigned hpre = 0; hpre < 16; ++hpre) {
        RCC->CFGR = hpre << RCC_CFGR_HPRE_Pos;
        SystemCoreClockUpdate();
        assert(SystemCoreClock == ((frequencies[fs] >> div) >> shifts[hpre]));
      }
    }
  }
  RCC->CFGR = RCC_CFGR_SWS_0;
  SystemCoreClockUpdate();
  assert(SystemCoreClock == HSE_VALUE);
  RCC->CFGR = RCC_CFGR_SWS_1 | RCC_CFGR_SWS_0;
  SystemCoreClockUpdate();
  assert(SystemCoreClock == LSI_VALUE);
#if defined(RCC_LSE_SUPPORT)
  RCC->CFGR = RCC_CFGR_SWS_2;
  SystemCoreClockUpdate();
  assert(SystemCoreClock == LSE_VALUE);
#endif
#if defined(RCC_PLL_SUPPORT)
  RCC->CFGR = RCC_CFGR_SWS_1;
  RCC->PLLCFGR = RCC_PLLCFGR_PLLSRC_HSI;
  RCC->ICSCR = 2U << RCC_ICSCR_HSI_FS_Pos;
  SystemCoreClockUpdate();
  assert(SystemCoreClock == 32000000);
  RCC->PLLCFGR = RCC_PLLCFGR_PLLSRC_HSE;
  SystemCoreClockUpdate();
  assert(SystemCoreClock == 2U * HSE_VALUE);
#endif
  RCC->CFGR = 0;
  RCC->CR = 0;
  LL_RCC_HSI_SetCalibFreq(4U << RCC_ICSCR_HSI_FS_Pos);
  assert(SystemCoreClock == 24000000);
  assert(LL_RCC_GetSysClkSource() == LL_RCC_SYS_CLKSOURCE_STATUS_HSISYS);
}

static void test_exti_and_bus(void)
{
  const uint32_t line = LL_EXTI_LINE_2;
  EXTI->RTSR = LL_EXTI_LINE_1;
  LL_EXTI_EnableFallingTrig(line);
  assert(EXTI->FTSR == line);
  assert(EXTI->RTSR == LL_EXTI_LINE_1);
  LL_EXTI_DisableFallingTrig(line);
  assert(EXTI->FTSR == 0);
  LL_EXTI_EnableRisingTrig(line);
  assert(EXTI->RTSR == (line | LL_EXTI_LINE_1));
  LL_EXTI_DisableRisingTrig(line);
  assert(EXTI->RTSR == LL_EXTI_LINE_1);
  LL_EXTI_EnableIT(line);
  assert(EXTI->IMR == line);
  LL_EXTI_DisableIT(line);
  assert(EXTI->IMR == 0);
  LL_EXTI_EnableEvent(line);
  assert(EXTI->EMR == line);
  LL_EXTI_DisableEvent(line);
  assert(EXTI->EMR == 0);
  LL_EXTI_GenerateSWI(line);
  assert(EXTI->SWIER == line);
  EXTI->EXTICR[0] = 0x01000001;
  LL_EXTI_SetEXTISource(LL_EXTI_CONFIG_PORTF, LL_EXTI_CONFIG_LINE2);
  assert(EXTI->EXTICR[0] == 0x01020001);
  assert(LL_EXTI_GetEXTISource(LL_EXTI_CONFIG_LINE2) == LL_EXTI_CONFIG_PORTF);
  EXTI->PR = line;
  assert(LL_EXTI_IsActiveFlag(line));
  LL_EXTI_ClearFlag(line);
  assert(EXTI->PR == line); /* Verify the write-one-to-clear value. */

  RCC->IOPENR = 0;
  RCC->APBENR1 = 0;
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  assert(RCC->APBENR1 == LL_APB1_GRP1_PERIPH_PWR);
  assert(RCC->IOPENR == 0);
}

int main(void)
{
  test_clocks();
  test_exti_and_bus();
  puts("PY32 clock, EXTI and peripheral clock tests passed");
  return 0;
}
