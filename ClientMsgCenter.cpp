#include"ClientMsgCenter.h"
#include<iostream>



bool ClientMsgCenter::enroll(std::shared_ptr<Client>client)
{
    std::lock_guard<std::mutex> lock(clientCtrMtx);
    if (clients.find(client) == clients.end())
    {
        clients[client] = std::vector<ClientMsg>();
        return true; // 成功註冊
    }
    return false; // client 已經存在
}



bool ClientMsgCenter::notifyClients(ClientMsg clientMsg)
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

    std::vector<std::shared_ptr<Client>> tempClients = msgCenter[clientMsg];
    auto sendToClient = mappingFunctions[clientMsg];
    std::vector<std::shared_ptr<Client>>errorClients;
    for (auto tempClientPtr : tempClients)
    {
        auto tempClient = tempClientPtr.get();
        if (isClientError(tempClient))
            errorClients.push_back(tempClientPtr);
        else
            sendToClient(tempClient);
    }

    for (auto& tempClient : errorClients)
        killClient(tempClient, true);
    errorClients.clear();
    return false;
}

bool ClientMsgCenter::killClient(std::shared_ptr<Client>client, bool IsLocked)
{
    if (!IsLocked)
        std::lock_guard<std::mutex> lock(clientCtrMtx);
    auto it = clients.find(client);
    if (it != clients.end())
    {
        clients.erase(it);
        for (auto& pair : msgCenter)
        {
            auto& vec = pair.second;
            auto indx = std::remove(vec.begin(), vec.end(), client);
            vec.erase(indx, vec.end());

        }
        return true; // 成功移除
    }
    return false; // client 不存在
}


bool ClientMsgCenter::subscribe(std::shared_ptr<Client>client, ClientMsg msg)
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


bool ClientMsgCenter::isClientError(Client* client)
{
    return true;

}





ClientMsgCenter::ClientMsgCenter()
{

}
 ClientMsgCenter::~ClientMsgCenter()
{
    killAllClients();
}



Client::~Client()
{

}




bool ClientMsgCenter::killAllClients()
{
    std::lock_guard<std::mutex> lock(clientCtrMtx);
    clients.clear();
    msgCenter.clear();
    return true;
}



Client::Client(std::string id) :ID(id)
{

}

void Client::dummy()
{

}


//template<typename ClientIDType> ClientMsgCenter<ClientIDType>::~ClientMsgCenter()
//{
//    killAllClients();
//}
//
//
//template<typename ClientIDType>
//Client<ClientIDType>::~Client()
//{
//
//}