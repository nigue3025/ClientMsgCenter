//#pragma once
#include<string>
#include<vector>
#include<map>
#include<unordered_map>
#include<mutex>
#include<memory>
#include<functional>


//#define Client HWND

#ifndef CLIENTMSGCENTER_H
#define CLIENTMSGCENTER_H
#endif

#ifdef _WIN32
#include<windows.h>
//#define MsgCtrClientMsg static const wchar_t*
#define MsgCtrClientMsg std::string
#else //for Linux
#define MsgCtrClientMsg const char*
#endif

//template<typename ClientIDType>
class Client
{
public:
    std::string ID;
    Client(std::string t);
    virtual void dummy();
    ~Client();
};

//template<typename ClientIDType>
class ClientMsgCenter
{
public:
    std::mutex clientCtrMtx;
    std::map<std::string, std::shared_ptr<Client>> clients; //儲存以client ID對應實體client的物件的map
    std::map<std::shared_ptr<Client>, std::vector<MsgCtrClientMsg>> clients_sublst;//儲存每個client訂閱的服務字串
    std::map<MsgCtrClientMsg, std::vector<std::shared_ptr<Client>>> msgCenter; //儲存每個服務字串對應的client
    std::unordered_map<MsgCtrClientMsg, std::function<bool(Client*)>> subscriptionReponseFunctions; //訂閱服務字串與對應的服務function,仰賴library(API)使用者注入以實現功能
    std::unordered_map<MsgCtrClientMsg, std::function<bool(Client*)>> queryResponseFunctions; //查詢服務字串與對應的服務function,仰賴library(API)使用者注入以實現功能
    bool BlockNotDefinedSubMsg = true; //Flag: 是否允許尚未定義的字串可先提供client訂閱 
    bool enroll(std::shared_ptr<Client>);
    bool subscribe(std::shared_ptr<Client>, MsgCtrClientMsg); //訂閱服務
    bool unsubscribe(std::shared_ptr<Client>, MsgCtrClientMsg); //取消訂閱服務
    bool killClient(std::shared_ptr<Client>, bool IsLocked = false); //刪除特定client
    bool killAllClients(); //刪除所有client
    bool responseQuery(std::shared_ptr<Client>, MsgCtrClientMsg); //查詢服務

    virtual bool isClientError(Client*) = 0; //提供實作,偵測client失效或斷線的判斷方式
    virtual bool notifyClients(MsgCtrClientMsg);
    std::string getLastError();

    ClientMsgCenter();
    ~ClientMsgCenter();

    static std::string HWNDToString(HWND hwnd);

protected:
    virtual bool notifyClients_internal(const MsgCtrClientMsg&); //依據訂閱資訊判定是否要傳送訊息給訂閱端client
    std::string LastErrorStr = ""; //錯誤message儲存字串
};










