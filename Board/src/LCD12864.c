/***********************************************************************************


************************************************************************************/
#include "include.h"
#include "LCD12864.h"
#include "LCD12864lib.h"

/*
4线SPI使用说明：
VCC 供内部逻辑电压 1.8~6V
GND 接地端

SCL 为时钟信号输入。
SDA 为数据或命令信号输入。
DC  为数据、命令选择器，HIGH 代表输入的是数据，LOW 代表输入的是命令。
RES 为复位引脚，当 RES 为 LOW 时进行初始化，在使用 OLED 过程中应保持 RES 为 HIGH。
*/
//--------这里修改定义端口------//
//---------别忘了初始化定义的这些IO口的方向--------------------//
/*
#define LCD_DC	PTC19_OUT  //C19
#define LCD_RST	PTC18_OUT  //C18
#define LCD_SDA	PTC17_OUT  //C17
#define LCD_SCL	PTC16_OUT  //C16
*/


#define SLCD_SCL gpio_set(PTC16,1)  //OLED D0
#define RLCD_SCL gpio_set(PTC16,0)
#define SLCD_SDA gpio_set(PTC17,1)  //OLED D1
#define RLCD_SDA gpio_set(PTC17,0)
#define SLCD_RST gpio_set(PTC18,1)  //OLED RST
#define RLCD_RST gpio_set(PTC18,0)
#define SLCD_DC  gpio_set(PTC19,1)  //OLED DC
#define RLCD_DC  gpio_set(PTC19,0)


#define X_WIDTH 128
#define Y_WIDTH 64


//======================================================
// 128X64I液晶底层驱动[6X8]字体库
// 设计者: powerint
// 描  述: [6X8]西文字符的字模数据 (纵向取模,字节倒序)
// !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
//======================================================
const unsigned char F6x8[][6] =
{
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // sp
  { 0x00, 0x00, 0x00, 0x2f, 0x00, 0x00 },   // !
  { 0x00, 0x00, 0x07, 0x00, 0x07, 0x00 },   // "
  { 0x00, 0x14, 0x7f, 0x14, 0x7f, 0x14 },   // #
  { 0x00, 0x24, 0x2a, 0x7f, 0x2a, 0x12 },   // $
  { 0x00, 0x62, 0x64, 0x08, 0x13, 0x23 },   // %
  { 0x00, 0x36, 0x49, 0x55, 0x22, 0x50 },   // &
  { 0x00, 0x00, 0x05, 0x03, 0x00, 0x00 },   // '
  { 0x00, 0x00, 0x1c, 0x22, 0x41, 0x00 },   // (
  { 0x00, 0x00, 0x41, 0x22, 0x1c, 0x00 },   // )
  { 0x00, 0x14, 0x08, 0x3E, 0x08, 0x14 },   // *
  { 0x00, 0x08, 0x08, 0x3E, 0x08, 0x08 },   // +
  { 0x00, 0x00, 0x00, 0xA0, 0x60, 0x00 },   // ,
  { 0x00, 0x08, 0x08, 0x08, 0x08, 0x08 },   // -
  { 0x00, 0x00, 0x60, 0x60, 0x00, 0x00 },   // .
  { 0x00, 0x20, 0x10, 0x08, 0x04, 0x02 },   // /
  { 0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E },   // 0
  { 0x00, 0x00, 0x42, 0x7F, 0x40, 0x00 },   // 1
  { 0x00, 0x42, 0x61, 0x51, 0x49, 0x46 },   // 2
  { 0x00, 0x21, 0x41, 0x45, 0x4B, 0x31 },   // 3
  { 0x00, 0x18, 0x14, 0x12, 0x7F, 0x10 },   // 4
  { 0x00, 0x27, 0x45, 0x45, 0x45, 0x39 },   // 5
  { 0x00, 0x3C, 0x4A, 0x49, 0x49, 0x30 },   // 6
  { 0x00, 0x01, 0x71, 0x09, 0x05, 0x03 },   // 7
  { 0x00, 0x36, 0x49, 0x49, 0x49, 0x36 },   // 8
  { 0x00, 0x06, 0x49, 0x49, 0x29, 0x1E },   // 9
  { 0x00, 0x00, 0x36, 0x36, 0x00, 0x00 },   // :
  { 0x00, 0x00, 0x56, 0x36, 0x00, 0x00 },   // ;
  { 0x00, 0x08, 0x14, 0x22, 0x41, 0x00 },   // <
  { 0x00, 0x14, 0x14, 0x14, 0x14, 0x14 },   // =
  { 0x00, 0x00, 0x41, 0x22, 0x14, 0x08 },   // >
  { 0x00, 0x02, 0x01, 0x51, 0x09, 0x06 },   // ?
  { 0x00, 0x32, 0x49, 0x59, 0x51, 0x3E },   // @
  { 0x00, 0x7C, 0x12, 0x11, 0x12, 0x7C },   // A
  { 0x00, 0x7F, 0x49, 0x49, 0x49, 0x36 },   // B
  { 0x00, 0x3E, 0x41, 0x41, 0x41, 0x22 },   // C
  { 0x00, 0x7F, 0x41, 0x41, 0x22, 0x1C },   // D
  { 0x00, 0x7F, 0x49, 0x49, 0x49, 0x41 },   // E
  { 0x00, 0x7F, 0x09, 0x09, 0x09, 0x01 },   // F
  { 0x00, 0x3E, 0x41, 0x49, 0x49, 0x7A },   // G
  { 0x00, 0x7F, 0x08, 0x08, 0x08, 0x7F },   // H
  { 0x00, 0x00, 0x41, 0x7F, 0x41, 0x00 },   // I
  { 0x00, 0x20, 0x40, 0x41, 0x3F, 0x01 },   // J
  { 0x00, 0x7F, 0x08, 0x14, 0x22, 0x41 },   // K
  { 0x00, 0x7F, 0x40, 0x40, 0x40, 0x40 },   // L
  { 0x00, 0x7F, 0x02, 0x0C, 0x02, 0x7F },   // M
  { 0x00, 0x7F, 0x04, 0x08, 0x10, 0x7F },   // N
  { 0x00, 0x3E, 0x41, 0x41, 0x41, 0x3E },   // O
  { 0x00, 0x7F, 0x09, 0x09, 0x09, 0x06 },   // P
  { 0x00, 0x3E, 0x41, 0x51, 0x21, 0x5E },   // Q
  { 0x00, 0x7F, 0x09, 0x19, 0x29, 0x46 },   // R
  { 0x00, 0x46, 0x49, 0x49, 0x49, 0x31 },   // S
  { 0x00, 0x01, 0x01, 0x7F, 0x01, 0x01 },   // T
  { 0x00, 0x3F, 0x40, 0x40, 0x40, 0x3F },   // U
  { 0x00, 0x1F, 0x20, 0x40, 0x20, 0x1F },   // V
  { 0x00, 0x3F, 0x40, 0x38, 0x40, 0x3F },   // W
  { 0x00, 0x63, 0x14, 0x08, 0x14, 0x63 },   // X
  { 0x00, 0x07, 0x08, 0x70, 0x08, 0x07 },   // Y
  { 0x00, 0x61, 0x51, 0x49, 0x45, 0x43 },   // Z
  { 0x00, 0x00, 0x7F, 0x41, 0x41, 0x00 },   // [
  { 0x00, 0x55, 0x2A, 0x55, 0x2A, 0x55 },   // 55
  { 0x00, 0x00, 0x41, 0x41, 0x7F, 0x00 },   // ]
  { 0x00, 0x04, 0x02, 0x01, 0x02, 0x04 },   // ^
  { 0x00, 0x40, 0x40, 0x40, 0x40, 0x40 },   // _
  { 0x00, 0x00, 0x01, 0x02, 0x04, 0x00 },   // '
  { 0x00, 0x20, 0x54, 0x54, 0x54, 0x78 },   // a
  { 0x00, 0x7F, 0x48, 0x44, 0x44, 0x38 },   // b
  { 0x00, 0x38, 0x44, 0x44, 0x44, 0x20 },   // c
  { 0x00, 0x38, 0x44, 0x44, 0x48, 0x7F },   // d
  { 0x00, 0x38, 0x54, 0x54, 0x54, 0x18 },   // e
  { 0x00, 0x08, 0x7E, 0x09, 0x01, 0x02 },   // f
  { 0x00, 0x18, 0xA4, 0xA4, 0xA4, 0x7C },   // g
  { 0x00, 0x7F, 0x08, 0x04, 0x04, 0x78 },   // h
  { 0x00, 0x00, 0x44, 0x7D, 0x40, 0x00 },   // i
  { 0x00, 0x40, 0x80, 0x84, 0x7D, 0x00 },   // j
  { 0x00, 0x7F, 0x10, 0x28, 0x44, 0x00 },   // k
  { 0x00, 0x00, 0x41, 0x7F, 0x40, 0x00 },   // l
  { 0x00, 0x7C, 0x04, 0x18, 0x04, 0x78 },   // m
  { 0x00, 0x7C, 0x08, 0x04, 0x04, 0x78 },   // n
  { 0x00, 0x38, 0x44, 0x44, 0x44, 0x38 },   // o
  { 0x00, 0xFC, 0x24, 0x24, 0x24, 0x18 },   // p
  { 0x00, 0x18, 0x24, 0x24, 0x18, 0xFC },   // q
  { 0x00, 0x7C, 0x08, 0x04, 0x04, 0x08 },   // r
  { 0x00, 0x48, 0x54, 0x54, 0x54, 0x20 },   // s
  { 0x00, 0x04, 0x3F, 0x44, 0x40, 0x20 },   // t
  { 0x00, 0x3C, 0x40, 0x40, 0x20, 0x7C },   // u
  { 0x00, 0x1C, 0x20, 0x40, 0x20, 0x1C },   // v
  { 0x00, 0x3C, 0x40, 0x30, 0x40, 0x3C },   // w
  { 0x00, 0x44, 0x28, 0x10, 0x28, 0x44 },   // x
  { 0x00, 0x1C, 0xA0, 0xA0, 0xA0, 0x7C },   // y
  { 0x00, 0x44, 0x64, 0x54, 0x4C, 0x44 },   // z
  { 0x14, 0x14, 0x14, 0x14, 0x14, 0x14 }    // horiz lines
};

const unsigned char F14x16_Idx[] =
{
   "北京龙丘邱科技开发板智能车首选液晶室温度按键次电压"
};
const unsigned char F14x16[] = {
  0x20,0x20,0x20,0x20,0xFF,0x00,0x00,0x00,0xFF,0x40,0x20,0x30,0x18,0x10,
  0x30,0x18,0x08,0x04,0x7F,0x00,0x00,0x00,0x3F,0x40,0x40,0x40,0x40,0x78,//北0

  0x04,0x04,0xE4,0x24,0x24,0x25,0x26,0x24,0x24,0x24,0xE4,0x06,0x04,0x00,
  0x20,0x10,0x19,0x0D,0x41,0x81,0x7F,0x01,0x01,0x05,0x0D,0x38,0x10,0x00,//京1
  0x10,0x10,0x10,0x10,0x10,0xFF,0x10,0xF0,0x12,0x1C,0xD0,0x10,0x10,0x10,
  0x40,0x20,0x10,0x0C,0x03,0x10,0x08,0x3F,0x42,0x41,0x40,0x40,0x40,0x70,//龙2
  0x00,0x00,0xFE,0x84,0x84,0x84,0x84,0x82,0x82,0x82,0x82,0xC0,0x80,0x00,
  0x20,0x20,0x3F,0x20,0x20,0x20,0x20,0x20,0x3F,0x20,0x20,0x20,0x30,0x20,//丘3
  0x00,0xFC,0x24,0x24,0xE2,0x22,0x22,0x00,0xFE,0x02,0x22,0x52,0x8E,0x02,
  0x10,0x1F,0x10,0x08,0x0F,0x08,0x08,0x00,0xFF,0x00,0x08,0x08,0x10,0x0F,//邱4
  0x12,0x92,0x72,0xFE,0x51,0x91,0x00,0x22,0xCC,0x00,0x00,0xFF,0x00,0x00,
  0x02,0x01,0x00,0xFF,0x00,0x04,0x04,0x04,0x02,0x02,0x02,0xFF,0x01,0x01,//科5
  0x08,0x88,0xFF,0x48,0x28,0x00,0xC8,0x48,0x48,0x7F,0x48,0xC8,0x48,0x08,
  0x41,0x80,0x7F,0x00,0x40,0x40,0x20,0x13,0x0C,0x0C,0x12,0x21,0x60,0x20,//技6
  0x42,0x42,0x42,0x42,0xFE,0x42,0x42,0x42,0x42,0xFE,0x42,0x42,0x42,0x42,
  0x40,0x20,0x10,0x0C,0x03,0x00,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x00,//开7
  0x10,0x3E,0x10,0x10,0xF0,0x9F,0x90,0x90,0x92,0x94,0x1C,0x10,0x10,0x10,
  0x20,0x10,0x88,0x87,0x41,0x46,0x28,0x10,0x28,0x27,0x40,0xC0,0x40,0x00,//发8
  0x10,0xD0,0xFF,0x50,0x90,0x00,0xFE,0x62,0xA2,0x22,0x21,0xA1,0x61,0x00,
  0x03,0x00,0x7F,0x00,0x11,0x0E,0x41,0x20,0x11,0x0A,0x0E,0x31,0x60,0x20,//板9
  0x14,0x13,0x92,0x7E,0x32,0x52,0x92,0x00,0x7C,0x44,0x44,0x44,0x7C,0x00,
  0x01,0x01,0x00,0xFF,0x49,0x49,0x49,0x49,0x49,0x49,0xFF,0x00,0x00,0x00,//智10
  0xB8,0x97,0x92,0x90,0x94,0xB8,0x10,0x00,0x7F,0x48,0x48,0x44,0x74,0x20,
  0xFF,0x0A,0x0A,0x4A,0x8A,0x7F,0x00,0x00,0x3F,0x44,0x44,0x42,0x72,0x20,//能11
  0x04,0x84,0xC4,0xA4,0x9C,0x87,0x84,0xF4,0x84,0x84,0x84,0x84,0x84,0x00,
  0x04,0x04,0x04,0x04,0x04,0x04,0x04,0xFF,0x04,0x04,0x04,0x04,0x04,0x04,//车12
  0x04,0x04,0xE4,0x25,0x26,0x34,0x2C,0x24,0x24,0x26,0xE5,0x04,0x04,0x04,
  0x00,0x00,0x7F,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x7F,0x00,0x00,0x00,//首13
  0x40,0x42,0xCC,0x00,0x50,0x4E,0xC8,0x48,0x7F,0xC8,0x48,0x48,0x40,0x00,
  0x40,0x20,0x1F,0x20,0x48,0x46,0x41,0x40,0x40,0x47,0x48,0x48,0x4E,0x40,//选14
  0x61,0x06,0xE0,0x18,0x84,0xE4,0x1C,0x84,0x65,0xBE,0x24,0xA4,0x64,0x04,
  0x04,0xFF,0x00,0x01,0x00,0xFF,0x41,0x21,0x12,0x0C,0x1B,0x61,0xC0,0x40,//液15
  0x00,0x00,0x00,0x7E,0x2A,0x2A,0x2A,0x2A,0x2A,0x2A,0x7E,0x00,0x00,0x00,
  0x7F,0x25,0x25,0x25,0x25,0x7F,0x00,0x00,0x7F,0x25,0x25,0x25,0x25,0x7F,//晶16
  0x10,0x2C,0x24,0xA4,0x64,0x25,0x26,0x24,0x24,0xA4,0x24,0x34,0x2C,0x04,
  0x40,0x48,0x49,0x49,0x49,0x49,0x7F,0x49,0x49,0x49,0x4B,0x48,0x40,0x40,//室17
  0x21,0x86,0x70,0x00,0x7E,0x4A,0x4A,0x4A,0x4A,0x4A,0x7E,0x00,0x00,0x00,
  0xFE,0x01,0x40,0x7F,0x41,0x41,0x7F,0x41,0x41,0x7F,0x41,0x41,0x7F,0x40,//温18
  0x00,0xFC,0x04,0x24,0x24,0xFC,0xA5,0xA6,0xA4,0xFC,0x24,0x24,0x24,0x04,
  0x60,0x1F,0x80,0x80,0x42,0x46,0x2A,0x12,0x12,0x2A,0x26,0x42,0xC0,0x40,//度19
  0x10,0x10,0xFF,0x90,0x50,0x98,0x88,0x88,0xE9,0x8E,0x88,0x88,0x98,0x88,
  0x42,0x81,0x7F,0x00,0x40,0x40,0x26,0x25,0x18,0x08,0x16,0x31,0x60,0x20,//按20
  0x30,0xEF,0x28,0x28,0x44,0x64,0xDC,0x10,0x54,0xFF,0x54,0x54,0x7C,0x10,
  0x01,0x7F,0x21,0x51,0x22,0x14,0x0F,0x14,0x25,0x3F,0x45,0x45,0x45,0x44,//键21
  0x02,0x1C,0xC0,0x30,0x4C,0x30,0x0F,0x08,0xF8,0x08,0x08,0x28,0x18,0x08,
  0x5E,0x43,0x20,0x20,0x10,0x08,0x04,0x03,0x01,0x06,0x08,0x30,0x60,0x20,//次22
  0x00,0xF8,0x48,0x48,0x48,0x48,0xFF,0x48,0x48,0x48,0x48,0xF8,0x00,0x00,
  0x00,0x0F,0x04,0x04,0x04,0x04,0x3F,0x44,0x44,0x44,0x44,0x4F,0x40,0x70,//电23
  0x00,0xFE,0x02,0x42,0x42,0x42,0x42,0xFA,0x42,0x42,0x42,0x62,0x42,0x02,
  0x18,0x27,0x20,0x20,0x20,0x20,0x20,0x3F,0x20,0x21,0x2E,0x24,0x20,0x20,//压24  
};


//======================================================
// 128X64I液晶底层驱动[8X16]字体库
// 设计者: powerint
// 描  述: [8X16]西文字符的字模数据 (纵向取模,字节倒序)
// !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
//======================================================
const unsigned char F8X16[] =
{
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,// 0
  0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x33,0x30,0x00,0x00,0x00,//!1
  0x00,0x10,0x0C,0x06,0x10,0x0C,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//"2
  0x40,0xC0,0x78,0x40,0xC0,0x78,0x40,0x00,0x04,0x3F,0x04,0x04,0x3F,0x04,0x04,0x00,//#3
  0x00,0x70,0x88,0xFC,0x08,0x30,0x00,0x00,0x00,0x18,0x20,0xFF,0x21,0x1E,0x00,0x00,//$4
  0xF0,0x08,0xF0,0x00,0xE0,0x18,0x00,0x00,0x00,0x21,0x1C,0x03,0x1E,0x21,0x1E,0x00,//%5
  0x00,0xF0,0x08,0x88,0x70,0x00,0x00,0x00,0x1E,0x21,0x23,0x24,0x19,0x27,0x21,0x10,//&6
  0x10,0x16,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//'7
  0x00,0x00,0x00,0xE0,0x18,0x04,0x02,0x00,0x00,0x00,0x00,0x07,0x18,0x20,0x40,0x00,//(8
  0x00,0x02,0x04,0x18,0xE0,0x00,0x00,0x00,0x00,0x40,0x20,0x18,0x07,0x00,0x00,0x00,//)9
  0x40,0x40,0x80,0xF0,0x80,0x40,0x40,0x00,0x02,0x02,0x01,0x0F,0x01,0x02,0x02,0x00,//*10
  0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x1F,0x01,0x01,0x01,0x00,//+11
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xB0,0x70,0x00,0x00,0x00,0x00,0x00,//,12
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,//-13
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00,0x00,0x00,//.14
  0x00,0x00,0x00,0x00,0x80,0x60,0x18,0x04,0x00,0x60,0x18,0x06,0x01,0x00,0x00,0x00,///15
  0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x0F,0x10,0x20,0x20,0x10,0x0F,0x00,//016
  0x00,0x10,0x10,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//117
  0x00,0x70,0x08,0x08,0x08,0x88,0x70,0x00,0x00,0x30,0x28,0x24,0x22,0x21,0x30,0x00,//218
  0x00,0x30,0x08,0x88,0x88,0x48,0x30,0x00,0x00,0x18,0x20,0x20,0x20,0x11,0x0E,0x00,//319
  0x00,0x00,0xC0,0x20,0x10,0xF8,0x00,0x00,0x00,0x07,0x04,0x24,0x24,0x3F,0x24,0x00,//420
  0x00,0xF8,0x08,0x88,0x88,0x08,0x08,0x00,0x00,0x19,0x21,0x20,0x20,0x11,0x0E,0x00,//521
  0x00,0xE0,0x10,0x88,0x88,0x18,0x00,0x00,0x00,0x0F,0x11,0x20,0x20,0x11,0x0E,0x00,//622
  0x00,0x38,0x08,0x08,0xC8,0x38,0x08,0x00,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,//723
  0x00,0x70,0x88,0x08,0x08,0x88,0x70,0x00,0x00,0x1C,0x22,0x21,0x21,0x22,0x1C,0x00,//824
  0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x00,0x31,0x22,0x22,0x11,0x0F,0x00,//925
  0x00,0x00,0x00,0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00,//:26
  0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x60,0x00,0x00,0x00,0x00,//;27
  0x00,0x00,0x80,0x40,0x20,0x10,0x08,0x00,0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x00,//<28
  0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x00,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x00,//=29
  0x00,0x08,0x10,0x20,0x40,0x80,0x00,0x00,0x00,0x20,0x10,0x08,0x04,0x02,0x01,0x00,//>30
  0x00,0x70,0x48,0x08,0x08,0x08,0xF0,0x00,0x00,0x00,0x00,0x30,0x36,0x01,0x00,0x00,//?31
  0xC0,0x30,0xC8,0x28,0xE8,0x10,0xE0,0x00,0x07,0x18,0x27,0x24,0x23,0x14,0x0B,0x00,//@32
  0x00,0x00,0xC0,0x38,0xE0,0x00,0x00,0x00,0x20,0x3C,0x23,0x02,0x02,0x27,0x38,0x20,//A33
  0x08,0xF8,0x88,0x88,0x88,0x70,0x00,0x00,0x20,0x3F,0x20,0x20,0x20,0x11,0x0E,0x00,//B34
  0xC0,0x30,0x08,0x08,0x08,0x08,0x38,0x00,0x07,0x18,0x20,0x20,0x20,0x10,0x08,0x00,//C35
  0x08,0xF8,0x08,0x08,0x08,0x10,0xE0,0x00,0x20,0x3F,0x20,0x20,0x20,0x10,0x0F,0x00,//D36
  0x08,0xF8,0x88,0x88,0xE8,0x08,0x10,0x00,0x20,0x3F,0x20,0x20,0x23,0x20,0x18,0x00,//E37
  0x08,0xF8,0x88,0x88,0xE8,0x08,0x10,0x00,0x20,0x3F,0x20,0x00,0x03,0x00,0x00,0x00,//F38
  0xC0,0x30,0x08,0x08,0x08,0x38,0x00,0x00,0x07,0x18,0x20,0x20,0x22,0x1E,0x02,0x00,//G39
  0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,0x20,0x3F,0x21,0x01,0x01,0x21,0x3F,0x20,//H40
  0x00,0x08,0x08,0xF8,0x08,0x08,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//I41
  0x00,0x00,0x08,0x08,0xF8,0x08,0x08,0x00,0xC0,0x80,0x80,0x80,0x7F,0x00,0x00,0x00,//J42
  0x08,0xF8,0x88,0xC0,0x28,0x18,0x08,0x00,0x20,0x3F,0x20,0x01,0x26,0x38,0x20,0x00,//K43
  0x08,0xF8,0x08,0x00,0x00,0x00,0x00,0x00,0x20,0x3F,0x20,0x20,0x20,0x20,0x30,0x00,//L44
  0x08,0xF8,0xF8,0x00,0xF8,0xF8,0x08,0x00,0x20,0x3F,0x00,0x3F,0x00,0x3F,0x20,0x00,//M45
  0x08,0xF8,0x30,0xC0,0x00,0x08,0xF8,0x08,0x20,0x3F,0x20,0x00,0x07,0x18,0x3F,0x00,//N46
  0xE0,0x10,0x08,0x08,0x08,0x10,0xE0,0x00,0x0F,0x10,0x20,0x20,0x20,0x10,0x0F,0x00,//O47
  0x08,0xF8,0x08,0x08,0x08,0x08,0xF0,0x00,0x20,0x3F,0x21,0x01,0x01,0x01,0x00,0x00,//P48
  0xE0,0x10,0x08,0x08,0x08,0x10,0xE0,0x00,0x0F,0x18,0x24,0x24,0x38,0x50,0x4F,0x00,//Q49
  0x08,0xF8,0x88,0x88,0x88,0x88,0x70,0x00,0x20,0x3F,0x20,0x00,0x03,0x0C,0x30,0x20,//R50
  0x00,0x70,0x88,0x08,0x08,0x08,0x38,0x00,0x00,0x38,0x20,0x21,0x21,0x22,0x1C,0x00,//S51
  0x18,0x08,0x08,0xF8,0x08,0x08,0x18,0x00,0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00,//T52
  0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00,//U53
  0x08,0x78,0x88,0x00,0x00,0xC8,0x38,0x08,0x00,0x00,0x07,0x38,0x0E,0x01,0x00,0x00,//V54
  0xF8,0x08,0x00,0xF8,0x00,0x08,0xF8,0x00,0x03,0x3C,0x07,0x00,0x07,0x3C,0x03,0x00,//W55
  0x08,0x18,0x68,0x80,0x80,0x68,0x18,0x08,0x20,0x30,0x2C,0x03,0x03,0x2C,0x30,0x20,//X56
  0x08,0x38,0xC8,0x00,0xC8,0x38,0x08,0x00,0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00,//Y57
  0x10,0x08,0x08,0x08,0xC8,0x38,0x08,0x00,0x20,0x38,0x26,0x21,0x20,0x20,0x18,0x00,//Z58
  0x00,0x00,0x00,0xFE,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x7F,0x40,0x40,0x40,0x00,//[59
  0x00,0x0C,0x30,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x06,0x38,0xC0,0x00,//\60
  0x00,0x02,0x02,0x02,0xFE,0x00,0x00,0x00,0x00,0x40,0x40,0x40,0x7F,0x00,0x00,0x00,//]61
  0x00,0x00,0x04,0x02,0x02,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//^62
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,//_63
  0x00,0x02,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//`64
  0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x19,0x24,0x22,0x22,0x22,0x3F,0x20,//a65
  0x08,0xF8,0x00,0x80,0x80,0x00,0x00,0x00,0x00,0x3F,0x11,0x20,0x20,0x11,0x0E,0x00,//b66
  0x00,0x00,0x00,0x80,0x80,0x80,0x00,0x00,0x00,0x0E,0x11,0x20,0x20,0x20,0x11,0x00,//c67
  0x00,0x00,0x00,0x80,0x80,0x88,0xF8,0x00,0x00,0x0E,0x11,0x20,0x20,0x10,0x3F,0x20,//d68
  0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1F,0x22,0x22,0x22,0x22,0x13,0x00,//e69
  0x00,0x80,0x80,0xF0,0x88,0x88,0x88,0x18,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//f70
  0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x6B,0x94,0x94,0x94,0x93,0x60,0x00,//g71
  0x08,0xF8,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x3F,0x21,0x00,0x00,0x20,0x3F,0x20,//h72
  0x00,0x80,0x98,0x98,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//i73
  0x00,0x00,0x00,0x80,0x98,0x98,0x00,0x00,0x00,0xC0,0x80,0x80,0x80,0x7F,0x00,0x00,//j74
  0x08,0xF8,0x00,0x00,0x80,0x80,0x80,0x00,0x20,0x3F,0x24,0x02,0x2D,0x30,0x20,0x00,//k75
  0x00,0x08,0x08,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//l76
  0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x20,0x3F,0x20,0x00,0x3F,0x20,0x00,0x3F,//m77
  0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x3F,0x21,0x00,0x00,0x20,0x3F,0x20,//n78
  0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00,//o79
  0x80,0x80,0x00,0x80,0x80,0x00,0x00,0x00,0x80,0xFF,0xA1,0x20,0x20,0x11,0x0E,0x00,//p80
  0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x0E,0x11,0x20,0x20,0xA0,0xFF,0x80,//q81
  0x80,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x20,0x20,0x3F,0x21,0x20,0x00,0x01,0x00,//r82
  0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x33,0x24,0x24,0x24,0x24,0x19,0x00,//s83
  0x00,0x80,0x80,0xE0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x1F,0x20,0x20,0x00,0x00,//t84
  0x80,0x80,0x00,0x00,0x00,0x80,0x80,0x00,0x00,0x1F,0x20,0x20,0x20,0x10,0x3F,0x20,//u85
  0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,0x00,0x01,0x0E,0x30,0x08,0x06,0x01,0x00,//v86
  0x80,0x80,0x00,0x80,0x00,0x80,0x80,0x80,0x0F,0x30,0x0C,0x03,0x0C,0x30,0x0F,0x00,//w87
  0x00,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x31,0x2E,0x0E,0x31,0x20,0x00,//x88
  0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,0x80,0x81,0x8E,0x70,0x18,0x06,0x01,0x00,//y89
  0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x21,0x30,0x2C,0x22,0x21,0x30,0x00,//z90
  0x00,0x00,0x00,0x00,0x80,0x7C,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x3F,0x40,0x40,//{91
  0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,//|92
  0x00,0x02,0x02,0x7C,0x80,0x00,0x00,0x00,0x00,0x40,0x40,0x3F,0x00,0x00,0x00,0x00,//}93
  0x00,0x06,0x01,0x01,0x02,0x02,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//~94

};

//数据水平，字节垂直且倒序
const unsigned char LIBLOGO60x58[480] = {
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XC0,0X60,0X20,0X10,0X18,0X88,0XC8,0X6C,
0X7C,0XFC,0XFC,0XFC,0XFE,0XFC,0XFC,0XFC,0XFC,0XFC,0X7C,0X38,0X08,0X10,0X30,0X60,
0XC0,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XF8,0X0E,0X03,
0X00,0X80,0XD8,0XD0,0X98,0X11,0X00,0X00,0X04,0X26,0X21,0X20,0X21,0X41,0XC3,0X0F,
0X01,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X03,0X06,0X3C,0XF0,0X00,0X00,0X00,
0X00,0X00,0X00,0X80,0XCB,0X7D,0X00,0X00,0X1E,0X23,0X23,0X21,0X20,0X1F,0X00,0X00,
0X6C,0X8E,0X9E,0X8E,0X82,0X44,0X38,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X40,0X40,
0X80,0X00,0X00,0X00,0X83,0XFE,0X00,0X00,0X00,0X00,0X00,0X1F,0X31,0XE0,0X80,0X00,
0X00,0X00,0X08,0X18,0X30,0X71,0X31,0X10,0X10,0X10,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X13,0X90,0X88,0X67,0X20,0X18,0X07,0X00,0X00,0X00,
0X00,0X00,0X00,0X80,0X80,0XC0,0XE1,0X73,0XF2,0XFC,0X0C,0X08,0X10,0X10,0X30,0X20,
0XC0,0XE0,0X40,0XE0,0XC0,0XE0,0X30,0X18,0X08,0X08,0X1C,0X36,0XF2,0XB1,0X61,0XE1,
0XC0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
};




/***********************************************************************************
向 SDA 中写入一个 8 位二进制数据：

写入数据，要先将 DC 拉高；然后按照“OLED 引脚时序图”所说的那样
写入数据。先将 SCL 拉高，然后进入循环，在循环中判断要写的数据最高位是
0 还是 1，来决定在 SDA 上输出 0 还是 1。data&0x80 就是 data 与 1000 0000
作与运算，得到的结果是 data 的最高位。然后将 SCL 拉高，制造上升沿，这
样 OLED 就读取了数据的第一位，然后再将 SCL 拉低，并将 data 左移一位为
获取 data 的第二位做准备。这样就完成了一个循环，并让 OLED 读取了一位
数据，循环 8 次就可向 OLED 写入一个 8 位数据.
************************************************************************************/
void LCD_WrDat(unsigned char data)
{
	unsigned char i = 8;
	SLCD_DC;
	RLCD_SCL;
	while (i--)
	{
		if (data & 0x80) { SLCD_SDA; }
		else { RLCD_SDA; }
		SLCD_SCL;
		asm("nop");
		RLCD_SCL;
		data <<= 1;
	}
}

/*************************************************************************************
向 SDA 中写入一个 8 位二进制命令

这个程序与上面的基本一样，只是因为写入的是命令，所以要将 DC 拉低。
这两个函数在软件中控制各个引脚电平的高低，基于 SPI 协议，实现了单
片机与 OLED 之间的通讯，我们一般把这种方法称为软件 SPI；
硬件 SPI 则是利用单片机内置的 SPI 通讯模块，只需要进行少量必要的寄
存器配置，就可以使单片机内置的模块替我们完成时钟给予、数据移位、数据
收发等操作，这样的方法由于不需要 CPU 控制，所以更加节省运算资源，有兴
趣的同学可以自己研究。
************************************************************************************/
void LCD_WrCmd(unsigned char cmd)
{
	unsigned char i = 8;
	RLCD_DC;
	RLCD_SCL;
	while (i--)
	{
		if (cmd & 0x80) { SLCD_SDA; }
		else { RLCD_SDA; }
		SLCD_SCL;
		asm("nop");
		RLCD_SCL;
		cmd <<= 1;
	}
}

/************************************************************************************
 设置显示屏显示的位置（就是上面讲的设置要写入的寄存器的地址）
************************************************************************************/
void LCD_Set_Pos(unsigned char x, unsigned char y)
{
	LCD_WrCmd(0xb0 + y);//设置页 oxb0是指令前缀
	LCD_WrCmd(((x & 0xf0) >> 4) | 0x10);
	LCD_WrCmd((x & 0x0f) | 0x01);   /*这两行是设置行，要设置列地址为
									 x，则要写入 0001 0000 | (x 的高四位)，即 0001 0000 | ((x&1111 0000)>>4）, 也
									就是 ((x&0xf0)>>4)|0x10；还要写入 0000 0000 | (x 的低四位)，即 0000 0000 |
									(x&0000 1111)，也就是 (x&0x0f)|0x01。*/
}


/**********************************************************************************
为一个页面的寄存器中存要存的数据

y 为页地址，从 000 111。根据上个函数的学习可以看出循环中首先设置页地
址为 y，列地址为 0000 0001。然后将页地址相同的 128 列的寄存器全部写入
bmp_data。这里要特别说明一下。在初始化过程中寻址模式为页寻址模式，也
就是每写入一个 8 位数据，列地址会自动增 1，页地址不变。所以 x 的 fo
r 循环里可以把 0000 0000 1111 1111 列的 y 页全部写入bmp_data。
**********************************************************************************/
void LCD_Fill(unsigned char bmp_data)
{
	unsigned char y, x;
	for (y = 0; y < 8; y++)
	{
		LCD_WrCmd(0xb0 + y);
		LCD_WrCmd(0x01);   //低四位
		LCD_WrCmd(0x10);   //高四位
		for (x = 0; x < X_WIDTH; x++)  //页寻址模式！！
			LCD_WrDat(bmp_data);
	}
}

/**********************************************************************************
清屏（灭掉所有像素点）
***********************************************************************************/
void LCD_CLS(void)
{
	unsigned char y, x;
	for (y = 0; y < 8; y++)
	{
		LCD_WrCmd(0xb0 + y);
		LCD_WrCmd(0x01);
		LCD_WrCmd(0x10);
		for (x = 0; x < X_WIDTH; x++)
			LCD_WrDat(0);
	}
}


void LCD_DLY_ms(unsigned int ms)
{
	unsigned int a;
	while (ms)
	{
		a = 1335;
		while (a--);
		ms--;
	}
	return;
}

void LCD_Init(void)
{
	//-----端口初始化----//
	gpio_init(PTC16, GPO, 0);
	gpio_init(PTC17, GPO, 0);
	gpio_init(PTC18, GPO, 1);
	gpio_init(PTC19, GPO, 0);
	SLCD_SCL;
	RLCD_RST;
	LCD_DLY_ms(50);
	SLCD_RST;

	LCD_WrCmd(0xae);//--turn off oled panel
	LCD_WrCmd(0x00);//---set low column address
	LCD_WrCmd(0x10);//---set high column address
	LCD_WrCmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	LCD_WrCmd(0x81);//--set contrast control register
	LCD_WrCmd(0xc8); // Set SEG Output Current Brightness
	LCD_WrCmd(0xa1);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	LCD_WrCmd(0xc8);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	LCD_WrCmd(0xa6);//--set normal display
	LCD_WrCmd(0xa8);//--set multiplex ratio(1 to 64)
	LCD_WrCmd(0x3f);//--1/64 duty
	LCD_WrCmd(0xd3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	LCD_WrCmd(0x00);//-not offset
	LCD_WrCmd(0xd5);//--set display clock divide ratio/oscillator frequency
	LCD_WrCmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
	LCD_WrCmd(0xd9);//--set pre-charge period
	LCD_WrCmd(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	LCD_WrCmd(0xda);//--set com pins hardware configuration
	LCD_WrCmd(0x12);
	LCD_WrCmd(0xdb);//--set vcomh
	LCD_WrCmd(0x40);//Set VCOM Deselect Level
	LCD_WrCmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
	LCD_WrCmd(0x00);//
	LCD_WrCmd(0x8d);//--set Charge Pump enable/disable
	LCD_WrCmd(0x14);//--set(0x10) disable
	LCD_WrCmd(0xa4);// Disable Entire Display On (0xa4/0xa5)
	LCD_WrCmd(0xa6);// Disable Inverse Display On (0xa6/a7)
	LCD_WrCmd(0xaf);//--turn on oled panel
	LCD_Fill(0x00);  //初始清屏
	LCD_Set_Pos(0, 0);
}
//==============================================================
//函数名： void LCD_PutPixel(unsigned char x,unsigned char y)
//功能描述：绘制一个点（x,y）
//参数：真实坐标值(x,y),x的范围0～127，y的范围0～64
//返回：无
//==============================================================
void LCD_PutPixel(unsigned char x, unsigned char y)
{
	unsigned char data1;  //data1当前点的数据
	LCD_Set_Pos(x, (unsigned char)(y >> 3));//将 y 除以 8 取整数部分
	data1 = (unsigned char)(0x01 << (y % 8));//呆头
	//LCD_WrCmd((unsigned char)(0xb0+(y>>3)));   //这三行我觉得没什么必要  前面已经写过寄存器了
	//LCD_WrCmd((unsigned char)(((x&0xf0)>>4)|0x10));
	//LCD_WrCmd((unsigned char)((x&0x0f)|0x00));
	LCD_WrDat(data1);
}


/******************************************************
画点函数  感觉是多余的  但是其中有值得学习的东西
********************************************************/
uint8 OLED_GRAM[128][8];
void OLED_Refresh_Gram(void)
{
	uint8 i, n;
	for (i = 0; i < 8; i++)
	{
		LCD_WrCmd(0xb0 + i);
		LCD_WrCmd(0x00);
		LCD_WrCmd(0x10);
		for (n = 0; n < 128; n++)LCD_WrDat(OLED_GRAM[n][i]);
	}
}

void OLED_DrawPoint(uint8 x, uint8 y, uint8 t)
{
	uint8 pos, bx, temp = 0;
	if (x > 127 || y > 63)return;
	pos = 7 - y / 8;
	bx = y % 8;
	temp = 1 << (7 - bx);
	if (t)OLED_GRAM[x][pos] |= temp;
	else OLED_GRAM[x][pos] &= ~temp;
}
//==============================================================
//函数名： void LCD_Rectangle(unsigned char x1,unsigned char y1,
//                   unsigned char x2,unsigned char y2,unsigned char color,unsigned char gif)
//功能描述：绘制一个实心矩形
//参数：左上角坐标（x1,y1）,右下角坐标（x2，y2）
//      其中x1、x2的范围0～127，y1，y2的范围0～63，即真实坐标值
//返回：无
//==============================================================
void LCD_Rectangle(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char gif)
{
	unsigned char n;
	LCD_Set_Pos(x1, y1 >> 3);
	for (n = x1; n <= x2; n++)
	{
		LCD_WrDat(0x01 << (y1 % 8));
		if (gif == 1) 	LCD_DLY_ms(50);
	}
	LCD_Set_Pos(x1, y2 >> 3);
	for (n = x1; n <= x2; n++)
	{
		LCD_WrDat(0x01 << (y2 % 8));
		if (gif == 1) 	LCD_DLY_ms(5);
	}
}
//==============================================================
//函数名：LCD_P6x8Str(unsigned char x,unsigned char y,unsigned char *p)
//功能描述：写入一组标准ASCII字符串
//参数：显示的位置（x,y），y为页范围0～7，要显示的字符串
//返回：无
//==============================================================
void LCD_P6x8Str(unsigned char x, unsigned char y, unsigned char ch[])
{
	unsigned char c = 0, i = 0, j = 0;
	while (ch[j] != '\0')
	{
		c = ch[j] - 32;
		if (x > 126) { x = 0; y++; }
		LCD_Set_Pos(x, y);
		for (i = 0; i < 6; i++)
			LCD_WrDat(F6x8[c][i]);
		x += 6;
		j++;
	}
}
//==============================================================
//函数名：LCD_P8x16Str(unsigned char x,unsigned char y,unsigned char *p)
//功能描述：写入一组标准ASCII字符串
//参数：显示的位置（x,y），y为页范围0～7，要显示的字符串
//返回：无
//==============================================================
void LCD_P8x16Str(unsigned char x, unsigned char y, unsigned char ch[])
{
	unsigned char c = 0, i = 0, j = 0;

	while (ch[j] != '\0')
	{
		c = ch[j] - 32;
		if (x > 120) { x = 0; y++; }
		LCD_Set_Pos(x, y);
		for (i = 0; i < 8; i++)
			LCD_WrDat(F8X16[c * 16 + i]);
		LCD_Set_Pos(x, y + 1);
		for (i = 0; i < 8; i++)
			LCD_WrDat(F8X16[c * 16 + i + 8]);
		x += 8;
		j++;
	}
}

//输出汉字字符串
void LCD_P14x16Str(unsigned char x, unsigned char y, unsigned char ch[])
{
	unsigned char wm = 0, ii = 0;
	unsigned int adder = 1;

	while (ch[ii] != '\0')
	{
		wm = 0;
		adder = 1;
		while (F14x16_Idx[wm] > 127)
		{
			if (F14x16_Idx[wm] == ch[ii])
			{
				if (F14x16_Idx[wm + 1] == ch[ii + 1])
				{
					adder = wm * 14;
					break;
				}
			}
			wm += 2;
		}
		if (x > 118) { x = 0; y++; }
		LCD_Set_Pos(x, y);
		if (adder != 1)// 显示汉字
		{
			LCD_Set_Pos(x, y);
			for (wm = 0; wm < 14; wm++)
			{
				LCD_WrDat(F14x16[adder]);
				adder += 1;
			}
			LCD_Set_Pos(x, y + 1);
			for (wm = 0; wm < 14; wm++)
			{
				LCD_WrDat(F14x16[adder]);
				adder += 1;
			}
		}
		else			  //显示空白字符
		{
			ii += 1;
			LCD_Set_Pos(x, y);
			for (wm = 0; wm < 16; wm++)
			{
				LCD_WrDat(0);
			}
			LCD_Set_Pos(x, y + 1);
			for (wm = 0; wm < 16; wm++)
			{
				LCD_WrDat(0);
			}
		}
		x += 14;
		ii += 2;
	}
}

//输出汉字和字符混合字符串
void LCD_Print(unsigned char x, unsigned char y, unsigned char ch[])
{
	unsigned char ch2[3];
	unsigned char ii = 0;
	while (ch[ii] != '\0')
	{
		if (ch[ii] > 127)
		{
			ch2[0] = ch[ii];
			ch2[1] = ch[ii + 1];
			ch2[2] = '\0';			//汉字为两个字节
			LCD_P14x16Str(x, y, ch2);	//显示汉字
			x += 14;
			ii += 2;
		}
		else
		{
			ch2[0] = ch[ii];
			ch2[1] = '\0';			//字母占一个字节
			LCD_P8x16Str(x, y, ch2);	//显示字母
			x += 8;
			ii += 1;
		}
	}
}

//==============================================================
//函数名： void Draw_BMP(unsigned char x,unsigned char y)
//功能描述：显示BMP图片128×64
//参数：起始点坐标(x,y),x的范围0～127，y为页的范围0～64
//返回：无
//==============================================================
void Draw_BMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, const unsigned char* bmp)
{
	unsigned int ii = 0;
	unsigned char x, y;
	if (y1 % 8 == 0) y1 = y1 / 8;
	else y1 = y1 / 8 + 1;
	if (y0 % 8 == 0) y0 = y0 / 8;
	else y0 = y0 / 8 + 1;
	for (y = y0; y < y1; y++)
	{
		LCD_Set_Pos(x0, y);
		for (x = x0; x < x1; x++)
		{
			LCD_WrDat(bmp[ii++]);
		}
	}
}


void Draw_LQLogo(void)//画图
{
	unsigned int ii = 0;
	unsigned char x, y;

	for (y = 0; y < 8; y++)
	{
		LCD_Set_Pos(5, y);
		for (x = 14; x < 104; x++)
		{
			LCD_WrDat(LIBLOGO60x58[ii++]);
		}
	}
}



void LCD_PrintU16(unsigned char x, unsigned char y, unsigned int num)
{
	unsigned char tmp[6], i;
	tmp[5] = 0;
	tmp[4] = (unsigned char)(num % 10 + 0x30);
	tmp[3] = (unsigned char)(num / 10 % 10 + 0x30);
	tmp[2] = (unsigned char)(num / 100 % 10 + 0x30);
	tmp[1] = (unsigned char)(num / 1000 % 10 + 0x30);
	tmp[0] = (unsigned char)(num / 10000 % 10 + 0x30);

	for (i = 0; i < 4; i++)
	{
		if (tmp[i] == '0')
			tmp[i] = ' ';
		else
			break;
	}

	LCD_P6x8Str(x, y, tmp);

}
void LCD_Print16(unsigned char x, unsigned char y, int num)
{
	unsigned char tmp[6], i;
	tmp[5] = 0;
	//num *= 10;
	if (num < 0)
	{
		tmp[0] = '-';
		num = -num;
		tmp[4] = (unsigned char)((int)num % 10 + 0x30);
		tmp[3] = (unsigned char)((int)num / 10 % 10 + 0x30);
		tmp[2] = (unsigned char)((int)num / 100 % 10 + 0x30);
		tmp[1] = (unsigned char)((int)num / 1000 % 10 + 0x30);

	}
	else
	{
		tmp[0] = ' ';
		tmp[4] = (unsigned char)((int)num % 10 + 0x30);
		tmp[3] = (unsigned char)((int)num / 10 % 10 + 0x30);
		tmp[2] = (unsigned char)((int)num / 100 % 10 + 0x30);
		tmp[1] = (unsigned char)((int)num / 1000 % 10 + 0x30);
	}
	for (i = 0; i < 4; i++)
	{
		if (tmp[i] == '0')
			tmp[i] = ' ';
		else
			break;
	}
	LCD_P6x8Str(x, y, tmp);
}

void LCD_write_char(uint8 X, uint8 Y, uint16 c)
{

	uint8 line;
	LCD_Set_Pos(X, Y);
	c -= 32;
	for (line = 0; line < 6; line++)
	{
		LCD_WrDat(F6x8[c][line]);
	}
}
void LCD_Show_Number1(uint8 X, uint8 Y, uint16 number)
{
	LCD_write_char(X, Y, number + 48);
}

void LCD_Show_Number2(uint8 X, uint8 Y, uint16 number)
{
	LCD_write_char(X, Y, number / 10 + 48);
	LCD_write_char(X + 6, Y, number % 10 + 48);
}

void LCD_Show_Number3(uint8 X, uint8 Y, uint16 number)
{
	LCD_write_char(X, Y, number / 100 + 48);
	LCD_write_char(X + 6, Y, number % 100 / 10 + 48);
	LCD_write_char(X + 12, Y, number % 10 + 48);
}


void LCD_Show_Number4(uint8 X, uint8 Y, uint16 number)
{
	LCD_write_char(X, Y, number / 1000 + 48);
	LCD_write_char(X + 6, Y, number % 1000 / 100 + 48);
	LCD_write_char(X + 12, Y, number % 100 / 10 + 48);
	LCD_write_char(X + 18, Y, number % 10 + 48);
}

void OLED_PrintFloat(unsigned char x, unsigned char y, float num)
{
	int n = 0, i = 0;          //n用于 c 字符串的计数 , i 用于 b 字符串的计数
	unsigned char b[15], c[15]; // c [15]    b 为辅助字符串 ，c 为最后输出的字符串
	int integer1; // 用于存放num的整数部分
	for (int k = 0; k < 15; k++)// 将数组 c 全部刷入 ’\0 ’( 结束符 )
		c[k] = '\0';
	if (num < 0) // 若果是负数
	{
		c[n] = '-'; //在c 中写入负号
		n++;// 计数的n增1
		num = -num; //将num变为正数
	}
	integer1 = (int)num; //取 num的整数部分
	while ((integer1 / 10) > 0) //如果 integer1 >=10
	{
		b[i] = integer1 % 10 + '0'; //取 integer1 的最低位的 ascll 码
		i++;
		integer1 /= 10;
	}
	b[i] = integer1 + '0'; // 经过上述过程 ， 将num整数部分从低位到高位写入 b [0]~ b [15]
	//( 此时 b 中存的数是 integer1 的倒序 )
	for (int x = 0; x <= i; n++, x++)//将b 反向写入 c
	{
		c[n] = b[i - x];
	}
	c[n] = '.'; n++;               //写入 ’. ’
	int print_float = 0;
	// 如果小数部分未完全写入 c， 且小数位不足 4位
	while (((int)num - num) != 0 && print_float <= 3)
	{
		num = num * 10; // 以下为将小数位写入 c
		c[n] = ((int)num) % 10 + '0';
		n++;
		print_float++;
	}
	LCD_P6x8Str(x, y, c);    // 显示数组 c
}


void LCD_ClrDot(unsigned char x)
{
	LCD_Set_Pos(x, 6);
	LCD_WrCmd((unsigned char)(0xb0 + 6));
	LCD_WrCmd((unsigned char)(((x & 0xf0) >> 4) | 0x10));
	LCD_WrCmd((unsigned char)((x & 0x0f) | 0x00));
	LCD_WrDat(0x00);

	LCD_Set_Pos(x, 7);
	LCD_WrCmd((unsigned char)(0xb0 + 7));
	LCD_WrCmd((unsigned char)(((x & 0xf0) >> 4) | 0x10));
	LCD_WrCmd((unsigned char)((x & 0x0f) | 0x00));
	LCD_WrDat(0x00);
}

/*
void LCD_show_image()
{
  for(int j=0;j<CAMERA_H;j++)
  {
	for(int i=0;i<CAMERA_W;i++)
	{
	  if( img[j][i]!=0)
	  {
		OLED_DrawPoint(80-i,j,1);
	  }
	  else
	  {
		OLED_DrawPoint(80-i,j,0);
	  }
	}
  }
  OLED_Refresh_Gram();
}*/

void LCD_show_ZZF_image()
{
	for (int i = 0; i < ROW; i++)//100
	{
		if (i % 2 == 0)
		{
			for (int j = 0; j < COL; j++)//150
			{
				if (j % 2 == 0)
				{
					if (image_buff[i][j] > 120)
					{
						OLED_DrawPoint(j / 2, 64 - i / 2, 1);
					}
					else
					{
						OLED_DrawPoint(j / 2, 64 - i / 2, 0);
					}
				}
			}
		}
	}
	OLED_Refresh_Gram();
}
void LCD_show_ZZF_image_t(unsigned char t)
{
	for (int i = 0; i < ROW; i++)//100
	{
		if (i % 2 == 0)
		{
			for (int j = 0; j < COL; j++)//150
			{
				if (j % 2 == 0)
				{
					if (image_buff[i][j] > t)
					{
						OLED_DrawPoint(j / 2, 64 - i / 2, 1);
					}
					else
					{
						OLED_DrawPoint(j / 2, 64 - i / 2, 0);
					}
				}
				
			}
			OLED_DrawPoint(MiddleLine[i / 2]-2, 64 - i / 2, 0);
			OLED_DrawPoint(MiddleLine[i / 2]-1, 64 - i / 2, 1);
			OLED_DrawPoint(MiddleLine[i / 2], 64 - i / 2, 0);
			OLED_DrawPoint(MiddleLine[i / 2]+1, 64 - i / 2, 1);
			OLED_DrawPoint(MiddleLine[i / 2] + 2, 64 - i / 2, 0);

		}
		
	}
	OLED_Refresh_Gram();
}

void LCD_show_ZZF_image1()
{
	for (int i = 0; i < ROW; i++)//100
	{
		if (i % 2 == 0)
		{
			for (int j = 0; j < COL; j++)//150
			{
				if (j % 2 == 0)
				{
					if (image[i][j] > 0)
					{
						OLED_DrawPoint(j / 2, 64 - i / 2, 1);
					}
					else
					{
						OLED_DrawPoint(j / 2, 64 - i / 2, 0);
					}
					if (BlackLineData[i] == j)
					{
						OLED_DrawPoint(j / 2, 64 - i / 2, 0);
					}
				}
			}
		}
	}
	OLED_Refresh_Gram();
}

void LCD_draw_bmp()
{
	Draw_BMP(0, 0, 90, 64, cl);
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      OLED显示图像
//  @param      high  		图像高度
//  @param      width 		图像宽度
//  @param      *p    		图像地址（数组）
//  @param      value 		二值化阀值
//  @return     void
//  @since      v1.0
//  Sample usage:			
//-------------------------------------------------------------------------------------------------------------------
void dis_bmp(uint16 high, uint16 width, uint8 * p, uint8 value)
{
	int16 i, j;
	int16 temp, temp1;
	uint8 dat;


	temp1 = high % 8;
	if (temp1 == 0) temp = high / 8;
	else           temp = high / 8 + 1;


	for (i = 0; i < temp; i++)
	{
		LCD_Set_Pos(0, i);
		for (j = 0; j < width; j++)
		{
			dat = 0;
			if (i < (temp - 1) || !temp1 || temp1 >= 1)dat |= (*(p + i * 8 * width + j + width * 0) > value ? 1 : 0) << 0;
			if (i < (temp - 1) || !temp1 || temp1 >= 2)dat |= (*(p + i * 8 * width + j + width * 1) > value ? 1 : 0) << 1;
			if (i < (temp - 1) || !temp1 || temp1 >= 3)dat |= (*(p + i * 8 * width + j + width * 2) > value ? 1 : 0) << 2;
			if (i < (temp - 1) || !temp1 || temp1 >= 4)dat |= (*(p + i * 8 * width + j + width * 3) > value ? 1 : 0) << 3;
			if (i < (temp - 1) || !temp1 || temp1 >= 5)dat |= (*(p + i * 8 * width + j + width * 4) > value ? 1 : 0) << 4;
			if (i < (temp - 1) || !temp1 || temp1 >= 6)dat |= (*(p + i * 8 * width + j + width * 5) > value ? 1 : 0) << 5;
			if (i < (temp - 1) || !temp1 || temp1 >= 7)dat |= (*(p + i * 8 * width + j + width * 6) > value ? 1 : 0) << 6;
			if (i < (temp - 1) || !temp1 || temp1 >= 8)dat |= (*(p + i * 8 * width + j + width * 7) > value ? 1 : 0) << 7;

			LCD_WrDat(dat);
		}
	}
}