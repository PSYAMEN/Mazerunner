#ifndef _MINIFB_H_
#define _MINIFB_H_

#include <stdint.h>
#include <windows.h>
#include "CSimpleFrameBuffer.h"

template <typename T> T* ntrealloc(T* p, int s)
{
    if (s == 0 && p == nullptr) return nullptr;
    if (s == 0) { free(p); return nullptr; }
    if (p == nullptr) return (T*)malloc(s * sizeof(T));
    return (T*)realloc(p, s * sizeof(T));
}

template <typename T>
struct THPList
{
    T* list = nullptr; int Size = 0;            // list pointer and nb elements...

    bool inline IsEmpty() const { return !Size; }
    void SetSize(int new_size) { for (int i = new_size; i < Size; i++) list = ntrealloc(list, new_size); if (new_size > Size) memset(list + Size, 0, (new_size - Size) * sizeof(T)); Size = new_size; }

    int add(T const& o) { list = ntrealloc(list, Size + 1); list[Size] = o; Size++; return Size - 1; } // Add one object at the end. return it's index
    int add() { list = ntrealloc(list, Size + 1); memset(list + Size, 0, sizeof(T)); Size++; return Size - 1; } // add one slot at the end. zero it, return it's index
    int AddN(T const& o, int N) { int old_size = Size; list = ntrealloc(list, Size += N); while (N) list[Size - N--] = o; return old_size; } // Add N copies of o at the end. return index of first added; assumes N>=0
    int AddN(int N) { list = ntrealloc(list, Size += N); memset(list + Size - N, 0, N * sizeof(T)); return Size - N; } // Add N objects at the end. return index of first added; assumes N>=0
    int AddN(T const* o, int N) { list = ntrealloc(list, Size += N); memcpy(list + Size - N, o, N * sizeof(T)); return Size - N; } // Add N objects at the end. return index of first added; assumes N>=0
    int insert(int pos) { list = ntrealloc(list, ++Size); memmove(list + pos + 1, list + pos, sizeof(T) * (Size - 1 - pos)); memset(list + pos, 0, sizeof(T)); return pos; } // insert spot for element at pos. zero it out, return the index...
    int insert(T const& o, int pos) { list = ntrealloc(list, ++Size); memmove(list + pos + 1, list + pos, sizeof(T) * (Size - 1 - pos)); list[pos] = o; return pos; } // insert element at pos. return it's index
    int InsertN(int pos, T const& o, int N) { list = ntrealloc(list, Size += N); memmove(list + pos + N, list + pos, sizeof(T) * (Size - (pos + N))); while (N--) list[pos + N] = o; return pos; } // insert N copies of o starting at pos, return index of first copy; assumes N>=0
    void Delete(int pos) { memmove(list + pos, list + pos + 1, (--Size - pos) * sizeof(T)); list = ntrealloc(list, Size); } // remove an object at pos from list
    void RemoveN(int pos, int N) { Size -= N; memmove(list + pos, list + pos + N, (Size - pos) * sizeof(T)); } // removes N consecutive objects, starting at pos, from list; assumes N>=0
    int RemoveN(T* o, int pos, int N) { if (N + pos > Size) N = Size - pos; if (N <= 0) return 0; memcpy(o, list + pos, N * sizeof(T)); RemoveN(pos, N); return N; } // removes N consecutive objects, starting at pos, from list; assumes N>=0. returns the number of removed items.
    void swap(int i, int j) { if (i == j) return; SwapEm(list[i], list[j]); } // exchange 2 elements in the list
    void swap2(int i, int j) { if (i == j) return; SwapEm2(list[i], list[j]); } // exchange 2 elements in the list. Does not use Object copy, uses memory copy...
    void SwapN(int i, int j, int N) { if (i == j) return; while (N--) { SwapEm(list[i++], list[j++]); } } // exchange 2N elements in the list (i with j, i+1 with j+1, ...); assumes N>=0, assumes |i-j| is 0 or >=N (no partial overlap of ranges)
    void move(int src, int dst) {
        if (src == dst) return;
        T a = this->list[src];
        if (src < dst) memcpy(this->list + src, this->list + src + 1, sizeof(T) * (dst - src));
        else memmove(this->list + dst + 1, this->list + dst, sizeof(T) * (src - dst));
        this->list[dst] = a;
    } // move element from src to dst. move the list up or down as needed...
    T pop(int i = 1) { T ret = list[Size - 1]; list = ntrealloc(list, --Size); return ret; } // remove the last element from the list...

    T* TakeList() { T* ret = list; list = NULL; Size = 0; return ret; } // give away possession of the list content. remove it from *this, and reset *this
    void SetList(T* l, int size) { SetSize(0); this->list = l; this->Size = size; }
    void SetTo(int new_size, T* new_list) { SetSize(0); list = new_list; Size = new_size; }  // take possession of new_list; remove old contents of *this; *this takes on ownership of *new_list (responsibility to free(); assumes new_list is from the malloc heap)
    void TakeFrom(THPList<T>& take) { SetSize(0); this->Size = take.Size; this->list = take.TakeList(); } // Take the list from another list.

                                                                                                                                   // list access...
    T inline& at(int i) { return list[i]; }
    T inline& operator [](int i) { return list[i]; }
    T const inline& operator [](int i) const { return list[i]; }
    T inline& last() { return list[Size - 1]; }
    T const inline& last() const { return list[Size - 1]; }
    bool IsIndex(int i) { return 0 <= i && i < Size; }

    // template to get pointer to the object...
    template<typename T2> T2 inline* ptr(T2& obj) { return &obj; } //turn reference into pointer!
    template<typename T2> T2 inline* ptr(T2* obj) { return obj; }  //obj is already pointer, return it!

                                                                    // Find the index of an object. Object must had an id or a IsNameFor member for this to work!
    int find(T const& o) { for (int i = Size; --i >= 0;) if (list[i] == o) return i; return -1; }
    int find(int id) { for (int i = Size; --i >= 0;) if (ptr(list[i])->id == id) return i; return -1; }
    int findIsIdFor(int id) { for (int i = Size; --i >= 0;) if (ptr(list[i])->IsIdFor(id)) return i; return -1; } // Must define MatchIs
    int find(wchar_t const* name) { for (int i = Size; --i >= 0;) if (ptr(list[i])->IsNameFor(name)) return i; return -1; }
    int findWchar(wchar_t const* name) { for (int i = Size; --i >= 0;) if (wcscmp2(list[i], name) == 0) return i; return -1; }
    bool findO(int id, T& o) { int i = find(id);        if (i < 0) return false; o = list[i]; return true; }
    bool findO(wchar_t const* name, T& o) { int i = find(name);   if (i < 0) return false; o = list[i]; return true; }
    T findO(int id) { int i = find(id);        if (i < 0) return NULL; return list[i]; }
    T findOIsIdFor(int id) { int i = findIsIdFor(id); if (i < 0) return NULL; return list[i]; }
    T findO(wchar_t const* name) { int i = find(name);      if (i < 0) return NULL; return list[i]; }

    // find and remove object from list... see above for more info...
    bool FindDelete(T const& o) { int i = find(o);      if (i < 0) return false; Delete(i); return true; }
    bool FindDelete(wchar_t const* name) { int i = find(name);   if (i < 0) return false; Delete(i); return true; }
    bool FindDelete(int id) { int i = find(id);     if (i < 0) return false; Delete(i); return true; }
    int FindReplace(T const& o1, T const& o2) { int i = find(o1); if (i < 0) return add(o2); list[i] = o2; return i; }
    int FindReplace(wchar_t const* name, T const& o) { int i = find(name);   if (i < 0) return add(o); list[i] = o; return i; }
    int FindReplace(int id, T const& o) { int i = find(id); if (i < 0) return add(o); list[i] = o; return i; }
    int copy(THPList<T> const& l) { SetSize(l.Size); for (int i = l.Size; --i >= 0;) list[i] = l.list[i]; return Size; }
    int copy2(THPList<T> const& l) { SetSize(l.Size); memcpy(list, l.list, Size * sizeof(T)); return Size; }

    // Quick sort. THIS IS SELDOM TESTED, SO DO NOT TRUST IT AT THIS POINT!
    // compare returns <0 if o1 is < than o2. 0 if equal and >0 if larger
    // p is your own to deal with/use
    // set stop to true to stop the sorting. Will return true if this happend.
    template <typename T2> bool qsort(int (*compare)(T& o1, T& o2, T2 p, bool& stop), T2 p, int bottom = 0, int top = -1)
    {
        // Input validation
        if (bottom < 0) bottom = 0; if (bottom >= this->Size) bottom = this->Size - 1;
        bool stop = false;
        if (top == -1 || top >= this->Size) top = this->Size - 1;
        while (true) // Forever loop (does the terminal recursion part of the sort)
        {
            int n = top - bottom; // Deal with cases with 0, 1 or 2 items
            if (n <= 0) return false;
            if (n == 1) { if (compare(this->list[bottom], this->list[top], p, stop) > 0) this->swap2(bottom, top); return stop; }
            // Partition around the pivot... This assumes that a copy of T can be done easily...
            int mid = (bottom + top) / 2;
            int i = bottom - 1, j = top + 1;
            while (true)
            {
                while (compare(this->list[++i], this->list[mid], p, stop) < 0 && !stop); if (stop) return true;
                while (compare(this->list[--j], this->list[mid], p, stop) > 0 && !stop); if (stop) return true;
                if (i >= j) break;
                if (i != j) { SwapEm2(this->list[i], this->list[j]); if (i == mid) mid = j; else if (j == mid) mid = i; }
            }
            if (this->qsort<T2>(compare, p, bottom, j)) return true;
            bottom = i;
        }
    }
    bool inline qsort(int (*compare)(T& o1, T& o2, int unused), int bottom = 0, int top = -1) { return qsort<int>(compare, 0, bottom, top); }
};

enum {
    KB_MOD_SHIFT        = 0x0001,
    KB_MOD_CONTROL      = 0x0002,
    KB_MOD_ALT          = 0x0004,
    KB_MOD_SUPER        = 0x0008,
    KB_MOD_CAPS_LOCK    = 0x0010,
    KB_MOD_NUM_LOCK     = 0x0020
};

typedef enum {
    WF_RESIZABLE          = 0x01,
    WF_FULLSCREEN         = 0x02,
    WF_FULLSCREEN_DESKTOP = 0x04,
    WF_BORDERLESS         = 0x08,
    WF_ALWAYS_ON_TOP      = 0x10,
} mfb_window_flags;

class TThread { public:
    TThread()  { th= CreateThread(nullptr, 0, cb, this, CREATE_SUSPENDED, nullptr); }
    bool done= false;
    ~TThread()
    {
        TerminateThread(th, 0);
        WaitForSingleObject(th, 100);
        CloseHandle(th);
        done= true;
    }
    HANDLE th;
    static DWORD WINAPI cb(void *p)
    {
        TThread *This= (TThread*)p;
        This->doit();
        This->done= true;
        return 0;
    }
    virtual void doit() { }
    void resume() { ResumeThread(th); }
};

class CFBWindow : public CSimpleFrameBuffer { public:
    // Event callbacks
    virtual void active(bool isActive) {}
    virtual void resize(int width, int height) {}
    virtual bool close() { return true; } // return false to disallow closing!
    virtual void keyboard(int key, uint32_t mod, bool isPressed) {}
    virtual void char_input(unsigned int code) {}
    virtual void mouse_button(int button, uint32_t mod, bool isPressed) {}
    virtual void mouse_move(int x, int y) {}
    virtual void mouse_wheel(int deltav, int deltah, uint32_t mod) {}

    // Create a window that is used to display the buffer sent into the mfb_update function, returns 0 if fails
    CFBWindow(const wchar_t *title, int width, int height, unsigned flags= 0, int fbw= 0, int fbh= 0);
    // Close the window
    ~CFBWindow() { destroy_window_data(); windows.FindDelete(this);  }
    void destroy_window_data();
    
    bool update_display(); // Update the display
    bool update_events();  // update windows events

    // FPS
    int fps = 0, frameCount= 0;
    uint64_t nextFrame= 0;
    void setFps(int fps);
    bool wait_sync();

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    HWND                hWnd;
    WNDCLASS            wc;
    HDC                 hdc;
    struct { BITMAPINFO bitmapInfo; RGBQUAD other[2]; };
    long    s_window_style = WS_POPUP | WS_SYSMENU | WS_CAPTION;
    uint32_t window_width, window_height;
    int dst_offset_x, dst_offset_y, dst_width, dst_height;
    void set_viewport(int offset_x, int offset_y, int width, int height)
    { dst_offset_x= offset_x, dst_offset_y= offset_y, dst_width= width, dst_height= height; }
    void defaultViewport() { set_viewport(0, 0, window_width, window_height); }
    bool is_active= true, mouse_inside= true;
    bool closed= false;

    void get_monitor_scale(float &scale_x, float &scale_y);

    virtual void exec() { }
    class ExecThread : public TThread { public:
        ExecThread(CFBWindow *w): w(w) { }
        CFBWindow *w;
        void doit() { w->exec(); }
    } execThread;

    static THPList<CFBWindow*> windows;
    static void run();
};



#endif
