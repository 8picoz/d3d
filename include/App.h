#pragma once

#include<Windows.h>
#include<cstdint>
#include<d3d12.h>
#include<dxgi1_4.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

class App
{
public:
	App(uint32_t width, uint32_t height);
	~App();
	void Run();

private:
	static const uint32_t FrameCount = 2; //フレームバッファ数

	HINSTANCE m_hInst;	//アプリケーションインターフェイス用
	HWND m_hWnd;		//windowのハンドル
	uint32_t m_width;
	uint32_t m_height;

	ID3D12Device* m_pDevice; //デバイス
	ID3D12CommandQueue* m_pQueue; //コマンドキュー
	IDXGISwapChain3* m_pSwapChain; //スワップチェーン
	ID3D12Resource* m_pColorBuffer[FrameCount]; //カラーバッファ
	ID3D12CommandAllocator* m_pCmdAllocator[FrameCount]; //コマンドアロケータ
	ID3D12GraphicsCommandList* m_pCmdList; //コマンドリスト
	ID3D12DescriptorHeap* m_pHeapRTV; //ディスクリプタヒープ(レンダーターゲットビュー)
	ID3D12Fence* m_pFence; //フェンス
	HANDLE m_FenceEvent; //フェンスイベント
	uint64_t m_FenceCounter[FrameCount]; //フェンスカウンタ
	uint32_t m_FrameIndex; //フレーム番号
	D3D12_CPU_DESCRIPTOR_HANDLE m_HandleRTV[FrameCount]; //CPUディスクリプタ(レンダーターゲットビュー)

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