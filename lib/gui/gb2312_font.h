/*
 * @Description: https://www.107000.com/T-Hex/
 * @Author: chenzedeng
 * @Date: 2023-09-09 23:12:22
 * @LastEditTime: 2023-09-10 19:35:57
 */
#ifndef __GB2312_H
#define __GB2312_H

#include <constant.h>
typedef struct {
    const u16 decoder;
    const char* cn;
} gb2312_decoder;

#define GB2312_LEN 4
const gb2312_decoder fonts[GB2312_LEN] = {
    {0xCEC2, "温"},
    {0xB6C8, "度"},
    {0xCAAA, "湿"},
    {0xA1E6, "℃"},
};

void convert_gb2312(const char* str, u8* buf);

#endif