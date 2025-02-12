#include "MiniFB.h"

//#define kUseBilinearInterpolation

#if defined(kUseBilinearInterpolation)
//-------------------------------------
static uint32_t
interpolate(uint32_t *srcImage, uint32_t x, uint32_t y, uint32_t srcOffsetX, uint32_t srcOffsetY, uint32_t srcWidth, uint32_t srcHeight, uint32_t srcPitch) {
    uint32_t incX = x + 1 < srcWidth ? 1 : 0;
    uint32_t incY = y + 1 < srcHeight ? srcPitch : 0;
    uint8_t *p00 = (uint8_t *) &srcImage[(srcOffsetX >> 16)];
    uint8_t *p01 = (uint8_t *) &srcImage[(srcOffsetX >> 16) + incX];
    uint8_t *p10 = (uint8_t *) &srcImage[(srcOffsetX >> 16) + incY];
    uint8_t *p11 = (uint8_t *) &srcImage[(srcOffsetX >> 16) + incY + incX];

    uint32_t wx2 = srcOffsetX & 0xffff;
    uint32_t wy2 = srcOffsetY & 0xffff;
    uint32_t wx1 = 0x10000 - wx2;
    uint32_t wy1 = 0x10000 - wy2;

    uint32_t w1 = ((uint64_t) wx1 * wy1) >> 16;
    uint32_t w2 = ((uint64_t) wx2 * wy1) >> 16;
    uint32_t w3 = ((uint64_t) wx1 * wy2) >> 16;
    uint32_t w4 = ((uint64_t) wx2 * wy2) >> 16;

    // If you don't have uint64_t
    //uint32_t b = (((p00[0] * wx1 + p01[0] * wx2) >> 16) * wy1 + ((p10[0] * wx1 + p11[0] * wx2) >> 16) * wy2) >> 16;
    //uint32_t g = (((p00[1] * wx1 + p01[1] * wx2) >> 16) * wy1 + ((p10[1] * wx1 + p11[1] * wx2) >> 16) * wy2) >> 16;
    //uint32_t r = (((p00[2] * wx1 + p01[2] * wx2) >> 16) * wy1 + ((p10[2] * wx1 + p11[2] * wx2) >> 16) * wy2) >> 16;
    //uint32_t a = (((p00[3] * wx1 + p01[3] * wx2) >> 16) * wy1 + ((p10[3] * wx1 + p11[3] * wx2) >> 16) * wy2) >> 16;

    uint32_t b = ((p00[0] * w1 + p01[0] * w2) + (p10[0] * w3 + p11[0] * w4)) >> 16;
    uint32_t g = ((p00[1] * w1 + p01[1] * w2) + (p10[1] * w3 + p11[1] * w4)) >> 16;
    uint32_t r = ((p00[2] * w1 + p01[2] * w2) + (p10[2] * w3 + p11[2] * w4)) >> 16;
    uint32_t a = ((p00[3] * w1 + p01[3] * w2) + (p10[3] * w3 + p11[3] * w4)) >> 16;

    return (a << 24) + (r << 16) + (g << 8) + b;
}
#endif

// Only for 32 bits images
//-------------------------------------
void stretch_image(uint32_t *srcImage, uint32_t srcX, uint32_t srcY, uint32_t srcWidth, uint32_t srcHeight, uint32_t srcPitch,
    uint32_t *dstImage, uint32_t dstX, uint32_t dstY, uint32_t dstWidth, uint32_t dstHeight, uint32_t dstPitch) 
{
    uint32_t    x, y;
    uint32_t    srcOffsetX, srcOffsetY;

    if(srcImage == 0x0 || dstImage == 0x0) return;

    srcImage += srcX + srcY * srcPitch;
    dstImage += dstX + dstY * dstPitch;

    const uint32_t deltaX = (srcWidth  << 16) / dstWidth;
    const uint32_t deltaY = (srcHeight << 16) / dstHeight;

    srcOffsetY = 0;
    for(y=0; y<dstHeight; ++y) {
        srcOffsetX = 0;
        for(x=0; x<dstWidth; ++x) {
#if defined(kUseBilinearInterpolation)
            dstImage[x] = interpolate(srcImage, x+srcX, y+srcY, srcOffsetX, srcOffsetY, srcWidth, srcHeight, srcPitch);
#else
            dstImage[x] = srcImage[srcOffsetX >> 16];
#endif
            srcOffsetX += deltaX;
        }

        srcOffsetY += deltaY;
        if(srcOffsetY >= 0x10000) { srcImage += (srcOffsetY >> 16) * srcPitch; srcOffsetY &= 0xffff; }
        dstImage += dstPitch;
    }
}
#include "MiniFB.h"
#include <stdio.h>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t translate_mod();

// Copied (and modified) from Windows Kit 10 to avoid setting _WIN32_WINNT to a higher version
typedef enum mfb_PROCESS_DPI_AWARENESS {
    mfb_PROCESS_DPI_UNAWARE           = 0,
    mfb_PROCESS_SYSTEM_DPI_AWARE      = 1,
    mfb_PROCESS_PER_MONITOR_DPI_AWARE = 2
} mfb_PROCESS_DPI_AWARENESS;

typedef enum mfb_MONITOR_DPI_TYPE {
    mfb_MDT_EFFECTIVE_DPI             = 0,
    mfb_MDT_ANGULAR_DPI               = 1,
    mfb_MDT_RAW_DPI                   = 2,
    mfb_MDT_DEFAULT                   = mfb_MDT_EFFECTIVE_DPI
} mfb_MONITOR_DPI_TYPE;

#define mfb_DPI_AWARENESS_CONTEXT_UNAWARE               ((HANDLE) -1)
#define mfb_DPI_AWARENESS_CONTEXT_SYSTEM_AWARE          ((HANDLE) -2)
#define mfb_DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE     ((HANDLE) -3)
#define mfb_DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2  ((HANDLE) -4)
#define mfb_DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED     ((HANDLE) -5)

// user32.dll
typedef BOOL(WINAPI *PFN_SetProcessDPIAware)(void);
typedef BOOL(WINAPI *PFN_SetProcessDpiAwarenessContext)(HANDLE);
typedef UINT(WINAPI *PFN_GetDpiForWindow)(HWND);
typedef BOOL(WINAPI *PFN_EnableNonClientDpiScaling)(HWND);

HMODULE                           mfb_user32_dll                    = 0x0;
PFN_SetProcessDPIAware            mfb_SetProcessDPIAware            = 0x0;
PFN_SetProcessDpiAwarenessContext mfb_SetProcessDpiAwarenessContext = 0x0;
PFN_GetDpiForWindow               mfb_GetDpiForWindow               = 0x0;
PFN_EnableNonClientDpiScaling     mfb_EnableNonClientDpiScaling     = 0x0;

// shcore.dll
typedef HRESULT(WINAPI *PFN_SetProcessDpiAwareness)(mfb_PROCESS_DPI_AWARENESS);
typedef HRESULT(WINAPI *PFN_GetDpiForMonitor)(HMONITOR, mfb_MONITOR_DPI_TYPE, UINT *, UINT *);

HMODULE                           mfb_shcore_dll                    = 0x0;
PFN_SetProcessDpiAwareness        mfb_SetProcessDpiAwareness        = 0x0;
PFN_GetDpiForMonitor              mfb_GetDpiForMonitor              = 0x0;

//--
void load_functions() 
{
    if(mfb_user32_dll == 0x0) {
        mfb_user32_dll = LoadLibraryA("user32.dll");
        if (mfb_user32_dll != 0x0) {
            mfb_SetProcessDPIAware = (PFN_SetProcessDPIAware) GetProcAddress(mfb_user32_dll, "SetProcessDPIAware");
            mfb_SetProcessDpiAwarenessContext = (PFN_SetProcessDpiAwarenessContext) GetProcAddress(mfb_user32_dll, "SetProcessDpiAwarenessContext");
            mfb_GetDpiForWindow = (PFN_GetDpiForWindow) GetProcAddress(mfb_user32_dll, "GetDpiForWindow");
            mfb_EnableNonClientDpiScaling = (PFN_EnableNonClientDpiScaling) GetProcAddress(mfb_user32_dll, "EnableNonClientDpiScaling");
        }
    }

    if(mfb_shcore_dll == 0x0) {
        mfb_shcore_dll = LoadLibraryA("shcore.dll");
        if (mfb_shcore_dll != 0x0) {
            mfb_SetProcessDpiAwareness = (PFN_SetProcessDpiAwareness) GetProcAddress(mfb_shcore_dll, "SetProcessDpiAwareness");
            mfb_GetDpiForMonitor = (PFN_GetDpiForMonitor) GetProcAddress(mfb_shcore_dll, "GetDpiForMonitor");
        }
    }
}

//--
// NOT Thread safe. Just convenient (Don't do this at home guys)
char * GetErrorMessage() {
    static char buffer[256];

    buffer[0] = 0;
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,  // Not used with FORMAT_MESSAGE_FROM_SYSTEM
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        buffer,
        sizeof(buffer),
        NULL);

    return buffer;
}

//--
void CFBWindow::get_monitor_scale(float &scale_x, float &scale_y) 
{
    UINT    x, y;
    if(mfb_GetDpiForMonitor != 0x0) 
    {
        HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
        mfb_GetDpiForMonitor(monitor, mfb_MDT_EFFECTIVE_DPI, &x, &y);
    }
    else 
    {
        const HDC dc = GetDC(hWnd);
        x = GetDeviceCaps(dc, LOGPIXELSX);
        y = GetDeviceCaps(dc, LOGPIXELSY);
        ReleaseDC(NULL, dc);
    }
    scale_x = x / (float) USER_DEFAULT_SCREEN_DPI;
    if(scale_x == 0) scale_x = 1;
    scale_y = y / (float) USER_DEFAULT_SCREEN_DPI;
    if (scale_y == 0) scale_y = 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void timerCb(void *p, int id)
{
    printf("Timer\r\n");
    ((CFBWindow*)p)->update_display();
}

void CFBWindow::setFps(int fps)
{
    this->fps= fps;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    LRESULT res = 0;
    CFBWindow *fb= (CFBWindow*) GetWindowLongPtr(hWnd, GWLP_USERDATA);

    switch (message)
    {
    case WM_NCCREATE:
        if(mfb_EnableNonClientDpiScaling) mfb_EnableNonClientDpiScaling(hWnd);
        return DefWindowProc(hWnd, message, wParam, lParam);

        // TODO
        //case 0x02E4://WM_GETDPISCALEDSIZE:
        //{
        //    SIZE* size = (SIZE*) lParam;
        //    WORD dpi = LOWORD(wParam);
        //    return true;
        //    break;
        //}

        // TODO
        //case WM_DPICHANGED:
        //{
        //    const float xscale = HIWORD(wParam);
        //    const float yscale = LOWORD(wParam);
        //    break;
        //}

    case WM_PAINT:
        fb->bitmapInfo.bmiHeader.biWidth       = fb->w;
        fb->bitmapInfo.bmiHeader.biHeight      = -fb->h;
        StretchDIBits(fb->hdc, fb->dst_offset_x, fb->dst_offset_y, fb->dst_width, fb->dst_height, 0, 0, fb->w, fb->h, fb->fb, &fb->bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
        ValidateRect(hWnd, 0x0);
        break;
    case WM_CLOSE:
        if (fb->close()) { fb->closed = true; DestroyWindow(fb->hWnd); } // Obtain a confirmation of close
        break;

    case WM_DESTROY: fb->closed= true; break;

    case WM_KEYDOWN: case WM_SYSKEYDOWN: case WM_KEYUP: case WM_SYSKEYUP:
        fb->keyboard((lParam&0x7fffffff)>>16, translate_mod(), !((lParam >> 31) & 1));
        break;

    case WM_CHAR: case WM_SYSCHAR:
    {
        static WCHAR highSurrogate = 0;
        if (wParam >= 0xd800 && wParam <= 0xdbff) highSurrogate = (WCHAR) wParam;
        else {
            unsigned int codepoint = 0;
            if (wParam >= 0xdc00 && wParam <= 0xdfff) {
                if (highSurrogate != 0) {
                    codepoint += (highSurrogate - 0xd800) << 10;
                    codepoint += (WCHAR) wParam - 0xdc00;
                    codepoint += 0x10000;
                }
            }
            else codepoint = (WCHAR) wParam;
            highSurrogate = 0;
            fb->char_input(codepoint);
        }
        break;
    }

    case WM_UNICHAR:
    {
        if (wParam == UNICODE_NOCHAR) {
            // WM_UNICHAR is not sent by Windows, but is sent by some third-party input method engine
            // Returning TRUE here announces support for this message
            return TRUE;
        }

        fb->char_input(wParam);
        break;
    }

    case WM_LBUTTONUP: case WM_RBUTTONUP: case WM_MBUTTONUP: case WM_XBUTTONUP: case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK: case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK: case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
    {
        int button = 0;
        bool is_pressed= 0;
        switch(message) {
        case WM_LBUTTONDOWN: is_pressed = 1;
        case WM_LBUTTONUP: button = 1; break;
        case WM_RBUTTONDOWN: is_pressed = 1;
        case WM_RBUTTONUP: button = 2; break;
        case WM_MBUTTONDOWN: is_pressed = 1;
        case WM_MBUTTONUP: button = 3; break;
        default:
            button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1 ? 5 : 5);
            if (message == WM_XBUTTONDOWN) is_pressed = 1;
        }
        fb->mouse_button(button, translate_mod(), is_pressed);
        break;
    }

    case WM_MOUSEWHEEL:
        fb->mouse_wheel(HIWORD(wParam) / WHEEL_DELTA, 0, translate_mod());
        break;

    case WM_MOUSEHWHEEL:
        fb->mouse_wheel(0, HIWORD(wParam) / WHEEL_DELTA, translate_mod());
        break;

    case WM_MOUSEMOVE:
        if (fb->mouse_inside == false) 
        {
            fb->mouse_inside = true;
            TRACKMOUSEEVENT tme;
            ZeroMemory(&tme, sizeof(tme));
            tme.cbSize = sizeof(tme);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hWnd;
            TrackMouseEvent(&tme);
        }
        fb->mouse_move((short) LOWORD(lParam), (short) HIWORD(lParam));
        break;

    case WM_MOUSELEAVE: fb->mouse_inside = false; break;

    case WM_SIZE:
    {
        float scale_x, scale_y;
        if (wParam == SIZE_MINIMIZED) return res;
        fb->get_monitor_scale(scale_x, scale_y);
        fb->window_width= LOWORD(lParam), fb->window_height= HIWORD(lParam);
        fb->defaultViewport();
        fb->resize(LOWORD(lParam), HIWORD(lParam));
        BitBlt(fb->hdc, 0, 0, fb->window_width, fb->window_height, 0, 0, 0, BLACKNESS);
    }
    break;

    case WM_SETFOCUS: fb->is_active = true; fb->active(true); break;

    case WM_KILLFOCUS: fb->is_active = false; fb->active(false); break;

    default: res = DefWindowProc(hWnd, message, wParam, lParam); break;
    }
    return res;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
THPList<CFBWindow*> CFBWindow::windows;
void CFBWindow::run()
{
    int64_t frequency; QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
    while (windows.Size)
    {
        uint64_t now; QueryPerformanceCounter((LARGE_INTEGER*)&now); now= now * 1000 / frequency;
        uint64_t next= -1;
        for (int i=windows.Size; --i>=0;) 
        {
            if (windows[i]->nextFrame<=now) { windows[i]->update_display(); windows[i]->nextFrame= now+1000/windows[i]->fps; }
            if (windows[i]->nextFrame<next) next= windows[i]->nextFrame;
        }
        Sleep(next-now);
    }
}


CFBWindow::CFBWindow(const wchar_t *title, int width, int height, unsigned flags, int fbw, int fbh): CSimpleFrameBuffer(fbw!=0 ? fbw:width, fbh!=0 ? fbh:height), execThread(this)
{
    windows.add(this);
    RECT rect = { 0 };
    int  x = 0, y = 0;

    load_functions();
    if (mfb_SetProcessDpiAwarenessContext != 0x0) {
        if(mfb_SetProcessDpiAwarenessContext(mfb_DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2) == false) {
            uint32_t error = GetLastError();
            if(error == ERROR_INVALID_PARAMETER) {
                error = NO_ERROR;
                if(mfb_SetProcessDpiAwarenessContext(mfb_DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE) == false) {
                    error = GetLastError();
                }
            }
            if(error != NO_ERROR) {
                fprintf(stderr, "Error (SetProcessDpiAwarenessContext): %s\n", GetErrorMessage());
            }
        }
    }
    else if (mfb_SetProcessDpiAwareness != 0x0) {
        if(mfb_SetProcessDpiAwareness(mfb_PROCESS_PER_MONITOR_DPI_AWARE) != S_OK) {
            fprintf(stderr, "Error (SetProcessDpiAwareness): %s\n", GetErrorMessage());
        }
    }
    else if (mfb_SetProcessDPIAware != 0x0) {
        if(mfb_SetProcessDPIAware() == false) {
            fprintf(stderr, "Error (SetProcessDPIAware): %s\n", GetErrorMessage());
        }
    }

    s_window_style = WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME;
    if (flags & WF_FULLSCREEN) 
    {
        flags = WF_FULLSCREEN;  // Remove all other flags
        rect.right  = GetSystemMetrics(SM_CXSCREEN);
        rect.bottom = GetSystemMetrics(SM_CYSCREEN);
        s_window_style = WS_POPUP & ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);

        DEVMODE settings = { 0 };
        EnumDisplaySettings(0, 0, &settings);
        settings.dmPelsWidth  = GetSystemMetrics(SM_CXSCREEN);
        settings.dmPelsHeight = GetSystemMetrics(SM_CYSCREEN);
        settings.dmBitsPerPel = 32;
        settings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        if (ChangeDisplaySettings(&settings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) flags = WF_FULLSCREEN_DESKTOP;
    }

    if (flags & WF_BORDERLESS) s_window_style = WS_POPUP;
    if (flags & WF_RESIZABLE) s_window_style |= WS_MAXIMIZEBOX | WS_SIZEBOX;

    if (flags & WF_FULLSCREEN_DESKTOP) 
    {
        s_window_style = WS_OVERLAPPEDWINDOW;
        width  = GetSystemMetrics(SM_CXFULLSCREEN);
        height = GetSystemMetrics(SM_CYFULLSCREEN);
        rect.right  = width;
        rect.bottom = height;
        AdjustWindowRect(&rect, s_window_style, 0);
        if (rect.left < 0) { width += rect.left * 2; rect.right += rect.left; rect.left = 0; }
        if (rect.bottom > (LONG) height) { height -= (rect.bottom - height); rect.bottom += (rect.bottom - height); rect.top = 0; }
    }
    else if (!(flags & WF_FULLSCREEN)) 
    {
        float scale_x, scale_y;
        get_monitor_scale(scale_x, scale_y);
        rect.right  = (LONG) (width  * scale_x);
        rect.bottom = (LONG) (height * scale_y);
        AdjustWindowRect(&rect, s_window_style, 0);
        rect.right  -= rect.left;
        rect.bottom -= rect.top;
        x = (GetSystemMetrics(SM_CXSCREEN) - rect.right) / 2;
        y = (GetSystemMetrics(SM_CYSCREEN) - rect.bottom + rect.top) / 2;
    }

    wc.style         = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.hCursor       = LoadCursor(0, IDC_ARROW);
    wc.lpszClassName = title;
    RegisterClass(&wc);

    //calc_dst_factor(width, height);

    window_width  = rect.right;
    window_height = rect.bottom;
    defaultViewport();

    hWnd = CreateWindowEx(0, title, title, s_window_style, x, y, window_width, window_height, 0, 0, 0, 0);if (!hWnd) return;

    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR) this);

    if (flags & WF_ALWAYS_ON_TOP) SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    ShowWindow(hWnd, SW_NORMAL);
    hdc= GetDC(hWnd);

    bitmapInfo.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bitmapInfo.bmiHeader.biPlanes      = 1;
    bitmapInfo.bmiHeader.biBitCount    = 32;
    bitmapInfo.bmiHeader.biCompression = BI_BITFIELDS;
    bitmapInfo.bmiColors[0].rgbRed     = 0xff;
    bitmapInfo.bmiColors[1].rgbGreen   = 0xff;
    bitmapInfo.bmiColors[2].rgbBlue    = 0xff;
    execThread.resume();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CFBWindow::update_display() 
{
    if (hWnd == 0x0) return false;
    if (closed) { destroy_window_data(); return false; }
    InvalidateRect(hWnd, 0x0, TRUE);
    frameCount++;
    SendMessage(hWnd, WM_PAINT, 0, 0);
    MSG msg; while (closed == false && PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE))  { TranslateMessage(&msg); DispatchMessage(&msg); }
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CFBWindow::update_events() 
{
    if (hWnd == 0x0) return false;
    if (closed) { destroy_window_data(); return false; }
    MSG msg; while (closed==false && PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE))  { TranslateMessage(&msg); DispatchMessage(&msg); }
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CFBWindow::wait_sync() 
{
    if (closed) { destroy_window_data(); return false; }
    int f= frameCount; while (f!=frameCount) Sleep(1);
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CFBWindow::destroy_window_data() 
{
    if (hWnd!=0 && hdc!=0) { ReleaseDC(hWnd, hdc); DestroyWindow(hWnd); }
    hWnd = 0; hdc = 0;
    closed = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t translate_mod() 
{
    uint32_t mods = 0;
    if (GetKeyState(VK_SHIFT) & 0x8000) mods |= KB_MOD_SHIFT;
    if (GetKeyState(VK_CONTROL) & 0x8000) mods |= KB_MOD_CONTROL;
    if (GetKeyState(VK_MENU) & 0x8000) mods |= KB_MOD_ALT;
    if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000) mods |= KB_MOD_SUPER;
    if (GetKeyState(VK_CAPITAL) & 1) mods |= KB_MOD_CAPS_LOCK;
    if (GetKeyState(VK_NUMLOCK) & 1) mods |= KB_MOD_NUM_LOCK;
    return mods;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//void CFBWindow::set_viewport(int offset_x, int offset_y, int width, int height) 
//{
//    //float scale_x= scale, scale_y= scale;
//    //if (scale==-1) get_monitor_scale(scale_x, scale_y);
//    dst_offset_x = (uint32_t) (offset_x ); //* scale_x);
//    dst_offset_y = (uint32_t) (offset_y ); //* scale_y);
//    dst_width    = (uint32_t) (width    ); //* scale_x);
//    dst_height   = (uint32_t) (height   ); //* scale_y);
//    BitBlt(hdc, 0, 0, window_width, window_height, 0, 0, 0, BLACKNESS);
//}
