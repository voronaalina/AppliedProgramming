#include "framework.h"
#include "Lab3_2.h"

#define MAX_LOADSTRING 100

// Стан програми 
static bool showPicture = false;   // чи виведено зображення
static int  skyIndex = 0;          // колір неба: 0 - день, 1 - вечір, 2 - ніч
static int  sunIndex = 0;          // колір сонця: 0 - жовтий, 1 - світло-жовтий
static bool addedObject = false;   // чи додано сонце

static const COLORREF skyColors[3] =
{
    RGB(176, 224, 230),   // день
    RGB(255, 160, 122),   // вечір
    RGB(25, 25, 112)      // ніч
};

static const COLORREF sunColors[2] =
{
    RGB(255, 215, 0),     // сонце
    RGB(249, 255, 231),   // місяць
};


// Клас для малювання ялинки
class FirTree
{
public:
    void show(HDC dc, int X, int Y)
    {
        // Стовбур
        HBRUSH trunkBrush = CreateSolidBrush(RGB(102, 51, 0));
        HGDIOBJ oldBrush = SelectObject(dc, trunkBrush);
        Rectangle(dc, X - 8, Y - 30, X + 8, Y);
        SelectObject(dc, oldBrush);
        DeleteObject(trunkBrush);

        // Три яруси для ялинки
        HBRUSH greenBrush = CreateSolidBrush(RGB(0, 100, 0));
        oldBrush = SelectObject(dc, greenBrush);

        POINT tier1[3] = { { X - 70, Y - 30 }, { X + 70, Y - 30 }, { X, Y - 110 } };
        Polygon(dc, tier1, 3);

        POINT tier2[3] = { { X - 55, Y - 85 }, { X + 55, Y - 85 }, { X, Y - 155 } };
        Polygon(dc, tier2, 3);

        POINT tier3[3] = { { X - 40, Y - 135 }, { X + 40, Y - 135 }, { X, Y - 195 } };
        Polygon(dc, tier3, 3);

        SelectObject(dc, oldBrush);
        DeleteObject(greenBrush);
    }
};

// Клас для малювання будинку
class House
{
public:
    void show(HDC dc, int X, int Y)
    {
        HBRUSH brush;
        HGDIOBJ oldBrush;

        // Стіни будинку 
        brush = CreateSolidBrush(RGB(255, 235, 205));
        oldBrush = SelectObject(dc, brush);
        Rectangle(dc, X, Y + 60, X + 160, Y + 160);
        SelectObject(dc, oldBrush);
        DeleteObject(brush);

        // Дах
        brush = CreateSolidBrush(RGB(139, 0, 0));
        oldBrush = SelectObject(dc, brush);
        POINT roof[3] = { { X - 15, Y + 60 }, { X + 175, Y + 60 }, { X + 80, Y } };
        Polygon(dc, roof, 3);
        SelectObject(dc, oldBrush);
        DeleteObject(brush);

        // Двері
        brush = CreateSolidBrush(RGB(101, 67, 33));
        oldBrush = SelectObject(dc, brush);
        Rectangle(dc, X + 65, Y + 105, X + 95, Y + 160);
        SelectObject(dc, oldBrush);
        DeleteObject(brush);

        // Вікна
        brush = CreateSolidBrush(RGB(242, 231, 92));
        oldBrush = SelectObject(dc, brush);
        Rectangle(dc, X + 15, Y + 85, X + 45, Y + 115);
        Rectangle(dc, X + 115, Y + 85, X + 145, Y + 115);
        SelectObject(dc, oldBrush);
        DeleteObject(brush);

        // Димохід
        brush = CreateSolidBrush(RGB(105, 105, 105));
        oldBrush = SelectObject(dc, brush);
        Rectangle(dc, X + 120, Y + 15, X + 140, Y + 55);
        SelectObject(dc, oldBrush);
        DeleteObject(brush);
    }
};

// Клас для малювання сніговика
class Snowman
{
private:
    void line(HDC dc, int x1, int y1, int x2, int y2)
    {
        MoveToEx(dc, x1, y1, nullptr);
        LineTo(dc, x2, y2);
    }
public:
    // X, Y - координати основи
    void show(HDC dc, int X, int Y)
    {
        HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
        HGDIOBJ oldBrush = SelectObject(dc, whiteBrush);

        // Три кулі тіла
        Ellipse(dc, X - 45, Y - 90, X + 45, Y);
        Ellipse(dc, X - 33, Y - 160, X + 33, Y - 90);
        Ellipse(dc, X - 23, Y - 210, X + 23, Y - 160);

        SelectObject(dc, oldBrush);
        DeleteObject(whiteBrush);

        // Капелюх
        HBRUSH hatBrush = CreateSolidBrush(RGB(0, 0, 0));
        oldBrush = SelectObject(dc, hatBrush);
        Rectangle(dc, X - 22, Y - 218, X + 22, Y - 208);
        Rectangle(dc, X - 12, Y - 245, X + 12, Y - 216);
        SelectObject(dc, oldBrush);
        DeleteObject(hatBrush);

        // Очі та ґудзики
        oldBrush = SelectObject(dc, GetStockObject(BLACK_BRUSH));
        Ellipse(dc, X - 9, Y - 195, X - 3, Y - 189);
        Ellipse(dc, X + 3, Y - 195, X + 9, Y - 189);
        Ellipse(dc, X - 5, Y - 130, X + 5, Y - 120);
        Ellipse(dc, X - 5, Y - 105, X + 5, Y - 95);
        Ellipse(dc, X - 5, Y - 55, X + 5, Y - 45);
        SelectObject(dc, oldBrush);

        // Ніс
        HBRUSH noseBrush = CreateSolidBrush(RGB(255, 140, 0));
        oldBrush = SelectObject(dc, noseBrush);
        POINT nose[3] = { { X, Y - 185 }, { X, Y - 179 }, { X + 20, Y - 182 } };
        Polygon(dc, nose, 3);
        SelectObject(dc, oldBrush);
        DeleteObject(noseBrush);

        // Руки
        HPEN branchPen = CreatePen(PS_SOLID, 2, RGB(102, 51, 0));
        HGDIOBJ oldPen = SelectObject(dc, branchPen);
        line(dc, X - 33, Y - 130, X - 70, Y - 150);
        line(dc, X + 33, Y - 130, X + 70, Y - 150);
        SelectObject(dc, oldPen);
        DeleteObject(branchPen);
    }
};

// Клас для малювання хмари
class Cloud
{
public:
    void show(HDC dc, int X, int Y)
    {
        HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));
        HGDIOBJ oldBrush = SelectObject(dc, brush);

        Ellipse(dc, X, Y, X + 80, Y + 40);
        Ellipse(dc, X + 40, Y - 20, X + 120, Y + 30);
        Ellipse(dc, X + 90, Y, X + 160, Y + 40);
        Ellipse(dc, X + 20, Y + 10, X + 140, Y + 45);

        SelectObject(dc, oldBrush);
        DeleteObject(brush);
    }
};

// Клас для малювання сонця 
class Sun
{
public:
    // X, Y - центр сонця
    void show(HDC dc, int X, int Y)
    {
        HBRUSH brush = CreateSolidBrush(sunColors[sunIndex]);
        HGDIOBJ oldBrush = SelectObject(dc, brush);

        // Диск сонця
        Ellipse(dc, X - 35, Y - 35, X + 35, Y + 35);

        SelectObject(dc, oldBrush);
        DeleteObject(brush);
    }
};

// Малювання сцени
void DrawScene(HDC dc, int width, int height)
{
    HBRUSH brush;
    HGDIOBJ oldBrush;

    // НЕБО 
    brush = CreateSolidBrush(skyColors[skyIndex]);
    oldBrush = SelectObject(dc, brush);
    Rectangle(dc, 0, 0, width, height - 130);
    SelectObject(dc, oldBrush);
    DeleteObject(brush);

    // ЗАСНІЖЕНА ЗЕМЛЯ
    brush = CreateSolidBrush(RGB(250, 250, 250));
    oldBrush = SelectObject(dc, brush);
    Rectangle(dc, 0, height - 130, width, height);
    SelectObject(dc, oldBrush);
    DeleteObject(brush);

    // ХМАРИ
    Cloud cloud;
    cloud.show(dc, 60, 40);
    cloud.show(dc, 320, 20);
    cloud.show(dc, 560, 60);

    // БУДИНОК
    House house;
    house.show(dc, 260, height - 280);

    // ЯЛИНКИ
    FirTree tree;
    tree.show(dc, 90, height - 130);
    tree.show(dc, 700, height - 130);

    // СНІГОВИК
    Snowman snowman;
    snowman.show(dc, 520, height - 20);

    // СОНЦЕ 
    if (addedObject)
    {
        Sun sun;
        sun.show(dc, width - 110, 90);
    }
}


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);


    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB32, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB32));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB32));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB32);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
            // Зображення: Рисунок
        case ID_IMAGE_WINTER:
            showPicture = true;
            skyIndex = 0;
            sunIndex = 0;
            addedObject = false;
            InvalidateRect(hWnd, nullptr, TRUE);
            break;

            // Трансформації: Зміна кольору (небо)
        case ID_TRANSFORM_COLOR:
            if (!showPicture)
            {
                MessageBoxW(hWnd, L"Спочатку виведіть зображення.", L"Помилка", MB_OK);
                break;
            }
            skyIndex = (skyIndex + 1) % 3;
            InvalidateRect(hWnd, nullptr, TRUE);
            break;

            // Трансформації: Зміна кольору сонця і перетворення на місяць
        case ID_TRANSFORM_MOON:
            if (!showPicture)
            {
                MessageBoxW(hWnd, L"Спочатку виведіть зображення.", L"Помилка", MB_OK);
                break;
            }
            if (!addedObject)
            {
                MessageBoxW(hWnd, L"Спочатку додайте сонце.", L"Помилка", MB_OK);
                break;
            }
            sunIndex = (sunIndex + 1) % 2;
            InvalidateRect(hWnd, nullptr, TRUE);
            break;

            // Трансформації: Індивідуальна дія (додавання сонця)
        case ID_TRANSFORM_SUN:
            if (!showPicture)
            {
                MessageBoxW(hWnd, L"Спочатку виведіть зображення.", L"Помилка", MB_OK);
                break;
            }
            addedObject = true;
            InvalidateRect(hWnd, nullptr, TRUE);
            break;

        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        if (showPicture)
        {
            RECT rect;
            GetClientRect(hWnd, &rect);
            DrawScene(hdc, rect.right, rect.bottom);
        }

        EndPaint(hWnd, &ps);
    }
    break;
    case WM_SIZE:
        InvalidateRect(hWnd, nullptr, TRUE);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}