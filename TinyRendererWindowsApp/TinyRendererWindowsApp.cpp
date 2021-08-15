#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")
#include "SimpleGraphics.h"
#include <iostream>
#include "model.h"
#include <assert.h>
#include "tgaimage.h"
#include "geometry.h"
#include "gaurad_shader.h"
#include "normal_map_shader.h"

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 1000;
using simple_graphics::SimpleColor;
using simple_graphics::SimpleGraphics;
using simple_graphics::ModelView;
using simple_graphics::Projection;
using simple_graphics::Viewport;
using simple_graphics::GauraudShader;
using simple_graphics::NormalMapShader;

constexpr float PI = 3.14159265359;


class CellShader : public GauraudShader {
public:
    CellShader(Model* model, TGAImage* texture, const Vec3f& light_direction) : GauraudShader(model, texture, light_direction) {}
    virtual bool fragment(Vec3f bar, SimpleColor& color) override {
        float intensity = varying_intensity_ * bar;
        intensity = std::round(intensity * 6) / 6;
        color = SimpleColor(255 * intensity, 255 * intensity, 255 * intensity, 255);
        return false;
    }
};

VOID OnPaint(HDC hdc, long time)
{
    Vec3f normal_vec(0, 0, 1);
    Vec3f light_direction_(0, 0, 1);
    Vec3f reflection = (normal_vec * (normal_vec * light_direction_ * 2.f) - light_direction_).normalize();
    SimpleColor red(255, 0, 0, 255);
    SimpleColor blue(0, 0, 255, 255);
    SimpleColor green(0, 255, 0, 255);
    SimpleGraphics graphics(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
   
    graphics.SetColor(red);
    Model model(R"(C:/Users/mhoro/source/repos/TinyRendererWindowsApp/TinyRendererWindowsApp/african_head.obj)");
    
    TGAImage tga_img;
    tga_img.read_tga_file("C:/Users/mhoro/Downloads/african_head_diffuse.tga");
    tga_img.flip_vertically();

    TGAImage normal_map;
    normal_map.read_tga_file("C:/Users/mhoro/Downloads/african_head_nm.tga");
    normal_map.flip_vertically();

    TGAImage spec_map;
    spec_map.read_tga_file("C:/Users/mhoro/Downloads/african_head_spec.tga");
    spec_map.flip_vertically();
    float image_plane_dist = 1000;
    float radian = PI /2 + 4 * PI * time / 2000;
    Vec3f light_direction(std::sin(radian), 1, std::cos(radian));
    light_direction.normalize();
    Vec3f eye(70,0,100);
    Vec3f center(0.00, 0.000, 0);
    Vec3f up(0.000, 1, 0.00);

    simple_graphics::lookat(eye, center, up);
    simple_graphics::projection(-1.f / (eye - center).norm());
    simple_graphics::viewport(0 , 0 , WINDOW_WIDTH, WINDOW_HEIGHT);
    
    Matrix m = Viewport * Projection * ModelView;
    //Matrix m_inverse_transpose = ModelView.transpose().inverse();

    NormalMapShader shader(&model, &tga_img,&normal_map,&spec_map, light_direction);
    //GauraudShader shader(&model, &tga_img, light_direction);
    const Vec3f forward(0, 0, -1);
    for (int i = 0; i < model.nfaces(); i++) {
        Vec3f triangle[3];
        for (int j = 0; j < 3; j++) {
            triangle[j] = shader.vertex(i,j);
        }
        if (graphics.DirectionalComponent(triangle, forward) < 0) {
            continue;
        }
        graphics.DrawTriangle(triangle, shader);
    }
    /*
    
    tga_img.flip_vertically();
    for (int y = 0; y < WINDOW_HEIGHT; ++y) {
        for (int x = 0; x < WINDOW_WIDTH; ++x) {
            TGAColor color = tga_img.get(x, y);
            graphics.SetColor(SimpleColor(color.bgra[2], color.bgra[1], color.bgra[0], 255));
            graphics.DrawPoint(x, y);
        }
    }*/
    graphics.Render();
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow)
{
    HWND                hWnd;
    MSG                 msg;
    WNDCLASS            wndClass;
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;

    // Initialize GDI+.
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = TEXT("GettingStarted");

    RegisterClass(&wndClass);

    hWnd = CreateWindow(
        TEXT("GettingStarted"),   // window class name
        TEXT("Getting Started"),  // window caption
        WS_OVERLAPPEDWINDOW,      // window style
        CW_USEDEFAULT,            // initial x position
        CW_USEDEFAULT,            // initial y position
        WINDOW_WIDTH + 20,            // initial x size
        WINDOW_HEIGHT + 50,            // initial y size
        NULL,                     // parent window handle
        NULL,                     // window menu handle
        hInstance,                // program instance handle
        NULL);                    // creation parameters

    ShowWindow(hWnd, iCmdShow);
    UpdateWindow(hWnd);
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Gdiplus::GdiplusShutdown(gdiplusToken);
    return msg.wParam;
}  // WinMain

RECT rcCurrent = { 0,0,0,0 };
POINT aptStar[6] = { 10,1, 1,19, 19,6, 1,6, 19,19, 10,1 };
int X = 2, Y = -1, idTimer = -1;
BOOL fVisible = FALSE;
HDC hdc;
long passed_time = 0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
    WPARAM wParam, LPARAM lParam)
{
   
    PAINTSTRUCT  ps;
    
    RECT rc;

    switch (message)
    {
    case WM_CREATE:
        // Calculate the starting point.  

        GetClientRect(hWnd, &rc);
        //OffsetRect(&rcCurrent, rc.right / 2, rc.bottom / 2);

        // Initialize the private DC.  

        hdc = GetDC(hWnd);
        SetViewportOrgEx(hdc, rcCurrent.left,
            rcCurrent.top, NULL);
        SetROP2(hdc, R2_NOT);

        // Start the timer.  

        SetTimer(hWnd, idTimer = 1, 1, NULL);
        return 0;
    case WM_TIMER:
        passed_time += 10;
        GetClientRect(hWnd, &rc);
        SetViewportOrgEx(hdc, rcCurrent.left,
            rcCurrent.top, NULL);
        OnPaint(hdc, passed_time);
        return 0;
    case WM_PAINT:
            BeginPaint(hWnd, &ps);
            
            EndPaint(hWnd, &ps);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
} // WndProc