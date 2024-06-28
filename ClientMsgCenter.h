#pragma once
#include<string>
#include<vector>
#include<map>
#include<unordered_map>
#include<mutex>
#include<memory>
#include<functional>
#include<windows.h>

//#define Client HWND


template<typename ClientIDType>
class Client
{

public:
    ClientIDType ID;
    Client(ClientIDType t);
    ~Client();
};


#ifdef _WIN32
#define ClientMsg const wchar_t*
#else //for Linux
#define ClientMsg const char*
#endif



template<typename ClientIDType>
class ClientMsgCenter
{
public:
    std::mutex clientCtrMtx;
    std::map<std::shared_ptr<Client<ClientIDType>>, std::vector<ClientMsg>> clients;
    std::map<ClientMsg, std::vector<std::shared_ptr<Client<ClientIDType>>>> msgCenter;
    bool enroll(std::shared_ptr<Client<ClientIDType>>);
    bool subscribe(std::shared_ptr<Client<ClientIDType>>, ClientMsg);
    bool killClient(std::shared_ptr<Client<ClientIDType>>, bool IsLocked=false );
    bool killAllClients();
    bool notifyClients(ClientMsg);
    virtual bool sendClient(Client<ClientIDType>*) = 0;
    virtual bool checkClientError(Client<ClientIDType>*);

    std::unordered_map<ClientMsg, std::function<bool(Client<ClientIDType>*)>> mappingFunctions;

    //   (*bool)(*Client<ClientIDType> a);

    ClientMsgCenter();
    ~ClientMsgCenter();
};


template<typename ClientIDType>
Client<ClientIDType>::Client(ClientIDType id):ID(id)
{

}

template<typename ClientIDType>
Client<ClientIDType>::~Client()
{

}




template<typename ClientIDType> ClientMsgCenter<ClientIDType>::ClientMsgCenter()
{
    

}
template<typename ClientIDType> ClientMsgCenter<ClientIDType>::~ClientMsgCenter()
{
    killAllClients();
}


template<typename ClientIDType>
bool ClientMsgCenter<ClientIDType>::enroll(std::shared_ptr<Client<ClientIDType>>client)
{
    std::lock_guard<std::mutex> lock(clientCtrMtx);
    if (clients.find(client) == clients.end())
    {
        clients[client] = std::vector<ClientMsg>();
        return true; // 成功註冊
    }
    return false; // client 已經存在
}

template<typename ClientIDType>
bool ClientMsgCenter<ClientIDType>::subscribe(std::shared_ptr<Client<ClientIDType>>client, ClientMsg msg)
{
    std::lock_guard<std::mutex> lock(clientCtrMtx);
    if (clients.find(client) == clients.end())
    {
        clients[client] = std::vector<ClientMsg>();
    }
    clients[client].push_back(msg);
    msgCenter[msg].push_back(client);
    return true;
}

template<typename ClientIDType>
bool ClientMsgCenter<ClientIDType>::checkClientError(Client<ClientIDType>* client)
{
    return true;

}

template<typename ClientIDType>
bool ClientMsgCenter<ClientIDType>::killClient(std::shared_ptr<Client<ClientIDType>>client,bool IsLocked)
{
    if(!IsLocked)
        std::lock_guard<std::mutex> lock(clientCtrMtx); 
    auto it = clients.find(client);
    if (it != clients.end())
    {
        clients.erase(it);
        for (auto& pair : msgCenter)
        {
            auto& vec = pair.second;
            auto indx = std::remove(vec.begin(), vec.end(), client);
            vec.erase(indx,vec.end());
  
        }
        return true; // 成功移除
    }
    return false; // client 不存在
}

template<typename ClientIDType>
bool ClientMsgCenter<ClientIDType>::killAllClients()
{
    std::lock_guard<std::mutex> lock(clientCtrMtx);
    clients.clear();
    msgCenter.clear();
    return true;
}
template<typename ClientIDType>
bool ClientMsgCenter<ClientIDType>::notifyClients(ClientMsg clientMsg)
{
    std::lock_guard<std::mutex> lock(clientCtrMtx);
    if (mappingFunctions.find(clientMsg) == mappingFunctions.end())
    {
        //Subscrition string not defined
        return false;
    }
    if (msgCenter.find(clientMsg) == msgCenter.end())
    {
        //Subscription string does not exist
        return false;
    }

    if (msgCenter[clientMsg].size() == 0)
    {
        //No client found
        return false;
    }

    std::vector<std::shared_ptr<Client<ClientIDType>>> tempClients = msgCenter[clientMsg];
    auto sendToClient = mappingFunctions[clientMsg];
    std::vector<std::shared_ptr<Client<ClientIDType>>>errorClients;
    for (auto tempClientPtr : tempClients)
    {
        auto tempClient = tempClientPtr.get();
        if (checkClientError(tempClient))
            errorClients.push_back(tempClientPtr);
        else
            sendToClient(tempClient);
    }

    for (auto& tempClient : errorClients)
        killClient(tempClient, true);
    errorClients.clear();
    return false;
}
