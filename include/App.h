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
	static const uint32_t FrameCount = 2; //フレームバッファ数

	HINSTANCE m_hInst;	//アプリケーションインターフェイス用
	HWND m_hWnd;		//windowのハンドル
	uint32_t m_width;
	uint32_t m_height;

	ComPtr<ID3D12Device> m_pDevice; //デバイス
	ComPtr<ID3D12CommandQueue> m_pQueue; //コマンドキュー
	ComPtr<IDXGISwapChain3> m_pSwapChain; //スワップチェーン
	ComPtr<ID3D12Resource> m_pColorBuffer[FrameCount]; //カラーバッファ
	ComPtr<ID3D12CommandAllocator> m_pCmdAllocator[FrameCount]; //コマンドアロケータ
	ComPtr<ID3D12GraphicsCommandList> m_pCmdList; //コマンドリスト
	ComPtr<ID3D12DescriptorHeap> m_pHeapRTV; //ディスクリプタヒープ(レンダーターゲットビュー)
	ComPtr<ID3D12Fence> m_pFence; //フェンス
	ComPtr<ID3D12DescriptorHeap> m_pHeapCBV; //ディスクリプタヒープ(定数バッファビュー・シェーダーリソースビュー・アンオーダードアクセスビュー)
	ComPtr<ID3D12Resource> m_pVB; //頂点バッファ
	ComPtr<ID3D12Resource> m_pCB[FrameCount]; //定数バッファ
	ComPtr<ID3D12RootSignature> m_pRootSignature; //ルートシグニチャ
	ComPtr<ID3D12PipelineState> m_pS0; //パイプラインステート

	HANDLE m_FenceEvent; //フェンスイベント
	uint64_t m_FenceCounter[FrameCount]; //フェンスカウンタ
	uint32_t m_FrameIndex; //フレーム番号
	D3D12_CPU_DESCRIPTOR_HANDLE m_HandleRTV[FrameCount]; //CPUディスクリプタ(レンダーターゲットビュー)
	D3D12_VERTEX_BUFFER_VIEW m_VBV; //頂点バッファビュー
	D3D12_VIEWPORT m_Viewport; //ビューポート
	D3D12_RECT m_Scissor; //シザー矩形
	ConstantBufferView<Transform> m_CBV[FrameCount]; //定数バッファビュー
	float m_RotateAngle; //回転角
	
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