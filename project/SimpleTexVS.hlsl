struct VSInput
{
	float3 Position : POSITION; //位置情報
	//疑問: なぜ頂点テクスチャフェッチをするとピクセルシェーダーで取ってくるより荒くなってしまうのか？
	//透視投影前なのでミップマップなどの情報が取れない？
	//いや頂点ごとにしか色が決められないから頂点がメッシュに詰まってない限りピンポイントでしか色が取れないだけ
	float2 TexCoord : TEXCOORD; //テクスチャ座標
};

struct VSOutput
{
	float4 Position : SV_POSITION; //位置情報
	float2 TexCoord : TEXCOORD; //テクスチャ座標
};

cbuffer Transform : register(b0)
{
	float4x4 World : packoffset(c0);
	float4x4 View : packoffset(c4);
	float4x4 Proj : packoffset(c8);
}

VSOutput main(VSInput input)
{
	VSOutput output = (VSOutput)0;

	float4 localPos = float4(input.Position, 1.0f);
	float4 worldpos = mul(World, localPos);
	float4 viewPos = mul(View, worldpos);
	float4 projPos = mul(Proj, viewPos);

	output.Position = projPos;
	output.TexCoord = input.TexCoord;

	return output;
}