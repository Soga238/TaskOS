#include ".\include\TinyOS.h"
#include "..\..\bsp\rcc\rcc.h"

void SysTick_Handler(void)
{
    static uint32_t s_wCount = 0;

    // 10MS 定时周期切换任务
    if (++s_wCount % 10 == 0) {
        tTaskSystemTickHandler();
    }
}

