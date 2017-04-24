#pragma once
#include<functional>
#include<exception>
#include"Reader.h"
typedef void(*Action)();
void defaultAction() 
{
}
class DFARunner
{
	DFA dfa;
	int curStatus;
	bool error;
private:
	DFARunner(DFA &&dfa) : dfa(dfa), curStatus(0), error(false) {}
	Action move(int inputChar)
	{
		int nxt = dfa.getNextStatus(curStatus, inputChar);
		Action ret = dfa.getAction(curStatus, inputChar);
		curStatus = nxt;
		if (nxt == -1)
		{
			error = true;
		}
		return ret;
	}
	string run(Reader &rd)
	{
		while (rd.cur() != EOF)
		{
			Action action = move(rd.cur());
			if (action != nullptr)
				action();
			if (curStatus == 0)
			{
				return rd.pop();
				break;
			}
			if (curStatus == -1)
			{

				throw std::exception("");
			}
		}
	}
	~DFARunner()
	{
	}
};
class DFA
{
private:
	Action** action;
	int** toStatus;
	int numStatus;
	int numChar;
public:

	DFA(int numStatus, int numChar):numStatus(numStatus),numChar(numChar)
	{
		action = new Action*[numStatus];
		toStatus = new int*[numStatus];
		for (int i = 0; i < numStatus; i++)
		{
			action[i] = new Action[numChar];
			std::fill(action[i], action[i] + numChar, defaultAction);

			toStatus[i] = new int[numChar];
			std::fill(toStatus[i], toStatus[i] + numChar, -1);
		}
	}

	int getNextStatus(int currentStatus, int inputChar)
	{
		if (currentStatus < 0 || currentStatus >= numStatus || inputChar < 0 || inputChar >= numChar)
		{
			return -1;
		}
		return toStatus[currentStatus][inputChar];
	}

	Action getAction(int currentStatus, int inputChar)
	{
		if (currentStatus < 0 || currentStatus >= numStatus || inputChar < 0 || inputChar >= numChar)
		{
			return nullptr;
		}
		return action[currentStatus][inputChar];
	}

	DFA& set(int fromStatus, int inputChar, int toStatus, Action act = defaultAction)
	{
		if (fromStatus < 0 || fromStatus >= numStatus || toStatus < 0 || toStatus >= numStatus || inputChar < 0 || inputChar >= numChar)
		{
			throw std::exception("");
		}
		if (act == nullptr)
		{
			act = defaultAction;
		}
		action[fromStatus][inputChar] = act;
		this->toStatus[fromStatus][inputChar] = toStatus;
		return *this;
	}

	~DFA()
	{
		for (int i = 0; i < numStatus; i++)
		{
			delete[] action[i];
			action[i] = nullptr;
			delete[] toStatus[i];
			toStatus[i] = nullptr;
		}
		delete[] action;
		action = nullptr;
		delete[] toStatus;
		toStatus = nullptr;
	}
};

