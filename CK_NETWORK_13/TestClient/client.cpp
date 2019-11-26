#include <iostream>
#include <string>
#include <thread>
#include "network.h"
#include "vector.h"
#include "packet.h"

using namespace std;

//enum opcode
//{
//	None,
//	Alpha,
//	Beta
//};
//
//struct Data
//{
//	opcode opcode;
//	vector3 position;
//};

//testCode for Network module
int main()
{
	//server Initialization
	if (!NetworkModule::GetInstance()
		.Initialize(NetworkModule::_LOOP_BACK_IP, NetworkModule::_TARGET_PORT_)
		.TryConnect())
		return 0;

	//Data data{ Alpha,vector3(1,2,3) };
	//char* pData = (char*)&data;

	//test packet

	Packet packet;
	packet.opcode = OpCodes::kStart;
	packet.request.move_message = MoveMessage
	{
		1,
		true,
		vector2(100,200),
		VK_UP
	};

	auto str = packet.ToString();
	auto packet2 = Packet::ToPacket(str);



	//Output thread 
	std::thread show(std::bind([=]()
	{
		while (NetworkModule::GetInstance().getIsConnected())
		{
			auto message = NetworkModule::GetInstance().SyncDequeueMessage();

			//if (Data* temp = reinterpret_cast<Data*>(const_cast<char*>(message.c_str())))
			//{
			//	cout << "message is Data class :: ";
			//	cout << "opCode is " << temp->opcode << ", position.x is " << temp->position.x << endl;
			//}

			Packet packet = Packet::ToPacket(message);

			cout << "message is packet class :: ";
			cout << "opCode is " << (int)packet.opcode << ", keyDown is " << packet.request.move_message.isKeyDown
				<< ",KeyCode is " << packet.request.move_message.KeyCode << endl; ;

			if(false)
			{
				if (!message.empty())
					cout << message << endl;
			}
			
		}
	}));

	//NetworkModule::GetInstance().Send(string(pData,sizeof(data)));
	NetworkModule::GetInstance().Send(packet.ToString());
	
	//Input thread
	while (true)
	{
		string str;
		cin >> str;

		NetworkModule::GetInstance().Send(str);
	}
}