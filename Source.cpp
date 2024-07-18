#include<iostream>
#include<windows.h>
#include"ClientMsgCenter.h"
#include<sstream>
#include<string>


//*****�ۭqClient class*****
class CustomedWindowClient :public Client
{
public:
#ifdef _WIN32
	HWND WindowHandle;
#endif
	CustomedWindowClient(std::string id) :Client(id) 
	{
		//ID = id;
	}

};

//*****�ۭqMsgCenter class*****
class CustomedWindowsMsgCenter :public ClientMsgCenter
{
public:
	CustomedWindowsMsgCenter()	{}
	CustomedWindowsMsgCenter(std::unordered_map<MsgCtrClientMsg, std::function<bool(Client*)>>*){}
	~CustomedWindowsMsgCenter(){}


	//*****�ۭqClient���ħP�_�覡*****
	bool isClientError(Client* client) override
	{
		
		auto tmpClient= dynamic_cast<CustomedWindowClient*>(client);
		if (tmpClient == nullptr)
		{
			std::cout << "Client error:" << client->ID << " is not a CustomedWindowsClient class." << std::endl;;
			return true;
		}
		
		//�w��Windows���x,�ˬd�����O�_�s�b
		//return IsWindow(tmpClient->WindowHandle);
		

		return false;
	}

	bool sendMessage1(Client* client)
	{
		auto tmpClient = dynamic_cast<CustomedWindowClient*>(client);
		if (tmpClient == nullptr)
			return false;
		
		//�w��Windows���x,�ǰe�T��
		//sendMessage(tmpClient->WindowHandle);
	
		
		printf("message1 is sent to %s\n",tmpClient->ID.c_str());
		return true;
	}
	bool provideLocalTime(Client* client)
	{
		auto customedClient= dynamic_cast<CustomedWindowClient*>(client);
		if (customedClient == nullptr)
			return false;	

		bool sendResult = true;
		SYSTEMTIME st;
		GetLocalTime(&st);
		std::stringstream ss;
		ss<< st.wYear << "/" << st.wMonth << "/" << st.wDay << " " << st.wHour << ":" << st.wMinute << ":" << st.wSecond << ":" << st.wMilliseconds;
		std::string timeStr = ss.str();
		
		//�w��Windows���x,�ǰe�T��
		//sendResult=SendMessage(client->WindowHandle, WM_SETTEXT, 0, (LPARAM)timeStr.c_str());
		std::cout <<"Instant reply to "<<client->ID <<" " << ss.str() << std::endl;

		return sendResult;
	
	}

	void init()
	{
		//*****�]�w�q�\���^�����*****
		subscriptionReponseFunctions["message1"] = [this](Client* client) {return sendMessage1(client); };
		subscriptionReponseFunctions["hello"] = [](Client* client) -> bool {for (int i = 0; i < 2; ++i)std::cout << "hi"; std::cout <<" to "<<client->ID << std::endl; return true;	};
		
		//*****�]�w�d�ߪ��^�����*****
		queryResponseFunctions["getSystemTime"] = [this](Client* client) {return provideLocalTime(client); };
	}

};

//*****�ۭqClient class*****
class CustomedLiunxClient :public Client
{
public:
	CustomedLiunxClient(std::string id) :Client(id) {}
	
};



int main()
{
	//��l�ƭq�\����
	CustomedWindowsMsgCenter windowClientMsgCenter;
	windowClientMsgCenter.init(); 

	//����case 1 (����Client�q�\�O�_����)
	std::cout << "*****case1*****" << std::endl;
	//client1�q�\
	std::shared_ptr<Client>  client = std::make_shared<CustomedWindowClient>("client1");
	windowClientMsgCenter.subscribe(client, "hello");
	windowClientMsgCenter.subscribe(client, "message1");
	if (!windowClientMsgCenter.subscribe(client, "fakeMessage")) //�q�\����,�]��fakeMessage���w�q
		std::cout << windowClientMsgCenter.getLastError() << std::endl;
	//client2�q�\
	std::shared_ptr<Client>  client2 = std::make_shared<CustomedWindowClient>("client2");
	windowClientMsgCenter.subscribe(client2, "hello");
	//�q��client1�Pclient2
	windowClientMsgCenter.notifyClients("hello");
	windowClientMsgCenter.notifyClients("message1");

	//����case 2 (���ը����q�\�O�_���\)
	std::cout << "\n*****case2*****" << std::endl;
	//�����q�\
	windowClientMsgCenter.unsubscribe(client, "hello");
	windowClientMsgCenter.unsubscribe(client, "message1");
	//�q��client1�Pclient2
	windowClientMsgCenter.notifyClients("hello");
	windowClientMsgCenter.notifyClients("message1");

	//�d��-����case 3 (���լd�߬O�_���\)
	std::cout << "\n*****case3*****" << std::endl;
	windowClientMsgCenter.responseQuery(client2, "getSystemTime"); //�ߧY���o�ɶ�,�D�q�\�Ҧ�
	windowClientMsgCenter.responseQuery(client, "getSystemTime"); //�ߧY���o�ɶ�,�D�q�\�Ҧ�
	
	//����case 4 (����LinuxClient�O�_�i��)
	std::cout << "\n*****case4*****" << std::endl;
	std::shared_ptr<Client>  linuxClient = std::make_shared<CustomedLiunxClient>("linuxClient");
	windowClientMsgCenter.subscribe(linuxClient, "hello"); 
	windowClientMsgCenter.notifyClients("hello"); //�]����function���w�FCustomedWindowClient,�ҥH�L�k�q����linuxClient
	windowClientMsgCenter.responseQuery(linuxClient, "getSystemTime"); //�]����function���w�FCustomedWindowClient,�ҥH�L�k�d�ߨ�linuxClient
	return 0;
}

