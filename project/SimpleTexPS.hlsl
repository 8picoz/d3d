/*
テクスチャフェッチ用のメソッドは大きく分けるとSample系とLoad系が存在
-- Sample系
Sample()
SampleBias()
SampleCmp()
SampleCmpLevelZero()
SampleGrad()
SampleLevel()
-- Load系
Load()
--
に分かれている
Sample系とLoad系の違いはサンプラーを必要とするかどうかで
サンプラーを使用するSample系はテクスチャ亜ドレッシンモードやテクスチャフィルタリングが使用できる
またSample系は座標が[0, 1]で正規化されているがLoad系は例えば512x512の画像だったらそのまま[1, 512]となる(? <- [0, 511]かも)
*/

struct VSOutput
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

struct PSOutput
{
	float4 Color : SV_TARGET0;
};

//サンプラー
//サンプラーレジスターの番号は"s + 番号"
SamplerState ColorSmp : register(s0);
//テクスチャ本体
//テクスチャレジスタの番号は"t + 番号"
Texture2D ColorMap : register(t0);

PSOutput main(VSOutput input)
{
	PSOutput output = (PSOutput)0;

	output.Color = ColorMap.Sample(ColorSmp, input.TexCoord);

	return output;
}