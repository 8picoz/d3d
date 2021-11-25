#include "App.h"

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