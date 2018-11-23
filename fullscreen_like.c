#include <windows.h>

#define WINDOW_FIRST TEXT("WindowFirst")
#define WINDOW_SECOND TEXT("WindowSecond")

HWND hWndTarget;
RECT rectClientTarget;
RECT rectWindowTarget;
POINT pointOriginTarget;

LRESULT CALLBACK WndProcFirst(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT CALLBACK WndProcSecond(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
  WNDCLASSEX wndClassFirst;
  WNDCLASSEX wndClassSecond;
  HWND hWndFirst;
  HWND hWndSecondTop;
  HWND hWndSecondLeft;
  HWND hWndSecondBottom;
  HWND hWndSecondRight;
  HWND hWndSecondBackground;
  MSG msg;
  
  extern HWND hWndTarget;
  extern RECT rectClientTarget;
  extern RECT rectWindowTarget;
  extern POINT pointOriginTarget;
  
  RECT rectWindowTargetBuffer;
  int flag;
  
  DWORD dwThreadId;
  DWORD dwThreadIdTarget;
  
  wndClassFirst.cbSize = sizeof(WNDCLASSEX);
  wndClassFirst.style = CS_HREDRAW | CS_VREDRAW;
  wndClassFirst.lpfnWndProc = (WNDPROC)WndProcFirst;
  wndClassFirst.cbClsExtra = 0;
  wndClassFirst.cbWndExtra = 0;
  wndClassFirst.hInstance = hInstance;
  wndClassFirst.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wndClassFirst.hCursor = LoadCursor(NULL, IDC_ARROW);
  wndClassFirst.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wndClassFirst.lpszMenuName = NULL;
  wndClassFirst.lpszClassName = WINDOW_FIRST;
  wndClassFirst.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
  if (!RegisterClassEx(&wndClassFirst)) return -1;
  
  wndClassSecond.cbSize = sizeof(WNDCLASSEX);
  wndClassSecond.style = 0;
  wndClassSecond.lpfnWndProc = WndProcSecond;
  wndClassSecond.cbClsExtra = 0;
  wndClassSecond.cbWndExtra = 0;
  wndClassSecond.hInstance = hInstance;
  wndClassSecond.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wndClassSecond.hCursor = LoadCursor(NULL, IDC_ARROW);
  wndClassSecond.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wndClassSecond.lpszMenuName = NULL;
  wndClassSecond.lpszClassName = WINDOW_SECOND;
  wndClassSecond.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
  if (!RegisterClassEx(&wndClassSecond)) return -1;
  
  hWndFirst = CreateWindow(
    WINDOW_FIRST, TEXT("windowFirst"), WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 200, 200, NULL, NULL, hInstance, NULL
  );
  if (hWndFirst == NULL) return -1;
  
  while (GetMessage(&msg, NULL, 0, 0)) { // 対象ウィンドウを選択するまでのメッセージループ
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  
  RECT rectDesk;
  HWND hWndDesk = GetDesktopWindow();
  GetWindowRect(hWndDesk, &rectDesk);
  
  hWndSecondBackground = CreateWindow( // 対象ウィンドウの後ろにウィンドウを敷いておく
    WINDOW_SECOND, TEXT("windowSecondBackground"), WS_POPUP | WS_VISIBLE,
    rectDesk.left,
    rectDesk.top,
    rectDesk.right + 1,
    rectDesk.bottom + 1,
    NULL, NULL, hInstance, NULL
  );
  if (hWndSecondBackground == NULL) return -1;
  SetWindowPos(hWndSecondBackground, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
  
  hWndSecondTop = CreateWindow( // 上側のウィンドウ
    WINDOW_SECOND, TEXT("windowSecondTop"), WS_POPUP | WS_VISIBLE,
    rectDesk.left,
    rectDesk.top,
    rectDesk.right + 1,
    pointOriginTarget.y + 1,
    NULL, NULL, hInstance, NULL
  );
  hWndSecondLeft = CreateWindow( // 左側のウィンドウ
    WINDOW_SECOND, TEXT("windowSecondLeft"), WS_POPUP | WS_VISIBLE,
    rectDesk.left,
    rectDesk.top,
    pointOriginTarget.x + 1,
    rectDesk.bottom + 1,
    NULL, NULL, hInstance, NULL
  );
  hWndSecondBottom = CreateWindow( // 下側のウィンドウ
    WINDOW_SECOND, TEXT("windowSecondBottom"), WS_POPUP | WS_VISIBLE,
    rectDesk.left,
    pointOriginTarget.y + rectClientTarget.bottom - 1,
    rectDesk.right + 1,
    rectDesk.bottom - pointOriginTarget.y - rectClientTarget.bottom + 2,
    NULL, NULL, hInstance, NULL
  );
  hWndSecondRight = CreateWindow( // 右側のウィンドウ
    WINDOW_SECOND, TEXT("windowSecondRight"), WS_POPUP | WS_VISIBLE,
    pointOriginTarget.x + rectClientTarget.right - 1,
    rectDesk.top,
    rectDesk.right - pointOriginTarget.x - rectClientTarget.right + 2,
    rectDesk.bottom + 1,
    NULL, NULL, hInstance, NULL
  );
  if (hWndSecondTop == NULL) return -1;
  if (hWndSecondLeft == NULL) return -1;
  if (hWndSecondBottom == NULL) return -1;
  if (hWndSecondRight == NULL) return -1;
  SetWindowPos(hWndSecondTop, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
  SetWindowPos(hWndSecondLeft, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
  SetWindowPos(hWndSecondBottom, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
  SetWindowPos(hWndSecondRight, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
  
  dwThreadId = GetCurrentThreadId(); // このプロセスのスレッドid
  dwThreadIdTarget = GetWindowThreadProcessId(hWndTarget, NULL); // 対象ウィンドウのスレッドid
  if (dwThreadId != dwThreadIdTarget) {
    if (!AttachThreadInput(dwThreadId, dwThreadIdTarget, TRUE)) return -1;
  }
  
  SetActiveWindow(hWndTarget); // 使いやすさのため、対象ウィンドウを最前面・アクティブにする。
  
  while (TRUE) { // メインループ
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { // hWndSecondのどれかがメッセージを受け取っているとき
      if (msg.message == WM_QUIT) break;
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    else {
      if (GetKeyState(VK_ESCAPE) < 0) {
        flag = TRUE; // Escが押されているとき
      }
      else if (GetWindowRect(hWndTarget, &rectWindowTargetBuffer)) {
        if (
          rectWindowTargetBuffer.left == rectWindowTarget.left &&
          rectWindowTargetBuffer.top == rectWindowTarget.top &&
          rectWindowTargetBuffer.right == rectWindowTarget.right &&
          rectWindowTargetBuffer.bottom == rectWindowTarget.bottom
        ) flag = FALSE; // 対象ウィンドウが移動していないとき
        else flag = TRUE; // 対象ウィンドウが移動したとき
      }
      else flag = TRUE; // ウィンドウがないとき
      if (flag == TRUE) {
        AttachThreadInput(dwThreadId, dwThreadIdTarget, FALSE);
        PostQuitMessage(0);
      }
    }
  }
  
  return msg.wParam;
}

LRESULT CALLBACK WndProcFirst(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
  
  extern HWND hWndTarget;
  extern RECT rectClientTarget;
  extern RECT rectWindowTarget;
  extern POINT pointOriginTarget;
  
  switch (msg) {
    case WM_DESTROY: {
      PostQuitMessage(0);
      return 0;
    }
    case WM_KEYDOWN: {
      if (wp == VK_ESCAPE) PostMessage(hWnd, WM_CLOSE, 0, 0);
      return 0;
    }
    case WM_KILLFOCUS: {
      hWndTarget = GetForegroundWindow();
      GetClientRect(hWndTarget, &rectClientTarget); // 幅、高さ
      GetWindowRect(hWndTarget, &rectWindowTarget); // ウィンドウのスクリーン座標
      pointOriginTarget.x = (LONG)0;
      pointOriginTarget.y = (LONG)0;
      ClientToScreen(hWndTarget, &pointOriginTarget); // クライアント領域の左上の点のスクリーン座標
      SendMessage(hWnd, WM_CLOSE, 0, 0);
      return 0;
    }
  }
  return DefWindowProc(hWnd, msg, wp, lp);
}

LRESULT CALLBACK WndProcSecond(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
  switch (msg) {
    case WM_DESTROY: {
      PostQuitMessage(0);
      return 0;
    }
    case WM_KEYDOWN: {
      if (wp == VK_ESCAPE) PostMessage(hWnd, WM_CLOSE, 0, 0);
      return 0;
    }
  }
  return DefWindowProc(hWnd, msg, wp, lp);
}