
struct VSInput
{
	//�R�����̌�̓Z�}���e�B�N�X
	//�Z�}���e�B�N�X�Ƃ͂��̃f�[�^�����ۂɂǂ̂悤�ɉ��߂��邩�̒�`
	//���R�ɐ؂�m�邱�Ƃ��ł���
	float3 Position : POSITION; //�ʒu���W
	float4 Color : COLOR; //���_�J���[
};

struct VSOutput
{
	//�o�̓f�[�^�ɂ��Ă̓Z�}���e�B�N�������S�Ɏ��R�Ɍ��߂���킯�ł͂Ȃ��A�V�X�e��������`���Ă�����̂�����
	//�V�X�e���l�Z�}���e�B�b�N��"SV_"����n�܂�
	float4 Position : SV_POSITION; //�ʒu���W
	float4 Color : COLOR; //���_�J���[
};

//�萔�o�b�t�@
/*
�萔�o�b�t�@�Ƃ͒��_�f�[�^�ȊO��GPU���ɑ��邱�Ƃ��ł���萔�̉�
*/
//register(b0)�͒萔�o�b�t�@�p�̃��W�X�^��0�Ԗڂ��g�p���邱�Ƃ�����
/*
���W�X�^�͑�Q������register(b0, space0)�̂悤�ɓ���邱�Ƃ��ł������register(b0, space1)�̂悤�ɃJ�E���g�A�b�v����
�����͂����閼�O��Ԃ̂悤�Ȃ��̂ł��背�W�X�^��ԂƌĂ΂�A���W�X�^�̋���������ł���
�Ⴆ��
register(t3, space0)��register(t3, space1)�͓���t3���W�X�^�������W�X�^��Ԃ��Ⴄ�̂ŋ��������Ƀ��W�X�^���㏑�������̂�j�~�ł���
�ȗ������space0�ƂȂ�
*/
cubuffer Transform : register(b0)
{
	//packoffset�͒萔�o�b�t�@�̐퓬����̃I�t�Z�b�g���w��ł���Ƃ���
	//c0�͐擪�f�[�^�Ac1�͐擪����float��4����16byte��A�Ȃ̂�c4��c0����float��16������w��
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
