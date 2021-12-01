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
			0, //nodeMask: ������GPU�m�[�h������ꍇ�ɁA�m�[�h�����ʂ��邽�߂̃r�b�g�}�X�N��ݒ肵�܂�
			D3D12_COMMAND_LIST_TYPE_DIRECT, //type: �쐬����R�}���h���X�g�̃^�C�v���w��A�R�}���h�L���[�ɒ��ړo�^�\�ȃR�}���h�݂̂�������
			m_pCmdAllocator[m_FrameIndex], //pCommandAllocator: �R�}���h���X�g���쐬����Ƃ��̃R�}���h�A���P�[�^���w��A2���邤���̕`��R�}���h����ł����̂Ɏg�p����̂̓o�b�N�o�b�t�@�̔ԍ��ɑΉ�������̂Ȃ̂ŃC���f�b�N�X��m_FrameIndex���w��
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
		hr = m_pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pHeapRTV));
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
			hr = m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_pColorBuffer[i]));
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
			m_pDevice->CreateRenderTargetView(m_pColorBuffer[i], &viewDesc, handle);

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
			IID_PPV_ARGS(&m_pFence));
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
	m_pCmdList->Reset(m_pCmdAllocator[m_FrameIndex], nullptr);

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
	barrier.Transition.pResource = m_pColorBuffer[m_FrameIndex]; //�J�ڂɎg�p���邽�߂̃��\�[�X�̃|�C���^���w��
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

	//�`�揈��
	{
		//TODO: �|���S���`��p�̏�����ǉ�
	}

	//���\�[�X�o���A�̐ݒ�
	/*
	���������_�[�^�[�Q�b�g�ɏ������ޕK�v�͂Ȃ��̂ŏ�L�Ɠ����悤�ɏ�Ԃ�"�\������"�Ƀ��\�[�X�o���A�����Ƃɖ߂�
	*/
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_pColorBuffer[m_FrameIndex];
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	//���\�[�X�o���A
	m_pCmdList->ResourceBarrier(1, &barrier);

	//�R�}���h�̋L�^���I��
	m_pCmdList->Close();

	//�R�}���h�����s
	//�쐬�����R�}���h���X�g�͉������Ȃ���Ύ��s����Ȃ��̂ł����ŃR�}���h�L���[�ɓn���Ă�����
	ID3D12CommandList* ppCmdLists[] = { m_pCmdList };
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
		m_pFence, //pFence: �t�F���X�I�u�W�F�N�g�̃|�C���^ 
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
	m_pQueue->Signal(m_pFence, m_FenceCounter[m_FrameIndex]);

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
	SafeRelease(m_pFence);

	//�����_�[�^�[�Q�b�g�r���[�̔j��
	SafeRelease(m_pHeapRTV);
	for (auto i = 0u; i < FrameCount; ++i)
	{
		SafeRelease(m_pColorBuffer[i]);
	}

	//�R�}���h���X�g�̔j��
	SafeRelease(m_pCmdList);

	//�R�}���h�A���P�[�^�̔j��
	for (auto i = 0u; i < FrameCount; ++i)
	{
		SafeRelease(m_pCmdAllocator[i]);
	}

	SafeRelease(m_pSwapChain);

	//�R�}���h�L���[�̔j��
	SafeRelease(m_pQueue);

	//�f�o�C�X�̔j��
	SafeRelease(m_pDevice);
}
