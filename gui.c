#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/huffman.h"
#include "src/file_io.h"

#define ID_INPUT      101
#define ID_OUTPUT     102
#define ID_BROWSE_IN  103
#define ID_BROWSE_OUT 104
#define ID_COMPRESS   105
#define ID_DECOMPRESS 106
#define ID_STATUS     107

#define BG_COLOR      RGB(18, 25, 40)
#define FIELD_COLOR   RGB(39, 51, 73)
#define TEXT_COLOR    RGB(235, 241, 250)
#define MUTED_COLOR   RGB(160, 175, 198)
#define BLUE_COLOR    RGB(50, 125, 235)
#define GREEN_COLOR   RGB(55, 180, 125)
#define RED_COLOR     RGB(225, 85, 95)

static HWND inputBox, outputBox, statusBox;
static HINSTANCE appInstance;
static HFONT titleFont, normalFont, buttonFont;
static HBRUSH bgBrush, fieldBrush;
static COLORREF statusColor = MUTED_COLOR;

static void set_status(const char *text, COLORREF color)
{
    statusColor = color;
    SetWindowTextA(statusBox, text);
    InvalidateRect(statusBox, NULL, TRUE);
}

static void choose_file(HWND owner, BOOL save, HWND target)
{
    char path[MAX_PATH] = "";
    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = owner;
    ofn.lpstrFile = path;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = "All files\0*.*\0";
    ofn.Flags = OFN_PATHMUSTEXIST |
                (save ? OFN_OVERWRITEPROMPT : OFN_FILEMUSTEXIST);

    BOOL ok = save ? GetSaveFileNameA(&ofn) : GetOpenFileNameA(&ofn);
    if (ok)
        SetWindowTextA(target, path);
}

static void run_operation(HWND owner, BOOL compress)
{
    char input[MAX_PATH], output[MAX_PATH];
    GetWindowTextA(inputBox, input, MAX_PATH);
    GetWindowTextA(outputBox, output, MAX_PATH);

    if (!input[0] || !output[0]) {
        set_status("Select an input file and an output location.", RED_COLOR);
        MessageBoxA(owner, "Choose both an input and output file.",
                    "Missing file", MB_OK | MB_ICONWARNING);
        return;
    }

    if (_stricmp(input, output) == 0) {
        set_status("Input and output paths must be different.", RED_COLOR);
        MessageBoxA(owner, "Input and output files must be different.",
                    "Invalid output", MB_OK | MB_ICONWARNING);
        return;
    }

    DWORD attributes = GetFileAttributesA(output);
    if (attributes != INVALID_FILE_ATTRIBUTES) {
        if (MessageBoxA(owner, "Output file already exists. Replace it?",
                        "Confirm overwrite",
                        MB_YESNO | MB_ICONWARNING) != IDYES) {
            set_status("Cancelled; existing file was not changed.", MUTED_COLOR);
            return;
        }
    }

    uint8_t *in = NULL, *out = NULL;
    size_t inSize = 0, outSize = 0;

    set_status("Working...", BLUE_COLOR);
    UpdateWindow(statusBox);

    if (read_file(input, &in, &inSize) != 0) {
        set_status("Could not read the input file.", RED_COLOR);
        MessageBoxA(owner, "Could not read the input file.",
                    "Read error", MB_OK | MB_ICONERROR);
        return;
    }

    HufStatus result = compress
        ? huf_compress(in, inSize, &out, &outSize)
        : huf_decompress(in, inSize, &out, &outSize);

    free(in);

    if (result != HUF_OK) {
        char message[256];
        snprintf(message, sizeof(message), "Huffman operation failed: %s",
                 huf_strerror(result));
        set_status(message, RED_COLOR);
        MessageBoxA(owner, message, "Operation failed",
                    MB_OK | MB_ICONERROR);
        free(out);
        return;
    }

    if (write_file(output, out, outSize) != 0) {
        free(out);
        set_status("Could not write the output file.", RED_COLOR);
        MessageBoxA(owner, "Could not write the output file.",
                    "Write error", MB_OK | MB_ICONERROR);
        return;
    }

    free(out);

    char message[300];
    if (compress && inSize > 0) {
        double ratio = 100.0 * (double)outSize / (double)inSize;
        snprintf(message, sizeof(message),
                 "Compression complete | Original: %llu bytes | Compressed: %llu bytes | %.1f%% of original",
                 (unsigned long long)inSize,
                 (unsigned long long)outSize, ratio);
    } else {
        snprintf(message, sizeof(message),
                 "Decompression complete | Input: %llu bytes | Output: %llu bytes",
                 (unsigned long long)inSize,
                 (unsigned long long)outSize);
    }

    set_status(message, GREEN_COLOR);
}

static HWND add_control(HWND parent, const char *klass, const char *text,
                        DWORD style, int x, int y, int w, int h, int id)
{
    HWND control = CreateWindowExA(
        0, klass, text, WS_CHILD | WS_VISIBLE | style,
        x, y, w, h, parent, (HMENU)(INT_PTR)id,
        appInstance, NULL);

    if (normalFont)
        SendMessageA(control, WM_SETFONT, (WPARAM)normalFont, TRUE);

    return control;
}

static void draw_button(const DRAWITEMSTRUCT *item)
{
    HDC dc = item->hDC;
    RECT rect = item->rcItem;
    COLORREF color = item->CtlID == ID_COMPRESS
        ? BLUE_COLOR : RGB(48, 62, 85);

    if (item->itemState & ODS_SELECTED)
        color = RGB(35, 95, 190);

    HBRUSH brush = CreateSolidBrush(color);
    FillRect(dc, &rect, brush);
    DeleteObject(brush);

    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, TEXT_COLOR);
    SelectObject(dc, buttonFont);

    char text[64];
    GetWindowTextA(item->hwndItem, text, sizeof(text));
    DrawTextA(dc, text, -1, &rect,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

static LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg) {
    case WM_CREATE:
        titleFont = CreateFontA(
            25, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH, "Segoe UI");

        normalFont = CreateFontA(
            15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH, "Segoe UI");

        buttonFont = CreateFontA(
            16, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH, "Segoe UI");

        bgBrush = CreateSolidBrush(BG_COLOR);
        fieldBrush = CreateSolidBrush(FIELD_COLOR);

        add_control(hwnd, "STATIC", "HUFFMAN", 0, 26, 18, 250, 34, 0);
        add_control(hwnd, "STATIC", "FILE COMPRESSION TOOL", 0,
                    28, 52, 300, 22, 0);

        add_control(hwnd, "STATIC", "INPUT FILE", 0,
                    28, 95, 120, 20, 0);
        inputBox = add_control(hwnd, "EDIT", "",
                    WS_BORDER | ES_AUTOHSCROLL,
                    28, 118, 465, 30, ID_INPUT);
        add_control(hwnd, "BUTTON", "Browse", BS_PUSHBUTTON,
                    505, 118, 95, 30, ID_BROWSE_IN);

        add_control(hwnd, "STATIC", "OUTPUT FILE", 0,
                    28, 169, 120, 20, 0);
        outputBox = add_control(hwnd, "EDIT", "",
                    WS_BORDER | ES_AUTOHSCROLL,
                    28, 192, 465, 30, ID_OUTPUT);
        add_control(hwnd, "BUTTON", "Save as", BS_PUSHBUTTON,
                    505, 192, 95, 30, ID_BROWSE_OUT);

        add_control(hwnd, "BUTTON", "Compress", BS_OWNERDRAW,
                    28, 246, 180, 42, ID_COMPRESS);
        add_control(hwnd, "BUTTON", "Decompress", BS_OWNERDRAW,
                    220, 246, 180, 42, ID_DECOMPRESS);

        add_control(hwnd, "STATIC", "STATUS", 0,
                    28, 315, 120, 20, 0);
        statusBox = add_control(hwnd, "STATIC",
                    "Choose an input file to get started.",
                    SS_LEFT, 28, 338, 570, 55, ID_STATUS);
        return 0;

    case WM_CTLCOLORSTATIC: {
        HDC dc = (HDC)wp;
        HWND control = (HWND)lp;

        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, control == statusBox ? statusColor : TEXT_COLOR);
        return (LRESULT)bgBrush;
    }

    case WM_CTLCOLOREDIT: {
        HDC dc = (HDC)wp;
        SetTextColor(dc, TEXT_COLOR);
        SetBkColor(dc, FIELD_COLOR);
        return (LRESULT)fieldBrush;
    }

    case WM_DRAWITEM:
        draw_button((const DRAWITEMSTRUCT *)lp);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wp)) {
        case ID_BROWSE_IN:
            choose_file(hwnd, FALSE, inputBox);
            return 0;
        case ID_BROWSE_OUT:
            choose_file(hwnd, TRUE, outputBox);
            return 0;
        case ID_COMPRESS:
            run_operation(hwnd, TRUE);
            return 0;
        case ID_DECOMPRESS:
            run_operation(hwnd, FALSE);
            return 0;
        }
        break;

    case WM_DESTROY:
        DeleteObject(titleFont);
        DeleteObject(normalFont);
        DeleteObject(buttonFont);
        DeleteObject(bgBrush);
        DeleteObject(fieldBrush);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcA(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous, LPSTR command, int show)
{
    (void)previous;
    (void)command;
    appInstance = instance;

    WNDCLASSA wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.lpfnWndProc = window_proc;
    wc.hInstance = instance;
    wc.lpszClassName = "HuffmanGuiWindow";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(BG_COLOR);

    if (!RegisterClassA(&wc))
        return 1;

    HWND hwnd = CreateWindowA(
        wc.lpszClassName, "Huffman Compression Tool",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        660, 450, NULL, NULL, instance, NULL);

    if (!hwnd)
        return 1;

    ShowWindow(hwnd, show);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return (int)msg.wParam;
}