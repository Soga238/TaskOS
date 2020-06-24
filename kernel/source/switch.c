#include ".\include\TinyOS.h"

#define NVIC_INT_CTRL       0xE000ED04      // 中断控制及状态寄存器
#define NVIC_PENDSVSET      0x10000000      // 触发软件中断的值
#define NVIC_SYSPRI2        0xE000ED22      // 系统优先级寄存器
#define NVIC_PENDSV_PRI     0x000000FF      // 配置优先级

#define MEM32(__ADDR)       (*(volatile unsigned long *)(__ADDR))
#define MEM8(__ADDR)        (*(volatile unsigned char *)(__ADDR))

void tTaskRunFirst(void)
{
    __set_PSP(0);   // 作为判断第一次切换任务的标志

    // 触发PEND_SV中断
    MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
    MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

void tTaskSwitch(void)
{
    // 触发PEND_SV中断
    MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

