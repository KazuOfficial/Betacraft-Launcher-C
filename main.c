#include<windows.h>  
#define ID_EDIT 1
  
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lparam);  

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)  
{  
    TCHAR appname[] = TEXT("Betacraft Launcher C");  
    WNDCLASS wndclass;
    MSG msg;
    HWND hwnd;
  
    wndclass.cbClsExtra = 0;  
    wndclass.cbWndExtra = 0;  
    wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 4);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);  
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);  
    wndclass.hInstance = hInstance;  
    wndclass.lpfnWndProc = WndProc;  
    wndclass.lpszClassName = appname;  
    wndclass.lpszMenuName = NULL;  
    wndclass.style = CS_HREDRAW | CS_VREDRAW;  
  
    // check this window class is registered or not  
    if (!RegisterClass(&wndclass))  
    {  
        MessageBox(NULL, TEXT("Window class is not registered"), TEXT("Error...."), MB_ICONERROR);  
        return 0;  
    }  
  
    hwnd = CreateWindow(appname,  //window name  
                            appname,   // window text  
                            // WS_OVERLAPPEDWINDOW, // window style  
                            WS_SYSMENU,
                            CW_USEDEFAULT,   //default window position x  
                            CW_USEDEFAULT,   //default window position y  
                            800,   //width  
                            450,    // height  
                            NULL,  
                            NULL,  
                            hInstance,  
                            NULL  
                       );  
  
    // show & update created window  
    ShowWindow(hwnd, iCmdShow);  
    UpdateWindow(hwnd);  
  
    // get message from queue  
    while (GetMessage(&msg, NULL, 0, 0))  
    {  
        TranslateMessage(&msg);  
        DispatchMessage(&msg);  
    }  
  
    return msg.wParam;  
}  
  
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)  
{  
    HDC hdc;
    PAINTSTRUCT ps;  
    static HWND hwndEdit;
    static wchar_t *authorLabel =  L"Made by Kazu";
  
    switch (message)  
    { 
    case WM_CREATE :
          CreateWindowW(L"Static", authorLabel, 
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                30, 390, 94, 17, 
                hwnd, (HMENU) 1, NULL, NULL);

          hwndEdit = CreateWindowW(L"Edit", NULL, 
                WS_CHILD | WS_VISIBLE | WS_BORDER,
                320, 320, 150, 20, hwnd, (HMENU) ID_EDIT,
                NULL, NULL);

          CreateWindowW(L"button", L"PLAY",    
              WS_VISIBLE | WS_CHILD ,
              320, 350, 150, 35, hwnd, (HMENU) 1, NULL, NULL);  
          break;
    case WM_COMMAND:	

        if (HIWORD(wParam) == BN_CLICKED) {
            //StartGame(text);
        }

        break;
    case WM_DESTROY :  
        PostQuitMessage(EXIT_SUCCESS);  
        return 0;  
    }  
  
    return DefWindowProc(hwnd, message, wParam, lParam);  
}

// void LoadMyImage(void) {

//     hBitmap = LoadImageW(NULL, L"D:\\background.bmp", IMAGE_BITMAP,
//         0, 0, LR_LOADFROMFILE);
// }