;/*********************************************************************
;*                      Open Source Software                          *
;*                                                                    *
;**********************************************************************
;*                                                                    *
;*                 (c) 2018 - 8102 ZhangJianHang                      *
;*                                                                    *
;*          www.icore.ink     Conntact: embedzjh@gmail.com            *
;*                                                                    *
;**********************************************************************
;*                                                                    *
;*       os_cpu.h *                                                   *
;*                                                                    *
;**********************************************************************
;*/
                PRESERVE8
                THUMB
                AREA    |.text2|, CODE, READONLY

PendSV_Handler  PROC
        IMPORT g_blockPtr
        EXPORT  PendSV_Handler

        LDR     R0, =g_blockPtr     ; 变量 blockPtr 的地址值赋值给R0，R0 = &blockPtr
        LDR     R0, [R0]            ; 取 R0 所在地址的字数据赋值给R0, R0 = blockPtr
        LDR     R0, [R0]            ; 取 R0 所在地址的字数据赋值给R0, R0 = blockPtr->blockPtr = &stack[128]

        ; DB表示R0 每次传送前需要先减4，因为 blockPtr 存储的是&stack[128]的值，而不是&stack[127]
        ; 加入 !，表示R0 最终递减地址写回R0 
        STMDB   R0!, { R4 - R11 }   ; R0先减4，按照R11 -> R4的顺序保存寄存器

        LDR     R1, =g_blockPtr
        LDR     R1, [R1]
        STR     R0, [R1]

        ADD     R4, #1
        ADD     R5, #1

        ; IA表示R0 每次传送后地址加4
        ; 加入 !，表示R0 最终递增地址写回R0 
        LDMIA   R0!, { R4 - R11 }     ; R0取出数据，按照R4 -> R11的顺序恢复到寄存器

        BX      LR
        ENDP
        END