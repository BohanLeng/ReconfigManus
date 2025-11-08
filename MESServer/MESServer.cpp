//
// Created by bohanleng on 28/10/2025.
//

#include "MESServer.h"
#include "LogMacros.h"
#include "mes_server_def.h"

MESServer::~MESServer()
{
}

bool MESServer::OnClientConnectionRequest(std::shared_ptr<TCPConn::ITCPConn<TCPConn::TCPMsg>> client)
{
    std::cout << "Client " << client->GetID() << " requested\n";
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
    switch (msg.header.type)
    {
        // TODO
        case MSG_STATION_ACTION_QUERY:
            {
                ST_StationActionQuery qry;
                msg >> qry;
                INFO_MSG("Action query received: workstation_id: {}, tray_id: {}", qry.workstation_id, qry.tray_id);
                ST_StationActionRsp rsp;
                rsp.qry = qry;
                rsp.action_type = 1;
                TCPConn::TCPMsg rsp_msg;
                rsp_msg.header.type = MSG_STATION_ACTION_RSP;
                rsp_msg << rsp;
                client->Send(rsp_msg);
            }
            break;
        case MSG_STATION_ACTION_DONE_QUERY:
            {
                ST_StationActionQuery qry;
                msg >> qry;
                INFO_MSG("Action done query received: workstation_id: {}, tray_id: {}", qry.workstation_id, qry.tray_id);
                ST_StationActionRsp rsp;
                rsp.qry.workstation_id = qry.workstation_id;
                rsp.qry.tray_id = qry.tray_id;
                rsp.action_type = 0;
                if (qry.workstation_id == 1)
                    rsp.next_station_id = 2;
                else
                    rsp.next_station_id = 1;
                TCPConn::TCPMsg rsp_msg;
                rsp_msg.header.type = MSG_STATION_ACTION_RSP;
                rsp_msg << rsp;
                client->Send(rsp_msg);
            }
            break;
        default:
            break;
    }

}
