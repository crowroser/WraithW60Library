// Wraith W60 Heatmap - Win32 GUI
// Real-time graphical keyboard heatmap with pressure visualization.

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cmath>

struct KeyDef {
    int row, col;
    const char* label;
    int vk;
    float width; // relative width (1.0 = standard key)
};

static const KeyDef KEYS[] = {
    // Row 0: Number row
    {0,0,"Esc",VK_ESCAPE,1},{0,1,"1",'1',1},{0,2,"2",'2',1},{0,3,"3",'3',1},
    {0,4,"4",'4',1},{0,5,"5",'5',1},{0,6,"6",'6',1},{0,7,"7",'7',1},
    {0,8,"8",'8',1},{0,9,"9",'9',1},{0,10,"0",'0',1},{0,11,"-",VK_OEM_MINUS,1},
    {0,12,"=",VK_OEM_PLUS,1},{0,13,"Bksp",VK_BACK,2},
    // Row 1: QWERTY
    {1,0,"Tab",VK_TAB,1.5},{1,1,"Q",'Q',1},{1,2,"W",'W',1},{1,3,"E",'E',1},
    {1,4,"R",'R',1},{1,5,"T",'T',1},{1,6,"Y",'Y',1},{1,7,"U",'U',1},
    {1,8,"I",'I',1},{1,9,"O",'O',1},{1,10,"P",'P',1},{1,11,"[",VK_OEM_4,1},
    {1,12,"]",VK_OEM_6,1},
    // Row 2: Home row
    {2,0,"Caps",VK_CAPITAL,1.75},{2,1,"A",'A',1},{2,2,"S",'S',1},{2,3,"D",'D',1},
    {2,4,"F",'F',1},{2,5,"G",'G',1},{2,6,"H",'H',1},{2,7,"J",'J',1},
    {2,8,"K",'K',1},{2,9,"L",'L',1},{2,10,";",VK_OEM_1,1},{2,11,"'",VK_OEM_7,1},
    {2,12,"Enter",VK_RETURN,2.25},
    // Row 3: Shift row
    {3,0,"Shift",VK_LSHIFT,2.25},{3,1,"Z",'Z',1},{3,2,"X",'X',1},{3,3,"C",'C',1},
    {3,4,"V",'V',1},{3,5,"B",'B',1},{3,6,"N",'N',1},{3,7,"M",'M',1},
    {3,8,",",VK_OEM_COMMA,1},{3,9,".",VK_OEM_PERIOD,1},{3,10,"/",VK_OEM_2,1},
    {3,11,"Shift",VK_RSHIFT,2.75},
    // Row 4: Bottom row
    {4,0,"Ctrl",VK_LCONTROL,1.25},{4,1,"Win",VK_LWIN,1.25},{4,2,"Alt",VK_LMENU,1.25},
    {4,3,"Space",VK_SPACE,6.25},{4,4,"Alt",VK_RMENU,1.25},{4,5,"App",VK_APPS,1.25},
    {4,6,"Ctrl",VK_RCONTROL,1.25}
};
static const int KEY_COUNT = sizeof(KEYS) / sizeof(KEYS[0]);

// State
static int pressCount[KEY_COUNT] = {};
static float pressTime[KEY_COUNT] = {}; // how long held (seconds)
static int maxPresses = 1;
static bool wasDown[256] = {};
static LARGE_INTEGER freq;

// Colors
static COLORREF heatColor(float ratio) {
    // Blue -> Cyan -> Green -> Yellow -> Red
    int r, g, b;
    if (ratio < 0.25) {
        r = 0; g = (int)(ratio * 4 * 255); b = 255;
    } else if (ratio < 0.5) {
        r = 0; g = 255; b = (int)((0.5 - ratio) * 4 * 255);
    } else if (ratio < 0.75) {
        r = (int)((ratio - 0.5) * 4 * 255); g = 255; b = 0;
    } else {
        r = 255; g = (int)((1.0 - ratio) * 4 * 255); b = 0;
    }
    return RGB(r, g, b);
}

static COLORREF textColor(float ratio) {
    return (ratio > 0.5) ? RGB(255,255,255) : RGB(200,200,200);
}

// Layout constants
static const int KEY_W = 52;
static const int KEY_H = 52;
static const int KEY_GAP = 4;
static const int PAD_X = 30;
static const int PAD_Y = 60;
static const int WIN_W = PAD_X * 2 + (int)(14 * (KEY_W + KEY_GAP));
static const int WIN_H = PAD_Y + 5 * (KEY_H + KEY_GAP) + 80;

static HWND g_hwnd = NULL;

void drawKeyboard(HDC hdc) {
    // Background
    HBRUSH bgBrush = CreateSolidBrush(RGB(30, 30, 30));
    RECT rc = {0, 0, WIN_W, WIN_H};
    FillRect(hdc, &rc, bgBrush);
    DeleteObject(bgBrush);

    // Title
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));
    HFONT titleFont = CreateFontA(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, 0, 0, 0, 0, "Consolas");
    SelectObject(hdc, titleFont);
    char title[128];
    sprintf(title, "Wraith W60 Heatmap  (max: %d presses)", maxPresses);
    TextOutA(hdc, PAD_X, 10, title, (int)strlen(title));
    DeleteObject(titleFont);

    // Draw each key
    HFONT keyFont = CreateFontA(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, 0, 0, 0, 0, "Segoe UI");
    SelectObject(hdc, keyFont);

    // Track X position per row (for variable-width keys)
    float rowX[5] = {};

    for (int i = 0; i < KEY_COUNT; i++) {
        const KeyDef& k = KEYS[i];
        int x = PAD_X + (int)(rowX[k.row] * (KEY_W + KEY_GAP));
        int y = PAD_Y + k.row * (KEY_H + KEY_GAP);
        int w = (int)(k.width * (KEY_W + KEY_GAP)) - KEY_GAP;
        rowX[k.row] += k.width;

        // Heat ratio
        float ratio = (maxPresses > 1) ? (float)pressCount[i] / maxPresses : 0;
        if (pressCount[i] > 0 && ratio < 0.05f) ratio = 0.05f;

        // Key background
        COLORREF bgColor;
        if (pressCount[i] == 0) {
            bgColor = RGB(60, 60, 65);
        } else {
            bgColor = heatColor(ratio);
        }

        HBRUSH keyBrush = CreateSolidBrush(bgColor);
        HPEN keyPen = CreatePen(PS_SOLID, 1, RGB(80, 80, 85));
        SelectObject(hdc, keyBrush);
        SelectObject(hdc, keyPen);

        // Rounded rectangle
        RoundRect(hdc, x, y, x + w, y + KEY_H, 6, 6);
        DeleteObject(keyBrush);
        DeleteObject(keyPen);

        // Key label
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, textColor(ratio));
        RECT keyRc = {x, y, x + w, y + KEY_H - 14};
        DrawTextA(hdc, k.label, -1, &keyRc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        // Press count
        if (pressCount[i] > 0) {
            char cnt[16];
            sprintf(cnt, "%d", pressCount[i]);
            SetTextColor(hdc, RGB(200, 200, 200));
            RECT cntRc = {x, y + KEY_H - 16, x + w, y + KEY_H};
            DrawTextA(hdc, cnt, -1, &cntRc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
    }
    DeleteObject(keyFont);

    // Legend
    HFONT legendFont = CreateFontA(13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, 0, 0, 0, 0, "Segoe UI");
    SelectObject(hdc, legendFont);

    int lx = PAD_X;
    int ly = WIN_H - 50;
    const char* labels[] = {"0", "25", "50", "75", "100"};
    for (int i = 0; i < 5; i++) {
        float ratio = i / 4.0f;
        HBRUSH lb = CreateSolidBrush(heatColor(ratio));
        RECT lrc = {lx, ly, lx + 30, ly + 16};
        FillRect(hdc, &lrc, lb);
        DeleteObject(lb);

        SetTextColor(hdc, RGB(180, 180, 180));
        SetBkMode(hdc, TRANSPARENT);
        TextOutA(hdc, lx + 2, ly + 20, labels[i], (int)strlen(labels[i]));
        lx += 50;
    }

    SetTextColor(hdc, RGB(140, 140, 140));
    TextOutA(hdc, lx + 10, ly + 4, "Press count %", 13);
    DeleteObject(legendFont);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        drawKeyboard(hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    QueryPerformanceFrequency(&freq);

    WNDCLASSA wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "WraithHeatmap";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassA(&wc);

    g_hwnd = CreateWindowExA(0, "WraithHeatmap", "Wraith W60 Heatmap",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, WIN_W, WIN_H,
        NULL, NULL, hInstance, NULL);

    ShowWindow(g_hwnd, SW_SHOW);
    UpdateWindow(g_hwnd);

    LARGE_INTEGER lastTick;
    QueryPerformanceCounter(&lastTick);

    MSG msg;
    while (TRUE) {
        // PeekMessage loop for real-time updates
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) goto done;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Poll keys
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        float dt = (float)(now.QuadPart - lastTick.QuadPart) / freq.QuadPart;
        lastTick = now;

        bool changed = false;
        for (int i = 0; i < KEY_COUNT; i++) {
            int vk = KEYS[i].vk;
            bool down = (GetAsyncKeyState(vk) & 0x8000) != 0;

            if (down && !wasDown[vk]) {
                // Key just pressed
                pressCount[i]++;
                if (pressCount[i] > maxPresses) maxPresses = pressCount[i];
                changed = true;
            }
            wasDown[vk] = down;

            // Track hold time
            if (down) {
                pressTime[i] += dt;
                changed = true;
            }
        }

        if (changed) {
            InvalidateRect(g_hwnd, NULL, FALSE);
        }

        Sleep(16); // ~60fps
    }

done:
    return 0;
}

#else
int main() { printf("Windows only.\n"); return 1; }
#endif
