
struct VSInput
{
	//コロンの後はセマンティクス
	//セマンティクスとはこのデータが実際にどのように解釈するかの定義
	//自由に切れmることができる
	float3 Position : POSITION; //位置座標
	float4 Color : COLOR; //頂点カラー
};

struct VSOutput
{
	//出力データについてはセマンティク名を完全に自由に決められるわけではなく、システム側が定義しているものがある
	//システム値セマンティックは"SV_"から始まる
	float4 Position : SV_POSITION; //位置座標
	float4 Color : COLOR; //頂点カラー
};

//定数バッファ
/*
定数バッファとは頂点データ以外にGPU側に送ることができる定数の塊
*/
//register(b0)は定数バッファ用のレジスタの0番目を使用することを示す
/*
レジスタは第２引数にregister(b0, space0)のように入れることができこれはregister(b0, space1)のようにカウントアップする
これらはいわゆる名前空間のようなものでありレジスタ空間と呼ばれ、レジスタの競合を回避できる
例えば
register(t3, space0)とregister(t3, space1)は同じt3レジスタだがレジスタ空間が違うので競合せずにレジスタが上書きされるのを阻止できる
省略するとspace0となる
*/
cubuffer Transform : register(b0)
{
	//packoffsetは定数バッファの戦闘からのオフセットを指定できるところ
	//c0は先頭データ、c1は先頭からfloatが4つ分の16byte先、なのでc4はc0からfloatが16個分先を指す
	float4x4 World : packoffset(c0);
	float4x4 View : packoffset(c4);
	float4x4 Proj : packoffset(c8);
}

VSOutput main(VSInput input)
{
	VSOutput output = (VSOutput)0;

	float4 localPos = float4(input.Position, 1.0f);
	float4 worldPos = mul(World, localPos);
	float4 viewPos = mul(View, worldPos);
	float4 projPos = mul(Proj, viewPos);

	output.Position = projPos;
	output.Color = input.Color;

	return output;
}
