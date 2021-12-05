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

//VSInput�ɍ��킹���\����
struct Vertex 
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT4 Color;
};

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

	//DIrect3D12�̏�����
	if (!InitD3D())
	{
		return false;
	}

	if (!OnInit())
	{
		return false;
	}

	//����I��
	return true;
}

//�E�B���h�E�̏���������
bool App::InitWnd() 
{
	//�f�o�b�O���C���̗L����
#if defined(DEBUG) || defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> debug;
		auto hr = D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf()));

		//�f�o�b�O���C����L����
		if (SUCCEEDED(hr))
		{
			debug->EnableDebugLayer();
		}
	}
#endif

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
		IID_PPV_ARGS(m_pDevice.GetAddressOf())
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
		hr = m_pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(m_pQueue.GetAddressOf()));
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
		hr = pFactory->CreateSwapChain(m_pQueue.Get(), &desc, &pSwapChain);
		if (FAILED(hr))
		{
			SafeRelease(pFactory);
			return false;
		}

		//��������_�u���o�b�t�@�����O���s���ۂ̃o�b�N�o�b�t�@�ԍ����擾

		//IDXGISwapChain3���擾
		hr = pSwapChain->QueryInterface(IID_PPV_ARGS(m_pSwapChain.GetAddressOf()));
		if (FAILED(hr))
		{
			SafeRelease(pFactory);
			SafeRelease(pSwapChain);
			return false;
		}


		//�o�b�N�o�b�t�@�ԍ����擾
		//�^��: �o�b�N�o�b�t�@�̓X���b�v�����ŏ�ɓ���ւ��̂ɂ����Ō��ߑł��Ŏ擾���Ă��̂܂܂ŗǂ��̂��H,A: ���̎��_�ł̃o�b�N�o�b�t�@�̔ԍ����Ă���
		m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

		//�s�v�ɂȂ����̂ŊJ��
		SafeRelease(pFactory);
		SafeRelease(pSwapChain);
	}

	//�R�}���h�A���P�[�^�̐���
	/*
	�R�}���h���X�g���g�p���郁�������������邽�߂̂���
	*/
	{
		//�t���[���o�b�t�@���Ƃɍ쐬
		for (auto i = 0u; i < FrameCount; ++i) 
		{
			hr = m_pDevice->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT, //�R�}���h�L���[�ɒ��ړo�^�\�ȃR�}���h�݂̂�������
				IID_PPV_ARGS(m_pCmdAllocator[i].GetAddressOf()));
			if (FAILED(hr))
			{
				return false;
			}
		}
	}

	//�R�}���h���X�g�̐���
	{
		hr = m_pDevice->CreateCommandList(
			0, //nodeMask: ������GPU�m�[�h������ꍇ�ɁA�m�[�h�����ʂ��邽�߂̃r�b�g�}�X�N��ݒ肵�܂�
			D3D12_COMMAND_LIST_TYPE_DIRECT, //type: �쐬����R�}���h���X�g�̃^�C�v���w��A�R�}���h�L���[�ɒ��ړo�^�\�ȃR�}���h�݂̂�������
			m_pCmdAllocator[m_FrameIndex].Get(), //pCommandAllocator: �R�}���h���X�g���쐬����Ƃ��̃R�}���h�A���P�[�^���w��A2���邤���̕`��R�}���h����ł����̂Ɏg�p����̂̓o�b�N�o�b�t�@�̔ԍ��ɑΉ�������̂Ȃ̂ŃC���f�b�N�X��m_FrameIndex���w��
			nullptr, //pInitialState: �p�C�v���C���X�e�[�g���w��A���̈����̓I�v�V�����Ȃ̂�nullptr�ł��A���ƂŖ����I�Ɏw�肷�邽�߂ɂ����ł͐ݒ肵�Ȃ��B
			IID_PPV_ARGS(&m_pCmdList)
		);
		if (FAILED(hr))
		{
			return false;
		}
	}

	//�����_�[�^�[�Q�b�g�r���[�̐���
	/*
	�����_�[�^�[�Q�b�g�Ƃ͕`��̑Ώۂł���A���̎��Ԃ̓o�b�N�o�b�t�@��e�N�X�`���Ƃ��������\�[�X�ƂȂ�B
	�ȉ����Ԉꗗ
	-
	�o�b�t�@
	�ꎟ���e�N�X�`��
	�ꎟ���e�N�X�`���z��
	�񎟌��e�N�X�`��
	�񎟌��e�N�X�`���z��
	�}���`�T���v���񎟌��e�N�X�`��
	�}���`�T���v���񎟌��e�N�X�`���z��
	�O�����e�N�X�`��
	�L���[�u�}�b�v
	�L���[�u�}�b�v�z��
	-
	�����̃}���`�T���v���Ƃ͊e�s�N�Z���̐F�����肷��ۂɕ����̃T���v���_���g�p���ăs�N�Z���̐F�����肷�鎖�������B�}���`�T���v���̑Δ���V���O���T���v���Ƃ���
	�}���`�T���v���̎g�����̓A���`�G�C���A�X�Ȃ�
	�}���`�T���v���̃f�����b�g�̓V�F�[�_�[�ł̃f�[�^�^�V���O���T���v���ƈقȂ�����t�F�b�`���߂��قȂ����菈�����S��������Ƃ������_������
	*/
	/*
	��������̃��\�[�X�͒P�Ƀ������̈悪�킩���Ă邾���ł��̃o�b�t�@�̋�؂�����񎟌��e�N�X�`���z��Ȃ̂��O�����e�N�X�`���Ȃ̂��킩��Ȃ�
	�����œo�ꂷ��̂����\�[�X�r���[�I�u�W�F�N�g
	�����_�[�^�[�Q�b�g�̏ꍇ�̃��\�[�X�r���[�I�u�W�F�N�g�������_�[�^�[�Q�b�g�r���[�ƂȂ�
	*/
	/*
	�����_�[�^�[�Q�b�g�r���[�����̗���:
	-
	���\�[�X�̐���
	�f�B�X�N���v�^�q�[�v�̐���
	�����_�[�^�[�Q�b�g�r���[�̐���
	-
	����̃��\�[�X�̓o�b�N�o�b�t�@�Ȃ̂Ń��\�[�X�̐����͖����ėǂ�
	*/
	{
		//�f�B�X�N���v�^�q�[�v�̐ݒ�
		/*
		�f�B�X�N���v�^�q�[�v�Ƃ̓f�B�X�N���v�^��ۑ����邽�߂̔z��B
		����GPU�ł̃f�B�X�N���v�^�́AGPU��������ɑ��݂���l�X�ȃf�[�^��o�b�t�@�̎�ނ�ʒu�傫����\���\���̂̂悤�Ȃ���
		*/
		//https://cocoa-programing.hatenablog.com/entry/2018/11/21/%E3%80%90DirectX12%E3%80%91%E3%83%87%E3%82%B9%E3%82%AF%E3%83%AA%E3%83%97%E3%82%BF%E3%83%92%E3%83%BC%E3%83%97%E3%81%AE%E4%BD%9C%E6%88%90%E3%80%90%E5%88%9D%E6%9C%9F%E5%8C%96%E3%80%91
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = FrameCount; //�q�[�v���̃f�B�X�N���v�^��
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; //�f�B�X�N���v�^�q�[�v�̃^�C�v
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE; //�V�F�[�_������Q�Ƃł��邩�ǂ����A����͂Ȃ�
		desc.NodeMask = 0; //������GPU�m�[�h������ꍇ�Ƀm�[�h����肷�邽�߂̃r�b�g�}�X�N��ݒ�

		//�f�B�X�N���v�^�q�[�v�𐶐�
		hr = m_pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_pHeapRTV.GetAddressOf()));
		if (FAILED(hr))
		{
			return false;
		}

		//�f�B�X�N���v�^�q�[�v�擪�Ɋi�[����Ă���CPU�f�B�X�N���v�^�n���h�����擾
		auto handle = m_pHeapRTV->GetCPUDescriptorHandleForHeapStart();
		//�f�B�X�N���v�^����̏ꍇ�͂��̂܂܎g���Ηǂ����A�Q�ȏ゠��ꍇ�͐擪�A�h���X����ǂꂭ�炢���点�΂悢���킩��Ȃ��̂ł����Ŏ擾����
		//���̒l�̓f�o�C�X�ˑ��Ȃ̂�GPU���ƂɎ擾����K�v������
		auto incrementSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		for (auto i = 0u; i < FrameCount; ++i)
		{
			//�����_�[�^�[�Q�b�g�r���[�̐����ɂ͂ǂ̃��\�[�X���g�p����̂���n���ďグ��K�v������̂Ńo�b�t�@���擾
			hr = m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(m_pColorBuffer[i].GetAddressOf()));
			if (FAILED(hr))
			{
				return false;
			}

			//�����_�[�^�[�Q�b�g�r���[�̐����ɕK�v�Ȑݒ�p�̍\����
			D3D12_RENDER_TARGET_VIEW_DESC viewDesc = {};
			viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; //��ʂŌ���Ƃ��̃s�N�Z���t�H�[�}�b�g���w��
			viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; //�ǂ̂悤�Ƀ����_�[�^�[�Q�b�g�̃��\�[�X�ɃA�N�Z�X���邩���w��
			//����͓񎟌��e�N�X�`���Ȃ̂œ񎟌��e�N�X�`���p�̐ݒ�
			viewDesc.Texture2D.MipSlice = 0; //����̓X���b�v�`�F�C���𐧍삷��Ƃ��Ƀ~�b�v�}�b�v���x����������Ȃ��悤�ɐݒ肵���̂�0�ɐݒ�
			viewDesc.Texture2D.PlaneSlice = 0; //�e�N�X�`���̕��ʃX���C�X�ԍ����w��A����͕��ʂ𕡐������f�[�^�ł͂Ȃ��̂�0���w��

			//�����_�[�^�[�Q�b�g�r���[�̐���
			m_pDevice->CreateRenderTargetView(m_pColorBuffer[i].Get(), &viewDesc, handle);

			m_HandleRTV[i] = handle;
			//�|�C���^�����炷
			handle.ptr += incrementSize;
		}
	}

	//�t�F���X�̐���
	/*
	�t�F���X�I�u�W�F�N�g�Ƃ͓������Ƃ邽�߂̂���
	�t�F���X���̂̒l���C���N�������g���ꂽ���ǂ����ŕ`�悪�������������m�F�����������
	*/
	{
		//�t�F���X�J�E���^�[�����Z�b�g
		for (auto i = 0u; i < FrameCount; ++i)
		{
			m_FenceCounter[i] = 0;
		}

		//�t�F���X�̐���
		hr = m_pDevice->CreateFence(
			m_FenceCounter[m_FrameIndex], //�t�F���X�̏������ɗp����l
			D3D12_FENCE_FLAG_NONE, //�t���O�I�v�V�����A�t�F���X�����L���邩�ǂ����Ȃ�
			IID_PPV_ARGS(m_pFence.GetAddressOf()));
		if (FAILED(hr))
		{
			return false;
		}

		m_FenceCounter[m_FrameIndex]++;

		//�C�x���g�̐���
		//�I���܂ő҂Ƃ���������OS�̋@�\���g�p���čs��
		m_FenceEvent = CreateEvent(
			nullptr, //lpEventAttributes: �q�v���Z�X���擾�����n���h�����p���ł��邩�ǂ��������肷��\���̂ւ̃|�C���^���w��
			FALSE, //bManualReset: �蓮�̃��Z�b�g�I�u�W�F�N�g���쐬���邩�ǂ���
			FALSE, //bInitialState: �C�x���g�I�u�W�F�N�g�̏�����Ԃ̎w��ATRUE���w�肷��ƃV�O�i������
			nullptr //lpName: �C�x���g�I�u�W�F�N�g�̖��O��NULL�I�[������Ŏw��	
		);
		if (m_FenceEvent == nullptr)
		{
			return false;
		}
	}

	//�R�}���h���X�g�����
	m_pCmdList->Close();

	return true;
}

//�`�揈��
void App::Render()
{
	//�R�}���h�̋L�^���J�n

	//�R�}���h�o�b�t�@�̓��e��퓬�ɖ߂�
	m_pCmdAllocator[m_FrameIndex]->Reset();
	//�R�}���h���X�g�̏����������A��Q�����̓p�C�v���C���A���P�[�^��������͎g�p���Ȃ�
	m_pCmdList->Reset(m_pCmdAllocator[m_FrameIndex].Get(), nullptr);

	//���\�[�X�o���A�̐ݒ�
	/*
	DX12�̓}���`�X���b�h�Ή������Ă��邽��GPU���\�����Ă���̈���㏑�����ĕ\��������Ă��܂��Ȃǂ̊��荞�ݏ�����h���@�\
	�����ł����J�ڂƂ̓��\�[�X(�t���[���o�b�t�@(?))�ɑ΂��ď������ݏ�Ԃ��\����Ԃ̂ǂ�����ɑJ�ڂ��邱��
	�T�u���\�[�X�Ƃ̓~�b�v�}�b�v��L���[�[���}�b�v�Ȃǂ̃e�N�X�`���̍\���P��
	*/
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; //���\�[�X�o���A�̃^�C�v���w��A����͗l�X�Ȏg�p�p�r�̊Ԃ̃T�u���\�[�X�Z�b�g�̑J�ڂ������o���A
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE; //�t���O�̐ݒ�
	//Transition�̓T�u���\�[�X���قȂ�g�p�p�r�̏ꍇ�ɐݒ肷��
	barrier.Transition.pResource = m_pColorBuffer[m_FrameIndex].Get(); //�J�ڂɎg�p���邽�߂̃��\�[�X�̃|�C���^���w��
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; //�T�u���\�[�X�̎g�p�O�̏�Ԃ��w��
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; //�T�u���\�[�X�̎g�p��̏�Ԃ��w��
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; //�J�ڂ̂��߂̃T�u���\�[�X�̔ԍ����w��A����̏ꍇ���Ƃ��ׂẴT�u���\�[�X��J�ڂ�����

	//���\�[�X�o���A
	m_pCmdList->ResourceBarrier(1, &barrier);

	//�������ޏ�������

	//�����_�[�^�[�Q�b�g�̐ݒ�
	m_pCmdList->OMSetRenderTargets(
		1, //NumRenderTargetDescripter: �ݒ肷�郌���_�[�^�[�Q�b�g�r���[�p�̃f�B�X�N���v�^�n���h���̌���ݒ�
		&m_HandleRTV[m_FrameIndex], //pRenderTargetDescriptors: �ݒ肷�郌���_�[�^�[�Q�b�g�r���[�̃n���h���̔z����w��
		FALSE, //RTSingleHandleToDescriptorRange: �ݒ肷��f�B�X�N���v�^�n���h���̔z��͈̔͂�P�ƂƂ��邩�ǂ����w��B�Ⴆ��RTV��3�ݒ肷��ꍇTRUE�ɂ����3��RTV�ɑ΂��ē����f�B�X�N���v�^�n���h�����ݒ肳���
		nullptr //pDepthStencilDescriptor: �[�x�X�e���V���r���[�f�B�X�N���v�^�̐ݒ�
	);

	//�N���A�J���[�̐ݒ�, �w�i�F
	float clearColor[] = { 0.25f, 0.25f, 0.25f, 1.0f };

	//�����_�[�^�[�Q�b�g�r���[���N���A
	m_pCmdList->ClearRenderTargetView(
		m_HandleRTV[m_FrameIndex], //RenderTargetView: �����_�[�^�[�Q�b�g�r���[���N���A���邽�߂̃f�B�X�N���v�^�n���h�����w��
		clearColor, //ColorRGBA: �����_�[�^�[�Q�b�g���N���A���邽�߂̐F���w��
		0, //NumRects: �ݒ肷���`�̐����w��
		nullptr //pRects: �����_�[�^�[�Q�b�g���N���A���邽�߂̋�`�̔z����w��
	);

	//�X�V����
	{
		m_RotateAngle += 0.025f;
		m_CBV[m_FrameIndex].pBuffer->World = DirectX::XMMatrixRotationY(m_RotateAngle);
	}

	//�`�揈��
	{
		//���[�g�V�O�j�`����ݒ�
		m_pCmdList->SetGraphicsRootSignature(m_pRootSignature.Get());
		//�萔�q�[�v�f�B�X�N���v�^��ݒ�
		m_pCmdList->SetDescriptorHeaps(1, m_pHeapCBV.GetAddressOf());
		//�萔�o�b�t�@�r���[�Ŏg�p����V�F�[�_�̃��W�X�^�ԍ���GPU���z�A�h���X��ݒ�
		m_pCmdList->SetGraphicsRootConstantBufferView(0, m_CBV[m_FrameIndex].Desc.BufferLocation);
		//�p�C�v���C���X�e�[�g��ݒ�
		m_pCmdList->SetPipelineState(m_pPS0.Get());

		//�O�p�`���v���~�e�B�u�Ƃ��Đݒ�
		m_pCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//GPU�X���b�g�A���A���_�o�b�t�@�r���[��ݒ�
		m_pCmdList->IASetVertexBuffers(0, 1, &m_VBV);
		//�����_�[�^�[�Q�b�g�ւ̕`��̈��ݒ�
		m_pCmdList->RSSetViewports(1, &m_Viewport);
		m_pCmdList->RSSetScissorRects(1, &m_Scissor);

		m_pCmdList->DrawInstanced(3, 1, 0, 0);
	}

	//���\�[�X�o���A�̐ݒ�
	/*
	���������_�[�^�[�Q�b�g�ɏ������ޕK�v�͂Ȃ��̂ŏ�L�Ɠ����悤�ɏ�Ԃ�"�\������"�Ƀ��\�[�X�o���A�����Ƃɖ߂�
	*/
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_pColorBuffer[m_FrameIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	//���\�[�X�o���A
	m_pCmdList->ResourceBarrier(1, &barrier);

	//�R�}���h�̋L�^���I��
	m_pCmdList->Close();

	//�R�}���h�����s
	//�쐬�����R�}���h���X�g�͉������Ȃ���Ύ��s����Ȃ��̂ł����ŃR�}���h�L���[�ɓn���Ă�����
	ID3D12CommandList* ppCmdLists[] = { m_pCmdList.Get() };
	m_pQueue->ExecuteCommandLists(1, ppCmdLists);

	//��ʂɕ\��
	Present(1);
}

//��ʂɕ\�����A���t���[���̏������s��
/*
��̓I�ɂ̓o�b�N�o�b�t�@���f�B�X�v���C�֕\��������A�o�b�N�o�b�t�@�̓���ւ�������s��
*/
void App::Present(uint32_t interval)
{
	//��ʂɕ\��
	/*
	�t�����g�o�b�t�@����ʂɕ\�����ăo�b�N�o�b�t�@�̓���ւ�(�X���b�v)����
	*/
	m_pSwapChain->Present(
		//SyncInterval: �f�B�X�v���C�̐��������ƃt���[���̕\���𓯊�������@���w��
		/*
		0���w�肵���ꍇ�A�����ɓ����͍s���Ȃ�
		1���w�肵���ꍇ�A1��ڂ̐���������ɕ\���𓯊�����B�f�B�X�v���C�̍X�V������60Hz�̏ꍇ60fpss�ƂȂ�
		2���w�肵���ꍇ�A����������2����s����ĕ\�����邱�Ƃ��Ӗ�����
		3���w�肵���ꍇ�A����������3����s����ĕ\�����邱�Ƃ�����
		*/
		interval, 
		0 //Flags: �X���b�v�`�F�C���̕\���I�v�V�������w�肷��B
	);

	//�V�O�i������
	/*
	��������邱�ƂŃR�}���h�L���[�̎��s�����������Ƃ��Ƀt�F���X�̒l���X�V�����悤�ɂȂ�
	�t�F���X�̒l���X�V���ꂽ��GPU��ł̃R�}���h�̎��s�������������̂Ƃ��Ĕ��f�ł���悤�ɂȂ�
	*/
	const auto currentValue = m_FenceCounter[m_FrameIndex];
	m_pQueue->Signal(
		m_pFence.Get(), //pFence: �t�F���X�I�u�W�F�N�g�̃|�C���^ 
		currentValue //�t�F���X�ɐݒ肷��l
	);

	//�o�b�N�o�b�t�@�ԍ����X�V
	/*
	�X���b�v�������������Ƃɂ���ăo�b�N�o�b�t�@���ς�����̂ŃC���f�b�N�X���X�V
	*/
	m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	//���̃t���[���̕`�揀�����܂��ł���Αҋ@����
	/*
	�l���X�V����ĂȂ����菬�����Ȃ�
	*/
	if (m_pFence->GetCompletedValue() < m_FenceCounter[m_FrameIndex])
	{
		//�������Ă��Ȃ������̂ł��̊֐��Ńt�F���X�̒l���w��l�ɂȂ�����C�x���g���N�����Ƃ����ݒ���s��
		m_pFence->SetEventOnCompletion(
			m_FenceCounter[m_FrameIndex], //Value: �C�x���g���V�O�i����ԂɂȂ������ɐݒ肷��t�F���X�̒l 
			m_FenceEvent //�C�x���g�I�u�W�F�N�g�ւ̃n���h��
		);
		//�����Ŏ��ۂɃC�x���g���N����܂őҋ@
		WaitForSingleObjectEx(
			m_FenceEvent, //hHandle: �I�u�W�F�N�g�̃n���h�����w�� 
			INFINITE, //dwMilliseconds: �A�E�g�^�C������
			FALSE //bAlertable: I/O�������[�`����񓯊��v���V�[�W���Ăяo�����L���[�ɂ������������Ԃ����ǂ������w��
		);
	}

	//���t���[���̃t�F���X�J�E���^�[�𑝂₷
	m_FenceCounter[m_FrameIndex] = currentValue + 1;
}

/*
�I���������s���O��GPU�̎��s���ɕK�v�ȃI�u�W�F�N�g���J�����Ă��܂��ƁA�F�X�悭�Ȃ��̂ł����ł��ׂďI������܂őҋ@
*/
void App::WaitGpu()
{
	assert(m_pQueue != nullptr);
	assert(m_pFence != nullptr);
	assert(m_FenceEvent != nullptr);

	//�V�O�i������
	m_pQueue->Signal(m_pFence.Get(), m_FenceCounter[m_FrameIndex]);

	//�������ɃC�x���g��ݒ肷��..
	m_pFence->SetEventOnCompletion(m_FenceCounter[m_FrameIndex], m_FenceEvent);

	//�ҋ@����
	WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);

	//�J�E���^�[�𑝂₷
	m_FenceCounter[m_FrameIndex]++;
}

/*
�I������
�F��Ȃ��̂�������Ă���
*/
void App::TermD3D()
{
	//GPU�����̊�����ҋ@
	WaitGpu();

	//�C�x���g�j��
	if (m_FenceEvent != nullptr)
	{
		CloseHandle(m_FenceEvent);
		m_FenceEvent = nullptr;
	}

	//�t�F���X�j��
	m_pFence.Reset();

	//�����_�[�^�[�Q�b�g�r���[�̔j��
	m_pHeapRTV.Reset();
	for (auto i = 0u; i < FrameCount; ++i)
	{
		m_pColorBuffer[i].Reset();
	}

	//�R�}���h���X�g�̔j��
	m_pCmdList.Reset();

	//�R�}���h�A���P�[�^�̔j��
	for (auto i = 0u; i < FrameCount; ++i)
	{
		m_pCmdAllocator[i].Reset();
	}

	//�X���b�v�`�F�C���̔j��
	m_pSwapChain.Reset();

	//�R�}���h�L���[�̔j��
	m_pQueue.Reset();

	//�f�o�C�X�̔j��
	m_pDevice.Reset();
}

bool App::OnInit()
{
	//���_�o�b�t�@�̐���
	/*
	���_�o�b�t�@�͒��_�f�[�^��Z�߂đ��邽�߂ɍ쐬����I�u�W�F�N�g�B
	
	���_�o�b�t�@����邽�߂ɂ�
	ID3D12Resource�I�u�W�F�N�g�𐶐�����
	GPU�̉��z�A�h���X�A���_�S�̂̃f�[�^�T�C�Y�A1���_������̃f�[�^�T�C�Y�����ꂼ��D3D12_VERTEX_BUFFER_VIEW�\���̂ɐݒ�
	�����Ď��ۂ�VRAM�Ƀ}�b�s���O���������Ƃ�
	�����Ē��_�o�b�t�@�r���[�𐶐�
	*/
	{
		//���_�f�[�^
		//����͎O�p�`
		//XMFLOAT3��Position, XMFLOAT4��Color
		Vertex vertices[] = {
			{ DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
			{ DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
			{ DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		};

		//�q�[�v�v���p�e�B
		D3D12_HEAP_PROPERTIES prop = {};
		//�q�[�v�^�C�v�̎w��
		/*
		D3D12_HEAP_TYPE_UPLOAD��CPU�ւ̏������݂���x��GPU�ǂݍ��݂���x�̃f�[�^�ɓK���Ă���
		*/
		prop.Type = D3D12_HEAP_TYPE_UPLOAD;
		//�q�[�v��GPU�y�[�W�v���p�e�B���w��
		//�^��: �{�ł�GPU�ɂȂ��Ă�����CPU�y�[�W�v���p�e�B�ł́H
		/*
		����̒��_�f�[�^��1�񂵂��������݂��Ȃ��̂ŁA����CPU�y�[�W�v���p�e�B���C�ɂ���K�v�͂Ȃ�
		���C�g�R���o�C���͏������ݎw����ꎞ�~�ς��Ă����Ă���K�؂ȃ^�C�~���O�ł܂Ƃ߂ď����o�����@
		���C�g�o�b�N�͍����ȃL���b�V���������ɏ������݂��s���Ă����ACPU�̋󂫎��ԓ��𗘗p���ăL���b�V�����������烁�C���������ɏ������ޕ���
		*/
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		//�q�[�v�ɑ΂��郁�����v�[���̎w��
		/*
		D3D12_MEMORY_POOL_L0�̓V�X�e����������CPU�ɑ΂���ш敝�͑傫����GPU�ш敝�͏��Ȃ�
		D3D12_MEMORY_POOL_L1��VRAM��GPU�ɑ΂���ш敝�͑傫����CPU����A�N�Z�X�ł��Ȃ��A�܂�UMA(���j�t�@�C�h�������A�[�L�e�N�`��)�ł͂���͎g�p�ł��Ȃ�
		���̃T���v���ł̓��������ӎ����Ȃ��Ă��ǂ��̂�UNKNOWN���w�肵�Ă���
		*/
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		prop.CreationNodeMask = 1; //����GPU�ɂ��Đ��������ׂ����\�[�X�̏ꏊ�̃m�[�h���w�肷��B�����ŒP��GPU�ɂ�铮���O��Ƃ��Ă���̂�1���w��
		prop.VisibleNodeMask = 1; //����GPU�ɂ��ă��\�[�X�����ł���ꏊ�̃m�[�h�̏W�����w��B�����ŒP��GPU�ɂ�铮���O��Ƃ��Ă���̂�1���w��

		//���\�[�X�̐ݒ�
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER; //���\�[�X�̎����������ŗ�(�o�b�t�@��ꎟ���e�N�X�`���Ȃ�)
		desc.Alignment = 0; //���\�[�X�̃A���C�����g���w��, ���\�[�X�̎�����D3D12_RESOURCE_DIMENSION_BUFFER�̏ꍇD3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT(64kb)��0���w�肷��
		desc.Width = sizeof(vertices); //�o�b�t�@�T�C�Y�̎w��, �����ł͒��_�o�b�t�@�̃T�C�Y�ł���sizeof(vertices)��ݒ�
		desc.Height = 1; //�o�b�t�@�̏ꍇ��1, �e�N�X�`���̏ꍇ�͏c�����w��
		desc.DepthOrArraySize = 1; //�o�b�t�@�̏ꍇ��1, �O�����e�N�X�`���̏ꍇ�͉��s�����e�N�X�`���z��̏ꍇ�͔z�񐔂��w�肷��
		desc.MipLevels = 1; //�o�b�t�@�̏ꍇ��1�A�e�N�X�`���̏ꍇ�̓~�b�v�}�b�v���x�������w�肷��
		desc.Format = DXGI_FORMAT_UNKNOWN; //�o�b�t�@�̏ꍇ��DXGI_FORMAT_UNKNOWN���e�N�X�`���̏ꍇ�̓s�N�Z���t�H�[�}�b�g���w�肷��
		//�e�N�X�`���̏ꍇ�̓}���`�T���v���̐ݒ������
		desc.SampleDesc.Count = 1; //�o�b�t�@�̏ꍇ��1
		desc.SampleDesc.Quality = 0; //�o�b�t�@�̏ꍇ��0
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR; //�e�N�X�`�����C�A�E�g���w��
		desc.Flags = D3D12_RESOURCE_FLAG_NONE; //�r�b�g�_���a�Ńt���O���w��

		//���\�[�X�𐶐�
		//���_�o�b�t�@�Ɏg�p���郊�\�[�X�̐���
		auto hr = m_pDevice->CreateCommittedResource(
			&prop, //D3D12_HEAP_PROPERTIES�̍\���̂ւ̃|�C���^��ݒ�
			D3D12_HEAP_FLAG_NONE, //�q�[�v�I�v�V�������r�b�g�_���a�Ƃ��Đݒ肷��
			&desc, //D3D12_RESOURCE_DESC�̍\���̂ւ̃|�C���^��ݒ�
			D3D12_RESOURCE_STATE_GENERIC_READ, //���\�[�X�̏�����Ԃ��w��, �����HEAP_PROPERTIES��type��D3D12_HEAP_TYPE_UPLOAD�ɂ����ꍇ�͕K��D3D12_RESOURCE_STATE_GENERIC_READ��ݒ肵�Ȃ���΂����Ȃ�
			nullptr, //D3D12_CLEAR_VALUE�\���̂ւ̃|�C���^���w�肵�āA�N���A�l��ݒ肷��B���̈����̓����_�[�^�[�Q�b�g�Ɛ[�x�X�e���V���e�N�X�`���̂��߂̃I�v�V�����BD3D12_RESOURCE_DIMENSION_BUFFER���w�肵���ꍇ��nullptr���w�肷��K�v������
			IID_PPV_ARGS(m_pVB.GetAddressOf())
		);

		if (FAILED(hr))
		{
			return false;
		}

		//�}�b�s���O����(���_�f�[�^�̏�������)
		void* ptr = nullptr;
		hr = m_pVB->Map(
			//�T�u���\�[�X�̔ԍ����w��
			/*
			�T�u���\�[�X�Ƃ̓e�N�X�`���̃~�b�v�}�b�v�f�[�^�̂܂Ƃ܂�̂���
			*/
			0, 
			nullptr, //����͒��_�o�b�t�@�̑S�̈���}�b�s���O����̂ŁAnullptr���w�肷��
			&ptr //���\�[�X�f�[�^�ւ̃|�C���^���󂯎�郁�����u���b�N�ւ̃|�C���^���w��
		);
		if (FAILED(hr))
		{
			return false;
		}
		
		//���_�f�[�^���}�b�s���O��ɐݒ�
		memcpy(ptr, vertices, sizeof(vertices));

		//�}�b�s���O����
		m_pVB->Unmap(
			0, //�T�u���\�[�X�ԍ�
			nullptr //CPU���ύX�����͈͂��w��
		);

		//���_�o�b�t�@�r���[�̐ݒ�
		/*
		���_�o�b�t�@�r���[�͕`��R�}���h���쐬����ۂɎg�p����\����
		*/
		m_VBV.BufferLocation = m_pVB->GetGPUVirtualAddress(); //GPU�̉��z�A�h���X���w��, 
		m_VBV.SizeInBytes = static_cast<UINT>(sizeof(vertices)); //���_�o�b�t�@�S�̂̃T�C�Y��ݒ肷��
		m_VBV.StrideInBytes = static_cast<UINT>(sizeof(Vertex)); //1���_������̃T�C�Y��ݒ�
	}
	//���_�o�b�t�@�̓f�B�X�N���v�^�q�[�v�ł��������Ȃ��̂��H

	//�萔�o�b�t�@�p�f�B�X�N���v�^�q�[�v�̐���
	/*
	�萔�o�b�t�@�̓V�F�[�_���Ōv�Z�Ɏg���萔�l

	���_�o�b�t�@�Ɠ��l�ɒ�`��p�ӂ��āA���̌�Ƀ��\�[�X�������s���A�萔�o�b�t�@�r���[���쐬����
	�萔�o�b�t�@�̍쐬�ł�CPU���Ńf�[�^�ύX������\������r�I�ɍ����AGPU�̏������Ƀo�b�t�@�����������Ă��܂��Ƒz�肵�Ă��Ȃ�����ɂȂ鋰�ꂪ����B����ă����_�[�^�[�Q�b�g�Ɠ��l�ɒ萔�̓_�u���o�b�t�@�����Ă����Ɨǂ�
	*/
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		//�^��: �萔�o�b�t�@���_�u���o�b�t�@������̂�FrameCount�Ɉˑ�����͉̂����Ⴄ�悤��
		/*
		1�̐}�`��`�����߂ɕK�v�ȑ�����"1 * FrameCount"�ł���̂�(?)
		*/
		desc.NumDescriptors = 1 * FrameCount;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; //�����_�[�^�[�Q�b�g�̂Ƃ��̓V�F�[�_���猩����K�v���Ȃ�����HEAP_NONE���w�肵�Ă���������͒��_�V�F�[�_�[���Ŏg�p���邽�߂�����w��
		desc.NodeMask = 0; //GPU����ł����0���w��B������GPU�m�[�h�����݂���ꍇ��GPU�m�[�h�����ʂ��邽�߂̃r�b�g���w��

		//�f�B�X�N���v�^�q�[�v����
		auto hr = m_pDevice->CreateDescriptorHeap(
			&desc,
			IID_PPV_ARGS(m_pHeapCBV.GetAddressOf())
		);

		if (FAILED(hr))
		{
			return false;
		}
	}

	//�萔�o�b�t�@�̐���
	{
		//�q�[�v�v���p�e�B
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_UPLOAD; //���_�V�F�[�_�Ŏg�p����̂�Type��D3D12_HEAP_TYPE_UPLOAD��
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN; //�����͓��ɂ�����肪�Ȃ��̂�UNKNOWN
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN; //�����͓��ɂ�����肪�Ȃ��̂�UNKNOWN
		prop.CreationNodeMask = 1;
		prop.VisibleNodeMask = 1;

		//���\�[�X�̐ݒ�
		//�o�b�t�@�̃T�C�Y�ȊO�͊�{�I�ɒ��_�o�b�t�@�Ɠ���
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

		//�n���h�����ǂꂭ�炢�����߂邩��\��
		/*
		CBV: Constant Buffer View
		SRV: Shader Resource View
		UAV: Unordered Access View
		*/
		auto incrementSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		//�萔�o�b�t�@�r���[�͍���̓_�u���o�b�t�@�Ȃ̂ł��̕��K�v�ƂȂ�
		for (auto i = 0; i < FrameCount; ++i)
		{
			//���\�[�X�̐���
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

			//GPU�̉��z�A�h���X���擾
			auto address = m_pCB[i]->GetGPUVirtualAddress();
			//���̂Q�̓f�B�X�N���v�^�q�[�v�̐擪�n���h�����擾����
			auto handleCPU = m_pHeapCBV->GetCPUDescriptorHandleForHeapStart();
			auto handleGPU = m_pHeapCBV->GetGPUDescriptorHandleForHeapStart();

			//�n���h�����ړ�
			handleCPU.ptr = incrementSize * i;
			handleGPU.ptr = incrementSize * i;

			//�萔�o�b�t�@�r���[�̐ݒ�
			m_CBV[i].HandleCPU = handleCPU;
			m_CBV[i].HandleGPU = handleGPU;
			m_CBV[i].Desc.BufferLocation = address; //GPU�̉��z�A�h���X
			m_CBV[i].Desc.SizeInBytes = sizeof(Transform); //�萔�o�b�t�@�̃T�C�Y���w��, ������alignment��256byte�ɂ��Ȃ���΂����Ȃ�

			//�萔�o�b�t�@�r���[�𐶐�
			//��Q������D3D12_CPU_DESCRIPTOR_HANDLE�^���w��A���̒l�̓f�B�X�N���v�^�q�[�v����擾�\
			m_pDevice->CreateConstantBufferView(&m_CBV[i].Desc, handleCPU);

			//�}�b�s���O
			//Map()���\�b�h�̓|�C���^���擾���邱�Ƃ��ł���
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

			//�ϊ��s��̐ݒ�
			m_CBV[i].pBuffer->World = DirectX::XMMatrixIdentity();
			//�J��������̊����쐬����̂Ɏg�p
			m_CBV[i].pBuffer->View = DirectX::XMMatrixLookAtRH(eyePos, targetPos, upward);
			//near clip�̑傫���Ɏˉe�ϊ��s�񂪈ˑ�����
			m_CBV[i].pBuffer->Proj = DirectX::XMMatrixPerspectiveFovRH(fovY, aspect, 1.0f, 1000.0f);
		}
	}

	//���[�g�V�O�j�`���̐���
	/*
	���[�g�V�O�j�`���͒萔�o�b�t�@��e�N�X�`���A�T���v���[�Ƃ������V�F�[�_�[���Ŏg�p���郊�\�[�X�̃��C�A�E�g�����߂�I�u�W�F�N�g
	�ǂ̃V�F�[�_�[�̃��W�X�^�ɂǂ̃��\�[�X���֘A�t���邩�����܂�
	���W�X�^�͒萔�o�b�t�@���`����Ƃ��Ɍ��߂�
	*/
	{
		//�A�v���P�[�V���������̓A�Z���u�����g�p���邱�Ƃ�����
		//���̃t���O���Ȃ��ƃ��[�g�V�O�j�`���̍œK�������K�͂ɂȂ��Ă��܂�
		auto flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		//�h���C���V�F�[�_�̃��[�g�V�O�j�`���ւ̃A�N�Z�X�����ۂ���
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
		//�n���V�F�[�_�̃��[�g�V�O�j�`���ւ̃A�N�Z�X�����ۂ���
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
		//�W�I���g���V�F�[�_�[�̃��[�g�V�O�j�`���ւ̃A�N�Z�X�����ۂ���
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

		//���[�g�p�����[�^�̐ݒ�
		D3D12_ROOT_PARAMETER param = {};
		//���[�g�p�����[�^�̎w��
		/*
		���[�g�p�����[�^��
		--
		���[�g�萔: 32bit�f�[�^�����������ꍇ��32bit�f�[�^�̉���g�������ꍇ
		�萔�o�b�t�@�r���[: ���[�g�萔�ɔ�ׂ�Ƒ傫�߂̃f�[�^�A���邢�͍\���̂̂悤�Ƀf�[�^��Z�߂Ďg�������ꍇ
		�V�F�[�_���\�[�X�r���[: �e�N�X�`���Ƃ��Ĉ��������ꍇ
		�A���I�[�_�[�h�A�N�Z�X�r���[: �R���s���[�g�V�F�[�_�ȂǂŁA�f�[�^�̓ǂ�/�������s���o�b�t�@�Ƃ��Ďg�������ꍇ
		�f�B�X�N���v�^�e�[�u��: �萔�o�b�t�@�r���[�A�V�F�[�_���\�[�X�r���[�A�A���I�[�_�[�h�A�N�Z�X�r���[�A�T���v���[��4��ނ̃��[�g�p�����^���܂Ƃ߂Ĉ��������ꍇ
		--
		������A�����͂��ꂼ��Ή�����\���̂����݂���
		���̂��ꂼ��̍\���̂ɏo�Ă��郁���o�ϐ��̖��O�̈Ӗ���
		ShaderRegister: �V�F�[�_�̃��W�X�^�ԍ����w��B�Ⴆ��b1���W�X�^�Ȃ�1���w��
		RegisterSpace: �V�F�[�_�̘_���I�ȃ��W�X�^��Ԃ��w��
		Num32BitValue: ���[�g�萔�Ŏg�p�A�P��̃V�F�[�_�[�X���b�g�����Ă�萔�̐����w��B�Ⴆ��float3�Ȃ�3���w��
		�����̓��[�g�萔�ACBV�ASRV�AUAV�Ŏg�p����A
		�f�B�X�N���v�^�e�[�u����pDescriptorRanges�Ƃ���D3D12_DESCRIPTOR_RANGE�^�̔z��Ƃ��̔z��̑傫�������߂�NumRangeDescriptorRanges�Ƃ���UINT�^�̃p�^���[�^
		D3D12_DESCRIPTOR_RANGE��
		--
		RangeType: D3D12_DESCRIPTOR_RANGE_TYPE�Ŏw��BSRV��UAV��CBV���T���v���[�̂ǂ���g�p���邩�����߂�
		NumDescriptors: �f�B�X�N���v�^�̐����w��A-1�܂���UINT_MAX���w�肷��Ɨ̈斳���ƂȂ�
		RegisterSpace: ���W�X�^��Ԃ��w��
		OffsetInDescriptorsFromTableStart: ���[�g�V�O�j�`���̐擪����̃f�B�X�N���v�^�̃I�t�Z�b�g���w��A�f�[�^���A��������є�тɂȂ�ꍇ�̓I�t�Z�b�g���w��
		--
		*/
		/*
		��{�I�Ȏg�������Ƃ��Ă�
		10�ȏ��ݒ肷��K�v������ꍇ�̓f�B�X�N���v�^�e�[�u���ł���ȊO�̓��[�g�p�����[�^�̃^�C�v�ɉ����Čʐݒ肷��Ɨǂ�
		�萔�o�b�t�@�ɂ��Ă̓T�C�Y�����������̂����[�g�萔�ɂ��āA�傫�����̂�CBV�ɂ���Ƃ����g���������ǂ�
		*/
		//�^��: �T���v���[�Ƃ�
		param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		//�����b0�Ȃ̂�0
		param.Descriptor.ShaderRegister = 0;
		param.Descriptor.RegisterSpace = 0;
		//����͒��_�V�F�[�_�[�ŃA�N�Z�X�ł���Ηǂ��̂�
		param.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

		//���[�g�V�O�j�`���̐ݒ�
		D3D12_ROOT_SIGNATURE_DESC desc = {};
		desc.NumParameters = 1; //���[�g�p�����[�^�̔z��
		desc.NumStaticSamplers = 0; //�X�^�e�B�b�N�T���v���[�̔z��
		desc.pParameters = &param; //���[�g�p�����[�^�̔z��
		desc.pStaticSamplers = nullptr; //�X�^�e�B�b�N�T���v���[�̔z��
		desc.Flags = flag; //�t���O�w��

		//�ȉ��̃V���A���C�Y���ʂ��������܂��
		ComPtr<ID3DBlob> pBlob;
		//�V���A���C�Y�����s�����ꍇ�̓G���[���e���������܂��
		ComPtr<ID3DBlob> pErrorBlob;

		//���[�g�V�O�j�`���̃V���A���C�Y
		auto hr = D3D12SerializeRootSignature(
			&desc,
			D3D_ROOT_SIGNATURE_VERSION_1_0, //���[�g�V�O�j�`���̃o�[�W����
			pBlob.GetAddressOf(),
			pErrorBlob.GetAddressOf()
		);

		if (FAILED(hr))
		{
			return false;
		}

		//���[�g�V�O�j�`���𐶐�
		hr = m_pDevice->CreateRootSignature(
			0, //nodeMask: �P��GPU�̏ꍇ�̓[�����w��
			pBlob->GetBufferPointer(), //pBlobWithRootSignature: �V���A���C�Y�����f�[�^�̃|�C���^���w��
			pBlob->GetBufferSize(), //blobLengthInBytes: �V���A���C�Y�����f�[�^�̃T�C�Y���o�C�g�P�ʂŎw��
			IID_PPV_ARGS(m_pRootSignature.GetAddressOf())
		);

		if (FAILED(hr))
		{
			return false;
		}
	}

	//�p�C�v���C���X�e�[�g�̐���
	/*
	�`��Ɏg�p����V�F�[�_�Ⓒ�_�V�F�[�_�ւ̓��̓f�[�^�A�`��X�e�[�g��ݒ肷�邽�߂̃p�C�v���C���X�e�[�g�𐶐�
	�p�C�v���C���X�e�[�g�͂Q��ނ���A1�͕`��p�Ɏg�p����O���t�B�b�N�X�p�C�v���C���X�e�[�g��
	����1�̓R���s���[�g�V�F�[�_�ɂ��v�Z�p�Ɏg�p����R���s���[�g�p�C�v���C���X�e�[�g
	����͌v�Z�ł͂Ȃ��`��Ɏg�p���邽�߂̃O���t�B�b�N�X�p�C�v���C���X�e�[�g�𐶐�
	*/
	{
		//���̓��C�A�E�g�̐ݒ�
		/*
		���̓��C�A�E�g�͒��_�V�F�[�_�̓��͂��ǂ̂悤�ȃf�[�^�\���ł��邩��ݒ肷�邽�߂̂���
		���_���Ɋ܂܂��1�v�f��D3D12_INPUT_ELEMENT_DESC�Œ�`����B

		*/
		D3D12_INPUT_ELEMENT_DESC elements[2];
		elements[0].SemanticName = "POSITION"; //�Z�}���e�B�N�X�����w��
		//�Z�}���e�B�N�X�ԍ����w��
		/*
		���̔ԍ����K�v�ɂȂ�̂�HLSL���̒�`�ŃZ�}���e�B�N�X����"POSITION0", "POSITION1", "POSITION2"�̂悤�ɂȂ��Ă�����̂ł���A
		���̏ꍇ������0, 1, 2�̂悤�ɃZ�}���e�B�N�X�ԍ����w�肷��
		*/
		elements[0].SemanticIndex = 0;
		elements[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT; //float3�^�Ȃ�DXGI_FORMAT_R32G32B32A32_FLOAT�ƂȂ�
		//���̓X���b�g�ԍ����w��, �L���l��0����15�ł���A����͕����̒��_�o�b�t�@������Ȃ��̂�0���w��
		/*
		�X���b�g��GPU�����_�f�[�^������"�̂�����"�̂悤�Ȃ��̂��ƃC���[�W����Ɨǂ�
		�����̒��_�f�[�^����x�ɗ������݂����Ƃ���
		���W�f�[�^�Ɩ@���A�J���[�̂悤�ɃX���b�g�𕪂��邱�Ƃ�����
		*/
		elements[0].InputSlot = 0; 
		elements[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT; //�e�v�f�̃I�t�Z�b�g���o�C�g�P�ʒP�ʂŎw��
		//�f�[�^�̓��͒P�ʂ��w��
		/*
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA�̏ꍇ�͒��_���Ƃ̓��̓f�[�^�����_�V�F�[�_�ɗ���
		D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA�̏ꍇ�̓C���X�^���X���Ƃ̓��̓f�[�^�����_�V�F�[�_�ɗ���
		*/
		elements[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		elements[0].InstanceDataStepRate = 0; //�C���X�^���X���Ƃ̃f�[�^�̌J��Ԃ��񐔂��w�肵�܂��B2�ȏゾ�Ƃ��̃f�[�^�͂��̐����������f�[�^���g���񂷂��ƂɂȂ�

		//��Ɠ���
		elements[1].SemanticName = "COLOR";
		elements[1].SemanticIndex = 0;
		elements[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		elements[1].InputSlot = 0;
		elements[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		elements[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		elements[1].InstanceDataStepRate = 0;

		//���X�^���C�U�[�X�e�[�g�̐ݒ�
		/*
		���X�^���C�Y���s���ۂ̐ݒ�
		*/
		D3D12_RASTERIZER_DESC descRS;
		descRS.FillMode = D3D12_FILL_MODE_SOLID; //wireframe��`�悷�邩�ǂ����̐ݒ�
		descRS.CullMode = D3D12_CULL_MODE_NONE; //�w�肳�ꂽ�����ɖʂ���O�p�`��`�悵�Ȃ��悤�ɐݒ�
		descRS.FrontCounterClockwise = FALSE; //�S�ʂ������v��肩�ǂ������w��, ���̏ꍇ�͎��v����S�ʂƂ���
		descRS.DepthBias - D3D12_DEFAULT_DEPTH_BIAS; //�s�N�Z���ɉ��Z����[�x�l��ݒ肷��
		descRS.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP; //�s�N�Z���ő�[�x�o�C�A�X�l��ݒ�
		descRS.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS; //�s�N�Z���̌�y�ɉ������[�x�o�C�A�X�l���X�P�[������X�J���[�l��ݒ�
		descRS.DepthClipEnable = FALSE; //�����Ɋ�Â��N���b�s���O�����邩�ǂ���
		descRS.MultisampleEnable = FALSE; //�}���`�T���v�����O�L���ɂ��邩�ǂ������w��
		descRS.AntialiasedLineEnable = FALSE; //���̃A���`�G�C���A�V���O��L���ɂ��邩�ǂ������w��
		descRS.ForcedSampleCount = 0; //UAV�̕`�悩�������̓��X�^���C�Y�̊ԁA�T���v�����������I�ɌŒ�l�ɂ���B0�̏ꍇ�̓T���v��������������Ȃ����Ƃ�����
		//�R���T�o�e�B�u���X�^���C�[�[�V������L���ɂ��邩�ǂ������w��
		/*
		�R���T�o�e�B�u���X�^���C�[�[�V�����͂�����Ƃł��s�N�Z���ɂ��������烉�X�^���C�Y�����s����Ƃ����ێ�I�ȃ��X�^���[�[�V�������s�����߂̃��[�h
		�ʏ�̃��X�^���C�[�[�V�����̓T���v���|�C���g�ƌĂ΂�锻��_������A�|���S�������̃T���v���|�C���g�ɏd�Ȃ�ꍇ�̓��X�^���C�[�[�V���������s����A�d�Ȃ�Ȃ��ꍇ�͎��s����Ȃ�
		*/
		//�^��: �T���v���|�C���g?
		descRS.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		/*
		�u�����h�X�e�[�g�Ƃ�
		���͗v�f�Ɖ��Z����A�o�͗v�f��3��p���Ăǂ̂悤�ɍ����������s���������߂邽�߂̃X�e�[�g
		���͗v�f�Əo�͗v�f�͂��ꂼ��W����ݒ肷�邱�Ƃ��ł��A
		���Z����͓��͗v�f�Əo�͗v�f���ǂ̂悤�ɉ��Z���邩��ݒ�ł���
		����ɂ�蔼�����Ȃǂ������\�ƂȂ�
		*/

		//�����_�[�^�[�Q�b�g�̃u�����h�ݒ�
		/*
		���������珇�Ԃ�
		BlendEnable: �u�����f�B���O��L���ɂ��邩�ǂ����̃t���O
		LogicIoEnable: �_�����Z��L���ɂ��邩�ǂ����̃t���O
		SrcBlend: �s�N�Z���V�F�[�_�[����o�͂��ꂽRGB�l�ɑ΂��Ď��s����u�����h�I�v�V�������w��
		DestBlend: ���݂̃����_�[�^�[�Q�b�g��RGB�l�ɑ΂��Ď��s����u�����h�I�v�V�������w��
		BlendOp: SrcBlend��DestBlend���ǂ̂悤�Ɍ������邩���`
		SrcBlendAlpha: �s�N�Z���V�F�[�_����o�͂��ꂽ�A���t�@�l�ɑ΂��Ď��s����u�����h�I�v�V�������w��, _COLOR�ŏI���u�����h�I�v�V�����͎w��ł��Ȃ�
		DestBlendAlpha: ���݂̃����_�[�^�[�Q�b�g�̃A���t�@�l�ɑ΂��Ď��s����u�����h�I�v�V�������w��m _COLOR�ŏI���u�����h�I�v�V�����͎w��ł��Ȃ�
		BlendOpAlpha: SrcBlendAlpha��DestBlendAlpha���ǂ̂悤�Ɍ������邩���`����
		LogicOp: �����_�[�^�[�Q�b�g�ɑ΂��Đݒ肷��_�����Z���w��
		RenderTargetWriteMask: �����_�[�^�[�Q�b�g�̏������݃}�X�N���w�肷��A�r�b�g���Ƃ�OR���Z���g�p���Č��������
		*/
		/*
		���͐F�Əo�͐F�ɂ��ꂼ���Z����W����D3D12_BLEND�ꗗ
		�����͑傫���̂ňȉ��Ŏg�p����Ă���̂����グ��
		D3D12_BLEND_ZERO: �W����(0, 0, 0, 0)
		D3D12_BLEND_ONE: �W����(1, 1, 1, 1)
		*/
		/*
		�u�����h�I�v�V�����ꗗ
		D3D12_BLEND_OP_ADD: ���͌��I�y�����h�Əo�͐�I�y�����h�����Z
		D3D12_BLEND_OP_SUBTRACT: �o�͐�I�y�����h������͌��I�y�����h�����Z
		D3D12_BLEND_OP_REV_SUBTRACT: ���͌��I�y�����h����o�͐�I�y�����h�����Z
		D3D12_BLEND_OP_MIN: ���͌��I�y�����h�Əo�͐�I�y�����h�̍ŏ��l�����߂�
		D3D12_BLEND_OP_MAX: ���͌��I�y�����h�Əo�͐�I�y�����h�̍ő�l�����߂�
		*/
		D3D12_RENDER_TARGET_BLEND_DESC descRTBS = {
			FALSE, FALSE,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD, //Alpha
			D3D12_LOGIC_OP_NOOP, //�������Ȃ�
			D3D12_COLOR_WRITE_ENABLE_ALL
		};

		//�u�����h�X�e�[�g�̐ݒ�
		D3D12_BLEND_DESC descBS;
		//�A���t�@�g�D�K�o���b�W�ƌĂ΂��@�\��L���ɂ���t���O
		/*
		�A���t�@�g�D�K�o���b�W�̓s�N�Z���V�F�[�_����o�͂��ꂽ�A���t�@�������擾���A�}���`�T���v�����O�A���`�G�C���A�X������K�p����@�\
		*/
		descBS.AlphaToCoverageEnable = FALSE;
		//�����_�[�^�[�Q�b�g�̓Ɨ������u�����h���g�p���邩�ǂ����̐ݒ�t���O
		descBS.IndependentBlendEnable = FALSE;
		for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		{
			descBS.RenderTarget[i] = descRTBS;
		}

		ComPtr<ID3DBlob> pVSBlob;
		ComPtr<ID3DBlob> pPSBlob;

		//�v���W�F�N�g���r���h�����ۂɃV�F�[�_�͏o�̓f�B���N�g����(Filename).cso�Ƃ��ďo�͂����

		//���_�V�F�[�_�ǂݍ���
		auto hr = D3DReadFileToBlob(L"SimpleVS.cso", pVSBlob.GetAddressOf());
		if (FAILED(hr))
		{
			return false;
		}

		//�s�N�Z���V�F�[�_�ǂݍ���
		hr = D3DReadFileToBlob(L"SimplePS.cso", pPSBlob.GetAddressOf());
		if (FAILED(hr))
		{
			return false;
		}

		//�p�C�v���C���X�e�[�g�̐ݒ�
		/*
		�p�C�v���C���X�e�[�g�̐����͂��̐ݒ���s�����\���̂������Ƃ��ēn��
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

		//�r���[�|�[�g�ƃV�U�[��`�̐ݒ�

		{
			//����̓E�B���h�E�S�̂ɏ������ނ̂Ńr���[�|�[�g�͂���Ȋ���
			m_Viewport.TopLeftX = 0;
			m_Viewport.TopLeftY = 0;
			m_Viewport.Width = static_cast<float>(m_width);
			m_Viewport.Height = static_cast<float>(m_height);
			m_Viewport.MinDepth = 0.0f;
			m_Viewport.MaxDepth = 1.0f;

			//�V�U�[��`�͊�����̈���w��
			//����̓r���[�|�[�g�Ɠ����T�C�Y�ɂ��Ă����A�r���[�|�[�g�̊O�͕`�悵�Ȃ��悤�ɂ���
			m_Scissor.left = 0;
			m_Scissor.right = m_width;
			m_Scissor.top = 0;
			m_Scissor.bottom = m_height;
		}

		return true;
	}
}

//�I�����̏���
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