#include <iostream>
#include <string>
#include <thread>
#include "network.h"

using namespace std;

//testCode for Network module
int main()
{
	//server Initialization
	if (!NetworkModule::GetInstance()
		.Initialize(NetworkModule::_LOOP_BACK_IP, NetworkModule::_TARGET_PORT_)
		.TryConnect())
		return 0;

	//Output thread 
	std::thread show(std::bind([=]()
	{
		while (NetworkModule::GetInstance().getIsConnected())
		{
			auto message = NetworkModule::GetInstance().SyncDequeueMessage();

			if (!message.empty())
				cout << message << endl;
		}
	}));

	//Input thread
	while (true)
	{
		string str;
		cin >> str;

		NetworkModule::GetInstance().Send(str);
	}
}