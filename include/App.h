#pragma once

#include<Windows.h>
#include<cstdint>
#include<d3d12.h>
#include<dxgi1_4.h>
#include<wrl/client.h>
#include<DirectXMath.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

class App
{
public:
	App(uint32_t width, uint32_t height);
	~App();
	void Run();

private:
	static const uint32_t FrameCount = 2; //�t���[���o�b�t�@��

	HINSTANCE m_hInst;	//�A�v���P�[�V�����C���^�[�t�F�C�X�p
	HWND m_hWnd;		//window�̃n���h��
	uint32_t m_width;
	uint32_t m_height;

	ComPtr<ID3D12Device> m_pDevice; //�f�o�C�X
	ComPtr<ID3D12CommandQueue> m_pQueue; //�R�}���h�L���[
	ComPtr<IDXGISwapChain3> m_pSwapChain; //�X���b�v�`�F�[��
	ComPtr<ID3D12Resource> m_pColorBuffer[FrameCount]; //�J���[�o�b�t�@
	ComPtr<ID3D12CommandAllocator> m_pCmdAllocator[FrameCount]; //�R�}���h�A���P�[�^
	ComPtr<ID3D12GraphicsCommandList> m_pCmdList; //�R�}���h���X�g
	ComPtr<ID3D12DescriptorHeap> m_pHeapRTV; //�f�B�X�N���v�^�q�[�v(�����_�[�^�[�Q�b�g�r���[)
	ComPtr<ID3D12Fence> m_pFence; //�t�F���X
	ComPtr<ID3D12DescriptorHeap> m_pHeapCBV; //�f�B�X�N���v�^�q�[�v(�萔�o�b�t�@�r���[�E�V�F�[�_�[���\�[�X�r���[�E�A���I�[�_�[�h�A�N�Z�X�r���[)
	ComPtr<ID3D12Resource> m_pVB; //���_�o�b�t�@
	ComPtr<ID3D12Resource> m_pCB[FrameCount]; //�萔�o�b�t�@
	ComPtr<ID3D12RootSignature> m_pRootSignature; //���[�g�V�O�j�`��
	ComPtr<ID3D12PipelineState> m_pS0; //�p�C�v���C���X�e�[�g

	HANDLE m_FenceEvent; //�t�F���X�C�x���g
	uint64_t m_FenceCounter[FrameCount]; //�t�F���X�J�E���^
	uint32_t m_FrameIndex; //�t���[���ԍ�
	D3D12_CPU_DESCRIPTOR_HANDLE m_HandleRTV[FrameCount]; //CPU�f�B�X�N���v�^(�����_�[�^�[�Q�b�g�r���[)
	D3D12_VERTEX_BUFFER_VIEW m_VBV; //���_�o�b�t�@�r���[
	D3D12_VIEWPORT m_Viewport; //�r���[�|�[�g
	D3D12_RECT m_Scissor; //�V�U�[��`
	ConstantBufferView<Transform> m_CBV[FrameCount]; //�萔�o�b�t�@�r���[
	float m_RotateAngle; //��]�p
	
	bool InitApp();
	void TermApp();
	bool InitWnd();
	void TermWnd();
	void MainLoop();
	bool InitD3D();
	void TermD3D();
	void Render();
	void WaitGpu();
	void Present(uint32_t interval);

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
};