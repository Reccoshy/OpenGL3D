#pragma once

//コマンドセレクトに使用するクラス.
class CommandSelect {
public:

	void InitCommand(int Max, int initialIndex = 0);
	void IncreaseIndex();
	void DecreaseIndex();

	int getIndex() { return index; }

private:
	//選択中のインデックス.
	int index = 0;
	//選択可能最大インデックス.
	int MaxIndex = 0;
};