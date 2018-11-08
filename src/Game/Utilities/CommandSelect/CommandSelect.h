#pragma once

class CommandSelect {
public:

	void InitCommand(int Max, int initialIndex = 0);
	void IncreaseIndex();
	void DecreaseIndex();

	int getIndex() { return index; }

private:

	int index = 0;
	int MaxIndex = 0;
};