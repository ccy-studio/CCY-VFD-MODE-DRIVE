/*
 * @Description:
 * @Author: chenzedeng
 * @Date: 2023-09-10 13:39:09
 * @LastEditTime: 2023-09-10 15:04:12
 */
#include "gb2312_font.h"

void convert_gb2312(const char* str, u8* buf) {
    memset(buf, 0, sizeof(buf));
    u8 index = 0;
    for (size_t i = 0; i < strlen(str); i++) {
        if ((str[i] & 0xE0) == 0xE0) {
            char cn1 = str[i++];
            char cn2 = str[i++];
            char cn3 = str[i];
            for (size_t j = 0; j < GB2312_LEN; j++) {
                gb2312_decoder font = fonts[j];
                if (cn1 == font.cn[0] && cn2 == font.cn[1] &&
                    cn3 == font.cn[2]) {
                    buf[index++] =
                        (uint8_t)((font.decoder >> 8) & 0xFF);  // 获取高8位
                    buf[index++] = (uint8_t)(font.decoder & 0xFF);  // 获取低8位
                    break;
                }
            }
        } else if (str[i] && str[i] != '\0') {
            buf[index++] = str[i];
        }
    }
    buf[index] = '\0';
}