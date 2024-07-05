#include"ClientMsgCenter.h"
#include<iostream>
#include<string>
#include<sstream>
#ifdef _WIN32
#include<windows.h>
#endif


bool ClientMsgCenter::enroll(std::shared_ptr<Client>client)
{
    std::lock_guard<std::mutex> lock(clientCtrMtx);
    if (clients_sublst.find(client) == clients_sublst.end())
    {
        clients_sublst[client] = std::vector<MsgCtrClientMsg>();
        return true; // 成功註冊
    }
    return false; // client 已經存在
}

std::string ClientMsgCenter::getLastError()
{
    std::string tmpStr = LastErrorStr;
    LastErrorStr.clear();
    return tmpStr;
}

bool  ClientMsgCenter::responseQuery(std::shared_ptr<Client>client, MsgCtrClientMsg clientMsg)
{
    if (queryResponseFunctions.find(clientMsg) == queryResponseFunctions.end())
    {
        LastErrorStr = "Queried clientMsg not found:" + clientMsg;
        return false;
    }

    return queryResponseFunctions[clientMsg](client.get());


}

bool ClientMsgCenter::notifyClients_internal(const MsgCtrClientMsg& clientMsg)
{
    LastErrorStr.clear();
    std::lock_guard<std::mutex> lock(clientCtrMtx);
    if (subscriptionReponseFunctions.find(clientMsg) == subscriptionReponseFunctions.end())
    {
        LastErrorStr = "Subscrition string not defined: " + clientMsg;
        return false;
    }
    if (msgCenter.find(clientMsg) == msgCenter.end())
    {
        LastErrorStr = "No client has even subscribed:" + clientMsg;
        return false;
    }

    if (msgCenter[clientMsg].size() == 0)
    {
        LastErrorStr = "No client found with the corresponding subscription string:" + clientMsg;
        return false;
    }

    std::vector<std::shared_ptr<Client>> tempClients = msgCenter[clientMsg];
    auto sendToClient = subscriptionReponseFunctions[clientMsg];
    std::vector<std::shared_ptr<Client>>errorClients;
    for (auto tempClientPtr : tempClients)
    {
        auto tempClient = tempClientPtr.get();
        if (isClientError(tempClient))
        {

            errorClients.push_back(tempClientPtr);
        }
        else
            sendToClient(tempClient);
    }

    for (auto& tempClient : errorClients)
        killClient(tempClient, true);
    errorClients.clear();
    return true;
}

bool ClientMsgCenter::notifyClients(MsgCtrClientMsg clientMsg)
{
    return notifyClients_internal(clientMsg);
}

bool ClientMsgCenter::killClient(std::shared_ptr<Client>client, bool IsLocked)
{
    if (!IsLocked)
        std::lock_guard<std::mutex> lock(clientCtrMtx);
    auto it = clients_sublst.find(client);
    if (it != clients_sublst.end())
    {
        clients_sublst.erase(it);
        for (auto& pair : msgCenter)
        {
            auto& vec = pair.second;
            auto indx = std::remove(vec.begin(), vec.end(), client);
            vec.erase(indx, vec.end());

        }

    }
    else
    {
        LastErrorStr = "Kill client failure. The desired client:" + client->ID + " does not exist in clients_sublst";
        return false;
    }

    if (clients.find(client->ID) != clients.end())
        clients.erase(client->ID);
    else
    {
        LastErrorStr = "Kill client failure. The desired client:" + client->ID + " does not exist in client list";
        return false;
    }

    return true; // client 不存在
}


bool ClientMsgCenter::subscribe(std::shared_ptr<Client>theClient, MsgCtrClientMsg msg)
{
    std::lock_guard<std::mutex> lock(clientCtrMtx);

    if (BlockNotDefinedSubMsg)
    {

        if (subscriptionReponseFunctions.find(msg) == subscriptionReponseFunctions.end())
        {
            LastErrorStr = "Subscription message not permitted:" + msg;
            return false;
        }
    }

    if (clients.find(theClient->ID) == clients.end())
    {
        clients[theClient->ID] = theClient;
        clients_sublst[theClient] = std::vector<MsgCtrClientMsg>();

    }
    auto client = clients[theClient->ID];
    auto client_sub = &clients_sublst[client];
    std::vector<MsgCtrClientMsg>::iterator itr;
    if ((itr = std::find(client_sub->begin(), client_sub->end(), msg)) == client_sub->end())
    {
        client_sub->push_back(msg);
        msgCenter[msg].push_back(client);
        return true;
    }
    else
    {
        LastErrorStr = "Repeat Subscription";
    }
    //

    return false;
}

bool ClientMsgCenter::unsubscribe(std::shared_ptr<Client> theClient, MsgCtrClientMsg msg)
{
    std::lock_guard<std::mutex> lock(clientCtrMtx);
    if (clients.find(theClient->ID) == clients.end())
    {
        LastErrorStr = "The client:" + theClient->ID + "not found";
        return false; //The client not found
    }
    auto client = clients[theClient->ID];


    auto client_sub = &clients_sublst[client];
    std::vector<MsgCtrClientMsg>::iterator itr;
    if ((itr = std::find(client_sub->begin(), client_sub->end(), msg)) != client_sub->end())
    {
        client_sub->erase(itr);

        //client_sub->push_back(msg);
        auto clientLst = &msgCenter[msg];

        std::vector<std::shared_ptr<Client>>::iterator clItr;
        if ((clItr = std::find(clientLst->begin(), clientLst->end(), client)) != clientLst->end())
        {
            clientLst->erase(clItr);
            return true;
        }
        else
        {
            LastErrorStr = "The client:" + client->ID + " is not found in msgCenter";
        }

    }
    else
    {
        LastErrorStr = "The client:" + client->ID + " is not found in clients_sublst";
    }

    return false;
}


bool ClientMsgCenter::isClientError(Client* client)
{
    LastErrorStr = "Virtual function isClientError(Client* client) is not implemented! Should be declared override in derived class";
    return true;

}
#ifdef _WIN32
std::string ClientMsgCenter::HWNDToString(HWND hwnd)
{
    std::stringstream ss;
    ss << "0x" << hwnd;
    return ss.str();
}

#endif 



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
    clients_sublst.clear();
    msgCenter.clear();
    clients.clear();
    return true;
}



Client::Client(std::string id) :ID(id)
{

}

void Client::dummy()
{

}
