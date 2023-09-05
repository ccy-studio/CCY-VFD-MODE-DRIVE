
/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-09-05 11:42:49
 * @LastEditTime: 2023-09-05 14:08:50
 */
#ifndef __GUI_H
#define __GUI_H

#include <constant.h>

/**
 * 初始化
 */
void gui_init();
/**
 * 打印字符
 * @param x 起始X坐标
 * @param y 起始y坐标
 * @param str 字符串
 */
void gui_print(u8 x, u8 y, const char* str);

/**
 * 打印字符-自动换行
 * @param str
 */
void gui_print(const char* str);
/**
 * 清空屏幕
 */
void gui_clear_all();
/**
 * 清空指定行
 * @param y 指定坐标y
 */
void gui_clear_line(u8 y);
/**
 * 指定亮度
 * @param brightness 0~255
 */
void gui_set_brightness(u8 brightness);

#endif