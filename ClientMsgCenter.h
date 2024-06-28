//#pragma once
#include<string>
#include<vector>
#include<map>
#include<unordered_map>
#include<mutex>
#include<memory>
#include<functional>
#include<windows.h>

//#define Client HWND

#ifndef _H
#define _H
#endif

#ifdef _WIN32
#define ClientMsg const wchar_t*
#else //for Linux
#define ClientMsg const char*
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
    std::map<std::shared_ptr<Client>, std::vector<ClientMsg>> clients;
    std::map<ClientMsg, std::vector<std::shared_ptr<Client>>> msgCenter;
    bool enroll(std::shared_ptr<Client>);
    bool subscribe(std::shared_ptr<Client>, ClientMsg);
    bool killClient(std::shared_ptr<Client>, bool IsLocked=false );
    bool killAllClients();
    bool notifyClients(ClientMsg);
    virtual bool sendClient(Client*) = 0;
    virtual bool isClientError(Client*)=0;
    std::unordered_map<ClientMsg, std::function<bool(Client*)>> mappingFunctions;
    ClientMsgCenter() ;
    ~ClientMsgCenter();
};










