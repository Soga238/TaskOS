#ifndef TLIB_H
#define TLIB_H

#include ".\tk_compiler.h"
#include ".\tk_types.h"

typedef struct {
     uint32_t bitmap;
}tBitmap;


void tBitmapInit(tBitmap *bitmap);
uint32_t tBitmapPosCount(void);

void tBitmapSet(tBitmap *bitmap, uint32_t pos);
void tBitmapClear(tBitmap *bitmap, uint32_t pos);
uint32_t tBitmapGetFirstSet(tBitmap *bitmap);

typedef struct _tNode
{
    struct _tNode* preNode;
    struct _tNode* nextNode;
}tNode;

void tNodeInit(tNode* node);

typedef struct _tList
{
    tNode headNode;
    uint32_t nodeCount;
}tList;

#endif
