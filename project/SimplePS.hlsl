//vertex shader����󂯎��l
struct VSOutput
{
	float4 Position : SV_POSITION; //�ʒu���W
	float4 Color : COLOR; //���_�J���[
};

struct PSOutput
{
	//�����ł̓����_�[�^�[�Q�b�g�͈�����Ȃ��̂Ŕz���0�Ԗڂɏ������݂�������SV_TARGET0���w��
	//�^��: �����ł̓X���b�v�`�F�C���ɂQ�ێ����Ă��悤���A����CPU���ŃX���b�v���Đ؂�ւ��ĊǗ����Ă�����̂Ȃ̂Ń����_�[�^�[�Q�b�g�Ƃ��Ă�1�ƂȂ�H
	//2�ԖڂȂ�SV_TARGET2�̂悤�ɂȂ�
	float4 Color : SV_TARGET0;
};

PSOutput main(VSOutput input)
{
	PSOutput output = (PSOutput)0;

	output.Color = input.Color;

	return output;
}