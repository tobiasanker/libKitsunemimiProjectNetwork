﻿/**
 *  @file       session_test.h
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

#ifndef SESSION_TEST_H
#define SESSION_TEST_H

#include <iostream>
#include <libKitsunePersistence/logger/logger.h>
#include <libKitsuneProjectCommon/network_session/session_controller.h>
#include <network_session/session_handler.h>
#include <libKitsuneProjectCommon/network_session/session.h>
#include <libKitsuneNetwork/abstract_socket.h>

#include <libKitsuneCommon/unit_test.h>

namespace Kitsune
{
namespace Project
{
namespace Common
{

class Session_Test
        : public Kitsune::Common::UnitTest
{
public:
    Session_Test();

    void initTestCase();
    void runTest();

    template<typename  T>
    void compare(T isValue, T shouldValue)
    {
        UNITTEST(isValue, shouldValue);
    }

    std::string m_staticMessage = "";
    std::string m_dynamicMessage = "";
    std::string m_multiBlockMessage = "";
};

} // namespace Common
} // namespace Project
} // namespace Kitsune

#endif // SESSION_TEST_H