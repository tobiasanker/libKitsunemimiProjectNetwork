/**
 *  @file       session_handler.h
 *
 *  @author     Tobias Anker <tobias.anker@kitsunemimi.moe>
 *
 *  @copyright  Apache License Version 2.0
 *
 *      Copyright 2019 Tobias Anker
 *
 *      Licensed under the Apache License, Version 2.0 (the "License");
 *      you may not use this file except in compliance with the License.
 *      You may obtain a copy of the License at
 *
 *          http://www.apache.org/licenses/LICENSE-2.0
 *
 *      Unless required by applicable law or agreed to in writing, software
 *      distributed under the License is distributed on an "AS IS" BASIS,
 *      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *      See the License for the specific language governing permissions and
 *      limitations under the License.
 */

#ifndef SESSION_HANDLER_H
#define SESSION_HANDLER_H

#include <iostream>
#include <vector>
#include <map>
#include <atomic>

#include <libKitsuneProjectCommon/network_session/session.h>

namespace Kitsune
{
namespace Network {
class AbstractServer;
}
namespace Project
{
namespace Common
{
class TimerThread;

class SessionHandler
{
public:
    SessionHandler(void* sessionTarget,
                   void (*processSession)(void*, Session*),
                   void* dataTarget,
                   void (*processData)(void*, const uint32_t, void*, const uint32_t),
                   void* errorTarget,
                   void (*processError)(void*, const uint32_t, const uint8_t, const std::string));
    ~SessionHandler();

    static Kitsune::Project::Common::TimerThread* m_timerThread;
    static Kitsune::Project::Common::SessionHandler* m_sessionHandler;

    // server
    uint32_t addUnixDomainServer(const std::string socketFile);
    uint32_t addTcpServer(const uint16_t port);
    uint32_t addTlsTcpServer(const uint16_t port,
                             const std::string certFile,
                             const std::string keyFile);
    bool closeServer(const uint32_t id);
    Network::AbstractServer* getServer(const uint32_t id);

    // session
    void startUnixDomainSession(const std::string socketFile);
    void startTcpSession(const std::string address,
                         const uint16_t port);
    void startTlsTcpSession(const std::string address,
                            const uint16_t port,
                            const std::string certFile,
                            const std::string keyFile);
    bool closeSession(const uint32_t id);
    Session* getSession(const uint32_t id);
    bool isIdUsed(const uint32_t id);


    // (for internal usage)
    void addSession(const uint32_t id, Session* session);
    void confirmSession(const uint32_t id);
    Session* removeSession(const uint32_t id);

    uint32_t increaseMessageIdCounter();
    uint16_t increaseSessionIdCounter();

private:
    // object-holder
    std::map<uint32_t, Session*> m_sessions;
    std::map<uint32_t, Network::AbstractServer*> m_servers;

    // callback-parameter
    void* m_sessionTarget = nullptr;
    void (*m_processSession)(void*, Session*);
    void* m_dataTarget = nullptr;
    void (*m_processData)(void*, const uint32_t, void*, const uint32_t);
    void* m_errorTarget = nullptr;
    void (*m_processError)(void*, const uint32_t, const uint8_t, const std::string);

    // counter
    std::atomic_flag m_messageIdCounter_lock = ATOMIC_FLAG_INIT;
    uint32_t m_messageIdCounter = 0;
    std::atomic_flag m_sessionIdCounter_lock = ATOMIC_FLAG_INIT;
    uint16_t m_sessionIdCounter = 0;
    uint32_t m_serverIdCounter = 0;
};

} // namespace Common
} // namespace Project
} // namespace Kitsune

#endif // SESSION_HANDLER_H
