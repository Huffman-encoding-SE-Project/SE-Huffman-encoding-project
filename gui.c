#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/huffman.h"
#include "src/file_io.h"

#define ID_INPUT       101
#define ID_OUTPUT      102
#define ID_BROWSE_IN   103
#define ID_BROWSE_OUT  104
#define ID_COMPRESS    105
#define ID_DECOMPRESS  106
#define ID_STATUS      107

#define ID_TITLE       201
#define ID_SUBTITLE    202
#define ID_INPUT_LABEL 203
#define ID_OUTPUT_LABEL 204
#define ID_STATUS_LABEL 205

#define BG_COLOR      RGB(15, 23, 42)
#define PANEL_COLOR   RGB(22, 32, 52)
#define FIELD_COLOR   RGB(31, 43, 65)
#define TEXT_COLOR    RGB(235, 241, 250)
#define MUTED_COLOR   RGB(153, 170, 195)
#define BLUE_COLOR    RGB(55, 125, 245)
#define BLUE_HOVER    RGB(75, 145, 255)
#define BUTTON_COLOR  RGB(43, 58, 82)
#define GREEN_COLOR   RGB(60, 190, 135)
#define RED_COLOR     RGB(240, 95, 105)

static HWND inputBox, outputBox, statusBox;
static HINSTANCE appInstance;
static HFONT titleFont, normalFont, buttonFont;
static HBRUSH bgBrush, panelBrush, fieldBrush;
static COLORREF statusColor = MUTED_COLOR;
static BOOL customOutput = FALSE;

static void set_status(const char *text, COLORREF color)
{
    statusColor = color;
    SetWindowTextA(statusBox, text);
    InvalidateRect(statusBox, NULL, TRUE);
}

static int has_extension(const char *path, const char *extension)
{
    const char *dot = strrchr(path, '.');
    const char *slash = strrchr(path, '/');
    const char *backslash = strrchr(path, '\\');

    if (slash == NULL || (backslash != NULL && backslash > slash))
        slash = backslash;

    return dot != NULL && (slash == NULL || dot > slash)
        && _stricmp(dot, extension) == 0;
}

static void make_default_output(const char *input, BOOL compress,
                               char *output, size_t outputSize)
{
    const char *slash = strrchr(input, '\\');
    const char *forwardSlash = strrchr(input, '/');

    if (forwardSlash && (!slash || forwardSlash > slash))
        slash = forwardSlash;

    const char *filename = slash ? slash + 1 : input;
    size_t folderLength = slash ? (size_t)(slash - input + 1) : 0;

    char folder[MAX_PATH];
    char base[MAX_PATH];

    if (folderLength >= sizeof(folder))
        folderLength = sizeof(folder) - 1;

    memcpy(folder, input, folderLength);
    folder[folderLength] = '\0';

    strncpy(base, filename, sizeof(base) - 1);
    base[sizeof(base) - 1] = '\0';

    char *dot = strrchr(base, '.');
    if (dot)
        *dot = '\0';

    snprintf(output, outputSize, "%s%s_%s.%s",
             folder, base,
             compress ? "compressed" : "decompressed",
             compress ? "huf" : "bin");
}

static void choose_file(HWND owner, BOOL save)
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

    if (save)
        ofn.lpstrDefExt = "huf";

    BOOL ok = save ? GetSaveFileNameA(&ofn) : GetOpenFileNameA(&ofn);
    if (!ok)
        return;

    SetWindowTextA(save ? outputBox : inputBox, path);

    if (save) {
        customOutput = TRUE;
        set_status("Custom output location selected.", MUTED_COLOR);
    } else {
        char generated[MAX_PATH];
        customOutput = FALSE;
        make_default_output(path, TRUE, generated, sizeof(generated));
        SetWindowTextA(outputBox, generated);
        set_status("Input selected. Output path generated automatically.",
                   MUTED_COLOR);
    }
}

static void run_operation(HWND owner, BOOL compress)
{
    char input[MAX_PATH] = "";
    char output[MAX_PATH] = "";

    GetWindowTextA(inputBox, input, sizeof(input));

    if (!input[0]) {
        set_status("Select an input file first.", RED_COLOR);
        MessageBoxA(owner, "Choose an input file.",
                    "Missing input", MB_OK | MB_ICONWARNING);
        return;
    }

    if (customOutput) {
        GetWindowTextA(outputBox, output, sizeof(output));
    } else {
        make_default_output(input, compress, output, sizeof(output));
        SetWindowTextA(outputBox, output);
    }

    if (!output[0]) {
        set_status("Could not generate an output path.", RED_COLOR);
        return;
    }

    if (_stricmp(input, output) == 0) {
        set_status("Input and output paths must be different.", RED_COLOR);
        MessageBoxA(owner, "Input and output files must be different.",
                    "Invalid output", MB_OK | MB_ICONWARNING);
        return;
    }

    if (compress && !has_extension(output, ".huf")) {
        set_status("Compression output must use .huf.", RED_COLOR);
        MessageBoxA(owner, "Compressed output must use the .huf extension.",
                    "Invalid output extension", MB_OK | MB_ICONWARNING);
        return;
    }

    if (!compress && !has_extension(input, ".huf")) {
        set_status("Decompression input must use .huf.", RED_COLOR);
        MessageBoxA(owner, "Decompression input must use the .huf extension.",
                    "Invalid input extension", MB_OK | MB_ICONWARNING);
        return;
    }

    DWORD attributes = GetFileAttributesA(output);
    if (attributes != INVALID_FILE_ATTRIBUTES) {
        if (MessageBoxA(owner, "The output file already exists. Replace it?",
                        "Confirm overwrite",
                        MB_YESNO | MB_ICONWARNING) != IDYES) {
            set_status("Cancelled. Existing file was not changed.", MUTED_COLOR);
            return;
        }
    }

    uint8_t *in = NULL, *out = NULL;
    size_t inSize = 0, outSize = 0;

    set_status("Processing file...", BLUE_COLOR);

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
        char error[256];
        snprintf(error, sizeof(error), "Operation failed: %s",
                 huf_strerror(result));
        set_status(error, RED_COLOR);
        MessageBoxA(owner, error, "Operation failed",
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

    char success[300];
    if (compress && inSize > 0) {
        snprintf(success, sizeof(success),
                 "Compression complete  |  %llu bytes -> %llu bytes  |  %.1f%%",
                 (unsigned long long)inSize,
                 (unsigned long long)outSize,
                 100.0 * (double)outSize / (double)inSize);
    } else {
        snprintf(success, sizeof(success),
                 "Decompression complete  |  %llu bytes -> %llu bytes",
                 (unsigned long long)inSize,
                 (unsigned long long)outSize);
    }

    set_status(success, GREEN_COLOR);
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
    RECT r = item->rcItem;
    COLORREF color = item->CtlID == ID_COMPRESS
        ? BLUE_COLOR : BUTTON_COLOR;

    if (item->itemState & ODS_SELECTED)
        color = RGB(35, 95, 190);

    HBRUSH brush = CreateSolidBrush(color);
    FillRect(item->hDC, &r, brush);
    DeleteObject(brush);

    SetBkMode(item->hDC, TRANSPARENT);
    SetTextColor(item->hDC, TEXT_COLOR);
    SelectObject(item->hDC, buttonFont);

    char text[64];
    GetWindowTextA(item->hwndItem, text, sizeof(text));
    DrawTextA(item->hDC, text, -1, &r,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

static void layout_controls(HWND hwnd)
{
    RECT client;
    GetClientRect(hwnd, &client);

    int width = client.right - client.left;
    int height = client.bottom - client.top;

    int contentWidth = 620;
    int x = (width - contentWidth) / 2;
    if (x < 24) x = 24;

    int y = (height - 430) / 2;
    if (y < 30) y = 30;

    MoveWindow(GetDlgItem(hwnd, ID_TITLE), x, y, 300, 38, TRUE);
    MoveWindow(GetDlgItem(hwnd, ID_SUBTITLE), x + 2, y + 40, 320, 24, TRUE);

    MoveWindow(GetDlgItem(hwnd, ID_INPUT_LABEL), x, y + 88, 140, 22, TRUE);
    MoveWindow(inputBox, x, y + 112, 480, 36, TRUE);
    MoveWindow(GetDlgItem(hwnd, ID_BROWSE_IN), x + 495, y + 112, 110, 36, TRUE);

    MoveWindow(GetDlgItem(hwnd, ID_OUTPUT_LABEL), x, y + 172, 140, 22, TRUE);
    MoveWindow(outputBox, x, y + 196, 480, 36, TRUE);
    MoveWindow(GetDlgItem(hwnd, ID_BROWSE_OUT), x + 495, y + 196, 110, 36, TRUE);

    MoveWindow(GetDlgItem(hwnd, ID_COMPRESS), x, y + 260, 195, 48, TRUE);
    MoveWindow(GetDlgItem(hwnd, ID_DECOMPRESS), x + 210, y + 260, 195, 48, TRUE);

    MoveWindow(GetDlgItem(hwnd, ID_STATUS_LABEL), x, y + 332, 140, 22, TRUE);
    MoveWindow(statusBox, x, y + 358, contentWidth, 56, TRUE);
}

static LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg) {
    case WM_CREATE:
        titleFont = CreateFontA(29, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH, "Segoe UI");

        normalFont = CreateFontA(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH, "Segoe UI");

        buttonFont = CreateFontA(16, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH, "Segoe UI");

        panelBrush = CreateSolidBrush(PANEL_COLOR);
        fieldBrush = CreateSolidBrush(FIELD_COLOR);

        HWND title = add_control(hwnd, "STATIC", "HUFFMAN",
            SS_LEFT, 0, 0, 300, 38, ID_TITLE);
        SendMessageA(title, WM_SETFONT, (WPARAM)titleFont, TRUE);

        add_control(hwnd, "STATIC", "FILE COMPRESSION TOOL",
            SS_LEFT, 0, 0, 320, 24, ID_SUBTITLE);

        add_control(hwnd, "STATIC", "INPUT FILE",
            SS_LEFT, 0, 0, 140, 22, ID_INPUT_LABEL);
        inputBox = add_control(hwnd, "EDIT", "",
            WS_BORDER | ES_AUTOHSCROLL, 0, 0, 480, 36, ID_INPUT);
        add_control(hwnd, "BUTTON", "Browse",
            BS_PUSHBUTTON, 0, 0, 110, 36, ID_BROWSE_IN);

        add_control(hwnd, "STATIC", "OUTPUT FILE",
            SS_LEFT, 0, 0, 140, 22, ID_OUTPUT_LABEL);
        outputBox = add_control(hwnd, "EDIT", "",
            WS_BORDER | ES_AUTOHSCROLL, 0, 0, 480, 36, ID_OUTPUT);
        add_control(hwnd, "BUTTON", "Save as",
            BS_PUSHBUTTON, 0, 0, 110, 36, ID_BROWSE_OUT);

        add_control(hwnd, "BUTTON", "Compress",
            BS_OWNERDRAW, 0, 0, 195, 48, ID_COMPRESS);
        add_control(hwnd, "BUTTON", "Decompress",
            BS_OWNERDRAW, 0, 0, 195, 48, ID_DECOMPRESS);

        add_control(hwnd, "STATIC", "STATUS",
            SS_LEFT, 0, 0, 140, 22, ID_STATUS_LABEL);
        statusBox = add_control(hwnd, "STATIC",
            "Choose an input file to get started.",
            SS_LEFT, 0, 0, 620, 56, ID_STATUS);

        layout_controls(hwnd);
        return 0;

    case WM_SIZE:
        layout_controls(hwnd);
        return 0;

    case WM_ERASEBKGND: {
        RECT r;
        GetClientRect(hwnd, &r);
        FillRect((HDC)wp, &r, bgBrush);
        return 1;
    }

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
            choose_file(hwnd, FALSE);
            return 0;
        case ID_BROWSE_OUT:
            choose_file(hwnd, TRUE);
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
        DeleteObject(panelBrush);
        DeleteObject(fieldBrush);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcA(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous,
                   LPSTR command, int show)
{
    (void)previous;
    (void)command;

    appInstance = instance;
    bgBrush = CreateSolidBrush(BG_COLOR);

    WNDCLASSA wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.lpfnWndProc = window_proc;
    wc.hInstance = instance;
    wc.lpszClassName = "HuffmanGuiWindow";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = bgBrush;

    if (!RegisterClassA(&wc))
        return 1;

    int windowWidth = 820;
    int windowHeight = 620;
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    HWND hwnd = CreateWindowA(
        wc.lpszClassName,
        "Huffman Compression Tool",
        WS_OVERLAPPEDWINDOW,
        (screenWidth - windowWidth) / 2,
        (screenHeight - windowHeight) / 2,
        windowWidth, windowHeight,
        NULL, NULL, instance, NULL);

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