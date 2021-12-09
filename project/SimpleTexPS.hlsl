/*
�e�N�X�`���t�F�b�`�p�̃��\�b�h�͑傫���������Sample�n��Load�n������
-- Sample�n
Sample()
SampleBias()
SampleCmp()
SampleCmpLevelZero()
SampleGrad()
SampleLevel()
-- Load�n
Load()
--
�ɕ�����Ă���
Sample�n��Load�n�̈Ⴂ�̓T���v���[��K�v�Ƃ��邩�ǂ�����
�T���v���[���g�p����Sample�n�̓e�N�X�`�����h���b�V�����[�h��e�N�X�`���t�B���^�����O���g�p�ł���
�܂�Sample�n�͍��W��[0, 1]�Ő��K������Ă��邪Load�n�͗Ⴆ��512x512�̉摜�������炻�̂܂�[1, 512]�ƂȂ�(? <- [0, 511]����)
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

//�T���v���[
//�T���v���[���W�X�^�[�̔ԍ���"s + �ԍ�"
SamplerState ColorSmp : register(s0);
//�e�N�X�`���{��
//�e�N�X�`�����W�X�^�̔ԍ���"t + �ԍ�"
Texture2D ColorMap : register(t0);

PSOutput main(VSOutput input)
{
	PSOutput output = (PSOutput)0;

	output.Color = ColorMap.Sample(ColorSmp, input.TexCoord);

	return output;
}