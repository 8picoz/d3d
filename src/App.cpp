#include "App.h"
#include <cassert>

template<typename T>
void SafeRelease(T*& ptr) 
{
	if (ptr != nullptr)
	{
		ptr->Release();
		ptr = nullptr;
	}
}

//VSInputに合わせた構造体
struct Vertex 
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT4 Color;
};

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

	//DIrect3D12の初期化
	if (!InitD3D())
	{
		return false;
	}

	if (!OnInit())
	{
		return false;
	}

	//正常終了
	return true;
}

//ウィンドウの初期化処理
bool App::InitWnd() 
{
	//デバッグレイヤの有効化
#if defined(DEBUG) || defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> debug;
		auto hr = D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf()));

		//デバッグレイヤを有効に
		if (SUCCEEDED(hr))
		{
			debug->EnableDebugLayer();
		}
	}
#endif

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
		IID_PPV_ARGS(m_pDevice.GetAddressOf())
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
		hr = m_pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(m_pQueue.GetAddressOf()));
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
		hr = pFactory->CreateSwapChain(m_pQueue.Get(), &desc, &pSwapChain);
		if (FAILED(hr))
		{
			SafeRelease(pFactory);
			return false;
		}

		//ここからダブルバッファリングを行う際のバックバッファ番号を取得

		//IDXGISwapChain3を取得
		hr = pSwapChain->QueryInterface(IID_PPV_ARGS(m_pSwapChain.GetAddressOf()));
		if (FAILED(hr))
		{
			SafeRelease(pFactory);
			SafeRelease(pSwapChain);
			return false;
		}


		//バックバッファ番号を取得
		//疑問: バックバッファはスワップ処理で常に入れ替わるのにここで決め打ちで取得してそのままで良いのか？,A: この時点でのバックバッファの番号ってだけ
		m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

		//不要になったので開放
		SafeRelease(pFactory);
		SafeRelease(pSwapChain);
	}

	//コマンドアロケータの生成
	/*
	コマンドリストが使用するメモリを割当するためのもの
	*/
	{
		//フレームバッファごとに作成
		for (auto i = 0u; i < FrameCount; ++i) 
		{
			hr = m_pDevice->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT, //コマンドキューに直接登録可能なコマンドのみを扱える
				IID_PPV_ARGS(m_pCmdAllocator[i].GetAddressOf()));
			if (FAILED(hr))
			{
				return false;
			}
		}
	}

	//コマンドリストの生成
	{
		hr = m_pDevice->CreateCommandList(
			0, //nodeMask: 複数のGPUノードがある場合に、ノードを識別するためのビットマスクを設定します
			D3D12_COMMAND_LIST_TYPE_DIRECT, //type: 作成するコマンドリストのタイプを指定、コマンドキューに直接登録可能なコマンドのみを扱える
			m_pCmdAllocator[m_FrameIndex].Get(), //pCommandAllocator: コマンドリストを作成するときのコマンドアロケータを指定、2つあるうちの描画コマンドをつんでいくのに使用するのはバックバッファの番号に対応するものなのでインデックスにm_FrameIndexを指定
			nullptr, //pInitialState: パイプラインステートを指定、この引数はオプションなのでnullptrでも可、あとで明示的に指定するためにここでは設定しない。
			IID_PPV_ARGS(&m_pCmdList)
		);
		if (FAILED(hr))
		{
			return false;
		}
	}

	//レンダーターゲットビューの生成
	/*
	レンダーターゲットとは描画の対象であり、その実態はバックバッファやテクスチャといったリソースとなる。
	以下実態一覧
	-
	バッファ
	一次元テクスチャ
	一次元テクスチャ配列
	二次元テクスチャ
	二次元テクスチャ配列
	マルチサンプル二次元テクスチャ
	マルチサンプル二次元テクスチャ配列
	三次元テクスチャ
	キューブマップ
	キューブマップ配列
	-
	ここのマルチサンプルとは各ピクセルの色を決定する際に複数のサンプル点を使用してピクセルの色を決定する事を言う。マルチサンプルの対比をシングルサンプルという
	マルチサンプルの使い所はアンチエイリアスなど
	マルチサンプルのデメリットはシェーダーでのデータ型シングルサンプルと異なったりフェッチ命令が異なったり処理負担が増えるといった点がある
	*/
	/*
	メモリ上のリソースは単にメモリ領域がわかってるだけでそのバッファの区切り方が二次元テクスチャ配列なのか三次元テクスチャなのかわからない
	そこで登場するのがリソースビューオブジェクト
	レンダーターゲットの場合のリソースビューオブジェクトがレンダーターゲットビューとなる
	*/
	/*
	レンダーターゲットビュー生成の流れ:
	-
	リソースの生成
	ディスクリプタヒープの生成
	レンダーターゲットビューの生成
	-
	今回のリソースはバックバッファなのでリソースの生成は無くて良い
	*/
	{
		//ディスクリプタヒープの設定
		/*
		ディスクリプタヒープとはディスクリプタを保存するための配列。
		このGPUでのディスクリプタは、GPUメモリ上に存在する様々なデータやバッファの種類や位置大きさを表す構造体のようなもの
		*/
		//https://cocoa-programing.hatenablog.com/entry/2018/11/21/%E3%80%90DirectX12%E3%80%91%E3%83%87%E3%82%B9%E3%82%AF%E3%83%AA%E3%83%97%E3%82%BF%E3%83%92%E3%83%BC%E3%83%97%E3%81%AE%E4%BD%9C%E6%88%90%E3%80%90%E5%88%9D%E6%9C%9F%E5%8C%96%E3%80%91
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = FrameCount; //ヒープ内のディスクリプタ数
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; //ディスクリプタヒープのタイプ
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE; //シェーダ内から参照できるかどうか、今回はなし
		desc.NodeMask = 0; //複数のGPUノードがある場合にノードを特定するためのビットマスクを設定

		//ディスクリプタヒープを生成
		hr = m_pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_pHeapRTV.GetAddressOf()));
		if (FAILED(hr))
		{
			return false;
		}

		//ディスクリプタヒープ先頭に格納されているCPUディスクリプタハンドルを取得
		auto handle = m_pHeapRTV->GetCPUDescriptorHandleForHeapStart();
		//ディスクリプタが一個の場合はこのまま使えば良いが、２個以上ある場合は先頭アドレスからどれくらいずらせばよいかわからないのでここで取得する
		//この値はデバイス依存なのでGPUごとに取得する必要がある
		auto incrementSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		for (auto i = 0u; i < FrameCount; ++i)
		{
			//レンダーターゲットビューの生成にはどのリソースを使用するのかを渡して上げる必要があるのでバッファを取得
			hr = m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(m_pColorBuffer[i].GetAddressOf()));
			if (FAILED(hr))
			{
				return false;
			}

			//レンダーターゲットビューの生成に必要な設定用の構造体
			D3D12_RENDER_TARGET_VIEW_DESC viewDesc = {};
			viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; //画面で見るときのピクセルフォーマットを指定
			viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; //どのようにレンダーターゲットのリソースにアクセスするかを指定
			//今回は二次元テクスチャなので二次元テクスチャ用の設定
			viewDesc.Texture2D.MipSlice = 0; //今回はスワップチェインを制作するときにミップマップレベルを一つしかないように設定したので0に設定
			viewDesc.Texture2D.PlaneSlice = 0; //テクスチャの平面スライス番号を指定、今回は平面を複数枚持つデータではないので0を指定

			//レンダーターゲットビューの生成
			m_pDevice->CreateRenderTargetView(m_pColorBuffer[i].Get(), &viewDesc, handle);

			m_HandleRTV[i] = handle;
			//ポインタをずらす
			handle.ptr += incrementSize;
		}
	}

	//フェンスの生成
	/*
	フェンスオブジェクトとは同期をとるためのもの
	フェンス自体の値がインクリメントされたかどうかで描画が完了したかを確認し同期を取る
	*/
	{
		//フェンスカウンターをリセット
		for (auto i = 0u; i < FrameCount; ++i)
		{
			m_FenceCounter[i] = 0;
		}

		//フェンスの生成
		hr = m_pDevice->CreateFence(
			m_FenceCounter[m_FrameIndex], //フェンスの初期化に用いる値
			D3D12_FENCE_FLAG_NONE, //フラグオプション、フェンスを共有するかどうかなど
			IID_PPV_ARGS(m_pFence.GetAddressOf()));
		if (FAILED(hr))
		{
			return false;
		}

		m_FenceCounter[m_FrameIndex]++;

		//イベントの生成
		//終わるまで待つという処理をOSの機能を使用して行う
		m_FenceEvent = CreateEvent(
			nullptr, //lpEventAttributes: 子プロセスが取得したハンドルを継承できるかどうかを決定する構造体へのポインタを指定
			FALSE, //bManualReset: 手動のリセットオブジェクトを作成するかどうか
			FALSE, //bInitialState: イベントオブジェクトの初期状態の指定、TRUEを指定するとシグナル情報に
			nullptr //lpName: イベントオブジェクトの名前をNULL終端文字列で指定	
		);
		if (m_FenceEvent == nullptr)
		{
			return false;
		}
	}

	//コマンドリストを閉じる
	m_pCmdList->Close();

	return true;
}

//描画処理
void App::Render()
{
	//コマンドの記録を開始

	//コマンドバッファの内容を戦闘に戻す
	m_pCmdAllocator[m_FrameIndex]->Reset();
	//コマンドリストの初期化処理、第２引数はパイプラインアロケータだが今回は使用しない
	m_pCmdList->Reset(m_pCmdAllocator[m_FrameIndex].Get(), nullptr);

	//リソースバリアの設定
	/*
	DX12はマルチスレッド対応をしているためGPUが表示している領域を上書きして表示を崩してしまうなどの割り込み処理を防ぐ機能
	ここでいう遷移とはリソース(フレームバッファ(?))に対して書き込み状態か表示状態のどちからに遷移すること
	サブリソースとはミップマップやキューー部マップなどのテクスチャの構成単位
	*/
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; //リソースバリアのタイプを指定、今回は様々な使用用途の間のサブリソースセットの遷移を示すバリア
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE; //フラグの設定
	//Transitionはサブリソースが異なる使用用途の場合に設定する
	barrier.Transition.pResource = m_pColorBuffer[m_FrameIndex].Get(); //遷移に使用するためのリソースのポインタを指定
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; //サブリソースの使用前の状態を指定
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; //サブリソースの使用後の状態を指定
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; //遷移のためのサブリソースの番号を指定、今回の場合だとすべてのサブリソースを遷移させる

	//リソースバリア
	m_pCmdList->ResourceBarrier(1, &barrier);

	//書き込む準備完了

	//レンダーターゲットの設定
	m_pCmdList->OMSetRenderTargets(
		1, //NumRenderTargetDescripter: 設定するレンダーターゲットビュー用のディスクリプタハンドルの個数を設定
		&m_HandleRTV[m_FrameIndex], //pRenderTargetDescriptors: 設定するレンダーターゲットビューのハンドルの配列を指定
		FALSE, //RTSingleHandleToDescriptorRange: 設定するディスクリプタハンドルの配列の範囲を単独とするかどうか指定。例えばRTVを3つ設定する場合TRUEにすると3つのRTVに対して同じディスクリプタハンドルが設定される
		nullptr //pDepthStencilDescriptor: 深度ステンシルビューディスクリプタの設定
	);

	//クリアカラーの設定, 背景色
	float clearColor[] = { 0.25f, 0.25f, 0.25f, 1.0f };

	//レンダーターゲットビューをクリア
	m_pCmdList->ClearRenderTargetView(
		m_HandleRTV[m_FrameIndex], //RenderTargetView: レンダーターゲットビューをクリアするためのディスクリプタハンドルを指定
		clearColor, //ColorRGBA: レンダーターゲットをクリアするための色を指定
		0, //NumRects: 設定する矩形の数を指定
		nullptr //pRects: レンダーターゲットをクリアするための矩形の配列を指定
	);

	//更新処理
	{
		m_RotateAngle += 0.025f;
		m_CBV[m_FrameIndex].pBuffer->World = DirectX::XMMatrixRotationY(m_RotateAngle);
	}

	//描画処理
	{
		//ルートシグニチャを設定
		m_pCmdList->SetGraphicsRootSignature(m_pRootSignature.Get());
		//定数ヒープディスクリプタを設定
		m_pCmdList->SetDescriptorHeaps(1, m_pHeapCBV.GetAddressOf());
		//定数バッファビューで使用するシェーダのレジスタ番号とGPU仮想アドレスを設定
		m_pCmdList->SetGraphicsRootConstantBufferView(0, m_CBV[m_FrameIndex].Desc.BufferLocation);
		//パイプラインステートを設定
		m_pCmdList->SetPipelineState(m_pPS0.Get());

		//三角形をプリミティブとして設定
		m_pCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//GPUスロット、個数、頂点バッファビューを設定
		m_pCmdList->IASetVertexBuffers(0, 1, &m_VBV);
		//レンダーターゲットへの描画領域を設定
		m_pCmdList->RSSetViewports(1, &m_Viewport);
		m_pCmdList->RSSetScissorRects(1, &m_Scissor);

		m_pCmdList->DrawInstanced(3, 1, 0, 0);
	}

	//リソースバリアの設定
	/*
	もうレンダーターゲットに書き込む必要はないので上記と同じように状態を"表示する"にリソースバリアをもとに戻す
	*/
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_pColorBuffer[m_FrameIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	//リソースバリア
	m_pCmdList->ResourceBarrier(1, &barrier);

	//コマンドの記録を終了
	m_pCmdList->Close();

	//コマンドを実行
	//作成したコマンドリストは何もしなければ実行されないのでここでコマンドキューに渡してあげる
	ID3D12CommandList* ppCmdLists[] = { m_pCmdList.Get() };
	m_pQueue->ExecuteCommandLists(1, ppCmdLists);

	//画面に表示
	Present(1);
}

//画面に表示し、次フレームの準備を行う
/*
具体的にはバックバッファをディスプレイへ表示したり、バックバッファの入れ替え操作を行う
*/
void App::Present(uint32_t interval)
{
	//画面に表示
	/*
	フロントバッファを画面に表示してバックバッファの入れ替え(スワップ)する
	*/
	m_pSwapChain->Present(
		//SyncInterval: ディスプレイの垂直同期とフレームの表示を同期する方法を指定
		/*
		0を指定した場合、即座に同期は行われない
		1を指定した場合、1回目の垂直同期後に表示を同期する。ディスプレイの更新周期が60Hzの場合60fpssとなる
		2を指定した場合、垂直同期が2回実行されて表示することを意味する
		3を指定した場合、垂直同期が3回実行されて表示することをする
		*/
		interval, 
		0 //Flags: スワップチェインの表示オプションを指定する。
	);

	//シグナル処理
	/*
	これをすることでコマンドキューの実行が完了したときにフェンスの値が更新されるようになる
	フェンスの値が更新されたらGPU上でのコマンドの実行が完了したものとして判断できるようになる
	*/
	const auto currentValue = m_FenceCounter[m_FrameIndex];
	m_pQueue->Signal(
		m_pFence.Get(), //pFence: フェンスオブジェクトのポインタ 
		currentValue //フェンスに設定する値
	);

	//バックバッファ番号を更新
	/*
	スワップ処理をしたことによってバックバッファが変わったのでインデックスを更新
	*/
	m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	//次のフレームの描画準備がまだであれば待機する
	/*
	値が更新されてない限り小さくなる
	*/
	if (m_pFence->GetCompletedValue() < m_FenceCounter[m_FrameIndex])
	{
		//完了していなかったのでこの関数でフェンスの値が指定値になったらイベントを起こすという設定を行う
		m_pFence->SetEventOnCompletion(
			m_FenceCounter[m_FrameIndex], //Value: イベントがシグナル状態になった時に設定するフェンスの値 
			m_FenceEvent //イベントオブジェクトへのハンドル
		);
		//ここで実際にイベントが起こるまで待機
		WaitForSingleObjectEx(
			m_FenceEvent, //hHandle: オブジェクトのハンドルを指定 
			INFINITE, //dwMilliseconds: アウトタイム時間
			FALSE //bAlertable: I/O完了ルーチンや非同期プロシージャ呼び出しをキューにおいた時制御を返すかどうかを指定
		);
	}

	//次フレームのフェンスカウンターを増やす
	m_FenceCounter[m_FrameIndex] = currentValue + 1;
}

/*
終了処理を行う前にGPUの実行中に必要なオブジェクトを開放してしまうと、色々よくないのでここですべて終了するまで待機
*/
void App::WaitGpu()
{
	assert(m_pQueue != nullptr);
	assert(m_pFence != nullptr);
	assert(m_FenceEvent != nullptr);

	//シグナル処理
	m_pQueue->Signal(m_pFence.Get(), m_FenceCounter[m_FrameIndex]);

	//完了時にイベントを設定する..
	m_pFence->SetEventOnCompletion(m_FenceCounter[m_FrameIndex], m_FenceEvent);

	//待機処理
	WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);

	//カウンターを増やす
	m_FenceCounter[m_FrameIndex]++;
}

/*
終了処理
色んなものを解放していく
*/
void App::TermD3D()
{
	//GPU処理の完了を待機
	WaitGpu();

	//イベント破棄
	if (m_FenceEvent != nullptr)
	{
		CloseHandle(m_FenceEvent);
		m_FenceEvent = nullptr;
	}

	//フェンス破棄
	m_pFence.Reset();

	//レンダーターゲットビューの破棄
	m_pHeapRTV.Reset();
	for (auto i = 0u; i < FrameCount; ++i)
	{
		m_pColorBuffer[i].Reset();
	}

	//コマンドリストの破棄
	m_pCmdList.Reset();

	//コマンドアロケータの破棄
	for (auto i = 0u; i < FrameCount; ++i)
	{
		m_pCmdAllocator[i].Reset();
	}

	//スワップチェインの破棄
	m_pSwapChain.Reset();

	//コマンドキューの破棄
	m_pQueue.Reset();

	//デバイスの破棄
	m_pDevice.Reset();
}

bool App::OnInit()
{
	//頂点バッファの生成
	/*
	頂点バッファは頂点データを纏めて送るために作成するオブジェクト。
	
	頂点バッファを作るためには
	ID3D12Resourceオブジェクトを生成して
	GPUの仮想アドレス、頂点全体のデータサイズ、1頂点あたりのデータサイズをそれぞれD3D12_VERTEX_BUFFER_VIEW構造体に設定
	そして実際にVRAMにマッピングをしたあとに
	そして中点バッファビューを生成
	*/
	{
		//頂点データ
		//今回は三角形
		//XMFLOAT3はPosition, XMFLOAT4はColor
		Vertex vertices[] = {
			{ DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
			{ DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
			{ DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		};

		//ヒーププロパティ
		D3D12_HEAP_PROPERTIES prop = {};
		//ヒープタイプの指定
		/*
		D3D12_HEAP_TYPE_UPLOADはCPUへの書き込みが一度でGPU読み込みが一度のデータに適している
		*/
		prop.Type = D3D12_HEAP_TYPE_UPLOAD;
		//ヒープのGPUページプロパティを指定
		//疑問: 本ではGPUになっていたがCPUページプロパティでは？
		/*
		今回の頂点データを1回しか書き込みしないので、特にCPUページプロパティを気にする必要はない
		ライトコンバインは書き込み指定を一時蓄積しておいてから適切なタイミングでまとめて書き出す方法
		ライトバックは高速なキャッシュメモリに書き込みを行っておき、CPUの空き時間等を利用してキャッシュメモリからメインメモリに書き込む方式
		*/
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		//ヒープに対するメモリプールの指定
		/*
		D3D12_MEMORY_POOL_L0はシステムメモリでCPUに対する帯域幅は大きいがGPU帯域幅は少ない
		D3D12_MEMORY_POOL_L1はVRAMでGPUに対する帯域幅は大きいがCPUからアクセスできない、またUMA(ユニファイドメモリアーキテクチャ)ではこれは使用できない
		このサンプルではメモリを意識しなくても良いのでUNKNOWNを指定している
		*/
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		prop.CreationNodeMask = 1; //複数GPUについて生成されるべきリソースの場所のノードを指定する。ここで単一GPUによる動作を前提としているので1を指定
		prop.VisibleNodeMask = 1; //複数GPUについてリソースが可視できる場所のノードの集合を指定。ここで単一GPUによる動作を前提としているので1を指定

		//リソースの設定
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER; //リソースの次元をここで列挙(バッファや一次元テクスチャなど)
		desc.Alignment = 0; //リソースのアライメントを指定, リソースの次元がD3D12_RESOURCE_DIMENSION_BUFFERの場合D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT(64kb)か0を指定する
		desc.Width = sizeof(vertices); //バッファサイズの指定, ここでは頂点バッファのサイズであるsizeof(vertices)を設定
		desc.Height = 1; //バッファの場合は1, テクスチャの場合は縦幅を指定
		desc.DepthOrArraySize = 1; //バッファの場合は1, 三次元テクスチャの場合は奥行きをテクスチャ配列の場合は配列数を指定する
		desc.MipLevels = 1; //バッファの場合は1、テクスチャの場合はミップマップレベル数を指定する
		desc.Format = DXGI_FORMAT_UNKNOWN; //バッファの場合はDXGI_FORMAT_UNKNOWNをテクスチャの場合はピクセルフォーマットを指定する
		//テクスチャの場合はマルチサンプルの設定をする
		desc.SampleDesc.Count = 1; //バッファの場合は1
		desc.SampleDesc.Quality = 0; //バッファの場合は0
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR; //テクスチャレイアウトを指定
		desc.Flags = D3D12_RESOURCE_FLAG_NONE; //ビット論理和でフラグを指定

		//リソースを生成
		//頂点バッファに使用するリソースの生成
		auto hr = m_pDevice->CreateCommittedResource(
			&prop, //D3D12_HEAP_PROPERTIESの構造体へのポインタを設定
			D3D12_HEAP_FLAG_NONE, //ヒープオプションをビット論理和として設定する
			&desc, //D3D12_RESOURCE_DESCの構造体へのポインタを設定
			D3D12_RESOURCE_STATE_GENERIC_READ, //リソースの初期状態を指定, これはHEAP_PROPERTIESのtypeをD3D12_HEAP_TYPE_UPLOADにした場合は必ずD3D12_RESOURCE_STATE_GENERIC_READを設定しなければいけない
			nullptr, //D3D12_CLEAR_VALUE構造体へのポインタを指定して、クリア値を設定する。この引数はレンダーターゲットと深度ステンシルテクスチャのためのオプション。D3D12_RESOURCE_DIMENSION_BUFFERを指定した場合はnullptrを指定する必要がある
			IID_PPV_ARGS(m_pVB.GetAddressOf())
		);

		if (FAILED(hr))
		{
			return false;
		}

		//マッピングする(頂点データの書き込み)
		void* ptr = nullptr;
		hr = m_pVB->Map(
			//サブリソースの番号を指定
			/*
			サブリソースとはテクスチャのミップマップデータのまとまりのこと
			*/
			0, 
			nullptr, //今回は頂点バッファの全領域をマッピングするので、nullptrを指定する
			&ptr //リソースデータへのポインタを受け取るメモリブロックへのポインタを指定
		);
		if (FAILED(hr))
		{
			return false;
		}
		
		//頂点データをマッピング先に設定
		memcpy(ptr, vertices, sizeof(vertices));

		//マッピング解除
		m_pVB->Unmap(
			0, //サブリソース番号
			nullptr //CPUが変更した範囲を指定
		);

		//頂点バッファビューの設定
		/*
		頂点バッファビューは描画コマンドを作成する際に使用する構造体
		*/
		m_VBV.BufferLocation = m_pVB->GetGPUVirtualAddress(); //GPUの仮想アドレスを指定, 
		m_VBV.SizeInBytes = static_cast<UINT>(sizeof(vertices)); //頂点バッファ全体のサイズを設定する
		m_VBV.StrideInBytes = static_cast<UINT>(sizeof(Vertex)); //1頂点あたりのサイズを設定
	}
	//頂点バッファはディスクリプタヒープでやり取りをしないのか？

	//定数バッファ用ディスクリプタヒープの生成
	/*
	定数バッファはシェーダ内で計算に使う定数値

	頂点バッファと同様に定義を用意して、その後にリソース生成を行い、定数バッファビューを作成する
	定数バッファの作成ではCPU側でデータ変更をする可能性が比較的に高く、GPUの処理中にバッファを書き換えてしまうと想定していない動作になる恐れがある。よってレンダーターゲットと同様に定数はダブルバッファ化しておくと良い
	*/
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		//疑問: 定数バッファをダブルバッファ化するのにFrameCountに依存するのは何か違うような
		/*
		1つの図形を描くために必要な総数は"1 * FrameCount"であるので(?)
		*/
		desc.NumDescriptors = 1 * FrameCount;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; //レンダーターゲットのときはシェーダから見える必要がないためHEAP_NONEを指定していたが今回は頂点シェーダー内で使用するためこれを指定
		desc.NodeMask = 0; //GPUが一つであれば0を指定。複数のGPUノードが存在する場合はGPUノードを識別するためのビットを指定

		//ディスクリプタヒープ生成
		auto hr = m_pDevice->CreateDescriptorHeap(
			&desc,
			IID_PPV_ARGS(m_pHeapCBV.GetAddressOf())
		);

		if (FAILED(hr))
		{
			return false;
		}
	}

	//定数バッファの生成
	{
		//ヒーププロパティ
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_UPLOAD; //頂点シェーダで使用するのでTypeはD3D12_HEAP_TYPE_UPLOADに
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN; //ここは特にこだわりがないのでUNKNOWN
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN; //ここは特にこだわりがないのでUNKNOWN
		prop.CreationNodeMask = 1;
		prop.VisibleNodeMask = 1;

		//リソースの設定
		//バッファのサイズ以外は基本的に頂点バッファと同じ
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Alignment = 0;
		desc.Width = sizeof(Transform);
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		//ハンドルをどれくらいすすめるかを表す
		/*
		CBV: Constant Buffer View
		SRV: Shader Resource View
		UAV: Unordered Access View
		*/
		auto incrementSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		//定数バッファビューは今回はダブルバッファなのでその分必要となる
		for (auto i = 0; i < FrameCount; ++i)
		{
			//リソースの生成
			auto hr = m_pDevice->CreateCommittedResource(
				&prop,
				D3D12_HEAP_FLAG_NONE,
				&desc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(m_pCB[i].GetAddressOf())
			);

			if (FAILED(hr))
			{
				return false;
			}

			//GPUの仮想アドレスを取得
			auto address = m_pCB[i]->GetGPUVirtualAddress();
			//この２つはディスクリプタヒープの先頭ハンドルを取得する
			auto handleCPU = m_pHeapCBV->GetCPUDescriptorHandleForHeapStart();
			auto handleGPU = m_pHeapCBV->GetGPUDescriptorHandleForHeapStart();

			//ハンドルを移動
			handleCPU.ptr = incrementSize * i;
			handleGPU.ptr = incrementSize * i;

			//定数バッファビューの設定
			m_CBV[i].HandleCPU = handleCPU;
			m_CBV[i].HandleGPU = handleGPU;
			m_CBV[i].Desc.BufferLocation = address; //GPUの仮想アドレス
			m_CBV[i].Desc.SizeInBytes = sizeof(Transform); //定数バッファのサイズを指定, ここはalignmentを256byteにしなければいけない

			//定数バッファビューを生成
			//第２引数はD3D12_CPU_DESCRIPTOR_HANDLE型を指定、この値はディスクリプタヒープから取得可能
			m_pDevice->CreateConstantBufferView(&m_CBV[i].Desc, handleCPU);

			//マッピング
			//Map()メソッドはポインタを取得することができる
			hr = m_pCB[i]->Map(0, nullptr, reinterpret_cast<void**>(&m_CBV[i].pBuffer));
			if (FAILED(hr))
			{
				return false;
			}

			auto eyePos = DirectX::XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f);
			auto targetPos = DirectX::XMVectorZero();
			auto upward = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

			auto fovY = DirectX::XMConvertToRadians(37.5f);
			auto aspect = static_cast<float>(m_width) / static_cast<float>(m_height);

			//変換行列の設定
			m_CBV[i].pBuffer->World = DirectX::XMMatrixIdentity();
			//カメラからの基底を作成するのに使用
			m_CBV[i].pBuffer->View = DirectX::XMMatrixLookAtRH(eyePos, targetPos, upward);
			//near clipの大きさに射影変換行列が依存する
			m_CBV[i].pBuffer->Proj = DirectX::XMMatrixPerspectiveFovRH(fovY, aspect, 1.0f, 1000.0f);
		}
	}

	//ルートシグニチャの生成
	/*
	ルートシグニチャは定数バッファやテクスチャ、サンプラーといったシェーダー内で使用するリソースのレイアウトを決めるオブジェクト
	どのシェーダーのレジスタにどのリソースを関連付けるかが決まる
	レジスタは定数バッファを定義するときに決める
	*/
	{
		//アプリケーションが入力アセンブラを使用することを示す
		//このフラグがないとルートシグニチャの最適化が小規模になってしまう
		auto flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		//ドメインシェーダのルートシグニチャへのアクセスを拒否する
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
		//ハルシェーダのルートシグニチャへのアクセスを拒否する
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
		//ジオメトリシェーダーのルートシグニチャへのアクセスを拒否する
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

		//ルートパラメータの設定
		D3D12_ROOT_PARAMETER param = {};
		//ルートパラメータの指定
		/*
		ルートパラメータは
		--
		ルート定数: 32bitデータを扱いたい場合や32bitデータの塊を使いたい場合
		定数バッファビュー: ルート定数に比べると大きめのデータ、あるいは構造体のようにデータを纏めて使いたい場合
		シェーダリソースビュー: テクスチャとして扱いたい場合
		アンオーダードアクセスビュー: コンピュートシェーダなどで、データの読み/書きを行うバッファとして使いたい場合
		ディスクリプタテーブル: 定数バッファビュー、シェーダリソースビュー、アンオーダードアクセスビュー、サンプラーの4種類のルートパラメタをまとめて扱いたい場合
		--
		があり、これらはそれぞれ対応する構造体が存在する
		このそれぞれの構造体に出てくるメンバ変数の名前の意味は
		ShaderRegister: シェーダのレジスタ番号を指定。例えばb1レジスタなら1を指定
		RegisterSpace: シェーダの論理的なレジスタ空間を指定
		Num32BitValue: ルート定数で使用、単一のシェーダースロットをしてる定数の数を指定。例えばfloat3なら3を指定
		これらはルート定数、CBV、SRV、UAVで使用され、
		ディスクリプタテーブルはpDescriptorRangesというD3D12_DESCRIPTOR_RANGE型の配列とその配列の大きさを決めるNumRangeDescriptorRangesというUINT型のパタメータ
		D3D12_DESCRIPTOR_RANGEは
		--
		RangeType: D3D12_DESCRIPTOR_RANGE_TYPEで指定。SRVかUAVかCBVかサンプラーのどれを使用するかを決める
		NumDescriptors: ディスクリプタの数を指定、-1またはUINT_MAXを指定すると領域無しとなる
		RegisterSpace: レジスタ空間を指定
		OffsetInDescriptorsFromTableStart: ルートシグニチャの先頭からのディスクリプタのオフセットを指定、データが連続せず飛び飛びになる場合はオフセットを指定
		--
		*/
		/*
		基本的な使い分けとしては
		10個以上を設定する必要がある場合はディスクリプタテーブルでそれ以外はルートパラメータのタイプに応じて個別設定すると良い
		定数バッファについてはサイズが小さいものをルート定数にして、大きいものをCBVにするという使い分けが良い
		*/
		//疑問: サンプラーとは
		param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		//今回はb0なので0
		param.Descriptor.ShaderRegister = 0;
		param.Descriptor.RegisterSpace = 0;
		//今回は頂点シェーダーでアクセスできれば良いので
		param.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

		//ルートシグニチャの設定
		D3D12_ROOT_SIGNATURE_DESC desc = {};
		desc.NumParameters = 1; //ルートパラメータの配列数
		desc.NumStaticSamplers = 0; //スタティックサンプラーの配列数
		desc.pParameters = &param; //ルートパラメータの配列
		desc.pStaticSamplers = nullptr; //スタティックサンプラーの配列
		desc.Flags = flag; //フラグ指定

		//以下のシリアライズ結果が書き込まれる
		ComPtr<ID3DBlob> pBlob;
		//シリアライズが失敗した場合はエラー内容が書き込まれる
		ComPtr<ID3DBlob> pErrorBlob;

		//ルートシグニチャのシリアライズ
		auto hr = D3D12SerializeRootSignature(
			&desc,
			D3D_ROOT_SIGNATURE_VERSION_1_0, //ルートシグニチャのバージョン
			pBlob.GetAddressOf(),
			pErrorBlob.GetAddressOf()
		);

		if (FAILED(hr))
		{
			return false;
		}

		//ルートシグニチャを生成
		hr = m_pDevice->CreateRootSignature(
			0, //nodeMask: 単一GPUの場合はゼロを指定
			pBlob->GetBufferPointer(), //pBlobWithRootSignature: シリアライズしたデータのポインタを指定
			pBlob->GetBufferSize(), //blobLengthInBytes: シリアライズしたデータのサイズをバイト単位で指定
			IID_PPV_ARGS(m_pRootSignature.GetAddressOf())
		);

		if (FAILED(hr))
		{
			return false;
		}
	}

	//パイプラインステートの生成
	/*
	描画に使用するシェーダや頂点シェーダへの入力データ、描画ステートを設定するためのパイプラインステートを生成
	パイプラインステートは２種類あり、1つは描画用に使用するグラフィックスパイプラインステートで
	もう1つはコンピュートシェーダによる計算用に使用するコンピュートパイプラインステート
	今回は計算ではなく描画に使用するためのグラフィックスパイプラインステートを生成
	*/
	{
		//入力レイアウトの設定
		/*
		入力レイアウトは頂点シェーダの入力がどのようなデータ構造であるかを設定するためのもの
		頂点内に含まれる1要素をD3D12_INPUT_ELEMENT_DESCで定義する。

		*/
		D3D12_INPUT_ELEMENT_DESC elements[2];
		elements[0].SemanticName = "POSITION"; //セマンティクス名を指定
		//セマンティクス番号を指定
		/*
		この番号が必要になるのはHLSL側の定義でセマンティクス名が"POSITION0", "POSITION1", "POSITION2"のようになっているものであり、
		この場合左から0, 1, 2のようにセマンティクス番号を指定する
		*/
		elements[0].SemanticIndex = 0;
		elements[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT; //float3型ならDXGI_FORMAT_R32G32B32A32_FLOATとなる
		//入力スロット番号を指定, 有効値は0から15であり、今回は複数の頂点バッファを扱わないので0を指定
		/*
		スロットはGPUが頂点データを見る"のぞき穴"のようなものだとイメージすると良い
		複数の頂点データを一度に流し込みたいときや
		座標データと法線、カラーのようにスロットを分けることもある
		*/
		elements[0].InputSlot = 0; 
		elements[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT; //各要素のオフセットをバイト単位単位で指定
		//データの入力単位を指定
		/*
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATAの場合は頂点ごとの入力データが頂点シェーダに来る
		D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATAの場合はインスタンスごとの入力データが頂点シェーダに来る
		*/
		elements[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		elements[0].InstanceDataStepRate = 0; //インスタンスごとのデータの繰り返し回数を指定します。2以上だとそのデータはその数だけ同じデータを使い回すことになる

		//上と同じ
		elements[1].SemanticName = "COLOR";
		elements[1].SemanticIndex = 0;
		elements[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		elements[1].InputSlot = 0;
		elements[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		elements[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		elements[1].InstanceDataStepRate = 0;

		//ラスタライザーステートの設定
		/*
		ラスタライズを行う際の設定
		*/
		D3D12_RASTERIZER_DESC descRS;
		descRS.FillMode = D3D12_FILL_MODE_SOLID; //wireframeを描画するかどうかの設定
		descRS.CullMode = D3D12_CULL_MODE_NONE; //指定された方向に面する三角形を描画しないように設定
		descRS.FrontCounterClockwise = FALSE; //全面が反時計回りかどうかを指定, この場合は時計回りを全面とする
		descRS.DepthBias - D3D12_DEFAULT_DEPTH_BIAS; //ピクセルに加算する深度値を設定する
		descRS.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP; //ピクセル最大深度バイアス値を設定
		descRS.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS; //ピクセルの後輩に応じた深度バイアス値をスケールするスカラー値を設定
		descRS.DepthClipEnable = FALSE; //距離に基づくクリッピングをするかどうか
		descRS.MultisampleEnable = FALSE; //マルチサンプリング有効にするかどうかを指定
		descRS.AntialiasedLineEnable = FALSE; //線のアンチエイリアシングを有効にするかどうかを指定
		descRS.ForcedSampleCount = 0; //UAVの描画かもしくはラスタライズの間、サンプル数を矯正的に固定値にする。0の場合はサンプル数が矯正されないことを示す
		//コンサバティブラスタライゼーションを有効にするかどうかを指定
		/*
		コンサバティブラスタライゼーションはちょっとでもピクセルにかかったらラスタライズを実行するという保守的なラスタ来ゼーションを行うためのモード
		通常のラスタライゼーションはサンプルポイントと呼ばれる判定点があり、ポリゴンがこのサンプルポイントに重なら場合はラスタライゼーションが実行され、重ならない場合は実行されない
		*/
		//疑問: サンプルポイント?
		descRS.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		/*
		ブレンドステートとは
		入力要素と演算操作、出力要素の3つを用いてどのように混合処理を行うかを決めるためのステート
		入力要素と出力要素はそれぞれ係数を設定することができ、
		演算操作は入力要素と出力要素をどのように演算するかを設定できる
		これにより半透明などが実現可能となる
		*/

		//レンダーターゲットのブレンド設定
		/*
		第一引数から順番に
		BlendEnable: ブレンディングを有効にするかどうかのフラグ
		LogicIoEnable: 論理演算を有効にするかどうかのフラグ
		SrcBlend: ピクセルシェーダーから出力されたRGB値に対して実行するブレンドオプションを指定
		DestBlend: 現在のレンダーターゲットのRGB値に対して実行するブレンドオプションを指定
		BlendOp: SrcBlendとDestBlendをどのように結合するかを定義
		SrcBlendAlpha: ピクセルシェーダから出力されたアルファ値に対して実行するブレンドオプションを指定, _COLORで終わるブレンドオプションは指定できない
		DestBlendAlpha: 現在のレンダーターゲットのアルファ値に対して実行するブレンドオプションを指定m _COLORで終わるブレンドオプションは指定できない
		BlendOpAlpha: SrcBlendAlphaとDestBlendAlphaをどのように結合するかを定義する
		LogicOp: レンダーターゲットに対して設定する論理演算を指定
		RenderTargetWriteMask: レンダーターゲットの書き込みマスクを指定する、ビットごとのOR演算を使用して結合される
		*/
		/*
		入力色と出力色にそれぞれ乗算する係数のD3D12_BLEND一覧
		ここは大きいので以下で使用されてるものを取り上げる
		D3D12_BLEND_ZERO: 係数は(0, 0, 0, 0)
		D3D12_BLEND_ONE: 係数は(1, 1, 1, 1)
		*/
		/*
		ブレンドオプション一覧
		D3D12_BLEND_OP_ADD: 入力元オペランドと出力先オペランドを加算
		D3D12_BLEND_OP_SUBTRACT: 出力先オペランドから入力元オペランドを減算
		D3D12_BLEND_OP_REV_SUBTRACT: 入力元オペランドから出力先オペランドを減算
		D3D12_BLEND_OP_MIN: 入力元オペランドと出力先オペランドの最小値を求める
		D3D12_BLEND_OP_MAX: 入力元オペランドと出力先オペランドの最大値を求める
		*/
		D3D12_RENDER_TARGET_BLEND_DESC descRTBS = {
			FALSE, FALSE,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD, //Alpha
			D3D12_LOGIC_OP_NOOP, //何もしない
			D3D12_COLOR_WRITE_ENABLE_ALL
		};

		//ブレンドステートの設定
		D3D12_BLEND_DESC descBS;
		//アルファトゥガバレッジと呼ばれる機能を有効にするフラグ
		/*
		アルファトゥガバレッジはピクセルシェーダから出力されたアルファ成分を取得し、マルチサンプリングアンチエイリアス処理を適用する機能
		*/
		descBS.AlphaToCoverageEnable = FALSE;
		//レンダーターゲットの独立したブレンドを使用するかどうかの設定フラグ
		descBS.IndependentBlendEnable = FALSE;
		for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		{
			descBS.RenderTarget[i] = descRTBS;
		}

		ComPtr<ID3DBlob> pVSBlob;
		ComPtr<ID3DBlob> pPSBlob;

		//プロジェクトをビルドした際にシェーダは出力ディレクトリに(Filename).csoとして出力される

		//頂点シェーダ読み込み
		auto hr = D3DReadFileToBlob(L"SimpleVS.cso", pVSBlob.GetAddressOf());
		if (FAILED(hr))
		{
			return false;
		}

		//ピクセルシェーダ読み込み
		hr = D3DReadFileToBlob(L"SimplePS.cso", pPSBlob.GetAddressOf());
		if (FAILED(hr))
		{
			return false;
		}

		//パイプラインステートの設定
		/*
		パイプラインステートの生成はこの設定を行った構造体を引数として渡す
		*/
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.InputLayout = { elements, _countof(elements) };
		desc.pRootSignature = m_pRootSignature.Get();
		desc.VS = { pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize() };
		desc.PS = { pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize() };
		desc.RasterizerState = descRS;
		desc.BlendState = descBS;
		desc.DepthStencilState.DepthEnable = FALSE;
		desc.DepthStencilState.StencilEnable = FALSE;
		desc.SampleMask = UINT_MAX;
		desc.PrimitiveTopologyType - D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		desc.DSVFormat = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		hr = m_pDevice->CreateGraphicsPipelineState(
			&desc,
			IID_PPV_ARGS(m_pPS0.GetAddressOf())
		);

		if (FAILED(hr))
		{
			return false;
		}

		//ビューポートとシザー矩形の設定

		{
			//今回はウィンドウ全体に書き込むのでビューポートはこんな感じ
			m_Viewport.TopLeftX = 0;
			m_Viewport.TopLeftY = 0;
			m_Viewport.Width = static_cast<float>(m_width);
			m_Viewport.Height = static_cast<float>(m_height);
			m_Viewport.MinDepth = 0.0f;
			m_Viewport.MaxDepth = 1.0f;

			//シザー矩形は刈り取る領域を指定
			//今回はビューポートと同じサイズにしておき、ビューポートの外は描画しないようにする
			m_Scissor.left = 0;
			m_Scissor.right = m_width;
			m_Scissor.top = 0;
			m_Scissor.bottom = m_height;
		}

		return true;
	}
}

//終了時の処理
void App::OnTerm() 
{
	for (auto i = 0; i < FrameCount; ++i)
	{
		if (m_pCB[i].Get() != nullptr)
		{
			m_pCB[i]->Unmap(0, nullptr);
			memset(&m_CBV[i], 0, sizeof(m_CBV[i]));
		}
		m_pCB[i].Reset();
	}

	m_pVB.Reset();
	m_pPS0.Reset();
}