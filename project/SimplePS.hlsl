//vertex shaderから受け取る値
struct VSOutput
{
	float4 Position : SV_POSITION; //位置座標
	float4 Color : COLOR; //頂点カラー
};

struct PSOutput
{
	//ここではレンダーターゲットは一つしかないので配列の0番目に書き込みたいからSV_TARGET0を指定
	//疑問: ここではスワップチェインに２つ保持していようがアレはCPU側でスワップして切り替えて管理しているものなのでレンダーターゲットとしては1つとなる？
	//2番目ならSV_TARGET2のようになる
	float4 Color : SV_TARGET0;
};

PSOutput main(VSOutput input)
{
	PSOutput output = (PSOutput)0;

	output.Color = input.Color;

	return output;
}