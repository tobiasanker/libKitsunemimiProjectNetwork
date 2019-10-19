/**
 *  @file       message_definitions.h
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

#ifndef MESSAGE_DEFINITIONS_H
#define MESSAGE_DEFINITIONS_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <network_session/ressource_handler.h>
#include <network_session/timer_thread.h>

namespace Kitsune
{
namespace Project
{
namespace Common
{

enum types
{
    UNDEFINED_TYPE = 0,
    SESSION_TYPE = 1,
    HEARTBEAT_TYPE = 2,
    ERROR_TYPE = 3,
    DATA_TYPE = 4,
    SUB_PROTOCOL_TYPE = 5,
};

enum session_subTypes
{
    SESSION_INIT_START_SUBTYPE = 1,
    SESSION_INIT_REPLY_SUBTYPE = 2,

    SESSION_CLOSE_START_SUBTYPE = 3,
    SESSION_CLOSE_REPLY_SUBTYPE = 4,
};

enum heartbeat_subTypes
{
    HEARTBEAT_START_SUBTYPE = 1,
    HEARTBEAT_REPLY_SUBTYPE = 2,
};

enum error_subTypes
{
    ERROR_FALSE_VERSION_SUBTYPE = 1,
    ERROR_UNKNOWN_SESSION_SUBTYPE = 2,
    ERROR_INVALID_MESSAGE_SUBTYPE = 3,
};

enum data_subTypes
{
    DATA_PLAIN_SUBTYPE = 1,
    DATA_REPLY_SUBTYPE = 2,
};

//==================================================================================================

struct CommonMessageHeader
{
    uint8_t version = 0x1;
    uint8_t type = 0;
    uint8_t subType = 0;
    uint8_t flags = 0;   // 0x1 = Reply required
    uint32_t messageId = 0;
    uint32_t sessionId = 0;
    uint32_t size = 0;
} __attribute__((packed));

struct CommonMessageEnd
{
    const uint32_t end = 1314472257;
} __attribute__((packed));

//==================================================================================================

struct Session_Init_Start_Message
{
    CommonMessageHeader commonHeader;
    uint32_t clientSessionId = 0;
    CommonMessageEnd commonEnd;

    Session_Init_Start_Message(const uint32_t sessionId,
                               const uint32_t messageId)
    {
        commonHeader.type = SESSION_TYPE;
        commonHeader.subType = SESSION_INIT_START_SUBTYPE;
        commonHeader.sessionId = sessionId;
        commonHeader.messageId = messageId;
        commonHeader.flags = 0x1;
        commonHeader.size = sizeof(*this);

        RessourceHandler::m_timerThread->addMessage(commonHeader.type,
                                                    commonHeader.sessionId,
                                                    commonHeader.messageId);
    }
} __attribute__((packed));

struct Session_Init_Reply_Message
{
    CommonMessageHeader commonHeader;
    uint32_t clientSessionId = 0;
    uint32_t completeSessionId = 0;
    CommonMessageEnd commonEnd;

    Session_Init_Reply_Message(const uint32_t sessionId,
                               const uint32_t messageId)
    {
        commonHeader.type = SESSION_TYPE;
        commonHeader.subType = SESSION_INIT_REPLY_SUBTYPE;
        commonHeader.sessionId = sessionId;
        commonHeader.messageId = messageId;
        commonHeader.size = sizeof(*this);
    }
} __attribute__((packed));

//==================================================================================================

struct Session_Close_Start_Message
{
    CommonMessageHeader commonHeader;
    uint32_t sessionId = 0;
    CommonMessageEnd commonEnd;

    Session_Close_Start_Message(const uint32_t sessionId,
                                const uint32_t messageId,
                                const bool replyExpected = false)
    {
        commonHeader.type = SESSION_TYPE;
        commonHeader.subType = SESSION_CLOSE_START_SUBTYPE;
        commonHeader.sessionId = sessionId;
        commonHeader.messageId = messageId;
        commonHeader.size = sizeof(*this);

        if(replyExpected)
        {
            commonHeader.flags = 0x1;
            RessourceHandler::m_timerThread->addMessage(commonHeader.type,
                                                        commonHeader.sessionId,
                                                        commonHeader.messageId);
        }
    }
} __attribute__((packed));

struct Session_Close_Reply_Message
{
    CommonMessageHeader commonHeader;
    uint32_t sessionId = 0;
    CommonMessageEnd commonEnd;

    Session_Close_Reply_Message(const uint32_t sessionId,
                                const uint32_t messageId)
    {
        commonHeader.type = SESSION_TYPE;
        commonHeader.subType = SESSION_CLOSE_REPLY_SUBTYPE;
        commonHeader.sessionId = sessionId;
        commonHeader.messageId = messageId;
        commonHeader.size = sizeof(*this);
    }
} __attribute__((packed));

//==================================================================================================

struct Heartbeat_Start_Message
{
    CommonMessageHeader commonHeader;
    CommonMessageEnd commonEnd;

    Heartbeat_Start_Message(const uint32_t sessionId,
                            const uint32_t messageId)
    {
        commonHeader.type = HEARTBEAT_TYPE;
        commonHeader.subType = HEARTBEAT_START_SUBTYPE;
        commonHeader.sessionId = sessionId;
        commonHeader.messageId = messageId;
        commonHeader.flags = 0x1;
        commonHeader.size = sizeof(*this);

        RessourceHandler::m_timerThread->addMessage(commonHeader.type,
                                                    commonHeader.sessionId,
                                                    commonHeader.messageId);
    }
} __attribute__((packed));

struct Heartbeat_Reply_Message
{
    CommonMessageHeader commonHeader;
    CommonMessageEnd commonEnd;

    Heartbeat_Reply_Message(const uint32_t sessionId,
                            const uint32_t messageId)
    {
        commonHeader.type = HEARTBEAT_TYPE;
        commonHeader.subType = HEARTBEAT_REPLY_SUBTYPE;
        commonHeader.sessionId = sessionId;
        commonHeader.messageId = messageId;
        commonHeader.size = sizeof(*this);
    }
} __attribute__((packed));

//==================================================================================================

struct Error_FalseVersion_Message
{
    CommonMessageHeader commonHeader;
    char message[500];
    uint64_t messageSize;
    CommonMessageEnd commonEnd;

    Error_FalseVersion_Message(const uint32_t sessionId,
                               const uint32_t messageId,
                               const std::string errorMessage)
    {
        commonHeader.type = ERROR_TYPE;
        commonHeader.subType = ERROR_FALSE_VERSION_SUBTYPE;
        commonHeader.sessionId = sessionId;
        commonHeader.messageId = messageId;
        commonHeader.size = sizeof(*this);

        messageSize = errorMessage.size();
        if(messageSize > 499) {
            messageSize = 499;
        }
        strncpy(message, errorMessage.c_str(), messageSize);
    }
} __attribute__((packed));

struct Error_UnknownSession_Message
{
    CommonMessageHeader commonHeader;
    char message[500];
    uint64_t messageSize;
    CommonMessageEnd commonEnd;

    Error_UnknownSession_Message(const uint32_t sessionId,
                                 const uint32_t messageId,
                                 const std::string errorMessage)
    {
        commonHeader.type = ERROR_TYPE;
        commonHeader.subType = ERROR_UNKNOWN_SESSION_SUBTYPE;
        commonHeader.sessionId = sessionId;
        commonHeader.messageId = messageId;
        commonHeader.size = sizeof(*this);

        messageSize = errorMessage.size();
        if(messageSize > 499) {
            messageSize = 499;
        }
        strncpy(message, errorMessage.c_str(), messageSize);
    }
} __attribute__((packed));

struct Error_InvalidMessage_Message
{
    CommonMessageHeader commonHeader;
    char message[500];
    uint64_t messageSize;
    CommonMessageEnd commonEnd;

    Error_InvalidMessage_Message(const uint32_t sessionId,
                                 const uint32_t messageId,
                                 const std::string errorMessage)
    {
        commonHeader.type = ERROR_TYPE;
        commonHeader.subType = ERROR_INVALID_MESSAGE_SUBTYPE;
        commonHeader.sessionId = sessionId;
        commonHeader.messageId = messageId;
        commonHeader.size = sizeof(*this);

        messageSize = errorMessage.size();
        if(messageSize > 499) {
            messageSize = 499;
        }
        strncpy(message, errorMessage.c_str(), messageSize);
    }
} __attribute__((packed));

//==================================================================================================

} // namespace Common
} // namespace Project
} // namespace Kitsune

#endif // MESSAGE_DEFINITIONS_H