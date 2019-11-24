#include <iostream>
#include <string>
#include <thread>
#include "network.h"
#include "..\Client\vector.h"

using namespace std;

enum opcode
{
	None,
	Alpha,
	Beta
};

struct Data
{
	opcode opcode;
	vector3 position;
};

//testCode for Network module
int main()
{
	//server Initialization
	if (!NetworkModule::GetInstance()
		.Initialize(NetworkModule::_LOOP_BACK_IP, NetworkModule::_TARGET_PORT_)
		.TryConnect())
		return 0;

	Data data{ Alpha,vector3(1,2,3) };
	char* pData = (char*)&data;


	//Output thread 
	std::thread show(std::bind([=]()
	{
		while (NetworkModule::GetInstance().getIsConnected())
		{
			auto message = NetworkModule::GetInstance().SyncDequeueMessage();

			if (Data* temp = reinterpret_cast<Data*>(const_cast<char*>(message.c_str())))
			{
				cout << "message is Data class :: ";
				cout << "opCode is " << temp->opcode << ", position.x is " << temp->position.x << endl;
			}
			else
			{
				if (!message.empty())
					cout << message << endl;
			}
			
		}
	}));

	NetworkModule::GetInstance().Send(string(pData,sizeof(data)));
	
	//Input thread
	while (true)
	{
		string str;
		cin >> str;

		NetworkModule::GetInstance().Send(str);
	}
}