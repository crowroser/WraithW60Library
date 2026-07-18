// Wraith W60 Heatmap - Real-time key press visualization
// Uses GetAsyncKeyState for polling-based key detection.
// Shows which keys are pressed and how often.

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <map>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

struct KeyDef {
    int row, col;
    const char* name;
    int vkCode;
};

static const KeyDef KEYS[] = {
    {0,0,"Esc",VK_ESCAPE}, {0,1,"1",'1'}, {0,2,"2",'2'}, {0,3,"3",'3'},
    {0,4,"4",'4'}, {0,5,"5",'5'}, {0,6,"6",'6'}, {0,7,"7",'7'},
    {0,8,"8",'8'}, {0,9,"9",'9'}, {0,10,"0",'0'}, {0,11,"-",VK_OEM_MINUS},
    {0,12,"=",VK_OEM_PLUS}, {0,13,"Bksp",VK_BACK},
    {1,0,"Tab",VK_TAB}, {1,1,"Q",'Q'}, {1,2,"W",'W'}, {1,3,"E",'E'},
    {1,4,"R",'R'}, {1,5,"T",'T'}, {1,6,"Y",'Y'}, {1,7,"U",'U'},
    {1,8,"I",'I'}, {1,9,"O",'O'}, {1,10,"P",'P'}, {1,11,"[",VK_OEM_4},
    {1,12,"]",VK_OEM_6},
    {2,0,"Caps",VK_CAPITAL}, {2,1,"A",'A'}, {2,2,"S",'S'}, {2,3,"D",'D'},
    {2,4,"F",'F'}, {2,5,"G",'G'}, {2,6,"H",'H'}, {2,7,"J",'J'},
    {2,8,"K",'K'}, {2,9,"L",'L'}, {2,10,";",VK_OEM_1}, {2,11,"'",VK_OEM_7},
    {2,12,"Enter",VK_RETURN},
    {3,0,"Shift",VK_LSHIFT}, {3,1,"Z",'Z'}, {3,2,"X",'X'}, {3,3,"C",'C'},
    {3,4,"V",'V'}, {3,5,"B",'B'}, {3,6,"N",'N'}, {3,7,"M",'M'},
    {3,8,",",VK_OEM_COMMA}, {3,9,".",VK_OEM_PERIOD}, {3,10,"/",VK_OEM_2},
    {3,11,"Shift",VK_RSHIFT},
    {4,0,"Ctrl",VK_LCONTROL}, {4,1,"Win",VK_LWIN}, {4,2,"Alt",VK_LMENU},
    {4,3,"Space",VK_SPACE}, {4,4,"Alt",VK_RMENU}, {4,5,"Menu",VK_APPS},
    {4,6,"Ctrl",VK_RCONTROL}
};
static const int KEY_COUNT = sizeof(KEYS) / sizeof(KEYS[0]);

static int pressCount[5][14] = {};
static int maxPresses = 1;
static bool keyCurrentlyDown[256] = {};

void printHeatmap() {
    int rowWidths[] = {14, 13, 13, 12, 7};
    float rowOffsets[] = {0, 0.5, 0.75, 1.0, 0.5};

    // Move cursor to top-left
    COORD coord = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

    printf("=== Wraith W60 Heatmap ===  (max: %d presses)\n\n", maxPresses);

    for (int r = 0; r < 5; r++) {
        for (int s = 0; s < (int)(rowOffsets[r] * 4); s++) printf(" ");
        for (int c = 0; c < rowWidths[r]; c++) {
            int count = pressCount[r][c];
            char h;
            if (count == 0) h = ' ';
            else {
                float ratio = (float)count / maxPresses;
                if (ratio > 0.8) h = '@';
                else if (ratio > 0.6) h = '#';
                else if (ratio > 0.4) h = '+';
                else if (ratio > 0.2) h = '=';
                else h = '.';
            }
            // Find key name
            const char* name = "?";
            for (int i = 0; i < KEY_COUNT; i++) {
                if (KEYS[i].row == r && KEYS[i].col == c) {
                    name = KEYS[i].name;
                    break;
                }
            }
            printf("[%c]%s ", h, name);
        }
        printf("\n");
    }

    printf("\nLegend: .=<20%%  =20-40%%  +40-60%%  #60-80%%  @>80%%\n");
    printf("Currently active: ");
    int activeCount = 0;
    for (int i = 0; i < KEY_COUNT; i++) {
        if (keyCurrentlyDown[KEYS[i].vkCode]) {
            printf("%s ", KEYS[i].name);
            activeCount++;
        }
    }
    if (activeCount == 0) printf("(none)");
    printf("\n\nPress ESC to exit.\n");
}

int main() {
    printf("Wraith W60 Heatmap\n");
    printf("Press keys to build the heatmap! ESC to exit.\n\n");

    // Enable ANSI escape codes
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hOut, &mode);
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    printHeatmap();

    auto start = std::chrono::steady_clock::now();

    while (true) {
        // Check ESC
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;

        bool changed = false;

        for (int i = 0; i < KEY_COUNT; i++) {
            int vk = KEYS[i].vkCode;
            bool isDown = (GetAsyncKeyState(vk) & 0x8000) != 0;

            if (isDown && !keyCurrentlyDown[vk]) {
                // Key just pressed
                pressCount[KEYS[i].row][KEYS[i].col]++;
                if (pressCount[KEYS[i].row][KEYS[i].col] > maxPresses)
                    maxPresses = pressCount[KEYS[i].row][KEYS[i].col];
                changed = true;
            }
            keyCurrentlyDown[vk] = isDown;
        }

        if (changed) printHeatmap();

        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60fps
    }

    // Final report
    printf("\n\n=== Final Heatmap ===\n");
    int rowWidths[] = {14, 13, 13, 12, 7};
    for (int r = 0; r < 5; r++) {
        for (int c = 0; c < rowWidths[r]; c++) {
            printf("%3d ", pressCount[r][c]);
        }
        printf("\n");
    }

    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - start).count();
    printf("\nTotal time: %lld seconds\n", (long long)elapsed);

    return 0;
}
