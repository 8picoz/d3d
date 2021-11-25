#include "App.h"

//コンストラクタ
App::App(uint32_t width, uint32_t height)
	: m_hInst(nullptr)
	, m_hWnd(nullptr)
	, m_width(width)
	, m_height(height)
{}

//デストラクタ
App::~App()
{}

namespace {
	const auto ClassName = TEXT("SampleWindowClass");
}

//実行する
void App::Run()
{
	if (InitApp())
	{
		MainLoop();
	}

	TermApp();
}

//初期化処理
bool App::InitApp() 
{
	//ウィンドウの初期化
	if (!InitWnd())
	{
		return false;
	}

	//正常終了
	return true;
}

//ウィンドウの初期化処理
bool App::InitWnd() 
{
	//インスタンスハンドルを処理
	auto hInst = GetModuleHandle(nullptr);
	if (hInst == nullptr) 
	{
		return false;
	}

	//ウィンドウの設定
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX); //WNDCLASSEX構造体自体のサイズを設定
	wc.style = CS_HREDRAW | CS_VREDRAW; //ウィンドウのスタイルを決めるフラグ、今回は水平方向と垂直方向のリサイズ時の再描画のフラグの論理和
	wc.lpfnWndProc = WndProc; //ウィンドウプロシージャの設定。ウィンドウプロシージャとはマウスのクリックやウィンドウのリサイズ時のメッセージが送られてきたときに呼ばれるコールバック関数
	//wc.hInstance = hInst; //アプリケーションのインスタンスハンドルを指定
	wc.hIcon = LoadIcon(hInst, IDI_APPLICATION); //アプリケーションのアイコンを設定。今回はWindowsで用意されているアイコンを使用
	wc.hCursor = LoadCursor(hInst, IDC_ARROW); //アプリケーション上で使用するカーソルを設定。今回はWindowsで用意されているカーソルを使用
	wc.hbrBackground = GetSysColorBrush(COLOR_BACKGROUND); //背景色設定システムカラーをブラシに変換
	wc.lpszMenuName = nullptr; //メニューバーの設定、今回は何も設定しない
	wc.lpszClassName = ClassName; //ウィンドウクラスを識別する名前の設定
	wc.hIconSm = LoadIcon(hInst, IDI_APPLICATION); //タイトルバーなどに含まれる小さいアイコンの設定

	//ウィンドウの登録
	if (!RegisterClassEx(&wc))
	{
		return false;
	}

	//インスタンスハンドル設定
	m_hInst = hInst;

	RECT rc = {};
	rc.right = static_cast<LONG>(m_width);
	rc.bottom = static_cast<LONG>(m_height);

	//ウィンドウサイズを調整
	auto style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
	AdjustWindowRect(&rc, style, FALSE);

	//ウィンドウを生成
	m_hWnd = CreateWindowEx(
		0, //dwExStyle
		ClassName, //lpClassName
		TEXT("Sample"), //lpWindowName
		style, //dwStyle
		CW_USEDEFAULT, //x座標
		CW_USEDEFAULT, //y座標
		rc.right - rc.left, //Width
		rc.bottom - rc.top, //Height
		nullptr, //hWndParent 作成するウィンドウの親ウィンドウやオーナーウィンドウのハンドルを指定
		nullptr, //hMenu
		m_hInst, //ウィンドウに関連付けられるインスタンスのハンドルを指定
		nullptr); //lpParam 任意のパラメータ

	if (m_hWnd == nullptr)
	{
		return false;
	}

	//ウィンドウを表示
	ShowWindow(m_hWnd, SW_SHOWNORMAL);
	
	//ウィンドウを更新
	UpdateWindow(m_hWnd);

	//ウィンドウにフォ＝カスを設定
	SetFocus(m_hWnd);

	//正常終了
	return true;
}

//ウィンドウプロシージャ
//Windows側からメッセージが送られてきたときに処理をするコールバック
LRESULT CALLBACK App::WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) 
{

	switch (msg)
	{
		case WM_DESTROY:
			//ここでウィンドウ終了命令を送信
			{ PostQuitMessage(0); }
			break;
		default:
			break;
	}

	return DefWindowProc(hWnd, msg, wp, lp);
}

void App::MainLoop()
{
	MSG msg = {};
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) == TRUE)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

void App::TermApp()
{
	TermWnd();
}

void App::TermWnd()
{
	if (m_hInst != nullptr)
	{
		UnregisterClass(ClassName, m_hInst);
	}

	m_hInst = nullptr;
	m_hWnd = nullptr;
}