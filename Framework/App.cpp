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

//�R���X�g���N�^
App::App(uint32_t width, uint32_t height)
	: m_hInst(nullptr)
	, m_hWnd(nullptr)
	, m_width(width)
	, m_height(height)
{}

//�f�X�g���N�^
App::~App()
{}

namespace {
	const auto ClassName = TEXT("SampleWindowClass");
}

//���s����
void App::Run()
{
	if (InitApp())
	{
		MainLoop();
	}

	TermApp();
}

//����������
bool App::InitApp() 
{
	//�E�B���h�E�̏�����
	if (!InitWnd())
	{
		return false;
	}

	//����I��
	return true;
}

//�E�B���h�E�̏���������
bool App::InitWnd() 
{
	//�C���X�^���X�n���h��������
	auto hInst = GetModuleHandle(nullptr);
	if (hInst == nullptr) 
	{
		return false;
	}

	//�E�B���h�E�̐ݒ�
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX); //WNDCLASSEX�\���̎��̂̃T�C�Y��ݒ�
	wc.style = CS_HREDRAW | CS_VREDRAW; //�E�B���h�E�̃X�^�C�������߂�t���O�A����͐��������Ɛ��������̃��T�C�Y���̍ĕ`��̃t���O�̘_���a
	wc.lpfnWndProc = WndProc; //�E�B���h�E�v���V�[�W���̐ݒ�B�E�B���h�E�v���V�[�W���Ƃ̓}�E�X�̃N���b�N��E�B���h�E�̃��T�C�Y���̃��b�Z�[�W�������Ă����Ƃ��ɌĂ΂��R�[���o�b�N�֐�
	//wc.hInstance = hInst; //�A�v���P�[�V�����̃C���X�^���X�n���h�����w��
	wc.hIcon = LoadIcon(hInst, IDI_APPLICATION); //�A�v���P�[�V�����̃A�C�R����ݒ�B�����Windows�ŗp�ӂ���Ă���A�C�R�����g�p
	wc.hCursor = LoadCursor(hInst, IDC_ARROW); //�A�v���P�[�V������Ŏg�p����J�[�\����ݒ�B�����Windows�ŗp�ӂ���Ă���J�[�\�����g�p
	wc.hbrBackground = GetSysColorBrush(COLOR_BACKGROUND); //�w�i�F�ݒ�V�X�e���J���[���u���V�ɕϊ�
	wc.lpszMenuName = nullptr; //���j���[�o�[�̐ݒ�A����͉����ݒ肵�Ȃ�
	wc.lpszClassName = ClassName; //�E�B���h�E�N���X�����ʂ��閼�O�̐ݒ�
	wc.hIconSm = LoadIcon(hInst, IDI_APPLICATION); //�^�C�g���o�[�ȂǂɊ܂܂�鏬�����A�C�R���̐ݒ�

	//�E�B���h�E�̓o�^
	if (!RegisterClassEx(&wc))
	{
		return false;
	}

	//�C���X�^���X�n���h���ݒ�
	m_hInst = hInst;

	RECT rc = {};
	rc.right = static_cast<LONG>(m_width);
	rc.bottom = static_cast<LONG>(m_height);

	//�E�B���h�E�T�C�Y�𒲐�
	auto style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
	AdjustWindowRect(&rc, style, FALSE);

	//�E�B���h�E�𐶐�
	m_hWnd = CreateWindowEx(
		0, //dwExStyle
		ClassName, //lpClassName
		TEXT("Sample"), //lpWindowName
		style, //dwStyle
		CW_USEDEFAULT, //x���W
		CW_USEDEFAULT, //y���W
		rc.right - rc.left, //Width
		rc.bottom - rc.top, //Height
		nullptr, //hWndParent �쐬����E�B���h�E�̐e�E�B���h�E��I�[�i�[�E�B���h�E�̃n���h�����w��
		nullptr, //hMenu
		m_hInst, //�E�B���h�E�Ɋ֘A�t������C���X�^���X�̃n���h�����w��
		nullptr); //lpParam �C�ӂ̃p�����[�^

	if (m_hWnd == nullptr)
	{
		return false;
	}

	//�E�B���h�E��\��
	ShowWindow(m_hWnd, SW_SHOWNORMAL);
	
	//�E�B���h�E���X�V
	UpdateWindow(m_hWnd);

	//�E�B���h�E�Ƀt�H���J�X��ݒ�
	SetFocus(m_hWnd);

	//����I��
	return true;
}

//�E�B���h�E�v���V�[�W��
//Windows�����烁�b�Z�[�W�������Ă����Ƃ��ɏ���������R�[���o�b�N
LRESULT CALLBACK App::WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) 
{

	switch (msg)
	{
		case WM_DESTROY:
			//�����ŃE�B���h�E�I�����߂𑗐M
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
����:
�f�o�C�X�̐���
�R�}���h�L���[�̐���
�X���b�v�`�F�C���̐���
�R�}���h�A���P�[�^�̐���
�R�}���h���X�g�̐���
�����_�[�^�[�Q�b�g�r���[�̐���
�t�F���X�̐���
*/
bool App::InitD3D()
{
	//�f�o�C�X�̐���
	/*
	�����f�o�C�X��\�����邽�߂̃C���^�[�t�F�C�X
	���\�[�X��p�C�v���C���X�e�[�g�I�u�W�F�N�g�͂��̃f�o�C�X��ʂ��Đ��������
	*/
	auto hr = D3D12CreateDevice(
		nullptr, //�r�f�I�A�_�v�^�ւ̃|�C���^�ݒ�, �f�t�H���g�p�������ꍇ��nullptr
		D3D_FEATURE_LEVEL_11_0, //�f�o�C�X�𐶐�����Ƃ��ɐ��������ł��낤�ŏ����̃o�[�W�������w��, ���Ƃ��΍Ō��12_0�ɂ����ꍇ��DX12���T�|�[�g���ꂽ���ł����ł��Ȃ����A11_0�Ȃ�11��12���T�|�[�g���ꂽ���łł���
		IID_PPV_ARGS(&m_pDevice)
	);

	/*
	��3�����Ƒ�4�����ɂ��Ă�IID_PPV_ARGS�Ƃ����}�N�����W�J����Ċ��蓖�Ă��Ă���
	��3�����̓f�o�C�X�C���^�[�t�F�C�X�����ʂ��邽�߂�GUID���w��
	��4�����̓f�o�C�X�C���^�[�t�F�C�X���󂯎�邽�߂̃|�C���^���w��A�����̌^��void**�Ȃ̂ŃL���X�g���K�v(�f�o�C�X�C���^�[�t�F�C�X�Ƃ�m_pDevice�̂���)

	����IID_PPV_ARGS���g�p���Ȃ��ꍇ
	auto hr = D3D12CreateDevice(
		nullptr,
		D3D_FEATURE_LEVEL_11_0,
		__uuidof(&m_pDevice),
		reinterpret_cast<void**>(&m_pDevice)
	);
	�ƂȂ�
	*/

	//���������s���m�F
	/*
	����FAILED�}�N����
	(((HRESULT)(hr)) < 0)
	�ƂȂ�
	*/
	if (FAILED(hr))
	{
		return false;
	}

	//�R�}���h�L���[�̐���
	/*
	�R�}���h�L���[��GPU�ɑ��邽�߂̕`��R�}���h�̓��e�A�܂��`��R�}���h���s�̓����������s�����߂̃��\�b�h�񋟂��s��
	*/
	{
		D3D12_COMMAND_QUEUE_DESC desc = {}; //�ǂ̂悤�ȃR�}���h�L���[�ł��邩��ݒ肷�邽�߂̍\���̂𐶐��A������R�}���h�L���[�𐶐�����Ƃ��ɐݒ肷��
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; //�R�}���h�L���[�ɓo�^�\�ȃR�}���h���X�g�̃^�C�v��ݒ�
		//https://hexadrive.jp/hexablog/program/13072/
		//https://cocoa-programing.hatenablog.com/entry/2018/11/24/%E3%80%90DirectX12%E3%80%91%E3%82%B3%E3%83%9E%E3%83%B3%E3%83%89%E3%82%A2%E3%83%AD%E3%82%B1%E3%83%BC%E3%82%BF%E3%81%AE%E4%BD%9C%E6%88%90%E3%80%90%E5%88%9D%E6%9C%9F%E5%8C%96%E3%80%91
		//http://www.project-asura.com/program/d3d12/d3d12_004.html
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL; //�R�}���h�L���[�̗D��x�w��
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE; //��MS��t�H�L���[�̓����������t���O
		desc.NodeMask = 0; //GPU����ł����0���w��B������GPU�m�[�h�����݂���ꍇ��GPU�m�[�h�����ʂ��邽�߂̃r�b�g���w��

		//CreateCommandQueue�͌��Ă̒ʂ�R�}���h�L���[�̐���
		hr = m_pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_pQueue));
		if (FAILED(hr)) 
		{
			return false;
		}
	}

	//�X���b�v�`�F�C���̐���
	/*
	�_�u���o�b�t�@�����O��g���v���o�b�t�@�����O���s�����߂̎d�g��
	�_�u���o�b�t�@�����O��g���v���o�b�t�@�����O�͈�̃t���[���o�b�t�@�������ƃe�B�A�����O�������N�����Ă��܂������������邽�߂ɍ��ꂽ
	��̃t���[���o�b�t�@�������ƃ��j�^�̃��t���b�V�����[�g���t���[�����[�g�������������Ă��܂��Ɖ�ʍX�V���Ƀo�b�t�@�����������Ă�r���ōX�V����ăe�B�A�����O��������
	������Q�t���[���o�b�t�@��p�ӂ��邱�ƂŕЕ��ɏ�������ł����Е����I��������ʂɏo�͂��邽�߂̃o�b�t�@�Ǝ��̃t���[�����������ރo�b�t�@��؂�ւ���
	�f�B�X�v���C�ɕ\������Ă���摜�Ɏg���Ă���o�b�t�@���t�����g�o�b�t�@�A�������݂��s���Ă���o�b�t�@���o�b�N�o�b�t�@�ƌĂ�
	�������邱�Ƃŏ������ݓr���̃o�b�t�@��\�����邱�Ƃ͂Ȃ��Ȃ�e�B�A�����O���Ȃ��Ȃ�
	*/
	{
		//DXGI�t�@�N�g���[�̐���
		/*
		DXGI�Ƃ�DirectX�O���t�B�b�N�X�C���X�g���N�`���[�̗��ŃJ�[�l�����[�h�h���C�o�܂��V�X�e���n�[�h�E�F�A�ƒʐM���邱�Ƃ�ړI�Ƃ���API
		DXGI���̂�Direct3D�Ɉˑ��֌W�������Ă��Ȃ�
		DXGI���̂͊�{�I�ɂ�GPU���ڑ�����Ă���̂��񋓂�����A�`�悵���t���[������ʂ֕\�������肷��̂Ɏg�p����
		*/
		//���̖����̐����͗Ⴆ�΂���IDXGIFactory4����DXGI���o�[�W����1.4�̂Ƃ��ɉ�����ē��������̂Ƃ����Ӗ����w��
		IDXGIFactory4* pFactory = nullptr;
		hr = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));
		if (FAILED(hr))
		{
			return false;
		}

		//�X���b�v�`�F�C���̐ݒ�
		DXGI_SWAP_CHAIN_DESC desc = {}; //swapchain�̐ݒ�p�\����
		desc.BufferDesc.Width = m_width; //�𑜓x�̉������w��
		desc.BufferDesc.Height = m_height; //�𑜓x�̏c�����w��
		//���t���b�V�����[�g(Hz)���w��
		desc.BufferDesc.RefreshRate.Numerator = 60; //���q
		desc.BufferDesc.RefreshRate.Denominator = 1; //����
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; //�������̃��[�h���w�� (�C���^���[�X���v���O���b�V�u���̈Ⴂ)
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; //�g��k���̐ݒ�
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //�\���`���̃s�N�Z���t�H�[�}�b�g���w�� (��Ƃ��Ă�R8G8B8A8_UNORM�̂悤��GPU�ɓn���Ƃ��ɐ��K������) https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format
		//�}���`�T���v�����O�ݒ�
		desc.SampleDesc.Count = 1; //�s�N�Z���P�ʂ̃}���`�T���v�����O�̐�
		desc.SampleDesc.Quality = 0; //�摜�̕i�����x�����w��A�i���������قǃp�t�H�[�}���X�͒ቺ
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //�o�b�N�o�b�t�@(swapchain�̏������݂��s���Ă���o�b�t�@)�̎g�p���@���w��B����͏o�̓����_�[�^�[�Q�b�g�Ƃ��Ďg�p
		desc.BufferCount = FrameCount; //�t���[���o�b�t�@�̐����w��A����̓_�u���o�b�t�@�����O�Ȃ̂�2���w��
		desc.OutputWindow = m_hWnd; //�o�͂���E�B���h�E�̃E�B���h�E�n���h�����w��
		desc.Windowed = TRUE; //
		//�o�b�N�o�b�t�@����ւ������ʂ��w��
		//DXGI_SWAP_EFFECT_SEQUENTIAL��IDXGISwapChain1::Present()�Ăяo����Ƀo�b�N�o�b�t�@�̓��e���ω����Ȃ��A����̓}���`�T���v�����O�Ŏg�����Ƃ��ł��Ȃ�
		//DXGI_SWAP_EFFECT_FLIP_DISCARD��IDXGISwapChain1::Present()�Ăяo����Ƀo�b�N�o�b�t�@�̓��e��j���A������}���`�T���v�����O�Ŏg�����Ƃ��ł��Ȃ�
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		//swapchain�̓���I�v�V�������w��, ����̏ꍇ��IDXGISwapChain::ResizeTarget()���Ăяo�����Ƃŕ\�����[�h��؂�ւ�����
		//���̃t���O�Ɋ܂܂����GDI�Ƃ����������邪�����Graphic Device Interface�̗���Direct2D����������2D API
		//https://www.itmedia.co.jp/pcuser/articles/0905/20/news103.html
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		//�X���b�v�`�F�C���̐���
		IDXGISwapChain* pSwapChain = nullptr;
		hr = pFactory->CreateSwapChain(m_pQueue, &desc, &pSwapChain);
		if (FAILED(hr))
		{
			SafeRelease(pFactory);
			return false;
		}

		//��������_�u���o�b�t�@�����O���s���ۂ̃o�b�N�o�b�t�@�ԍ����擾

		//IDXGISwapChain3���擾
		hr = pSwapChain->QueryInterface(IID_PPV_ARGS(&m_pSwapChain));
		if (FAILED(hr))
		{
			SafeRelease(pFactory);
			SafeRelease(pSwapChain);
			return false;
		}


		//�o�b�N�o�b�t�@�ԍ����擾
		//�^��: �o�b�N�o�b�t�@�̓X���b�v�����ŏ�ɓ���ւ��̂ɂ����Ō��ߑł��Ŏ擾���Ă��̂܂܂ŗǂ��̂��H
		m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

		//�s�v�ɂȂ����̂ŊJ��
		SafeRelease(pFactory);
		SafeRelease(pSwapChain);
	}

	//�R�}���h�A���P�[�^�̐���
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

	//�R�}���h���X�g�̐���
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

	//�����_�[�^�[�Q�b�g�r���[�̐���
	{
		//�f�B�X�N���v�^�q�[�v�̐ݒ�
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = FrameCount;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask = 0;

		//�f�B�X�N���v�^�q�[�v�𐶐�
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

			//�����_�[�^�[�Q�b�g�r���[�̐���
			m_pDevice->CreateRenderTargetView(m_pColorBuffer[i], &viewDesc, handle);

			m_HandleRTV[i] = handle;
			handle.ptr += incrementSize;
		}
	}

	//�t�F���X�̐���
	{
		//�t�F���X�J�E���^�[�����Z�b�g
		for (auto i = 0u; i < FrameCount; ++i)
		{
			m_FenceCounter[i] = 0;
		}

		//�t�F���X�̐���
		hr = m_pDevice->CreateFence(
			m_FenceCounter[m_FrameIndex],
			D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(&m_pFence));
		if (FAILED(hr))
		{
			return false;
		}

		m_FenceCounter[m_FrameIndex]++;

		//�C�x���g�̐���
		m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_FenceEvent == nullptr)
		{
			return false;
		}
	}

	//�R�}���h���X�g�����
	m_pCmdList->Close();

	return true;
}