//
//
// Tencent is pleased to support the open source community by making tRPC available.
//
// Copyright (C) 2023 THL A29 Limited, a Tencent company.
// All rights reserved.
//
// If you have downloaded a copy of the tRPC source code from Tencent,
// please note that tRPC source code is licensed under the  Apache 2.0 License,
// A copy of the Apache 2.0 License is included in this file.
//
//

#pragma once

#include <any>
#include <deque>
#include <string>

#include "trpc/codec/server_codec.h"
#include "trpc/server/server_context.h"

namespace trpc {

/// @brief Server codec for gRPC Unary RPC.
class GrpcServerCodec : public trpc::ServerCodec {
 public:
  ~GrpcServerCodec() override = default;

  std::string Name() const override { return "grpc"; }

  /// @brief Decodes a complete grpc protocol message from binary byte stream (zero-copy).
  ///
  /// @param conn is the connection object where the protocol integrity check is performed on the current binary bytes.
  /// @param in is a buffer contains input binary bytes read from the socket.
  /// @param out is a list of successfully decoded protocol message.
  /// @return Returns a number greater than or equal to 0 on success, less than 0 on failure.
  int ZeroCopyCheck(const ConnectionPtr& conn, NoncontiguousBuffer& in, std::deque<std::any>& out) override {
    return PacketChecker::PACKET_LESS;
  }

  /// @brief Decodes a grpc request protocol message object from a complete protocol message decoded from binary
  /// bytes (zero copy).
  ///
  /// @param ctx is server context for decoding.
  /// @param in is a complete protocol message decoded from binary bytes.
  ///           It is usually the output parameter of method "Check".
  /// @param out is grpc request protocol message object.
  /// @return Returns true on success, false otherwise.
  bool ZeroCopyDecode(const ServerContextPtr& ctx, std::any&& in, ProtocolPtr& out) override;

  /// @brief Encodes a grpc response protocol message object into a protocol message.
  ///
  /// @param ctx is server context for encoding.
  /// @param in  is grpc response protocol message object.
  /// @param out is a complete protocol message.
  /// @return Returns true on success, false otherwise.
  bool ZeroCopyEncode(const ServerContextPtr& ctx, ProtocolPtr& in, NoncontiguousBuffer& out) override;

  /// @brief Creates a protocol object of grpc request (unary call).
  ProtocolPtr CreateRequestObject() override;

  /// @brief Creates a protocol object of response (unary call).
  ProtocolPtr CreateResponseObject() override;

  /// @brief Extracts metadata of protocol message, e.g. frame-type of data, stream-id, frame-type of stream.
  bool Pick(const std::any& message, std::any& data) const override;

  /// @brief Reports whether streaming RPC is supported.
  bool HasStreamingRpc() const override { return true; }

  /// @brief Reports whether it is streaming protocol.
  bool IsStreamingProtocol() const override { return true; }

  /// @brief Return the return code of a specific protocol through a unified return code
  int GetProtocolRetCode(codec::ServerRetCode ret_code) const override;
};

}  // namespace trpc
