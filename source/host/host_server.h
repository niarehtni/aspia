//
// Aspia Project
// Copyright (C) 2019 Dmitry Chapyshev <dmitry@aspia.ru>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.
//

#ifndef HOST__HOST_SERVER_H
#define HOST__HOST_SERVER_H

#include "base/win/session_status.h"
#include "host/win/host_process.h"
#include "ipc/ipc_channel.h"
#include "net/network_server.h"

namespace host {

class SessionProcess;
class UiServer;
struct SrpUserList;

class HostServer : public QObject
{
    Q_OBJECT

public:
    HostServer(QObject* parent = nullptr);
    ~HostServer();

    void start();
    void stop();
    void setSessionEvent(base::win::SessionStatus status, base::win::SessionId session_id);

signals:
    void sessionEvent(base::win::SessionStatus status, base::win::SessionId session_id);

protected:
    // QObject implementation.
    void customEvent(QEvent* event) override;

private slots:
    void onNewConnection();
    void onHostFinished(SessionProcess* host);

private:
    QPointer<UiServer> ui_server_;

    // Accepts incoming network connections.
    QPointer<net::Server> network_server_;

    // Contains a list of connected sessions.
    std::list<SessionProcess*> session_list_;

    DISALLOW_COPY_AND_ASSIGN(HostServer);
};

} // namespace host

#endif // HOST__HOST_SERVER_H
