#include <Windows.h>

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

        POINT tier1[3] =
        {
            { X - 70, Y - 30 },
            { X + 70, Y - 30 },
            { X,      Y - 110 }
        };
        Polygon(dc, tier1, 3);

        POINT tier2[3] =
        {
            { X - 55, Y - 85 },
            { X + 55, Y - 85 },
            { X,      Y - 155 }
        };
        Polygon(dc, tier2, 3);

        POINT tier3[3] =
        {
            { X - 40, Y - 135 },
            { X + 40, Y - 135 },
            { X,      Y - 195 }
        };
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
        POINT roof[3] =
        {
            { X - 15, Y + 60 },
            { X + 175, Y + 60 },
            { X + 80,  Y }
        };
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
        POINT nose[3] =
        {
            { X,      Y - 185 },
            { X,      Y - 179 },
            { X + 20, Y - 182 }
        };
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


// Малювання всієї сцени
void DrawScene(HDC dc, int width, int height)
{
    HBRUSH brush;
    HGDIOBJ oldBrush;

    // НЕБО
    brush = CreateSolidBrush(RGB(176, 224, 230));
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
}

// Процедура обробки повідомлень
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        // Створення вікна
    case WM_CREATE:
    {
        return 0;
    }
    // Малювання вікна
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(hwnd, &ps);

        RECT rect;
        GetClientRect(hwnd, &rect);
        int width = rect.right;
        int height = rect.bottom;

        DrawScene(dc, width, height);

        EndPaint(hwnd, &ps);
        return 0;
    }
    // Натискання лівої кнопки миші
    case WM_LBUTTONDOWN:
    {
        MessageBox(hwnd, L"Натиснуто ліву кнопку миші.", L"WM_LBUTTONDOWN", MB_OK);
        return 0;
    }
    // Натискання клавіші
    case WM_KEYDOWN:
    {
        if (wParam == VK_SPACE)
        {
            DestroyWindow(hwnd);
        }
        return 0;
    }
    // Зміна розміру вікна
    case WM_SIZE:
    {
        InvalidateRect(hwnd, nullptr, TRUE);
        return 0;
    }
    // Закриття вікна
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

// Головна функція Windows-програми
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"Winter";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"Зима - WinAPI",
        WS_OVERLAPPEDWINDOW,
        100, 100, 900, 650, nullptr, nullptr, hInstance, nullptr);

    if (hwnd == nullptr)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG message = {};
    while (GetMessage(&message, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
    return 0;
}