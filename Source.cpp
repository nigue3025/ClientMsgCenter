
#include<iostream>
#include<windows.h>
#include"ClientMsgCenter.h"


class AdvCoverMsgCenter :public ClientMsgCenter
{
public:
	AdvCoverMsgCenter()
	{

	}
	AdvCoverMsgCenter(std::unordered_map<ClientMsg, std::function<bool(Client*)>>*)
	{

	}
	~AdvCoverMsgCenter()
	{

	}

	bool isClientError(Client* client) override;


	//bool  notifyClients(ClientMsg& c) override
	//{
	//    
	//	return false;
	//}
	bool sendClient(Client* a)  override
	{

		return false;
	}

};

bool func1(Client* client)
{
	std::cout << "false" << std::endl;
	return false;
}

class TestClass
{
private:
	int k = 1;

public:

	Client* client = nullptr;
	bool testClient(Client* client)
	{
		if (k == 1)
			return false;
	}

};


class winClient :public Client
{
public:
	HWND WindowHandle;
	winClient(std::string id):Client(id)
	{
		//ID = id;
	}

};

class fakeClient :public Client
{
public:
	fakeClient(std::string id) :Client(id) {}

};

bool AdvCoverMsgCenter::isClientError(Client* client)
{


	auto curClient = dynamic_cast<winClient*>(client);
	if (curClient != nullptr)
	{
		std::cout << "static cast to winClient complete\n";
		return IsWindow(curClient->WindowHandle);
	}
	else
	{
		printf("not winClient class");
		return true;
	}



	printf("checking error\n");
	return false;
}

int main()
{

	AdvCoverMsgCenter clientCenter;
	TestClass testclass;
	clientCenter.mappingFunctions[L"hi"] = [](Client* client) -> bool {
		for (int i = 0; i < 10; ++i)std::cout << "hi" << std::endl; return false;	};
	clientCenter.mappingFunctions[L"hi2"] = &func1;
	clientCenter.mappingFunctions[L"testClass"] = [&testclass](Client* client) -> bool
	{
		return testclass.testClient(client);
	};



	std::shared_ptr<Client>  client  =std::make_shared<winClient>("12345");
	//std::shared_ptr<Client>  client = std::make_shared<fakeClient>("12345");
	clientCenter.subscribe(client, L"hi");
	clientCenter.subscribe(client, L"hi2");
	clientCenter.subscribe(client, L"testClass");

	clientCenter.notifyClients(L"hi");
	clientCenter.notifyClients(L"hi2");
	clientCenter.notifyClients(L"testClass");




	return 0;
}

