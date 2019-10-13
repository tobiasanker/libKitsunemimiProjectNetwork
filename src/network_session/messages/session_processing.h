/**
 *  @file       session_init_processing.h
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
#include <network_session/messages/message_creation.h>
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
 * @brief process_Session_Init_Start
 */
inline void
process_Session_Init_Start(const Session_Init_Start_Message* message,
                          AbstractSocket* socket)
{
    LOG_DEBUG("process session init start");

    const uint32_t sessionId = message->offeredSessionId;
    if(SessionHandler::m_sessionHandler->isIdUsed(sessionId))
    {
        // get new sesstion id
        const uint32_t newSessionId = SessionHandler::m_sessionHandler->increaseSessionIdCounter();

        // create new session
        Session* newSession = new Session(socket);
        newSession->sessionId = newSessionId;
        newSession->connect();

        // send id-change-request
        sendSession_Init_IdChange(sessionId, newSessionId, socket);
        SessionHandler::m_sessionHandler->addPendingSession(newSessionId, newSession);
    }
    else
    {
        // create new session
        Session* newSession = new Session(socket);
        newSession->sessionId = sessionId;
        newSession->connect();

        // confirm id
        sendSession_Init_Reply(sessionId, socket);

        // try to finish session
        const bool ret = newSession->startSession();
        if(ret) {
            SessionHandler::m_sessionHandler->addSession(sessionId, newSession);
        } else {
            // TODO: error message
        }
    }
}

/**
 * @brief process_Session_Init_IdChange
 */
inline void
process_Session_Init_IdChange(const Session_Init_IdChange_Message* message,
                             AbstractSocket* socket)
{
    LOG_DEBUG("process session id change");

    const uint32_t sessionId = message->newOfferedSessionId;
    const uint32_t oldSessionId = message->oldOfferedSessionId;

    if(SessionHandler::m_sessionHandler->isIdUsed(sessionId))
    {
        // get new session-id and send it to the other side to get reply
        const uint32_t newSessionId = SessionHandler::m_sessionHandler->increaseSessionIdCounter();
        sendSession_Init_IdChange(sessionId, newSessionId, socket);

        // move session from the pending list to the final list
        Session* session = SessionHandler::m_sessionHandler->removePendingSession(oldSessionId);
        SessionHandler::m_sessionHandler->addPendingSession(newSessionId, session);
    }
    else
    {
        // move session from the pending list to the final list
        Session* session = SessionHandler::m_sessionHandler->removePendingSession(oldSessionId);
        SessionHandler::m_sessionHandler->addSession(sessionId, session);
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
    const uint32_t sessionId = message->sessionId;
    Session* session = SessionHandler::m_sessionHandler->removePendingSession(sessionId);

    // try to finish session
    const bool ret = session->startSession();
    if(ret) {
        SessionHandler::m_sessionHandler->addSession(sessionId, session);
    } else {
        // TODO: error message
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

    const bool ret = session->closeSession();
    if(ret) {
        sendSession_Close_Reply(sessionId, socket);
    } else {
        // TODO: error message
    }

    session->disconnect();
    delete session;
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

    sendSession_Close_Reply(sessionId, socket);

    session->disconnect();
    delete session;
}

} // namespace Common
} // namespace Project
} // namespace Kitsune

#endif // SESSION_INIT_PROCESSING_H