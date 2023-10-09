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

#include <memory>

#include "trpc/runtime/iomodel/reactor/common/accept_connection_info.h"
#include "trpc/runtime/iomodel/reactor/common/network_address.h"
#include "trpc/runtime/iomodel/reactor/common/socket.h"
#include "trpc/runtime/iomodel/reactor/default/acceptor.h"
#include "trpc/runtime/iomodel/reactor/reactor.h"

namespace trpc {

/// @brief The acceptor for tcp connection
class TcpAcceptor final : public Acceptor {
 public:
  explicit TcpAcceptor(Reactor* reactor, const NetworkAddress& tcp_addr);

  ~TcpAcceptor() override;

  bool EnableListen(int backlog = 1024) override;

  void DisableListen() override;

 protected:
  int HandleReadEvent() override;

 private:
  Reactor* reactor_{nullptr};

  NetworkAddress tcp_addr_;

  Socket socket_;

  // Idle fd, for EMFILE error
  int idle_fd_{-1};

  bool enable_{false};
};

}  // namespace trpc
