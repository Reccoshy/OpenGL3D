#pragma once

//�R�}���h�Z���N�g�Ɏg�p����N���X.
class CommandSelect {
public:

	void InitCommand(int Max, int initialIndex = 0);
	void IncreaseIndex();
	void DecreaseIndex();

	int getIndex() { return index; }

private:
	//�I�𒆂̃C���f�b�N�X.
	int index = 0;
	//�I���\�ő�C���f�b�N�X.
	int MaxIndex = 0;
};