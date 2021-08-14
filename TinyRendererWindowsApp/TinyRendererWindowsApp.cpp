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

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 1000;
using simple_graphics::SimpleColor;
using simple_graphics::SimpleGraphics;
using simple_graphics::ModelView;
using simple_graphics::Projection;
using simple_graphics::Viewport;


VOID OnPaint(HDC hdc)
{
    SimpleColor red(255, 0, 0, 255);
    SimpleColor blue(0, 0, 255, 255);
    SimpleColor green(0, 255, 0, 255);
    SimpleGraphics graphics(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
   
    graphics.SetColor(red);
    Model model(R"(C:/Users/mhoro/source/repos/TinyRendererWindowsApp/TinyRendererWindowsApp/african_head.obj)");
    Vec3f direction(0, 0, -1);
    Vec3f light_direction(0, 0, -1);
    light_direction.normalize();
    TGAImage tga_img;
    tga_img.read_tga_file("C:/Users/mhoro/Downloads/african_head_diffuse.tga");
    tga_img.flip_vertically();
    float image_plane_dist = 1000;

    Vec3f eye(0, 0, 100);
    Vec3f center(0.00, 0.000, 0);
    Vec3f up(0.000, 1, 0.00);

    simple_graphics::lookat(eye, center, up);
    simple_graphics::projection(-1.f / (eye - center).norm());
    simple_graphics::viewport(0 , 0 , WINDOW_WIDTH, WINDOW_HEIGHT);
    
    Matrix m = Viewport * Projection * ModelView;
    Matrix m_inverse_transpose = ModelView.transpose().inverse();

    Matrix light_direction_matrix = ModelView * light_direction.AsHomogenous();
    light_direction.x = light_direction_matrix[0][0];
    light_direction.y = light_direction_matrix[1][0];
    light_direction.z = light_direction_matrix[2][0];


    for (int i = 0; i < model.nfaces(); i++) {
        std::vector<FaceIndexes> face = model.face(i);
        Vec3f screen_coords[3];
        Vec3f world_coords[3];
        Vec3f normals[3];
        Vec2f textures[3];
        for (int j = 0; j < 3; j++) {
            const FaceIndexes& indexes = face[j];
            world_coords[j] = model.vert(indexes.vert_idx);
            
            Matrix reprojected = m * world_coords[j].AsHomogenous();
            Matrix unrprojected_normal = model.vert_normal(indexes.vert_normal_idx).AsHomogenous();
            unrprojected_normal[3][0] = 0;
            Matrix reproject_normal = m_inverse_transpose * unrprojected_normal;

            normals[j] = Vec3f(reproject_normal[0][0], reproject_normal[1][0], reproject_normal[2][0]);
            textures[j] = model.texture(indexes.texture_idx);
            screen_coords[j] = Vec3f(reprojected[0][0] / reprojected[3][0], reprojected[1][0] / reprojected[3][0], reprojected[2][0] / reprojected[3][0]);
        }
        
        float component = graphics.DirectionalComponent(screen_coords, direction);
        if (component < 0) {
            continue;
        }
        graphics.DrawTriangle(screen_coords,normals,textures,tga_img,light_direction);
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
    WPARAM wParam, LPARAM lParam)
{
    HDC          hdc;
    PAINTSTRUCT  ps;

    switch (message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        OnPaint(hdc);
        EndPaint(hWnd, &ps);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
} // WndProc