// Wraith W60 Heatmap - Cumulative key press frequency visualizer
// Runs in system tray, shows keyboard heatmap on hover/click.
// Based on WraithW60 key layout.

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cmath>

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAYICON 1
#define ID_SHOWWINDOW 2
#define ID_EXIT 3

// ---- Key layout from WraithW60Leds.cpp (physical indices) ----
struct KeyDef {
    uint8_t physIdx;
    int row, col;
    const char* label;
    float width;
};

static const KeyDef KEYS[] = {
    // Row 0: Number row
    {22,0,0,"Esc",1},{23,0,1,"1",1},{24,0,2,"2",1},{25,0,3,"3",1},
    {26,0,4,"4",1},{27,0,5,"5",1},{28,0,6,"6",1},{29,0,7,"7",1},
    {30,0,8,"8",1},{31,0,9,"9",1},{32,0,10,"0",1},{33,0,11,"-",1},
    {34,0,12,"=",1},{36,0,13,"Bksp",2},
    // Row 1: QWERTY
    {44,1,0,"Tab",1.5},{45,1,1,"Q",1},{46,1,2,"W",1},{47,1,3,"E",1},
    {48,1,4,"R",1},{49,1,5,"T",1},{50,1,6,"Y",1},{51,1,7,"U",1},
    {52,1,8,"I",1},{53,1,9,"O",1},{54,1,10,"P",1},{55,1,11,"[",1},
    {56,1,12,"]",1},
    // Row 2: Home row
    {66,2,0,"Caps",1.75},{68,2,1,"A",1},{69,2,2,"S",1},{70,2,3,"D",1},
    {71,2,4,"F",1},{72,2,5,"G",1},{73,2,6,"H",1},{74,2,7,"J",1},
    {75,2,8,"K",1},{76,2,9,"L",1},{77,2,10,";",1},{78,2,11,"'",1},
    {80,2,12,"Enter",2.25},
    // Row 3: Shift row
    {88,3,0,"Shift",2.25},{90,3,1,"Z",1},{91,3,2,"X",1},{92,3,3,"C",1},
    {93,3,4,"V",1},{94,3,5,"B",1},{95,3,6,"N",1},{96,3,7,"M",1},
    {97,3,8,",",1},{98,3,9,".",1},{99,3,10,"/",1},{100,3,11,"Shift",2.75},
    // Row 4: Bottom row
    {110,4,0,"Ctrl",1.25},{111,4,1,"Win",1.25},{112,4,2,"Alt",1.25},
    {116,4,3,"Space",6.25},{119,4,4,"Alt",1.25},{120,4,5,"App",1.25},
    {121,4,6,"RCtrl",1.25}
};
static const int KEY_COUNT = sizeof(KEYS) / sizeof(KEYS[0]);

// ---- State ----
static int pressCount[KEY_COUNT] = {};
static int maxPresses = 1;
static bool wasDown[256] = {};
static bool g_visible = false;
static HWND g_hwnd = NULL;
static HWND g_kbwnd = NULL;
static NOTIFYICONDATAA g_nid = {};
static LARGE_INTEGER g_freq;
static LARGE_INTEGER g_startTime;

// ---- Layout constants ----
static const float KEY_UNIT = 48.0f;
static const float KEY_GAP = 3.0f;
static const int PAD = 20;

// ---- Color interpolation ----
static COLORREF lerpColor(COLORREF a, COLORREF b, float t) {
    int r = (int)(GetRValue(a) + t * (GetRValue(b) - GetRValue(a)));
    int g = (int)(GetGValue(a) + t * (GetGValue(b) - GetGValue(a)));
    int bl = (int)(GetBValue(a) + t * (GetBValue(b) - GetBValue(a)));
    return RGB(r, g, bl);
}

static COLORREF heatColor(float ratio) {
    // 5-stop gradient: dark blue -> cyan -> green -> yellow -> red
    COLORREF stops[] = {
        RGB(13, 71, 161),    // dark blue
        RGB(0, 150, 136),    // teal/cyan
        RGB(76, 175, 80),    // green
        RGB(255, 193, 7),    // amber
        RGB(244, 67, 54)     // red
    };
    float segments = 4.0f;
    int seg = (int)(ratio * segments);
    if (seg >= 4) seg = 3;
    float t = (ratio * segments) - seg;
    return lerpColor(stops[seg], stops[seg + 1], t);
}

// ---- Keyboard window ----
static const int WIN_W = 900;
static const int WIN_H = 340;

void drawKeyboard(HDC hdc, RECT& rc) {
    // Background
    HBRUSH bgBrush = CreateSolidBrush(RGB(25, 25, 35));
    FillRect(hdc, &rc, bgBrush);
    DeleteObject(bgBrush);

    HFONT keyFont = CreateFontA(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, 0, 0, 0, 0, "Segoe UI");
    HFONT titleFont = CreateFontA(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, 0, 0, 0, 0, "Segoe UI");
    HFONT smallFont = CreateFontA(10, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, 0, 0, 0, 0, "Segoe UI");

    // Title
    SelectObject(hdc, titleFont);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(200, 200, 200));
    TextOutA(hdc, PAD, 8, "Wraith W60 Heatmap", 19);

    // Stats
    SelectObject(hdc, smallFont);
    SetTextColor(hdc, RGB(120, 120, 120));
    char stats[128];
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    float elapsed = (float)(now.QuadPart - g_startTime.QuadPart) / g_freq.QuadPart;
    int totalPresses = 0;
    for (int i = 0; i < KEY_COUNT; i++) totalPresses += pressCount[i];
    sprintf(stats, "Toplam basma: %d | Max: %d | Suresiz: %.0fs | Ort: %.1f/s",
        totalPresses, maxPresses, elapsed, elapsed > 0 ? totalPresses / elapsed : 0);
    TextOutA(hdc, PAD + 200, 12, stats, (int)strlen(stats));

    // Draw keys
    SelectObject(hdc, keyFont);
    float rowOffsets[] = {0, 0.5f, 0.75f, 1.0f, 0.5f};

    for (int i = 0; i < KEY_COUNT; i++) {
        const KeyDef& k = KEYS[i];
        float xOff = rowOffsets[k.row] * (KEY_UNIT + KEY_GAP);
        float x = PAD + xOff + k.col * (KEY_UNIT + KEY_GAP);
        // Calculate actual x based on widths of previous keys in this row
        float calcX = PAD + xOff;
        for (int j = 0; j < KEY_COUNT; j++) {
            if (KEYS[j].row == k.row && KEYS[j].col < k.col) {
                calcX += KEYS[j].width * (KEY_UNIT + KEY_GAP);
            }
        }
        float y = 40 + k.row * (KEY_UNIT + KEY_GAP);
        float w = k.width * (KEY_UNIT + KEY_GAP) - KEY_GAP;
        int ix = (int)calcX, iy = (int)y, iw = (int)w, ih = (int)KEY_UNIT;

        float ratio = (maxPresses > 1) ? (float)pressCount[i] / maxPresses : 0;
        if (pressCount[i] > 0 && ratio < 0.02f) ratio = 0.02f;

        // Key background
        COLORREF bgColor;
        if (pressCount[i] == 0) {
            bgColor = RGB(45, 45, 55);
        } else {
            bgColor = heatColor(ratio);
        }

        HBRUSH keyBrush = CreateSolidBrush(bgColor);
        HPEN keyPen = CreatePen(PS_SOLID, 1, RGB(60, 60, 70));
        HGDIOBJ oldBrush = SelectObject(hdc, keyBrush);
        HGDIOBJ oldPen = SelectObject(hdc, keyPen);
        RoundRect(hdc, ix, iy, ix + iw, iy + ih, 5, 5);
        SelectObject(hdc, oldBrush);
        SelectObject(hdc, oldPen);
        DeleteObject(keyBrush);
        DeleteObject(keyPen);

        // Key label
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, (pressCount[i] > 0 && ratio > 0.4f) ? RGB(255,255,255) : RGB(180,180,180));
        RECT keyRc = {ix, iy + 2, ix + iw, iy + ih - 12};
        DrawTextA(hdc, k.label, -1, &keyRc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        // Press count
        if (pressCount[i] > 0) {
            char cnt[16];
            sprintf(cnt, "%d", pressCount[i]);
            SetTextColor(hdc, RGB(200, 200, 200));
            RECT cntRc = {ix, iy + ih - 14, ix + iw, iy + ih};
            DrawTextA(hdc, cnt, -1, &cntRc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
    }

    // Legend bar at bottom
    int ly = WIN_H - 30;
    int lx = PAD;
    HFONT legendFont = CreateFontA(11, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, 0, 0, 0, 0, "Segoe UI");
    SelectObject(hdc, legendFont);
    SetTextColor(hdc, RGB(120, 120, 120));

    // Gradient bar
    for (int i = 0; i < 200; i++) {
        float t = i / 199.0f;
        HBRUSH lb = CreateSolidBrush(heatColor(t));
        RECT lrc = {lx + i, ly, lx + i + 2, ly + 14};
        FillRect(hdc, &lrc, lb);
        DeleteObject(lb);
    }
    TextOutA(hdc, lx, ly + 18, "Az", 2);
    TextOutA(hdc, lx + 190, ly + 18, "Cok", 3);

    DeleteObject(keyFont);
    DeleteObject(titleFont);
    DeleteObject(legendFont);
}

LRESULT CALLBACK KbWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc;
        GetClientRect(hwnd, &rc);
        drawKeyboard(hdc, rc);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_CLOSE:
        g_visible = false;
        ShowWindow(hwnd, SW_HIDE);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_TRAYICON) {
        if (lParam == WM_LBUTTONUP) {
            g_visible = !g_visible;
            if (g_visible) {
                ShowWindow(g_kbwnd, SW_SHOW);
                SetForegroundWindow(g_kbwnd);
                InvalidateRect(g_kbwnd, NULL, FALSE);
            } else {
                ShowWindow(g_kbwnd, SW_HIDE);
            }
        }
        else if (lParam == WM_RBUTTONUP) {
            POINT pt;
            GetCursorPos(&pt);
            HMENU menu = CreatePopupMenu();
            AppendMenuA(menu, MF_STRING, ID_SHOWWINDOW, g_visible ? "Gizle" : "Goster");
            AppendMenuA(menu, MF_SEPARATOR, 0, NULL);
            AppendMenuA(menu, MF_STRING, ID_EXIT, "Cikis");
            TrackPopupMenu(menu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
            DestroyMenu(menu);
        }
        return 0;
    }
    else if (msg == WM_COMMAND) {
        if (LOWORD(wParam) == ID_SHOWWINDOW) {
            g_visible = !g_visible;
            ShowWindow(g_kbwnd, g_visible ? SW_SHOW : SW_HIDE);
            if (g_visible) {
                SetForegroundWindow(g_kbwnd);
                InvalidateRect(g_kbwnd, NULL, FALSE);
            }
        }
        else if (LOWORD(wParam) == ID_EXIT) {
            Shell_NotifyIconA(NIM_DELETE, &g_nid);
            PostQuitMessage(0);
        }
        return 0;
    }
    else if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    QueryPerformanceFrequency(&g_freq);
    QueryPerformanceCounter(&g_startTime);

    // ---- Create keyboard window ----
    WNDCLASSA wc = {};
    wc.lpfnWndProc = KbWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "WraithHeatmapKB";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassA(&wc);

    g_kbwnd = CreateWindowExA(0, "WraithHeatmapKB", "Wraith W60 Heatmap",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, WIN_W, WIN_H,
        NULL, NULL, hInstance, NULL);

    // ---- Create hidden window for tray ----
    WNDCLASSA wc2 = {};
    wc2.lpfnWndProc = WndProc;
    wc2.hInstance = hInstance;
    wc2.lpszClassName = "WraithHeatmapTray";
    RegisterClassA(&wc2);

    g_hwnd = CreateWindowExA(0, "WraithHeatmapTray", "Tray", 0,
        0, 0, 0, 0, NULL, NULL, hInstance, NULL);

    // ---- System tray icon ----
    g_nid.cbSize = sizeof(g_nid);
    g_nid.hWnd = g_hwnd;
    g_nid.uID = ID_TRAYICON;
    g_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    g_nid.uCallbackMessage = WM_TRAYICON;
    g_nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    strcpy(g_nid.szTip, "Wraith W60 Heatmap - Tikla: Ac/Kapat");
    Shell_NotifyIconA(NIM_ADD, &g_nid);

    // ---- Key polling loop ----
    LARGE_INTEGER lastPoll;
    QueryPerformanceCounter(&lastPoll);

    // Raw input for keyboard
    RAWINPUTDEVICE rid;
    rid.usUsagePage = 0x01;
    rid.usUsage = 0x06;
    rid.dwFlags = 0;
    rid.hwndTarget = g_kbwnd;
    RegisterRawInputDevices(&rid, 1, sizeof(rid));

    MSG msg;
    while (TRUE) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) goto done;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Poll keyboard state
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        float dt = (float)(now.QuadPart - lastPoll.QuadPart) / g_freq.QuadPart;

        if (dt >= 0.008f) { // ~120Hz poll
            lastPoll = now;
            bool changed = false;

            for (int i = 0; i < KEY_COUNT; i++) {
                int vk = 0;
                // Map physical index to VK code
                const char* n = KEYS[i].label;
                if (strlen(n) == 1) {
                    char c = n[0];
                    if (c >= 'A' && c <= 'Z') vk = c;
                    else if (c >= '0' && c <= '9') vk = c;
                    else if (c == ';') vk = VK_OEM_1;
                    else if (c == '\'') vk = VK_OEM_7;
                    else if (c == ',') vk = VK_OEM_COMMA;
                    else if (c == '.') vk = VK_OEM_PERIOD;
                    else if (c == '/') vk = VK_OEM_2;
                    else if (c == '-') vk = VK_OEM_MINUS;
                    else if (c == '=') vk = VK_OEM_PLUS;
                    else if (c == '[') vk = VK_OEM_4;
                    else if (c == ']') vk = VK_OEM_6;
                }
                else if (strcmp(n, "Esc") == 0) vk = VK_ESCAPE;
                else if (strcmp(n, "Tab") == 0) vk = VK_TAB;
                else if (strcmp(n, "Caps") == 0) vk = VK_CAPITAL;
                else if (strcmp(n, "Shift") == 0) vk = VK_LSHIFT;
                else if (strcmp(n, "Ctrl") == 0) vk = VK_LCONTROL;
                else if (strcmp(n, "RCtrl") == 0) vk = VK_RCONTROL;
                else if (strcmp(n, "Win") == 0) vk = VK_LWIN;
                else if (strcmp(n, "Alt") == 0) vk = VK_LMENU;
                else if (strcmp(n, "Space") == 0) vk = VK_SPACE;
                else if (strcmp(n, "Enter") == 0) vk = VK_RETURN;
                else if (strcmp(n, "Bksp") == 0) vk = VK_BACK;
                else if (strcmp(n, "App") == 0) vk = VK_APPS;

                if (vk == 0) continue;

                bool down = (GetAsyncKeyState(vk) & 0x8000) != 0;
                if (down && !wasDown[vk]) {
                    pressCount[i]++;
                    if (pressCount[i] > maxPresses) maxPresses = pressCount[i];
                    changed = true;
                }
                wasDown[vk] = down;
            }

            if (changed && g_visible) {
                InvalidateRect(g_kbwnd, NULL, FALSE);
            }
        }

        Sleep(1);
    }

done:
    Shell_NotifyIconA(NIM_DELETE, &g_nid);
    return 0;
}

#else
int main() { printf("Windows only.\n"); return 1; }
#endif
