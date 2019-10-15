/**
 *  @file       session_processing.h
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

#ifndef SESSION_INIT_PROCESSING_H
#define SESSION_INIT_PROCESSING_H

#include <libKitsuneProjectCommon/network_session/session_handler.h>
#include <network_session/messages/message_definitions.h>
#include <libKitsuneNetwork/abstract_socket.h>

#include <libKitsunePersistence/logger/logger.h>

using Kitsune::Network::AbstractSocket;

namespace Kitsune
{
namespace Project
{
namespace Common
{

/**
 * @brief sendSession_InitStart
 * @param initialId
 * @param socket
 */
inline void
send_Session_Init_Start(const uint32_t initialId,
                        Network::AbstractSocket* socket)
{
    LOG_DEBUG("SEND session init start");

    // create message
    Session_Init_Start_Message message;
    message.clientSessionId = initialId;

    // update common-header
    message.commonHeader.sessionId = initialId;
    message.commonHeader.messageId = SessionHandler::m_sessionHandler->increaseMessageIdCounter();

    // send
    socket->sendMessage(&message, sizeof(message));
}

/**
 * @brief sendSessionIniReply
 * @param id
 * @param socket
 */
inline void
send_Session_Init_Reply(const uint32_t id,
                        const uint32_t clientSessionId,
                        Network::AbstractSocket* socket)
{
    LOG_DEBUG("SEND session init reply");

    Session_Init_Reply_Message message;
    message.completeSessionId = id;
    message.clientSessionId = clientSessionId;

    // update common-header
    message.commonHeader.sessionId = id;
    message.commonHeader.messageId = SessionHandler::m_sessionHandler->increaseMessageIdCounter();

    // send
    socket->sendMessage(&message, sizeof(message));
}

/**
 * @brief sendSession_Close_Start
 * @param id
 * @param replyRequired
 * @param socket
 */
inline void
send_Session_Close_Start(const uint32_t id,
                         bool replyExpected,
                         Network::AbstractSocket* socket)
{
    LOG_DEBUG("SEND session close start");

    Session_Close_Start_Message message(replyExpected);
    message.sessionId = id;

    // update common-header
    message.commonHeader.sessionId = id;
    message.commonHeader.messageId = SessionHandler::m_sessionHandler->increaseMessageIdCounter();

    // send
    socket->sendMessage(&message, sizeof(message));
}

/**
 * @brief sendSession_Close_Reply
 * @param id
 * @param socket
 */
inline void
send_Session_Close_Reply(const uint32_t id,
                         Network::AbstractSocket* socket)
{
    LOG_DEBUG("SEND session close reply");

    Session_Close_Reply_Message message;
    message.sessionId = id;

    // update common-header
    message.commonHeader.sessionId = id;
    message.commonHeader.messageId = SessionHandler::m_sessionHandler->increaseMessageIdCounter();

    // send
    socket->sendMessage(&message, sizeof(message));
}

/**
 * @brief process_Session_Init_Start
 */
inline void
process_Session_Init_Start(const Session_Init_Start_Message* message,
                          AbstractSocket* socket)
{
    LOG_DEBUG("process session init start");

    const uint32_t clientSessionId = message->clientSessionId;
    const uint16_t serverSessionId = SessionHandler::m_sessionHandler->increaseSessionIdCounter();
    const uint32_t completeSessionId = clientSessionId + (serverSessionId * 0x10000);

    // create new session
    Session* newSession = new Session(socket);
    newSession->sessionId = completeSessionId;
    newSession->connect();

    // confirm id
    send_Session_Init_Reply(completeSessionId, clientSessionId, socket);

    // try to finish session
    const bool ret = newSession->startSession();
    if(ret) {
        SessionHandler::m_sessionHandler->addSession(completeSessionId, newSession);
    } else {
        // TODO: error message
    }
}

/**
 * @brief process_Session_Init_Reply
 */
inline void
process_Session_Init_Reply(const Session_Init_Reply_Message* message,
                          AbstractSocket* socket)
{
    LOG_DEBUG("process session init reply");

    // get session
    const uint32_t completeSessionId = message->completeSessionId;
    const uint32_t initialId = message->clientSessionId;

    Session* session = SessionHandler::m_sessionHandler->removeSession(initialId);

    if(session != nullptr)
    {
        session->sessionId = completeSessionId;

        // try to finish session
        const bool ret = session->startSession();
        if(ret) {
            SessionHandler::m_sessionHandler->addSession(completeSessionId, session);
        } else {
            // TODO: error message
        }
    }
}

/**
 * @brief process_Session_Close_Start
 */
inline void
process_Session_Close_Start(const Session_Close_Start_Message* message,
                            AbstractSocket* socket)
{
    LOG_DEBUG("process session close start");

    const uint32_t sessionId = message->sessionId;

    Session* session = SessionHandler::m_sessionHandler->removeSession(sessionId);

    if(session != nullptr)
    {
        const bool ret = session->closeSession();
        if(ret) {
            send_Session_Close_Reply(sessionId, socket);
        } else {
            // TODO: error message
        }

        // TODO: better delete
        session->disconnect();
        delete session;
    }
}

/**
 * @brief process_Session_Close_Reply
 */
inline void
process_Session_Close_Reply(const Session_Close_Reply_Message* message,
                            AbstractSocket* socket)
{
    LOG_DEBUG("process session close reply");

    const uint32_t sessionId = message->sessionId;
    Session* session = SessionHandler::m_sessionHandler->removeSession(sessionId);

    if(session != nullptr)
    {
        // TODO: better delete
        session->disconnect();
        delete session;
    }
}

} // namespace Common
} // namespace Project
} // namespace Kitsune

#endif // SESSION_INIT_PROCESSING_H
