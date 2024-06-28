#include"ClientMsgCenter.h"
#include<iostream>
template<typename T>
class AdvCoverMsgCenter :public ClientMsgCenter<T>
{
public:
	AdvCoverMsgCenter()
	{

	}
	AdvCoverMsgCenter(std::unordered_map<ClientMsg, std::function<bool(Client<T>*)>>*)
	{

	}
	~AdvCoverMsgCenter()
	{

	}

	bool checkClientError(Client<T>* client) override
	{
		auto iswindow= IsWindow(client->ID);
		return iswindow;
	}

	//bool  notifyClients(ClientMsg& c) override
	//{
	//    
	//	return false;
	//}
	bool sendClient(Client<T>* a)  override
	{
		
		return false;
	}
		
};

bool func1(Client<HWND>* client)
{
	std::cout << "false" << std::endl;
	return false;
}

class TestClass
{
private:
	int k = 1;

public:

	Client<HWND>*client=nullptr;
	bool testClient(Client<HWND>* client)
	{
		if (k == 1)
			return false;
	}

};

int main()
{

	AdvCoverMsgCenter<HWND> clientCenter; 
	TestClass testclass;
	clientCenter.mappingFunctions[L"hi"] = [](Client<HWND>* client) -> bool {
		for (int i=0;i<10;++i)std::cout << "hi" << std::endl;return false;	};
	clientCenter.mappingFunctions[L"hi2"] = &func1;
	clientCenter.mappingFunctions[L"testClass"] = [&testclass](Client<HWND>* client) -> bool
	{
		return testclass.testClient(client);
	};


	HWND a;
	auto client = std::make_shared<Client<HWND>>(a);
	clientCenter.subscribe(client,L"hi");
	clientCenter.subscribe(client, L"hi2");
	clientCenter.subscribe(client, L"testClass");

	clientCenter.notifyClients(L"hi");
	clientCenter.notifyClients(L"hi2");
	clientCenter.notifyClients(L"testClass");

	//clientCenter.mappingFunctions[;
	
	std::vector<std::string*> strLst;//;; = { "a","b","c","d" };
	
	{
		std::string strAry[] = { "a","b","C","D" };

		strLst.push_back(&strAry[0]);
		strLst.push_back(&strAry[1]);
		strLst.push_back(&strAry[2]);
		strLst.push_back(&strAry[3]);


		auto k = strLst.begin();
		auto e = strLst.end();
		for (auto k = strLst.begin(); k != strLst.end();)
		{
			k = strLst.erase(k);
			//k = strLst.begin();

		}

	}


	return 0;
}