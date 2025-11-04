//
// Created by bohanleng on 28/10/2025.
//

#ifndef RECONFIGMANUS_MESSERVER_H
#define RECONFIGMANUS_MESSERVER_H

#include "TCPServer.h"
#include "GraphManager/GraphManager.h"

class MESServer : public TCPConn::ITCPServer<TCPConn::TCPMsg>
{
public:
    explicit MESServer(uint16_t port)
        : ITCPServer<TCPConn::TCPMsg>(port)
    {
    }

    ~MESServer() override;
    bool OnClientConnectionRequest(std::shared_ptr<TCPConn::ITCPConn<TCPConn::TCPMsg>> client) override;
    void OnClientConnected(std::shared_ptr<TCPConn::ITCPConn<TCPConn::TCPMsg>> client) override;
    void OnClientDisconnected(std::shared_ptr<TCPConn::ITCPConn<TCPConn::TCPMsg>> client) override;
    void OnMessage(std::shared_ptr<TCPConn::ITCPConn<TCPConn::TCPMsg>> client, TCPConn::TCPMsg& msg) override;
};


#endif //RECONFIGMANUS_MESSERVER_H