#include <windows.h>
#include <string>

#define ID_EDIT_X 101
#define ID_EDIT_Y 102
#define ID_BUTTON 103
#define ID_STATIC_RESULT 104

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

// Функція f(x, y) - якщо ділиться без остачі 1, з остачею 0
int f(int x, int y)
{
    if (x % y == 0)
        return 1;
    else
        return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszClassName = L"DivWindowClass";

    RegisterClassExW(&wc);

    HWND hwnd = CreateWindowExW(
        0, L"DivWindowClass", L"Функція f(x, y)",
        WS_OVERLAPPEDWINDOW,
        500, 300,
        500, 350,
        nullptr, nullptr, hInstance, nullptr
    );

    if (hwnd == nullptr)
        return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hEditX;
    static HWND hEditY;
    static HWND hButton;
    static HWND hResult;

    switch (message)
    {
    case WM_CREATE:
    {
        HINSTANCE hInstance = reinterpret_cast<LPCREATESTRUCTW>(lParam)->hInstance;

        hEditX = CreateWindowExW(0, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT,
            50, 70, 100, 25,
            hwnd, reinterpret_cast<HMENU>(ID_EDIT_X), hInstance, nullptr);

        hEditY = CreateWindowExW(0, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT,
            180, 70, 100, 25,
            hwnd, reinterpret_cast<HMENU>(ID_EDIT_Y), hInstance, nullptr);

        hButton = CreateWindowExW(0, L"BUTTON", L"Розрахувати",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            50, 120, 120, 30,
            hwnd, reinterpret_cast<HMENU>(ID_BUTTON), hInstance, nullptr);

        hResult = CreateWindowExW(0, L"STATIC", L"",
            WS_CHILD | WS_VISIBLE,
            180, 170, 250, 25,
            hwnd, reinterpret_cast<HMENU>(ID_STATIC_RESULT), hInstance, nullptr);
        break;
    }

    case WM_COMMAND:
    {
        if (LOWORD(wParam) == ID_BUTTON)
        {
            wchar_t bufferX[50]{};
            wchar_t bufferY[50]{};

            GetWindowTextW(hEditX, bufferX, 50);
            GetWindowTextW(hEditY, bufferY, 50);

            try
            {
                int x = std::stoi(bufferX);
                int y = std::stoi(bufferY);

                if (y == 0)
                {
                    SetWindowTextW(hResult, L"Помилка: y не може дорівнювати 0");
                }
                else
                {
                    int value = f(x, y);
                    SetWindowTextW(hResult, std::to_wstring(value).c_str());
                }
            }
            catch (...)
            {
                SetWindowTextW(hResult, L"Помилка введення");
            }
        }
        break;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        SetBkMode(hdc, TRANSPARENT);
        TextOutW(hdc, 50, 20, L"Введіть цілі числа x та y (у не може бути 0):", 45);
        TextOutW(hdc, 50, 50, L"x", 1);
        TextOutW(hdc, 180, 50, L"y", 1);
        TextOutW(hdc, 50, 170, L"Результат f(x, y):", 18);
        EndPaint(hwnd, &ps);
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProcW(hwnd, message, wParam, lParam);
    }
    return 0;
}