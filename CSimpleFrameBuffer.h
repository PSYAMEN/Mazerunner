#include <stdint.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
//////////////////////////////////////////////////////////////////////////////////////
// CSimpleFrameBuffer is a very simple graphical class that is designed to handle
// simple framebuffers and bitmaps.
// 
// This code it not intended to be fast, just to be small, independent and to work!
//
// It supports creation of a bitmap either as a memory based item or to use an existing framebuffer (direct to screen for example)
//
// It supports the following graphical operations:
// Pixel operations (pixel), filled rectangle (rect), Horizontal and Vertical lines (HLine & VLine)
// writing text using a 6*8 font (with zoom factor)
// bliting of a graphic object on another (blit)
// 
// Graphic operations support trensparency (encoded in the color, 0 is opaque, 255 trensparent)
// In the case of blits, an extra trensparency affecting the whole source bitmaps can be used
// 
// You can use the Color function to generate a color (with transparency)
//
// Assuming that the screen is 320*240 and that ScreenPointer is a pointer to the screen framebuffer,
// you can cerate a CSimpleFrameBuffer representing the screen by doing:
// CSimpleFrameBuffer screen(320, 240, (TSFBColor*)ScreenPointer);
// You can then use all the drawing primitives to draw on the screen.
//
// If you want do work 'off screen' to avoid flickering, then create a 2nd framebuffer to work on
// CSimpleFrameBuffer Offscreen(screen.w, screen.h);
// Do the drawing in Offscreen and call screen.blit(Offscreen); once finish to copy the offscreen buffer to the screen

class CSimpleFrameBuffer
{ public:
  // Color manipulation helpers
  typedef uint32_t TSFBColor; // color contains alpha channel! 0 is opaque, 255 trensparent
  static TSFBColor inline rgb565toargb888(uint16_t c) { uint32_t b= c&0x1f, g= (c>>5)&0x3f, r= (c>>(5+6))&0x1f; return (r<<3)+(r>>2) + (((g<<2)+(g>>4))<<8) + (((b<<3)+(b>>2))<<16); }
  static TSFBColor inline rgb565toargb888(uint16_t c, uint32_t alpha) { return rgb565toargb888(c)+(alpha<<24); }
  static TSFBColor inline Color(int r, int g, int b, int a=0) { return (a<<24)+(r<<16)+(g<<8)+b; }
  static TSFBColor const ClTrensparent=  0xff000000;
  static TSFBColor const ClBlack=  0x000000;
  static TSFBColor const ClWhite=  0xffffff;
  static TSFBColor const ClRed=    0xff0000;
  static TSFBColor const ClGreen=  0x00ff00;
  static TSFBColor const ClBlue=   0x0000ff;
  static TSFBColor const ClCyan=   0x00ffff;
  static TSFBColor const ClYellow= 0xffff00;
  static TSFBColor const ClHPBlue= 0x0099D7;
  
  static TSFBColor Doalpha(TSFBColor c1, TSFBColor c2) // return c1 combined with c2 depending on c2's opacity
  {
    uint32_t a1= c2>>24; uint32_t a2= 255-a1;   // a1= contribution of col 1, a2= contribution of col 2
    if (a1==0) return c2; if (a2==0) return c1; // fast exits!
    uint32_t r1= c1&0xff, r2= c2&0xff, g1= (c1&0xff00)>>8, g2= (c2&0xff00)>>8, b1= (c1&0xff0000)>>16, b2= (c2&0xff0000)>>16; // extract the composants
    r1= (r1*a1+r2*a2)>>8; g1= (g1*a1+g2*a2)>>8; b1= (b1*a1+b2*a2)>>8; // calculate result
    return r1+(g1<<8)+(b1<<16); // return final
  }
  
  // Actuall bitmap stuff....
  int w, h;      // bitmap witdh and height, in case you need them...
  TSFBColor *fb; // bitmap framebuffer (freed at the end unless NoFreefb=true)
  bool NoFreefb; // if true, then does not free the fb at destruction of object
  // Constructor for offscreen bitmap
  CSimpleFrameBuffer(int w, int h): w(w), h(h), fb((TSFBColor*)malloc(w*h*sizeof(TSFBColor))), NoFreefb(false), logstrs(NULL), printbuf(NULL) {}
  // Constructor when the object does not own the framebuffer. For example for the screen iteself!
  CSimpleFrameBuffer(int w, int h, TSFBColor *fb): w(w), h(h), fb(fb), NoFreefb(true), logstrs(NULL), printbuf(NULL) {}
  ~CSimpleFrameBuffer() { if (!NoFreefb) free(fb); free(logstrs); free(printbuf); }
  // If you ever change the size/framebuffer from outside, you can use this function
  template <typename T> void update(int w, int h, T *fb) { this->w= w; this->h= h; this->fb= (TSFBColor *)fb; }

  // Drawing primitives
  void inline pixel(int x, int y, TSFBColor color) { if (x>=0 && y>=0 && x<w && y<h) fb[x+y*w]= Doalpha(fb[x+y*w], color); } // set pixel. includes alpha
  TSFBColor inline pixel(int x, int y) const { return fb[x+y*w]; } // get pixel
  void vline(int x, int y, int h, TSFBColor color) { h= h+y; while (y<h) pixel(x, y++, color); }
  void hline(int x, int w, int y, TSFBColor color) { w= w+x; while (x<w) pixel(x++, y, color); }
  void line(int x1, int y1, int x2, int y2, TSFBColor color);
  void rect(int x, int y, int w, int h, TSFBColor color) { while (--h>=0) hline(x, w, y++, color); }
  void lrect(int x, int y, int w, int h, TSFBColor color) // border of a rectangle (4 lines)
  { 
    hline(x, w, y, color); hline(x, w, y+h-1, color);
    vline(x, y+1, h-2, color); vline(x+w-1, y+1, h-2, color); 
  }
  void circle(int x, int y, int r, TSFBColor color)
  {
      x = x + r; pixel(x, y, color);
      int X, Y, E;
      X = 2 * r - 1; Y = 1; E = 0;
      while (Y < X) { E += Y; Y += 2; y++; if (E >= X) { E -= X; X -= 2; x--; }pixel(x, y, color); }
      while (X > 0) { E -= X; X -= 2; x--; if (E < 0) { E += Y; Y += 2; y++; } pixel(x, y, color); }
      X = 2 * r - 1; Y = 1; E = 0;                                             
      while (Y < X) { E += Y; Y += 2; x--; if (E >= X) { E -= X; X -= 2; y--; }pixel(x, y, color); }
      while (X > 0) { E -= X; X -= 2; y--; if (E < 0) { E += Y; Y += 2; x--; } pixel(x, y, color); }
      X = 2 * r - 1; Y = 1; E = 0;                                             
      while (Y < X) { E += Y; Y += 2; y--; if (E >= X) { E -= X; X -= 2; x++; }pixel(x, y, color); }
      while (X > 0) { E -= X; X -= 2; x++; if (E < 0) { E += Y; Y += 2; y--; } pixel(x, y, color); }
      X = 2 * r - 1; Y = 1; E = 0;
      while (Y < X) { E += Y; Y += 2; x++; if (E >= X) { E -= X; X -= 2; y++; } pixel(x, y, color); }
      while (X > 0) { E -= X; X -= 2; y++; if (E < 0) { E += Y; Y += 2; x++; } pixel(x, y, color); }
      return;
  }
  void disque(int x, int y, int r, TSFBColor color)
  {
      for (int i = -r; i <= r;i++)
      {
          for (int j = -r; j <= r; j++)
          {
              if (i*i+j*j<=r*r+1) pixel(x+i, y+j, color);
          }
      }
  }

  static unsigned long long const font8[]; // This is the 256 character, 6*8 font data. Each long long represents 1 character, with 1 bit per pixel and 8 bits per line
  // Write some text on the screen. size is a zoom factor...
  template <typename T> int text(int x, int y, int maxwidth, T const *t, TSFBColor color=ClBlack, TSFBColor eraseColor= ClTrensparent, int size=1)
  {
    while (t!=NULL && *t!='\0' && maxwidth>=6*size)                 // while there is enough room for one more character
    {
      unsigned long long ca= font8[*t++];           // get character pixmap
      for (int l=8; --l>=0;)                        // character is 8 pixel high
      { // display the 6 pixels for this line of the character
        rect(x+0*size, y+l*size, size, size, (ca&0x10)?color:eraseColor);
        rect(x+1*size, y+l*size, size, size, (ca&0x20)?color:eraseColor);
        rect(x+2*size, y+l*size, size, size, (ca&0x40)?color:eraseColor);
        rect(x+3*size, y+l*size, size, size, (ca&0x80)?color:eraseColor);
        rect(x+4*size, y+l*size, size, size, (ca&0x1)?color:eraseColor);
        rect(x+5*size, y+l*size, size, size, (ca&0x2)?color:eraseColor);
        ca>>= 8;                                                       // next line of the character
      }
      x+= size*6; maxwidth-= size*6;
    } // next character
    // we will now erase the end (from here to w)
    rect(x, y, maxwidth, size*8, eraseColor);
    return x;
  }
  // write center text
  template <typename T> int centertext(int x, int y, T const *t, TSFBColor color=ClBlack, TSFBColor eraseColor= ClWhite, int size=1)
  {
    int len= t==NULL ? 0 : (strlen(t)*6*size);
    return text(x-len/2, y, len, t, color, eraseColor, size);
  }
  // write right allign text
  template <typename T> int righttext(int x, int y, T const *t, TSFBColor color=ClBlack, TSFBColor eraseColor= ClWhite, int size=1)
  {
    int len= t==NULL ? 0 : (strlen(t)*6*size);
    return text(x-len, y, len, t, color, eraseColor, size);
  }
  // Draws s on bitmap at position. alpha can be provided
  void blit(int x, int y, CSimpleFrameBuffer const *s, uint32_t alpha=0) // 0 is opaque
  {
    int sw= (s->w+x<=w) ? w : w-x; // calculate the number of columns
    int sh= (s->h+y<=h) ? h : h-y; // and row to copy (with clipping)
    alpha<<= 24;
    for (int Y= 0; Y<sh; Y++) for (int X= 0; X<sw; X++) pixel(x+X, y+Y, alpha+(s->pixel(X, Y))&0xffffff); // copy bitmap!
  }
  // This is a fast framebuffer copy used to transfert a frame buffer from one bitmap to another of the same size. The intended use is for double buffering of screen data...
  void blit(CSimpleFrameBuffer const *s) { if (s->w==w || s->h==h) return; memcpy(fb, s->fb, w*h*sizeof(TSFBColor)); }

  // the print stuff is a sprintf which does not rely on an external lib....
    char *printbuf;
  char *print(char const *format, ...); // printing is limited to 1024 bytes and is placed in printbuf object global variable
  char *print(char const *format, va_list argp);  // printing is limited to 1024 bytes and is placed in printbuf object global variable
    char *logstrs;
  // This will write stuff on the screen like a terminal...
  void log(char const *format, ...);
  void log2(char const *format);
};
char *hpsprintf(char *buf, int bufsize, char const *format, ...);
