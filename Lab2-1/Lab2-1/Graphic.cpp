#include <windows.h>
#include <math.h>
#include <stdio.h>

BOOL RegClass(WNDPROC, LPCTSTR, UINT);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE hInstance;
wchar_t szClassName[] = L"GraphClass";

#define N 100

typedef struct
{
    wchar_t name[20];
    float x[N];
    float y[N];
    float xmin, xmax;   // межі аргументу х
    float ymin, ymax;   // межі функції у 
} FUNC;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpszCmdLine, _In_ int nCmdShow)
{
    MSG msg; HWND hwnd;
    ::hInstance = hInstance;
    if (!RegClass(WndProc, szClassName, COLOR_WINDOW))
        return FALSE;
    hwnd = CreateWindow(szClassName, L"Графік функції y = sqrt(x^2+5)",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 700, 500,
        0, 0, hInstance, NULL);
    if (!hwnd) return FALSE;
    while (GetMessage(&msg, 0, 0, 0)) DispatchMessage(&msg);
    return (int)msg.wParam;
}

BOOL RegClass(WNDPROC Proc, LPCTSTR szName, UINT brBackground)
{
    WNDCLASS wc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.cbClsExtra = wc.cbWndExtra = 0;
    wc.lpfnWndProc = Proc;
    wc.hInstance = hInstance;
    wc.lpszClassName = szName;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(UINT_PTR)(brBackground + 1);
    wc.lpszMenuName = NULL;
    return (RegisterClass(&wc) != 0);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static int cx, cy;
    static FUNC f;

    switch (msg)
    {
    case WM_SIZE:
    {
        cx = LOWORD(lParam);
        cy = HIWORD(lParam);
        return 0;
    }
    case WM_CREATE:
    {
        wcscpy_s(f.name, 20, L"y = sqrt(x^2+5)");

        // межі аргументу -2<=x<=8
        f.xmin = -2.0f;
        f.xmax = 8.0f;

        float ymaxReal = 0.0f;
        for (int i = 0; i < N; i++)
        {
            f.x[i] = f.xmin + (f.xmax - f.xmin) * i / (N - 1);
            f.y[i] = sqrtf(f.x[i] * f.x[i] + 5.0f);
            if (f.y[i] > ymaxReal) ymaxReal = f.y[i];
        }

        //нижня і верхня межа
        f.ymin = 0.0f;
        f.ymax = ceilf(ymaxReal);
        return 0;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        int x0 = cx / 8;         // лівий крайт
        int xr = cx - cx / 15;   // правий край
        int y0 = cy / 10;        // верхній край 
        int yb = cy - cy / 6;    // нижній край 

        float dx = (float)(xr - x0) / (f.xmax - f.xmin);
        float dy = (float)(yb - y0) / (f.ymax - f.ymin);

        // Лямбди для перерахунку координат функції в екранні пікселі
        auto SX = [&](float xv) -> int { return x0 + (int)(dx * (xv - f.xmin)); };
        auto SY = [&](float yv) -> int { return yb - (int)(dy * (yv - f.ymin)); };

        // Заголовок 
        TextOut(hdc, x0 + 4, y0 / 3, f.name, (int)wcslen(f.name));

        // Вісь абсцис (x), розташована на рівні y = 0 
        int axisXpos = SY(0.0f);
        MoveToEx(hdc, x0, axisXpos, NULL);
        LineTo(hdc, xr + 10, axisXpos);
        TextOut(hdc, xr + 12, axisXpos - 8, L"x", 1);

        // Вісь ординат (y), розташована на рівні x = 0 
        int axisYpos = SX(0.0f);
        MoveToEx(hdc, axisYpos, yb, NULL);
        LineTo(hdc, axisYpos, y0 - 10);
        TextOut(hdc, axisYpos + 6, y0 - 20, L"y", 1);

        wchar_t buf[16];

        // Розмітка і підписи осі х 
        for (int xv = (int)ceilf(f.xmin); xv <= (int)floorf(f.xmax); xv++)
        {
            int sx = SX((float)xv);
            MoveToEx(hdc, sx, axisXpos - 3, NULL);
            LineTo(hdc, sx, axisXpos + 3);
            swprintf_s(buf, 16, L"%d", xv);
            TextOut(hdc, sx - 4, axisXpos + 6, buf, (int)wcslen(buf));
        }

        // Розмітка і підписи осі у 
        for (int yv = (int)ceilf(f.ymin); yv <= (int)floorf(f.ymax); yv++)
        {
            int sy = SY((float)yv);
            MoveToEx(hdc, axisYpos - 3, sy, NULL);
            LineTo(hdc, axisYpos + 3, sy);
            swprintf_s(buf, 16, L"%d", yv);
            TextOut(hdc, axisYpos - 24, sy - 8, buf, (int)wcslen(buf));
        }

        // графік функції
        HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

        MoveToEx(hdc, SX(f.x[0]), SY(f.y[0]), NULL);
        for (int i = 1; i < N; i++)
            LineTo(hdc, SX(f.x[i]), SY(f.y[i]));

        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);

        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}