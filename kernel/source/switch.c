#include <stdint.h>
#include "cmsis_compiler.h"

#define NVIC_INT_CTRL       0xE000ED04      // 中断控制及状态寄存器
#define NVIC_PENDSVSET      0x10000000      // 触发软件中断的值
#define NVIC_SYSPRI2        0xE000ED22      // 系统优先级寄存器
#define NVIC_PENDSV_PRI     0x000000FF      // 配置优先级

#define MEM32(__ADDR)       (*(volatile unsigned long *)(__ADDR))
#define MEM8(__ADDR)        (*(volatile unsigned char *)(__ADDR))

void triggrtPendSVC(void)
{
    MEM8(NVIC_SYSPRI2)  = NVIC_PENDSV_PRI;
    MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

__asm void PendSV_Handler(void)
{
    IMPORT g_blockPtr

        LDR     R0, =g_blockPtr     // 变量 blockPtr 的地址值赋值给R0，R0 = &blockPtr
        LDR     R0, [R0]            // 取 R0 所在地址的字数据赋值给R0, R0 = blockPtr
        LDR     R0, [R0]            // 取 R0 所在地址的字数据赋值给R0, R0 = blockPtr->blockPtr = &stack[128]

        // DB表示R0 每次传送前需要先减4，因为 blockPtr 存储的是&stack[128]的值，而不是&stack[127]
        // 加入 !，表示R0 最终递减地址写回R0 
        STMDB   R0!, { R4 - R11 }   // R0先减4，按照R11 -> R4的顺序保存寄存器

        LDR     R1, =g_blockPtr
        LDR     R1, [R1]
        STR     R0, [R1]

        ADD     R4, #1
        ADD     R5, #1

        // IA表示R0 每次传送后地址加4
        // 加入 !，表示R0 最终递增地址写回R0 
        LDMIA   R0!, { R4 - R11 }     // R0取出数据，按照R4 -> R11的顺序恢复到寄存器

        BX      LR
}
