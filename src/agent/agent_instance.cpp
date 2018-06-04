/*
 *    Copyright (c) 2017, The OpenThread Authors.
 *    All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *    POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file includes implementation for Thread border router agent instance.
 */

#include "agent_instance.hpp"

#include <assert.h>

#include "common/code_utils.hpp"
#include "common/logging.hpp"

namespace ot {

namespace BorderRouter {

AgentInstance::AgentInstance(const char *aIfName) :
    mNcp(Ncp::Controller::Create(aIfName)),
    mBorderAgent(mNcp) {}

otbrError AgentInstance::Init(void)
{
    otbrError error = OTBR_ERROR_NONE;

    SuccessOrExit(error = mNcp->Init());

    SuccessOrExit(error = mNcp->UdpProxyStart());

    SuccessOrExit(error = mBorderAgent.Start());

exit:
    if (error != OTBR_ERROR_NONE)
    {
        otbrLog(OTBR_LOG_ERR, "Failed to create border route agent instance: %d!", error);
    }

    return error;
}

void AgentInstance::UpdateFdSet(fd_set &aReadFdSet, fd_set &aWriteFdSet, fd_set &aErrorFdSet, int &aMaxFd,
                                timeval &aTimeout)
{
    mNcp->UpdateFdSet(aReadFdSet, aWriteFdSet, aErrorFdSet, aMaxFd);
    mBorderAgent.UpdateFdSet(aReadFdSet, aWriteFdSet, aErrorFdSet, aMaxFd, aTimeout);
}

void AgentInstance::Process(const fd_set &aReadFdSet, const fd_set &aWriteFdSet, const fd_set &aErrorFdSet)
{
    mNcp->Process(aReadFdSet, aWriteFdSet, aErrorFdSet);
    mBorderAgent.Process(aReadFdSet, aWriteFdSet, aErrorFdSet);
}

AgentInstance::~AgentInstance(void)
{
    otbrError error = OTBR_ERROR_NONE;

    if ((error = mNcp->UdpProxyStop()))
    {
        otbrLog(OTBR_LOG_ERR, "Failed to stop TMF proxy: %d!", error);
    }

    Ncp::Controller::Destroy(mNcp);
}

} // namespace BorderRouter

} // namespace ot
