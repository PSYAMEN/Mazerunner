#include "CSimpleFrameBuffer.h"
#include <stdarg.h>

unsigned long long const CSimpleFrameBuffer::font8[]= // This is the 256 character, 6*8 font data. Each long long represents 1 character, with 1 bit per pixel and 8 bits per line
    { 0x0000E0E0E0000000ULL, 0x0000E0E0E0000000ULL, 0x0000E0E0E0000000ULL, 0x0000E0E0E0000000ULL, 0x0000E0E0E0000000ULL, 0x0000E0E0E0000000ULL, 0x0000E0E0E0000000ULL, 0x0000E0E0E0000000ULL, 0x0000E0E0E0000000ULL, 0x0000E0E0E0000000ULL, 0x0000000000000000ULL, 0x0000E0E0E0000000ULL, 0x0000E0E0E0000000ULL, 0x0000E0E0E0101000ULL, 0x0000E0E0E0000000ULL, 0x0000E0E0E0000000ULL, 
      0x0000E0E0E0000000ULL, 0x0000E0E0E0000000ULL, 0x0000E0E0E0000000ULL, 0x0000E0E0E0000000ULL, 0x0000E0E0E0000000ULL, 0x0000E0E0E0000000ULL, 0x0000E0E0E0000000ULL, 0x0000E0E0E0000000ULL, 0x0000E0E0E0000000ULL, 0x0000E0E0E0000000ULL, 0x0000E0E0E0000000ULL, 0x0000E0E0E0000000ULL, 0x80C0E1F1E1C08000ULL, 0xF1F1F1F1F1F1F100ULL, 0x0000000000515100ULL, 0x0000000000515100ULL, 
      0x0000000000000000ULL, 0x4040404040004000ULL, 0xA0A0A00000000000ULL, 0xA0A0F1A0F1A0A000ULL, 0x40E150E041F04000ULL, 0x3031804020918100ULL, 0x2050502051906100ULL, 0x4040400000000000ULL, 0x8040202020408000ULL, 0x2040808080402000ULL, 0x00A040F140A00000ULL, 0x004040F140400000ULL, 0x0000000060604020ULL, 0x000000F100000000ULL, 0x0000000000606000ULL, 0x0001804020100000ULL, 
      0xE01191513111E000ULL, 0x406040404040E000ULL, 0xE01101C02010F100ULL, 0xE01101E00111E000ULL, 0x80C0A090F1808000ULL, 0xF110F0010111E000ULL, 0xC02010F01111E000ULL, 0xF101804020202000ULL, 0xE01111E01111E000ULL, 0xE01111E101806000ULL, 0x0060600060600000ULL, 0x0060600060604020ULL, 0x8040201020408000ULL, 0x0000F100F1000000ULL, 0x1020408040201000ULL, 0xE011018040004000ULL, 
      0xE01151D15010E100ULL, 0xE01111F111111100ULL, 0xF01111F01111F000ULL, 0xE01110101011E000ULL, 0x7090111111907000ULL, 0xF11010F01010F100ULL, 0xF11010F010101000ULL, 0xE01110109111E100ULL, 0x111111F111111100ULL, 0xE04040404040E000ULL, 0x010101011111E000ULL, 0x1190503050901100ULL, 0x101010101010F100ULL, 0x11B1515111111100ULL, 0x1111315191111100ULL, 0xE01111111111E000ULL, 
      0xF01111F010101000ULL, 0xE011111151906100ULL, 0xF01111F050901100ULL, 0xE01110E00111E000ULL, 0xF140404040404000ULL, 0x111111111111E000ULL, 0x111111A0A0404000ULL, 0x1111115151B11100ULL, 0x1111A040A0111100ULL, 0x1111A04040404000ULL, 0xF10180402010F100ULL, 0xE02020202020E000ULL, 0x0010204080010000ULL, 0xE08080808080E000ULL, 0x40A0110000000000ULL, 0x000000000000F100ULL, 
      0x2020400000000000ULL, 0x0000E001E111E100ULL, 0x1010F0111111F000ULL, 0x0000E1101010E100ULL, 0x0101E1111111E100ULL, 0x0000E011F110E000ULL, 0x40A0207020202000ULL, 0x0000E01111E101E0ULL, 0x1010F01111111100ULL, 0x400060404040E000ULL, 0x8000C08080809060ULL, 0x1010905030509000ULL, 0x604040404040E000ULL, 0x0000B05151511100ULL, 0x0000F01111111100ULL, 0x0000E0111111E000ULL, 
      0x0000F01111F01010ULL, 0x0000E11111E10101ULL, 0x0000D13010101000ULL, 0x0000E110E001F000ULL, 0x2020702020A04000ULL, 0x000011111111E100ULL, 0x0000111111A04000ULL, 0x000011115151A000ULL, 0x000011A040A01100ULL, 0x0000111111E101E0ULL, 0x0000F1804020F100ULL, 0xC02020102020C000ULL, 0x4040404040404000ULL, 0x6080800180806000ULL, 0x0000205180000000ULL, 0x51A051A051A05100ULL, 
      0x000180406090F100ULL, 0xF10011A040A01100ULL, 0x00F111A0A0400000ULL, 0xC140404050604000ULL, 0x8041404040502000ULL, 0xF12140804021F100ULL, 0x3070F0F1F0703000ULL, 0x0000F1A0A0A0A000ULL, 0x204080E11111E000ULL, 0x01804020F100F100ULL, 0x10204080F100F100ULL, 0x0080F140F1200000ULL, 0x0000006190906100ULL, 0x004080F180400000ULL, 0x004020F120400000ULL, 0x4040404051E04000ULL, 
      0x40E0514040404000ULL, 0x0000215180808000ULL, 0x402040E090906000ULL, 0x0000E010F010E000ULL, 0x0000A05141410101ULL, 0x609090F090906000ULL, 0x0010102040A01100ULL, 0x00C02121E0202010ULL, 0x0000E19090906000ULL, 0x0000E15040418000ULL, 0x000090115151A000ULL, 0x000040A011F10000ULL, 0xF1A0A0A0A0A0A000ULL, 0xE011111111A0B100ULL, 0x0000E0E0E0000000ULL, 0x0000A05151A00000ULL, 
      0xC02170207021C000ULL, 0x4000404040404000ULL, 0x0040E15050E14000ULL, 0xC02120702020F100ULL, 0x11E0111111E01100ULL, 0x1111A0F140F14000ULL, 0x4040400040404000ULL, 0xC020E011E0806000ULL, 0xA000000000000000ULL, 0xE01171317111E000ULL, 0x6080E09060F00000ULL, 0x0041A050A0410000ULL, 0x000000F080000000ULL, 0x000000F000000000ULL, 0xE0117171B111E000ULL, 0xF100000000000000ULL, 
      0xE0A0E00000000000ULL, 0x004040F14040F100ULL, 0xE080E020E0000000ULL, 0xE080E080E0000000ULL, 0x8040000000000000ULL, 0x0000009090907110ULL, 0xE171716141416100ULL, 0x0000006060000000ULL, 0x0000000000408060ULL, 0x604040E000000000ULL, 0xE01111E000F10000ULL, 0x0050A041A0500000ULL, 0x1090502051C10100ULL, 0x109050A111808100ULL, 0x3021B06071C10100ULL, 0x400040201011E000ULL, 
      0x2040E011F1111100ULL, 0x8040E011F1111100ULL, 0x40A0E011F1111100ULL, 0xA050E011F1111100ULL, 0xA000E011F1111100ULL, 0xE0A0E011F1111100ULL, 0xA15050F15050D100ULL, 0xE011101011E08060ULL, 0x2040F110F010F100ULL, 0x8040F110F010F100ULL, 0x40A0F110F010F100ULL, 0xA000F110F010F100ULL, 0x2040E0404040E000ULL, 0x8040E0404040E000ULL, 0x40A0E0404040E000ULL, 0xA000E0404040E000ULL, 
      0x60A0217121A06000ULL, 0x41A0113151911100ULL, 0x2040E0111111E000ULL, 0x8040E0111111E000ULL, 0x40A0E0111111E000ULL, 0xA050E0111111E000ULL, 0xA000E0111111E000ULL, 0x0011A040A0110000ULL, 0x01E0915131E01000ULL, 0x204011111111E000ULL, 0x804011111111E000ULL, 0x40A000111111E000ULL, 0xA00011111111E000ULL, 0x804011A040404000ULL, 0x7020E021E0207000ULL, 0xE011F01111F01010ULL, 
      0x2040E001E111E100ULL, 0x8040E001E111E100ULL, 0x40A0E001E111E100ULL, 0xA050E001E111E100ULL, 0xA000E001E111E100ULL, 0xE0A0E001E111E100ULL, 0x0000B141F150F100ULL, 0x0000E11010E18060ULL, 0x2040E011F110E000ULL, 0x8040E011F110E000ULL, 0x40A0E011F110E000ULL, 0xA000E011F110E000ULL, 0x204000604040E000ULL, 0x804000604040E000ULL, 0x40A000604040E000ULL, 0xA00000604040E000ULL, 
      0x80C180E090906000ULL, 0x41A000F011111100ULL, 0x204000E01111E000ULL, 0x804000E01111E000ULL, 0x40A000E01111E000ULL, 0x41A000E01111E000ULL, 0xA00000E01111E000ULL, 0x004000F100400000ULL, 0x000061905121D000ULL, 0x204000111111E100ULL, 0x804000111111E100ULL, 0x40A000111111E100ULL, 0xA00000111111E100ULL, 0x8040001111E101E0ULL, 0x0010709090701010ULL, 0xA000001111E101E0ULL };

  static char const s16[] = "0123456789ABCDEF"; 
  template <typename T>
  static int base(T n, char *b, int base)
  {
    int neg= 0; if (n<0) { *b++= '-'; n= 0-n; neg= 1; }
    int size= 0; do { b[size++]= s16[n%base]; n/=base; } while (n!=0);
    for (int i=0; i<size/2; i++) { char t= b[i]; b[i]= b[size-i-1]; b[size-i-1]= t; };
    return size+neg;
  }
  static char *NumberToHex(char *b, uint8_t n) { *b++= s16[n>>4]; *b++= s16[n&15]; return b; }
  static char *NumberToHex(char *b, uint16_t n) { return NumberToHex(NumberToHex(b, (uint8_t)(n>>8)), (uint8_t)n); }
  static char *NumberToHex(char *b, uint32_t n) { return NumberToHex(NumberToHex(b, (uint16_t)(n>>16)), (uint16_t)n); }
  static char *NumberToHex(char *b, uint64_t n) { return NumberToHex(NumberToHex(b, (uint32_t)(n>>32)), (uint32_t)n); }
char *CSimpleFrameBuffer::print(char const *format, ...)
{
  va_list argp;
  va_start(argp, format);
  char *t= print(format, argp);
  va_end(argp);
  return t;
}
char *hpsprintf2(char *buf, int bufsize, char const *format, va_list argp)
{
  char *printbuf= buf;
  while (*format != L'\0' && bufsize > 0)
  {
    if (*format != L'%') { *buf = *format; buf++; format++; bufsize--; continue; }
    if (format[1] == L'%') { *buf = '%'; buf++; format += 2; bufsize--; continue; }
    if (format[1] == 'c') { char c= (char)va_arg(argp, int); *buf= c; buf++; format += 2; bufsize--; continue; }
    if (format[1] == 'd') { int i = va_arg(argp, int); int l= base(i, buf, 10); buf+= l; bufsize-= l; format += 2; continue; }
    if (format[1] == 'u') { unsigned int i = va_arg(argp, unsigned int); int l= base(i, buf, 10); buf+= l; bufsize-= l; format += 2; continue; }
    if (format[1] == 'x') { unsigned int i = va_arg(argp, unsigned int); int l= base(i, buf, 16); buf+= l; bufsize-= l; format += 2; continue; }
    if (format[1] >= '0' && format[1] <= '8' && format[2] >= 'x') { unsigned int i = va_arg(argp, unsigned int); i &= ~((-1) << (4 * (format[1] - '0'))); int l= base(i, buf, 16); buf+= l; bufsize-= l; format += 3; continue; }
    if (format[1] == '0' && format[2] > '0' && format[2] <= '8' && format[3] >= 'x')
    {
      unsigned int i = va_arg(argp, unsigned int); i &= ~((-1) << (4 * (format[2] - '0'))); 
      int l= base(i, buf, 16); 
      int sze = format[2]-'0'-l; bufsize-= sze+l;
      while (--sze>=0) *buf++= '0';
      l= base(i, buf, 16);
      buf+= l; format += 4; continue; 
    }
    if (format[1] == 's') { char *b = va_arg(argp, char*); if (b!=NULL) { while (*b && --bufsize >= 0) *buf++ = *b++; } format += 2; continue; }
    if (format[1] == 'D')
    {
      unsigned char *b = va_arg(argp, unsigned char*); int size = va_arg(argp, int);
      if (b==NULL) { strcpy_s(buf, 100, "NULL"); bufsize-= 4, buf+= 4; format += 2; continue; }
      while (--size >= 0 && bufsize > 2) { buf= NumberToHex(buf, *b++); bufsize -= 2; }
      format += 2; continue;
    }
    if (format[1] == 'b')
    {
      format+= 2; int bytes= 1; if (*format==L'1') bytes= 1, format++; else if (*format==L'2') bytes= 2, format++; else if (*format==L'4') bytes= 4, format++; else if (*format==L'8') bytes= 8, format++;
      unsigned char *b = va_arg(argp, uint8_t*); int size = va_arg(argp, int);
      if (b==NULL) { *buf++= 'N'; *buf++= 'U'; *buf++= 'L'; *buf++= 'L'; bufsize-= 4; continue; }
      while (--size>=0 && bufsize>2*bytes+1) 
      { 
        if (bytes==1) buf= NumberToHex(buf, *b++); 
        else if (bytes==2) { uint16_t d; memcpy(&d, b, bytes); b+= bytes; buf= NumberToHex(buf, d); }
        else if (bytes==4) { uint32_t d; memcpy(&d, b, bytes); b+= bytes; buf= NumberToHex(buf, d); }
        else if (bytes==8) { uint64_t d; memcpy(&d, b, bytes); b+= bytes; buf= NumberToHex(buf, d); }
        *buf++= L' ';
        bufsize -= 2*bytes+1; }
      continue;
    }
    *buf = *format; buf++, format++; bufsize--; continue;
  }
  *buf = L'\0';
  va_end(argp);
  return printbuf;
}
char *hpsprintf(char *buf, int bufsize, char const *format, ...)
{
  va_list argp;
  va_start(argp, format);
  char *t= hpsprintf2(buf, bufsize, format, argp);
  va_end(argp);
  return t;
}
char *CSimpleFrameBuffer::print(char const *format, va_list argp)
{
  if (printbuf==NULL) printbuf= (char*)malloc(1024);
  if (printbuf==NULL) return NULL;
  return hpsprintf2(printbuf, 1024-10, format, argp);
}

void CSimpleFrameBuffer::log2(char const *s)
{
  int cols= w/6+10;
  int rows= h/8;
  if (logstrs==NULL) { logstrs= (char*)malloc(rows*cols); memset(logstrs, 0, rows*cols); }
  if (logstrs==NULL) return;
  memmove(logstrs+cols, logstrs, (rows-1)*cols);
  memcpy(logstrs, s, cols-1); logstrs[cols-1]= 0;
  for (int l= rows; --l>=0;) text(0, l*8, w, logstrs+(l*cols));
}
void CSimpleFrameBuffer::log(char const *format, ...)
{
  va_list argp; va_start(argp, format);
  log2(print(format, argp));
  va_end(argp);
}
void CSimpleFrameBuffer::line(int x1, int y1, int x2, int y2, TSFBColor color)
{
  if (x2<x1) { int t= x1; x1= x2; x2= t; t= y1; y1= y2; y2= t; } // always draw form left to right
  int DX= x2-x1, DY= y2-y1, pos= 1; // calculate delta x and y
  if (DY<0) pos=-1, DY= -DY;

  if (DX>=DY) // shallow, non-decreasing Y (e.g., 4 o'clock)
  { int er= -DX/2;
    while (x1 <= x2) { pixel(x1, y1, color); x1++; er+= DY; if (er<0) continue; er-= DX; y1+= pos; } 
  } else { // mostly vertical lines...
    int er= -DY/2;
    if (pos==1)
      while (y1<=y2) { pixel(x1, y1, color); y1++; er+= DX; if (er<0) continue; er-= DY; x1++; } 
    else
      while (y1>=y2) { pixel(x1, y1, color); y1--; er+= DX; if (er<0) continue; er-= DY; x1++; } 
  }
}
