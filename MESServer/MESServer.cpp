//
// Created by bohanleng on 28/10/2025.
//

#include "MESServer.h"

MESServer::~MESServer()
{
}

bool MESServer::OnClientConnectionRequest(std::shared_ptr<TCPConn::ITCPConn<TCPConn::TCPMsg>> client)
{
    return true;
}

void MESServer::OnClientConnected(std::shared_ptr<TCPConn::ITCPConn<TCPConn::TCPMsg>> client)
{
    std::cout << "Client " << client->GetID() << " connected\n";
}

void MESServer::OnClientDisconnected(std::shared_ptr<TCPConn::ITCPConn<TCPConn::TCPMsg>> client)
{
    ITCPServer<TCPConn::TCPMsg>::OnClientDisconnected(client);
}

void MESServer::OnMessage(std::shared_ptr<TCPConn::ITCPConn<TCPConn::TCPMsg>> client, TCPConn::TCPMsg& msg)
{
    std::cout << "Message received from client " << client->GetID() << "\n";
    std::cout << msg << std::endl;
}
