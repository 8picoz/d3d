struct VSInput
{
	float3 Position : POSITION; //�ʒu���
	//�^��: �Ȃ����_�e�N�X�`���t�F�b�`������ƃs�N�Z���V�F�[�_�[�Ŏ���Ă�����r���Ȃ��Ă��܂��̂��H
	//�������e�O�Ȃ̂Ń~�b�v�}�b�v�Ȃǂ̏�񂪎��Ȃ��H
	//���Ⓒ�_���Ƃɂ����F�����߂��Ȃ����璸�_�����b�V���ɋl�܂��ĂȂ�����s���|�C���g�ł����F�����Ȃ�����
	float2 TexCoord : TEXCOORD; //�e�N�X�`�����W
};

struct VSOutput
{
	float4 Position : SV_POSITION; //�ʒu���
	float2 TexCoord : TEXCOORD; //�e�N�X�`�����W
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