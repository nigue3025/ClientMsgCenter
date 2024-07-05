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
    std::map<std::string, std::shared_ptr<Client>> clients; //�x�s�Hclient ID��������client������map
    std::map<std::shared_ptr<Client>, std::vector<MsgCtrClientMsg>> clients_sublst;//�x�s�C��client�q�\���A�Ȧr��
    std::map<MsgCtrClientMsg, std::vector<std::shared_ptr<Client>>> msgCenter; //�x�s�C�ӪA�Ȧr�������client
    std::unordered_map<MsgCtrClientMsg, std::function<bool(Client*)>> subscriptionReponseFunctions; //�q�\�A�Ȧr��P�������A��function,����library(API)�ϥΪ̪`�J�H��{�\��
    std::unordered_map<MsgCtrClientMsg, std::function<bool(Client*)>> queryResponseFunctions; //�d�ߪA�Ȧr��P�������A��function,����library(API)�ϥΪ̪`�J�H��{�\��
    bool BlockNotDefinedSubMsg = true; //Flag: �O�_���\�|���w�q���r��i������client�q�\ 
    bool enroll(std::shared_ptr<Client>);
    bool subscribe(std::shared_ptr<Client>, MsgCtrClientMsg); //�q�\�A��
    bool unsubscribe(std::shared_ptr<Client>, MsgCtrClientMsg); //�����q�\�A��
    bool killClient(std::shared_ptr<Client>, bool IsLocked = false); //�R���S�wclient
    bool killAllClients(); //�R���Ҧ�client
    bool responseQuery(std::shared_ptr<Client>, MsgCtrClientMsg); //�d�ߪA��

    virtual bool isClientError(Client*) = 0; //���ѹ�@,����client���ĩ��_�u���P�_�覡
    virtual bool notifyClients(MsgCtrClientMsg);
    std::string getLastError();

    ClientMsgCenter();
    ~ClientMsgCenter();

    static std::string HWNDToString(HWND hwnd);

protected:
    virtual bool notifyClients_internal(const MsgCtrClientMsg&); //�̾ڭq�\��T�P�w�O�_�n�ǰe�T�����q�\��client
    std::string LastErrorStr = ""; //���~message�x�s�r��
};










