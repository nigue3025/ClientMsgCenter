#include<iostream>
#include<windows.h>
#include"ClientMsgCenter.h"
#include<sstream>
#include<string>


//*****自訂Client class*****
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

//*****自訂MsgCenter class*****
class CustomedWindowsMsgCenter :public ClientMsgCenter
{
public:
	CustomedWindowsMsgCenter()	{}
	CustomedWindowsMsgCenter(std::unordered_map<MsgCtrClientMsg, std::function<bool(Client*)>>*){}
	~CustomedWindowsMsgCenter(){}


	//*****自訂Client失效判斷方式*****
	bool isClientError(Client* client) override
	{
		
		auto tmpClient= dynamic_cast<CustomedWindowClient*>(client);
		if (tmpClient == nullptr)
		{
			std::cout << "Client error:" << client->ID << " is not a CustomedWindowsClient class." << std::endl;;
			return true;
		}
		
		//針對Windows平台,檢查視窗是否存在
		//return IsWindow(tmpClient->WindowHandle);
		

		return false;
	}

	bool sendMessage1(Client* client)
	{
		auto tmpClient = dynamic_cast<CustomedWindowClient*>(client);
		if (tmpClient == nullptr)
			return false;
		
		//針對Windows平台,傳送訊息
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
		
		//針對Windows平台,傳送訊息
		//sendResult=SendMessage(client->WindowHandle, WM_SETTEXT, 0, (LPARAM)timeStr.c_str());
		std::cout <<"Instant reply to "<<client->ID <<" " << ss.str() << std::endl;

		return sendResult;
	
	}

	void init()
	{
		//*****設定訂閱的回應函數*****
		subscriptionReponseFunctions["message1"] = [this](Client* client) {return sendMessage1(client); };
		subscriptionReponseFunctions["hello"] = [](Client* client) -> bool {for (int i = 0; i < 2; ++i)std::cout << "hi"; std::cout <<" to "<<client->ID << std::endl; return true;	};
		
		//*****設定查詢的回應函數*****
		queryResponseFunctions["getSystemTime"] = [this](Client* client) {return provideLocalTime(client); };
	}

};

//*****自訂Client class*****
class CustomedLiunxClient :public Client
{
public:
	CustomedLiunxClient(std::string id) :Client(id) {}
	
};



int main()
{
	//初始化訂閱中心
	CustomedWindowsMsgCenter windowClientMsgCenter;
	windowClientMsgCenter.init(); 

	//測試case 1 (測試Client訂閱是否有效)
	std::cout << "*****case1*****" << std::endl;
	//client1訂閱
	std::shared_ptr<Client>  client = std::make_shared<CustomedWindowClient>("client1");
	windowClientMsgCenter.subscribe(client, "hello");
	windowClientMsgCenter.subscribe(client, "message1");
	if (!windowClientMsgCenter.subscribe(client, "fakeMessage")) //訂閱失敗,因為fakeMessage未定義
		std::cout << windowClientMsgCenter.getLastError() << std::endl;
	//client2訂閱
	std::shared_ptr<Client>  client2 = std::make_shared<CustomedWindowClient>("client2");
	windowClientMsgCenter.subscribe(client2, "hello");
	//通知client1與client2
	windowClientMsgCenter.notifyClients("hello");
	windowClientMsgCenter.notifyClients("message1");

	//測試case 2 (測試取消訂閱是否成功)
	std::cout << "\n*****case2*****" << std::endl;
	//取消訂閱
	windowClientMsgCenter.unsubscribe(client, "hello");
	windowClientMsgCenter.unsubscribe(client, "message1");
	//通知client1與client2
	windowClientMsgCenter.notifyClients("hello");
	windowClientMsgCenter.notifyClients("message1");

	//查詢-測試case 3 (測試查詢是否成功)
	std::cout << "\n*****case3*****" << std::endl;
	windowClientMsgCenter.responseQuery(client2, "getSystemTime"); //立即取得時間,非訂閱模式
	windowClientMsgCenter.responseQuery(client, "getSystemTime"); //立即取得時間,非訂閱模式
	
	//測試case 4 (測試LinuxClient是否可用)
	std::cout << "\n*****case4*****" << std::endl;
	std::shared_ptr<Client>  linuxClient = std::make_shared<CustomedLiunxClient>("linuxClient");
	windowClientMsgCenter.subscribe(linuxClient, "hello"); 
	windowClientMsgCenter.notifyClients("hello"); //因對應function限定了CustomedWindowClient,所以無法通知到linuxClient
	windowClientMsgCenter.responseQuery(linuxClient, "getSystemTime"); //因對應function限定了CustomedWindowClient,所以無法查詢到linuxClient
	return 0;
}

