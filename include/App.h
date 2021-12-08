#pragma once

#include<Windows.h>
#include<cstdint>
#include<d3d12.h>
#include<dxgi1_4.h>
#include<wrl/client.h>
#include<DirectXMath.h>
#include<d3dcompiler.h> //D3DReadFileToBlob()

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

//D3D12の制限から定数バッファを作成する際のメモリアライメントが256byteにならなくては行けない
struct alignas(256) Transform {
	DirectX::XMMATRIX World;
	DirectX::XMMATRIX View;
	DirectX::XMMATRIX Proj;
};

//定数バッファービューを定義
template<typename T>
struct ConstantBufferView 
{
	D3D12_CONSTANT_BUFFER_VIEW_DESC Desc; //定数バッファの構成設定
	D3D12_CPU_DESCRIPTOR_HANDLE HandleCPU; //CPUディスクリプタハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE HandleGPU; //GPUディスクリプタハンドル
	T* pBuffer; //バッファ先頭へのポインタ
};

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
	uint32_t m_Width;
	uint32_t m_Height;

	ComPtr<ID3D12Device> m_pDevice; //デバイス
	ComPtr<ID3D12CommandQueue> m_pQueue; //コマンドキュー
	ComPtr<IDXGISwapChain3> m_pSwapChain; //スワップチェーン
	//ID3D12ResourceはCPUとGPUが物理メモリを読み書きするための一般的な機能をカプセル化したもの
	ComPtr<ID3D12Resource> m_pColorBuffer[FrameCount]; //カラーバッファ
	ComPtr<ID3D12CommandAllocator> m_pCmdAllocator[FrameCount]; //コマンドアロケータ
	ComPtr<ID3D12GraphicsCommandList> m_pCmdList; //コマンドリスト
	ComPtr<ID3D12DescriptorHeap> m_pHeapRTV; //ディスクリプタヒープ(レンダーターゲットビュー)
	ComPtr<ID3D12Fence> m_pFence; //フェンス
	ComPtr<ID3D12DescriptorHeap> m_pHeapCBV; //ディスクリプタヒープ(定数バッファビュー・シェーダーリソースビュー・アンオーダードアクセスビュー)
	ComPtr<ID3D12DescriptorHeap> m_pHeapDSV; //DSV: Depth Stencil View, 深度ステンシルビュー
	ComPtr<ID3D12Resource> m_pVB; //頂点バッファ
	ComPtr<ID3D12Resource> m_pCB[FrameCount * 2]; //定数バッファ
	ComPtr<ID3D12Resource> m_pIB; //インデックスバッファ
	ComPtr<ID3D12Resource> m_pDepthBuffer; //深度ステンシルバッファ
	ComPtr<ID3D12RootSignature> m_pRootSignature; //ルートシグニチャ
	ComPtr<ID3D12PipelineState> m_pPSO; //パイプラインステート

	HANDLE m_FenceEvent; //フェンスイベント
	uint64_t m_FenceCounter[FrameCount]; //フェンスカウンタ
	uint32_t m_FrameIndex; //フレーム番号
	D3D12_CPU_DESCRIPTOR_HANDLE m_HandleRTV[FrameCount]; //CPUディスクリプタハンドル(レンダーターゲットビュー)
	D3D12_CPU_DESCRIPTOR_HANDLE m_HandleDSV; //CPUディスクリプタハンドル(深度ステンシルバッファ)
	D3D12_VERTEX_BUFFER_VIEW m_VBV; //頂点バッファビュー
	D3D12_INDEX_BUFFER_VIEW m_IBV; //インデックスバッファビュー, 本には書いてない
	D3D12_VIEWPORT m_Viewport; //ビューポート
	D3D12_RECT m_Scissor; //シザー矩形
	//現在まだ定義されていないのでコメントアウト
	ConstantBufferView<Transform> m_CBV[FrameCount * 2]; //定数バッファビュー
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
	bool OnInit();
	void OnTerm();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
};