/*
 *    Copyright (c) 2020, The OpenThread Authors.
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

#include <string.h>

#include "dbus/common/dbus_message_helper.hpp"

namespace otbr {
namespace DBus {

otbrError DBusMessageExtract(DBusMessageIter *aIter, otbrError &aError)
{
    uint8_t   val;
    otbrError error = DBusMessageExtract(aIter, val);

    otbrVerifyOrExit(error == OTBR_ERROR_NONE);
    aError = static_cast<otbrError>(val);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const otbrError &aError)
{
    return DBusMessageEncode(aIter, static_cast<uint8_t>(aError));
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, ActiveScanResult &aScanResult)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    // Local variable to help convert an RSSI value.
    // Dbus doesn't have the concept of a signed byte
    int16_t rssi = 0;

    otbrVerifyOrExit(dbus_message_iter_get_arg_type(aIter) == DBUS_TYPE_STRUCT, error = OTBR_ERROR_DBUS);
    dbus_message_iter_recurse(aIter, &sub);

    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aScanResult.mExtAddress));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aScanResult.mNetworkName));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aScanResult.mExtendedPanId));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aScanResult.mSteeringData));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aScanResult.mPanId));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aScanResult.mJoinerUdpPort));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aScanResult.mChannel));
    otbrSuccessOrExit(error = DBusMessageExtract<int16_t>(&sub, rssi));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aScanResult.mLqi));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aScanResult.mVersion));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aScanResult.mIsNative));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aScanResult.mDiscover));

    // Double check the value is within int8 bounds and cast back
    otbrVerifyOrExit((rssi <= INT8_MAX) && (rssi >= INT8_MIN), error = OTBR_ERROR_PARSE);
    aScanResult.mRssi = static_cast<int8_t>(rssi);

    dbus_message_iter_next(aIter);
    error = OTBR_ERROR_NONE;
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const ActiveScanResult &aScanResult)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    otbrVerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aScanResult.mExtAddress));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aScanResult.mNetworkName));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aScanResult.mExtendedPanId));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aScanResult.mSteeringData));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aScanResult.mPanId));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aScanResult.mJoinerUdpPort));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aScanResult.mChannel));

    // Dbus doesn't have a signed byte, cast into an int16
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, static_cast<int16_t>(aScanResult.mRssi)));

    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aScanResult.mLqi));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aScanResult.mVersion));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aScanResult.mIsNative));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aScanResult.mDiscover));
    otbrVerifyOrExit(dbus_message_iter_close_container(aIter, &sub), error = OTBR_ERROR_DBUS);

    error = OTBR_ERROR_NONE;
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, EnergyScanResult &aResult)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    otbrVerifyOrExit(dbus_message_iter_get_arg_type(aIter) == DBUS_TYPE_STRUCT, error = OTBR_ERROR_DBUS);
    dbus_message_iter_recurse(aIter, &sub);

    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aResult.mChannel));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aResult.mMaxRssi));

    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const EnergyScanResult &aResult)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    otbrVerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);

    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aResult.mChannel));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aResult.mMaxRssi));

    otbrVerifyOrExit(dbus_message_iter_close_container(aIter, &sub), error = OTBR_ERROR_DBUS);

exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const LinkModeConfig &aConfig)
{
    otbrError       error = OTBR_ERROR_NONE;
    DBusMessageIter sub;

    otbrVerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);

    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aConfig.mRxOnWhenIdle));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aConfig.mDeviceType));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aConfig.mNetworkData));

    otbrVerifyOrExit(dbus_message_iter_close_container(aIter, &sub) == true, error = OTBR_ERROR_DBUS);
    error = OTBR_ERROR_NONE;
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, LinkModeConfig &aConfig)
{
    otbrError       error = OTBR_ERROR_DBUS;
    DBusMessageIter sub;

    otbrVerifyOrExit(dbus_message_iter_get_arg_type(aIter) == DBUS_TYPE_STRUCT);
    dbus_message_iter_recurse(aIter, &sub);

    otbrSuccessOrExit(DBusMessageExtract(&sub, aConfig.mRxOnWhenIdle));
    otbrSuccessOrExit(DBusMessageExtract(&sub, aConfig.mDeviceType));
    otbrSuccessOrExit(DBusMessageExtract(&sub, aConfig.mNetworkData));

    dbus_message_iter_next(aIter);
    error = OTBR_ERROR_NONE;
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const Ip6Prefix &aPrefix)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    otbrVerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);
    otbrVerifyOrExit(aPrefix.mPrefix.size() <= OTBR_IP6_PREFIX_SIZE, error = OTBR_ERROR_DBUS);

    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aPrefix.mPrefix));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aPrefix.mLength));

    otbrVerifyOrExit(dbus_message_iter_close_container(aIter, &sub), error = OTBR_ERROR_DBUS);

exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, Ip6Prefix &aPrefix)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    dbus_message_iter_recurse(aIter, &sub);
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aPrefix.mPrefix));
    otbrVerifyOrExit(aPrefix.mPrefix.size() <= OTBR_IP6_PREFIX_SIZE, error = OTBR_ERROR_DBUS);
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aPrefix.mLength));

    dbus_message_iter_next(aIter);

exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const ExternalRoute &aRoute)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    otbrVerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);

    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRoute.mPrefix));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRoute.mRloc16));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRoute.mPreference));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRoute.mStable));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRoute.mNextHopIsThisDevice));

    otbrVerifyOrExit(dbus_message_iter_close_container(aIter, &sub), error = OTBR_ERROR_DBUS);

exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, ExternalRoute &aRoute)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    dbus_message_iter_recurse(aIter, &sub);
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRoute.mPrefix));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRoute.mRloc16));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRoute.mPreference));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRoute.mStable));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRoute.mNextHopIsThisDevice));

    dbus_message_iter_next(aIter);

exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const OnMeshPrefix &aPrefix)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    otbrVerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);

    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aPrefix.mPrefix));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aPrefix.mRloc16));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aPrefix.mPreference));

    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aPrefix.mPreferred));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aPrefix.mSlaac));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aPrefix.mDhcp));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aPrefix.mConfigure));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aPrefix.mDefaultRoute));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aPrefix.mOnMesh));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aPrefix.mStable));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aPrefix.mNdDns));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aPrefix.mDp));
    otbrVerifyOrExit(dbus_message_iter_close_container(aIter, &sub), error = OTBR_ERROR_DBUS);

exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, OnMeshPrefix &aPrefix)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    dbus_message_iter_recurse(aIter, &sub);
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aPrefix.mPrefix));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aPrefix.mRloc16));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aPrefix.mPreference));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aPrefix.mPreferred));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aPrefix.mSlaac));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aPrefix.mDhcp));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aPrefix.mConfigure));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aPrefix.mDefaultRoute));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aPrefix.mOnMesh));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aPrefix.mStable));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aPrefix.mNdDns));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aPrefix.mDp));

    dbus_message_iter_next(aIter);

exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const MacCounters &aCounters)
{
    auto args = std::tie(aCounters.mTxTotal, aCounters.mTxUnicast, aCounters.mTxBroadcast, aCounters.mTxAckRequested,
                         aCounters.mTxAcked, aCounters.mTxNoAckRequested, aCounters.mTxData, aCounters.mTxDataPoll,
                         aCounters.mTxBeacon, aCounters.mTxBeaconRequest, aCounters.mTxOther, aCounters.mTxRetry,
                         aCounters.mTxErrCca, aCounters.mTxErrAbort, aCounters.mTxErrBusyChannel, aCounters.mRxTotal,
                         aCounters.mRxUnicast, aCounters.mRxBroadcast, aCounters.mRxData, aCounters.mRxDataPoll,
                         aCounters.mRxBeacon, aCounters.mRxBeaconRequest, aCounters.mRxOther,
                         aCounters.mRxAddressFiltered, aCounters.mRxDestAddrFiltered, aCounters.mRxDuplicated,
                         aCounters.mRxErrNoFrame, aCounters.mRxErrUnknownNeighbor, aCounters.mRxErrInvalidSrcAddr,
                         aCounters.mRxErrSec, aCounters.mRxErrFcs, aCounters.mRxErrOther);
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    otbrVerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);
    otbrSuccessOrExit(error = ConvertToDBusMessage(&sub, args));
    otbrVerifyOrExit(dbus_message_iter_close_container(aIter, &sub) == true, error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, MacCounters &aCounters)
{
    auto args = std::tie(aCounters.mTxTotal, aCounters.mTxUnicast, aCounters.mTxBroadcast, aCounters.mTxAckRequested,
                         aCounters.mTxAcked, aCounters.mTxNoAckRequested, aCounters.mTxData, aCounters.mTxDataPoll,
                         aCounters.mTxBeacon, aCounters.mTxBeaconRequest, aCounters.mTxOther, aCounters.mTxRetry,
                         aCounters.mTxErrCca, aCounters.mTxErrAbort, aCounters.mTxErrBusyChannel, aCounters.mRxTotal,
                         aCounters.mRxUnicast, aCounters.mRxBroadcast, aCounters.mRxData, aCounters.mRxDataPoll,
                         aCounters.mRxBeacon, aCounters.mRxBeaconRequest, aCounters.mRxOther,
                         aCounters.mRxAddressFiltered, aCounters.mRxDestAddrFiltered, aCounters.mRxDuplicated,
                         aCounters.mRxErrNoFrame, aCounters.mRxErrUnknownNeighbor, aCounters.mRxErrInvalidSrcAddr,
                         aCounters.mRxErrSec, aCounters.mRxErrFcs, aCounters.mRxErrOther);
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    otbrVerifyOrExit(dbus_message_iter_get_arg_type(aIter) == DBUS_TYPE_STRUCT, error = OTBR_ERROR_DBUS);
    dbus_message_iter_recurse(aIter, &sub);
    otbrSuccessOrExit(error = ConvertToTuple(&sub, args));
    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const IpCounters &aCounters)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;
    auto args = std::tie(aCounters.mTxSuccess, aCounters.mRxSuccess, aCounters.mTxFailure, aCounters.mRxFailure);

    otbrVerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);
    otbrSuccessOrExit(error = ConvertToDBusMessage(&sub, args));
    otbrVerifyOrExit(dbus_message_iter_close_container(aIter, &sub) == true, error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, IpCounters &aCounters)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;
    auto args = std::tie(aCounters.mTxSuccess, aCounters.mRxSuccess, aCounters.mTxFailure, aCounters.mRxFailure);

    otbrVerifyOrExit(dbus_message_iter_get_arg_type(aIter) == DBUS_TYPE_STRUCT, error = OTBR_ERROR_DBUS);
    dbus_message_iter_recurse(aIter, &sub);
    otbrSuccessOrExit(error = ConvertToTuple(&sub, args));
    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const ChildInfo &aChildInfo)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;
    auto            args  = std::tie(aChildInfo.mExtAddress, aChildInfo.mTimeout, aChildInfo.mAge, aChildInfo.mRloc16,
                                     aChildInfo.mChildId, aChildInfo.mNetworkDataVersion, aChildInfo.mLinkQualityIn,
                                     aChildInfo.mAverageRssi, aChildInfo.mLastRssi, aChildInfo.mFrameErrorRate,
                                     aChildInfo.mMessageErrorRate, aChildInfo.mRxOnWhenIdle, aChildInfo.mFullThreadDevice,
                                     aChildInfo.mFullNetworkData, aChildInfo.mIsStateRestoring);

    otbrVerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);
    otbrSuccessOrExit(error = ConvertToDBusMessage(&sub, args));
    otbrVerifyOrExit(dbus_message_iter_close_container(aIter, &sub) == true, error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, ChildInfo &aChildInfo)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;
    auto            args  = std::tie(aChildInfo.mExtAddress, aChildInfo.mTimeout, aChildInfo.mAge, aChildInfo.mRloc16,
                                     aChildInfo.mChildId, aChildInfo.mNetworkDataVersion, aChildInfo.mLinkQualityIn,
                                     aChildInfo.mAverageRssi, aChildInfo.mLastRssi, aChildInfo.mFrameErrorRate,
                                     aChildInfo.mMessageErrorRate, aChildInfo.mRxOnWhenIdle, aChildInfo.mFullThreadDevice,
                                     aChildInfo.mFullNetworkData, aChildInfo.mIsStateRestoring);

    otbrVerifyOrExit(dbus_message_iter_get_arg_type(aIter) == DBUS_TYPE_STRUCT, error = OTBR_ERROR_DBUS);
    dbus_message_iter_recurse(aIter, &sub);
    otbrSuccessOrExit(error = ConvertToTuple(&sub, args));
    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const NeighborInfo &aNeighborInfo)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;
    auto            args  = std::tie(aNeighborInfo.mExtAddress, aNeighborInfo.mAge, aNeighborInfo.mRloc16,
                                     aNeighborInfo.mLinkFrameCounter, aNeighborInfo.mMleFrameCounter, aNeighborInfo.mLinkQualityIn,
                                     aNeighborInfo.mAverageRssi, aNeighborInfo.mLastRssi, aNeighborInfo.mFrameErrorRate,
                                     aNeighborInfo.mMessageErrorRate, aNeighborInfo.mVersion, aNeighborInfo.mRxOnWhenIdle,
                                     aNeighborInfo.mFullThreadDevice, aNeighborInfo.mFullNetworkData, aNeighborInfo.mIsChild);

    otbrVerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);
    otbrSuccessOrExit(error = ConvertToDBusMessage(&sub, args));
    otbrVerifyOrExit(dbus_message_iter_close_container(aIter, &sub) == true, error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, NeighborInfo &aNeighborInfo)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;
    auto            args  = std::tie(aNeighborInfo.mExtAddress, aNeighborInfo.mAge, aNeighborInfo.mRloc16,
                                     aNeighborInfo.mLinkFrameCounter, aNeighborInfo.mMleFrameCounter, aNeighborInfo.mLinkQualityIn,
                                     aNeighborInfo.mAverageRssi, aNeighborInfo.mLastRssi, aNeighborInfo.mFrameErrorRate,
                                     aNeighborInfo.mMessageErrorRate, aNeighborInfo.mVersion, aNeighborInfo.mRxOnWhenIdle,
                                     aNeighborInfo.mFullThreadDevice, aNeighborInfo.mFullNetworkData, aNeighborInfo.mIsChild);

    otbrVerifyOrExit(dbus_message_iter_get_arg_type(aIter) == DBUS_TYPE_STRUCT, error = OTBR_ERROR_DBUS);
    dbus_message_iter_recurse(aIter, &sub);
    otbrSuccessOrExit(error = ConvertToTuple(&sub, args));
    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const LeaderData &aLeaderData)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;
    auto            args  = std::tie(aLeaderData.mPartitionId, aLeaderData.mWeighting, aLeaderData.mDataVersion,
                                     aLeaderData.mStableDataVersion, aLeaderData.mLeaderRouterId);

    otbrVerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);
    otbrSuccessOrExit(error = ConvertToDBusMessage(&sub, args));
    otbrVerifyOrExit(dbus_message_iter_close_container(aIter, &sub) == true, error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, LeaderData &aLeaderData)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;
    auto            args  = std::tie(aLeaderData.mPartitionId, aLeaderData.mWeighting, aLeaderData.mDataVersion,
                                     aLeaderData.mStableDataVersion, aLeaderData.mLeaderRouterId);

    otbrVerifyOrExit(dbus_message_iter_get_arg_type(aIter) == DBUS_TYPE_STRUCT, error = OTBR_ERROR_DBUS);
    dbus_message_iter_recurse(aIter, &sub);
    otbrSuccessOrExit(error = ConvertToTuple(&sub, args));
    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const ChannelQuality &aQuality)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;
    auto            args  = std::tie(aQuality.mChannel, aQuality.mOccupancy);

    otbrVerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub));
    otbrSuccessOrExit(error = ConvertToDBusMessage(&sub, args));
    otbrVerifyOrExit(dbus_message_iter_close_container(aIter, &sub) == true, error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, ChannelQuality &aQuality)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;
    auto            args  = std::tie(aQuality.mChannel, aQuality.mOccupancy);

    otbrVerifyOrExit(dbus_message_iter_get_arg_type(aIter) == DBUS_TYPE_STRUCT, error = OTBR_ERROR_DBUS);
    dbus_message_iter_recurse(aIter, &sub);
    otbrSuccessOrExit(error = ConvertToTuple(&sub, args));
    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const TxtEntry &aTxtEntry)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;
    auto            args  = std::tie(aTxtEntry.mKey, aTxtEntry.mValue);

    otbrVerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub));
    otbrSuccessOrExit(error = ConvertToDBusMessage(&sub, args));
    otbrVerifyOrExit(dbus_message_iter_close_container(aIter, &sub) == true, error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, TxtEntry &aTxtEntry)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;
    auto            args  = std::tie(aTxtEntry.mKey, aTxtEntry.mValue);

    otbrVerifyOrExit(dbus_message_iter_get_arg_type(aIter) == DBUS_TYPE_STRUCT, error = OTBR_ERROR_DBUS);
    dbus_message_iter_recurse(aIter, &sub);
    otbrSuccessOrExit(error = ConvertToTuple(&sub, args));
    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const SrpServerInfo::Registration &aRegistration)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;
    auto args = std::tie(aRegistration.mFreshCount, aRegistration.mDeletedCount, aRegistration.mLeaseTimeTotal,
                         aRegistration.mKeyLeaseTimeTotal, aRegistration.mRemainingLeaseTimeTotal,
                         aRegistration.mRemainingKeyLeaseTimeTotal);

    otbrVerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);
    otbrSuccessOrExit(error = ConvertToDBusMessage(&sub, args));
    otbrVerifyOrExit(dbus_message_iter_close_container(aIter, &sub) == true, error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, SrpServerInfo::Registration &aRegistration)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;
    auto args = std::tie(aRegistration.mFreshCount, aRegistration.mDeletedCount, aRegistration.mLeaseTimeTotal,
                         aRegistration.mKeyLeaseTimeTotal, aRegistration.mRemainingLeaseTimeTotal,
                         aRegistration.mRemainingKeyLeaseTimeTotal);

    otbrVerifyOrExit(dbus_message_iter_get_arg_type(aIter) == DBUS_TYPE_STRUCT, error = OTBR_ERROR_DBUS);
    dbus_message_iter_recurse(aIter, &sub);
    otbrSuccessOrExit(error = ConvertToTuple(&sub, args));
    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const SrpServerInfo::ResponseCounters &aResponseCounters)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;
    auto args = std::tie(aResponseCounters.mSuccess, aResponseCounters.mServerFailure, aResponseCounters.mFormatError,
                         aResponseCounters.mNameExists, aResponseCounters.mRefused, aResponseCounters.mOther);

    otbrVerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);
    otbrSuccessOrExit(error = ConvertToDBusMessage(&sub, args));
    otbrVerifyOrExit(dbus_message_iter_close_container(aIter, &sub) == true, error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, SrpServerInfo::ResponseCounters &aResponseCounters)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;
    auto args = std::tie(aResponseCounters.mSuccess, aResponseCounters.mServerFailure, aResponseCounters.mFormatError,
                         aResponseCounters.mNameExists, aResponseCounters.mRefused, aResponseCounters.mOther);

    otbrVerifyOrExit(dbus_message_iter_get_arg_type(aIter) == DBUS_TYPE_STRUCT, error = OTBR_ERROR_DBUS);
    dbus_message_iter_recurse(aIter, &sub);
    otbrSuccessOrExit(error = ConvertToTuple(&sub, args));
    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const SrpServerInfo &aSrpServerInfo)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    otbrVerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);

    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aSrpServerInfo.mState));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aSrpServerInfo.mPort));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aSrpServerInfo.mAddressMode));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aSrpServerInfo.mHosts));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aSrpServerInfo.mServices));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aSrpServerInfo.mResponseCounters));

    otbrVerifyOrExit(dbus_message_iter_close_container(aIter, &sub), error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, SrpServerInfo &aSrpServerInfo)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    dbus_message_iter_recurse(aIter, &sub);
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aSrpServerInfo.mState));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aSrpServerInfo.mPort));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aSrpServerInfo.mAddressMode));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aSrpServerInfo.mHosts));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aSrpServerInfo.mServices));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aSrpServerInfo.mResponseCounters));

    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const DnssdCounters &aDnssdCounters)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    otbrVerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);

    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aDnssdCounters.mSuccessResponse));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aDnssdCounters.mServerFailureResponse));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aDnssdCounters.mFormatErrorResponse));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aDnssdCounters.mNameErrorResponse));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aDnssdCounters.mNotImplementedResponse));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aDnssdCounters.mOtherResponse));

    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aDnssdCounters.mResolvedBySrp));

    otbrVerifyOrExit(dbus_message_iter_close_container(aIter, &sub), error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, DnssdCounters &aDnssdCounters)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    dbus_message_iter_recurse(aIter, &sub);

    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aDnssdCounters.mSuccessResponse));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aDnssdCounters.mServerFailureResponse));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aDnssdCounters.mFormatErrorResponse));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aDnssdCounters.mNameErrorResponse));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aDnssdCounters.mNotImplementedResponse));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aDnssdCounters.mOtherResponse));

    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aDnssdCounters.mResolvedBySrp));

    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const MdnsResponseCounters &aMdnsResponseCounters)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    otbrVerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);

    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aMdnsResponseCounters.mSuccess));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aMdnsResponseCounters.mNotFound));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aMdnsResponseCounters.mInvalidArgs));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aMdnsResponseCounters.mDuplicated));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aMdnsResponseCounters.mNotImplemented));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aMdnsResponseCounters.mUnknownError));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aMdnsResponseCounters.mAborted));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aMdnsResponseCounters.mInvalidState));

    otbrVerifyOrExit(dbus_message_iter_close_container(aIter, &sub), error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, MdnsResponseCounters &aMdnsResponseCounters)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    dbus_message_iter_recurse(aIter, &sub);

    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aMdnsResponseCounters.mSuccess));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aMdnsResponseCounters.mNotFound));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aMdnsResponseCounters.mInvalidArgs));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aMdnsResponseCounters.mDuplicated));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aMdnsResponseCounters.mNotImplemented));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aMdnsResponseCounters.mUnknownError));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aMdnsResponseCounters.mAborted));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aMdnsResponseCounters.mInvalidState));

    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const MdnsTelemetryInfo &aMdnsTelemetryInfo)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    otbrVerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);

    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aMdnsTelemetryInfo.mHostRegistrations));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aMdnsTelemetryInfo.mServiceRegistrations));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aMdnsTelemetryInfo.mHostResolutions));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aMdnsTelemetryInfo.mServiceResolutions));

    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aMdnsTelemetryInfo.mHostRegistrationEmaLatency));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aMdnsTelemetryInfo.mServiceRegistrationEmaLatency));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aMdnsTelemetryInfo.mHostResolutionEmaLatency));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aMdnsTelemetryInfo.mServiceResolutionEmaLatency));

    otbrVerifyOrExit(dbus_message_iter_close_container(aIter, &sub), error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, MdnsTelemetryInfo &aMdnsTelemetryInfo)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    dbus_message_iter_recurse(aIter, &sub);

    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aMdnsTelemetryInfo.mHostRegistrations));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aMdnsTelemetryInfo.mServiceRegistrations));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aMdnsTelemetryInfo.mHostResolutions));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aMdnsTelemetryInfo.mServiceResolutions));

    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aMdnsTelemetryInfo.mHostRegistrationEmaLatency));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aMdnsTelemetryInfo.mServiceRegistrationEmaLatency));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aMdnsTelemetryInfo.mHostResolutionEmaLatency));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aMdnsTelemetryInfo.mServiceResolutionEmaLatency));

    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const RadioSpinelMetrics &aRadioSpinelMetrics)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    otbrVerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);

    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRadioSpinelMetrics.mRcpTimeoutCount));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRadioSpinelMetrics.mRcpUnexpectedResetCount));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRadioSpinelMetrics.mRcpRestorationCount));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRadioSpinelMetrics.mSpinelParseErrorCount));

    otbrVerifyOrExit(dbus_message_iter_close_container(aIter, &sub), error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, RadioSpinelMetrics &aRadioSpinelMetrics)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    dbus_message_iter_recurse(aIter, &sub);

    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRadioSpinelMetrics.mRcpTimeoutCount));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRadioSpinelMetrics.mRcpUnexpectedResetCount));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRadioSpinelMetrics.mRcpRestorationCount));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRadioSpinelMetrics.mSpinelParseErrorCount));

    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const RcpInterfaceMetrics &aRcpInterfaceMetrics)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    otbrVerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);

    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRcpInterfaceMetrics.mRcpInterfaceType));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRcpInterfaceMetrics.mTransferredFrameCount));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRcpInterfaceMetrics.mTransferredValidFrameCount));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRcpInterfaceMetrics.mTransferredGarbageFrameCount));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRcpInterfaceMetrics.mRxFrameCount));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRcpInterfaceMetrics.mRxFrameByteCount));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRcpInterfaceMetrics.mTxFrameCount));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRcpInterfaceMetrics.mTxFrameByteCount));

    otbrVerifyOrExit(dbus_message_iter_close_container(aIter, &sub), error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, RcpInterfaceMetrics &aRcpInterfaceMetrics)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    dbus_message_iter_recurse(aIter, &sub);

    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRcpInterfaceMetrics.mRcpInterfaceType));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRcpInterfaceMetrics.mTransferredFrameCount));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRcpInterfaceMetrics.mTransferredValidFrameCount));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRcpInterfaceMetrics.mTransferredGarbageFrameCount));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRcpInterfaceMetrics.mRxFrameCount));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRcpInterfaceMetrics.mRxFrameByteCount));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRcpInterfaceMetrics.mTxFrameCount));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRcpInterfaceMetrics.mTxFrameByteCount));

    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const RadioCoexMetrics &aRadioCoexMetrics)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    otbrVerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);

    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRadioCoexMetrics.mNumGrantGlitch));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRadioCoexMetrics.mNumTxRequest));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRadioCoexMetrics.mNumTxGrantImmediate));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRadioCoexMetrics.mNumTxGrantWait));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRadioCoexMetrics.mNumTxGrantWaitActivated));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRadioCoexMetrics.mNumTxGrantWaitTimeout));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRadioCoexMetrics.mNumTxGrantDeactivatedDuringRequest));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRadioCoexMetrics.mNumTxDelayedGrant));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRadioCoexMetrics.mAvgTxRequestToGrantTime));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRadioCoexMetrics.mNumRxRequest));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRadioCoexMetrics.mNumRxGrantImmediate));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRadioCoexMetrics.mNumRxGrantWait));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRadioCoexMetrics.mNumRxGrantWaitActivated));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRadioCoexMetrics.mNumRxGrantWaitTimeout));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRadioCoexMetrics.mNumRxGrantDeactivatedDuringRequest));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRadioCoexMetrics.mNumRxDelayedGrant));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRadioCoexMetrics.mAvgRxRequestToGrantTime));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRadioCoexMetrics.mNumRxGrantNone));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aRadioCoexMetrics.mStopped));

    otbrVerifyOrExit(dbus_message_iter_close_container(aIter, &sub), error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, RadioCoexMetrics &aRadioCoexMetrics)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    dbus_message_iter_recurse(aIter, &sub);

    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRadioCoexMetrics.mNumGrantGlitch));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRadioCoexMetrics.mNumTxRequest));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRadioCoexMetrics.mNumTxGrantImmediate));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRadioCoexMetrics.mNumTxGrantWait));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRadioCoexMetrics.mNumTxGrantWaitActivated));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRadioCoexMetrics.mNumTxGrantWaitTimeout));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRadioCoexMetrics.mNumTxGrantDeactivatedDuringRequest));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRadioCoexMetrics.mNumTxDelayedGrant));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRadioCoexMetrics.mAvgTxRequestToGrantTime));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRadioCoexMetrics.mNumRxRequest));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRadioCoexMetrics.mNumRxGrantImmediate));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRadioCoexMetrics.mNumRxGrantWait));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRadioCoexMetrics.mNumRxGrantWaitActivated));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRadioCoexMetrics.mNumRxGrantWaitTimeout));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRadioCoexMetrics.mNumRxGrantDeactivatedDuringRequest));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRadioCoexMetrics.mNumRxDelayedGrant));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRadioCoexMetrics.mAvgRxRequestToGrantTime));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRadioCoexMetrics.mNumRxGrantNone));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aRadioCoexMetrics.mStopped));

    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const BorderRoutingCounters::PacketsAndBytes &aPacketsAndBytes)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    otbrVerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);

    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aPacketsAndBytes.mPackets));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aPacketsAndBytes.mBytes));

    otbrVerifyOrExit(dbus_message_iter_close_container(aIter, &sub), error = OTBR_ERROR_DBUS);

exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, BorderRoutingCounters::PacketsAndBytes &aPacketsAndBytes)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    dbus_message_iter_recurse(aIter, &sub);

    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aPacketsAndBytes.mPackets));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aPacketsAndBytes.mBytes));

    dbus_message_iter_next(aIter);

exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const BorderRoutingCounters &aBorderRoutingCounters)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    otbrVerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);

    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aBorderRoutingCounters.mInboundUnicast));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aBorderRoutingCounters.mInboundMulticast));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aBorderRoutingCounters.mOutboundUnicast));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aBorderRoutingCounters.mOutboundMulticast));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aBorderRoutingCounters.mRaRx));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aBorderRoutingCounters.mRaTxSuccess));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aBorderRoutingCounters.mRaTxFailure));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aBorderRoutingCounters.mRsRx));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aBorderRoutingCounters.mRsTxSuccess));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aBorderRoutingCounters.mRsTxFailure));

    otbrVerifyOrExit(dbus_message_iter_close_container(aIter, &sub), error = OTBR_ERROR_DBUS);

exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, BorderRoutingCounters &aBorderRoutingCounters)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    dbus_message_iter_recurse(aIter, &sub);

    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aBorderRoutingCounters.mInboundUnicast));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aBorderRoutingCounters.mInboundMulticast));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aBorderRoutingCounters.mOutboundUnicast));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aBorderRoutingCounters.mOutboundMulticast));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aBorderRoutingCounters.mRaRx));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aBorderRoutingCounters.mRaTxSuccess));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aBorderRoutingCounters.mRaTxFailure));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aBorderRoutingCounters.mRsRx));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aBorderRoutingCounters.mRsTxSuccess));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aBorderRoutingCounters.mRsTxFailure));

    dbus_message_iter_next(aIter);

exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const Nat64ComponentState &aNat64State)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    otbrVerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);

    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aNat64State.mPrefixManagerState));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aNat64State.mTranslatorState));

    otbrVerifyOrExit(dbus_message_iter_close_container(aIter, &sub), error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, Nat64ComponentState &aNat64State)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    dbus_message_iter_recurse(aIter, &sub);

    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aNat64State.mPrefixManagerState));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aNat64State.mTranslatorState));

    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const Nat64TrafficCounters &aCounters)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    otbrVerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);

    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aCounters.m4To6Packets));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aCounters.m4To6Bytes));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aCounters.m6To4Packets));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aCounters.m6To4Bytes));

    otbrVerifyOrExit(dbus_message_iter_close_container(aIter, &sub), error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, Nat64TrafficCounters &aCounters)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    dbus_message_iter_recurse(aIter, &sub);

    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aCounters.m4To6Packets));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aCounters.m4To6Bytes));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aCounters.m6To4Packets));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aCounters.m6To4Bytes));

    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const Nat64PacketCounters &aCounters)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    otbrVerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);

    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aCounters.m4To6Packets));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aCounters.m6To4Packets));

    otbrVerifyOrExit(dbus_message_iter_close_container(aIter, &sub), error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, Nat64PacketCounters &aCounters)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    dbus_message_iter_recurse(aIter, &sub);

    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aCounters.m4To6Packets));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aCounters.m6To4Packets));

    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const Nat64ProtocolCounters &aCounters)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    otbrVerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);

    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aCounters.mTotal));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aCounters.mIcmp));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aCounters.mUdp));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aCounters.mTcp));

    otbrVerifyOrExit(dbus_message_iter_close_container(aIter, &sub), error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, Nat64ProtocolCounters &aCounters)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    dbus_message_iter_recurse(aIter, &sub);

    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aCounters.mTotal));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aCounters.mIcmp));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aCounters.mUdp));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aCounters.mTcp));

    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const Nat64AddressMapping &aMapping)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    otbrVerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);

    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aMapping.mId));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aMapping.mIp4));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aMapping.mIp6));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aMapping.mRemainingTimeMs));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aMapping.mCounters));

    otbrVerifyOrExit(dbus_message_iter_close_container(aIter, &sub), error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, Nat64AddressMapping &aMapping)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    dbus_message_iter_recurse(aIter, &sub);

    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aMapping.mId));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aMapping.mIp4));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aMapping.mIp6));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aMapping.mRemainingTimeMs));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aMapping.mCounters));

    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const Nat64ErrorCounters &aCounters)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    otbrVerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);

    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aCounters.mUnknown));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aCounters.mIllegalPacket));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aCounters.mUnsupportedProto));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aCounters.mNoMapping));

    otbrVerifyOrExit(dbus_message_iter_close_container(aIter, &sub), error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, Nat64ErrorCounters &aCounters)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    dbus_message_iter_recurse(aIter, &sub);

    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aCounters.mUnknown));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aCounters.mIllegalPacket));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aCounters.mUnsupportedProto));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aCounters.mNoMapping));

    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const InfraLinkInfo &aInfraLinkInfo)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    otbrVerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);

    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aInfraLinkInfo.mName));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aInfraLinkInfo.mIsUp));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aInfraLinkInfo.mIsRunning));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aInfraLinkInfo.mIsMulticast));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aInfraLinkInfo.mLinkLocalAddresses));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aInfraLinkInfo.mUniqueLocalAddresses));
    otbrSuccessOrExit(error = DBusMessageEncode(&sub, aInfraLinkInfo.mGlobalUnicastAddresses));

    otbrVerifyOrExit(dbus_message_iter_close_container(aIter, &sub), error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, InfraLinkInfo &aInfraLinkInfo)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    dbus_message_iter_recurse(aIter, &sub);

    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aInfraLinkInfo.mName));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aInfraLinkInfo.mIsUp));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aInfraLinkInfo.mIsRunning));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aInfraLinkInfo.mIsMulticast));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aInfraLinkInfo.mLinkLocalAddresses));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aInfraLinkInfo.mUniqueLocalAddresses));
    otbrSuccessOrExit(error = DBusMessageExtract(&sub, aInfraLinkInfo.mGlobalUnicastAddresses));

    dbus_message_iter_next(aIter);
exit:
    return error;
}

} // namespace DBus
} // namespace otbr
