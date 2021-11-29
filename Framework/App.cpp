#include "App.h"

template<typename T>
void SafeRelease(T*& ptr) 
{
	if (ptr != nullptr)
	{
		ptr->Release();
		ptr = nullptr;
	}
}

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

/*
流れ:
デバイスの生成
コマンドキューの生成
スワップチェインの生成
コマンドアロケータの生成
コマンドリストの生成
レンダーターゲットビューの生成
フェンスの生成
*/
bool App::InitD3D()
{
	//デバイスの生成
	/*
	物理デバイスを表現するためのインターフェイス
	リソースやパイプラインステートオブジェクトはこのデバイスを通して生成される
	*/
	auto hr = D3D12CreateDevice(
		nullptr, //ビデオアダプタへのポインタ設定, デフォを使用したい場合はnullptr
		D3D_FEATURE_LEVEL_11_0, //デバイスを生成するときに成功されるであろう最小限のバージョンを指定, たとえば最後を12_0にした場合はDX12がサポートされた環境でしかできないが、11_0なら11と12がサポートされた環境でできる
		IID_PPV_ARGS(&m_pDevice)
	);

	/*
	第3引数と第4引数についてはIID_PPV_ARGSというマクロが展開されて割り当てられている
	第3引数はデバイスインターフェイスを識別するためのGUIDを指定
	第4引数はデバイスインターフェイスを受け取るためのポインタを指定、引数の型がvoid**なのでキャストが必要(デバイスインターフェイスとはm_pDeviceのこと)

	もしIID_PPV_ARGSを使用しない場合
	auto hr = D3D12CreateDevice(
		nullptr,
		D3D_FEATURE_LEVEL_11_0,
		__uuidof(&m_pDevice),
		reinterpret_cast<void**>(&m_pDevice)
	);
	となる
	*/

	//成功か失敗か確認
	/*
	このFAILEDマクロは
	(((HRESULT)(hr)) < 0)
	となる
	*/
	if (FAILED(hr))
	{
		return false;
	}

	//コマンドキューの生成
	/*
	コマンドキューはGPUに送るための描画コマンドの投稿、また描画コマンド実行の同期処理を行うためのメソッド提供を行う
	*/
	{
		D3D12_COMMAND_QUEUE_DESC desc = {}; //どのようなコマンドキューであるかを設定するための構造体を生成、これをコマンドキューを生成するときに設定する
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; //コマンドキューに登録可能なコマンドリストのタイプを設定
		//https://hexadrive.jp/hexablog/program/13072/
		//https://cocoa-programing.hatenablog.com/entry/2018/11/24/%E3%80%90DirectX12%E3%80%91%E3%82%B3%E3%83%9E%E3%83%B3%E3%83%89%E3%82%A2%E3%83%AD%E3%82%B1%E3%83%BC%E3%82%BF%E3%81%AE%E4%BD%9C%E6%88%90%E3%80%90%E5%88%9D%E6%9C%9F%E5%8C%96%E3%80%91
		//http://www.project-asura.com/program/d3d12/d3d12_004.html
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL; //コマンドキューの優先度指定
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE; //個MSんフォキューの特性を示すフラグ
		desc.NodeMask = 0; //GPUが一つであれば0を指定。複数のGPUノードが存在する場合はGPUノードを識別するためのビットを指定

		//CreateCommandQueueは見ての通りコマンドキューの生成
		hr = m_pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_pQueue));
		if (FAILED(hr)) 
		{
			return false;
		}
	}

	//スワップチェインの生成
	/*
	ダブルバッファリングやトリプルバッファリングを行うための仕組み
	ダブルバッファリングやトリプルバッファリングは一つのフレームバッファだけだとティアリングを引き起こしてしまう問題を解決するために作られた
	一つのフレームバッファだけだとモニタのリフレッシュレートがフレームレートよりもおそすぎてしまうと画面更新時にバッファを書き換えてる途中で更新されてティアリングがおこる
	それを２つフレームバッファを用意することで片方に書き込んでもう片方が終わったら画面に出力するためのバッファと次のフレームを書き込むバッファを切り替える
	ディスプレイに表示されている画像に使われているバッファをフロントバッファ、書き込みが行われているバッファをバックバッファと呼ぶ
	そうすることで書き込み途中のバッファを表示することはなくなりティアリングしなくなる
	*/
	{
		//DXGIファクトリーの生成
		/*
		DXGIとはDirectXグラフィックスインストラクチャーの略でカーネルモードドライバまたシステムハードウェアと通信することを目的としたAPI
		DXGI自体はDirect3Dに依存関係を持っていない
		DXGI自体は基本的にはGPUが接続されているのか列挙したり、描画したフレームを画面へ表示したりするのに使用する
		*/
		//この末尾の数字は例えばこのIDXGIFactory4だとDXGIがバージョン1.4のときに加わって入ったものという意味を指す
		IDXGIFactory4* pFactory = nullptr;
		hr = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));
		if (FAILED(hr))
		{
			return false;
		}

		//スワップチェインの設定
		DXGI_SWAP_CHAIN_DESC desc = {}; //swapchainの設定用構造体
		desc.BufferDesc.Width = m_width; //解像度の横幅を指定
		desc.BufferDesc.Height = m_height; //解像度の縦幅を指定
		//リフレッシュレート(Hz)を指定
		desc.BufferDesc.RefreshRate.Numerator = 60; //分子
		desc.BufferDesc.RefreshRate.Denominator = 1; //分母
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; //走査線のモードを指定 (インタレースかプログレッシブかの違い)
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; //拡大縮小の設定
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //表示形式のピクセルフォーマットを指定 (例としてはR8G8B8A8_UNORMのようなGPUに渡すときに正規化する) https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format
		//マルチサンプリング設定
		desc.SampleDesc.Count = 1; //ピクセル単位のマルチサンプリングの数
		desc.SampleDesc.Quality = 0; //画像の品質レベルを指定、品質が高いほどパフォーマンスは低下
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //バックバッファ(swapchainの書き込みが行われているバッファ)の使用方法を指定。今回は出力レンダーターゲットとして使用
		desc.BufferCount = FrameCount; //フレームバッファの数を指定、今回はダブルバッファリングなので2を指定
		desc.OutputWindow = m_hWnd; //出力するウィンドウのウィンドウハンドルを指定
		desc.Windowed = TRUE; //
		//バックバッファ入れ替え時効果を指定
		//DXGI_SWAP_EFFECT_SEQUENTIALはIDXGISwapChain1::Present()呼び出し後にバックバッファの内容が変化しない、これはマルチサンプリングで使うことができない
		//DXGI_SWAP_EFFECT_FLIP_DISCARDはIDXGISwapChain1::Present()呼び出し後にバックバッファの内容を破棄、これもマルチサンプリングで使うことができない
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		//swapchainの動作オプションを指定, 今回の場合はIDXGISwapChain::ResizeTarget()を呼び出すことで表示モードを切り替えられる
		//このフラグに含まれる一つでGDIという物があるがそれはGraphic Device Interfaceの略でDirect2Dよりも旧来の2D API
		//https://www.itmedia.co.jp/pcuser/articles/0905/20/news103.html
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		//スワップチェインの生成
		IDXGISwapChain* pSwapChain = nullptr;
		hr = pFactory->CreateSwapChain(m_pQueue, &desc, &pSwapChain);
		if (FAILED(hr))
		{
			SafeRelease(pFactory);
			return false;
		}

		//ここからダブルバッファリングを行う際のバックバッファ番号を取得

		//IDXGISwapChain3を取得
		hr = pSwapChain->QueryInterface(IID_PPV_ARGS(&m_pSwapChain));
		if (FAILED(hr))
		{
			SafeRelease(pFactory);
			SafeRelease(pSwapChain);
			return false;
		}


		//バックバッファ番号を取得
		//疑問: バックバッファはスワップ処理で常に入れ替わるのにここで決め打ちで取得してそのままで良いのか？
		m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

		//不要になったので開放
		SafeRelease(pFactory);
		SafeRelease(pSwapChain);
	}

	//コマンドアロケータの生成
	{
		for (auto i = 0u; i < FrameCount; ++i) 
		{
			hr = m_pDevice->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(&m_pCmdAllocator[i]));
			if (FAILED(hr))
			{
				return false;
			}
		}
	}

	//コマンドリストの生成
	{
		hr = m_pDevice->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_pCmdAllocator[m_FrameIndex],
			nullptr,
			IID_PPV_ARGS(&m_CmdList)
		);
		if (FAILED(hr))
		{
			return false;
		}
	}

	//レンダーターゲットビューの生成
	{
		//ディスクリプタヒープの設定
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = FrameCount;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask = 0;

		//ディスクリプタヒープを生成
		hr = m_pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pHeapRTV));
		if (FAILED(hr))
		{
			return false;
		}

		auto handle = m_pHeapRTV->GetCPUDescriptorHandleForHeapStart();
		auto incrementSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		for (auto i = 0u; i < FrameCount; ++i)
		{
			hr = m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_pColorBuffer[i]));
			if (FAILED(hr))
			{
				return false;
			}

			D3D12_RENDER_TARGET_VIEW_DESC viewDesc = {};
			viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			viewDesc.Texture2D.MipSlice = 0;
			viewDesc.Texture2D.PlaneSlice = 0;

			//レンダーターゲットビューの生成
			m_pDevice->CreateRenderTargetView(m_pColorBuffer[i], &viewDesc, handle);

			m_HandleRTV[i] = handle;
			handle.ptr += incrementSize;
		}
	}

	//フェンスの生成
	{
		//フェンスカウンターをリセット
		for (auto i = 0u; i < FrameCount; ++i)
		{
			m_FenceCounter[i] = 0;
		}

		//フェンスの生成
		hr = m_pDevice->CreateFence(
			m_FenceCounter[m_FrameIndex],
			D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(&m_pFence));
		if (FAILED(hr))
		{
			return false;
		}

		m_FenceCounter[m_FrameIndex]++;

		//イベントの生成
		m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_FenceEvent == nullptr)
		{
			return false;
		}
	}

	//コマンドリストを閉じる
	m_pCmdList->Close();

	return true;
}