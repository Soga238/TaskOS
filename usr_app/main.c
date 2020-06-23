#include <string.h>

// unsigned long stack[128];

// 使用联合体，方便观察寄存器的值
typedef union {
    struct {        
        unsigned long reserved[120];   
        unsigned long R4;
        unsigned long R5;
        unsigned long R6;
        unsigned long R7;
        unsigned long R8;
        unsigned long R9;
        unsigned long R10;
        unsigned long R11;
    };
    unsigned long stack[128];    
}un_stack_t;

un_stack_t g_tStack;

typedef struct {
    unsigned long *blockPtr;
} BlockType_t;

BlockType_t* g_blockPtr;
BlockType_t g_block;

extern void triggrtPendSVC(void);

int main(void)
{
    memset(g_tStack.stack, 0xFF, sizeof(g_tStack.stack));

    g_block.blockPtr = &g_tStack.stack[128];
    g_blockPtr = &g_block;
    
    triggrtPendSVC();

    while (1) {
        ;
    }
}
