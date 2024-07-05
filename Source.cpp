
#include<iostream>
#include<windows.h>
#include"ClientMsgCenter.h"


class AdvCoverMsgCenter :public ClientMsgCenter
{
public:
	AdvCoverMsgCenter()
	{

	}
	AdvCoverMsgCenter(std::unordered_map<MsgCtrClientMsg, std::function<bool(Client*)>>*)
	{

	}
	~AdvCoverMsgCenter()
	{

	}

	bool isClientError(Client* client) override;




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

	clientCenter.subscriptionReponseFunctions["hi"] = [](Client* client) -> bool {
		for (int i = 0; i < 10; ++i)std::cout << "hi" << std::endl; return false;	};
	clientCenter.subscriptionReponseFunctions["hi2"] = &func1;
	clientCenter.subscriptionReponseFunctions["testClass"] = [&testclass](Client* client) -> bool
	{
		return testclass.testClient(client);
	};



	std::shared_ptr<Client>  client  =std::make_shared<winClient>("12345");
	//std::shared_ptr<Client>  client = std::make_shared<fakeClient>("12345");
	clientCenter.subscribe(client, "hi");
	clientCenter.subscribe(client, "hi2");
	clientCenter.subscribe(client, "testClass");

	clientCenter.notifyClients("hi");
	clientCenter.notifyClients("hi2");
	clientCenter.notifyClients("testClass");




	return 0;
}

