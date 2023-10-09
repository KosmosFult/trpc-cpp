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

#include <atomic>
#include <csignal>
#include <cstdint>

#include "gflags/gflags.h"

#include "trpc/client/make_client_context.h"
#include "trpc/client/trpc_client.h"
#include "trpc/common/runtime_manager.h"
#include "trpc/future/future_utility.h"
#include "trpc/util/time.h"

#include "examples/features/future_forward/proxy/forward.trpc.pb.h"

DEFINE_string(target, "trpc.test.route.Forward", "callee service name");
DEFINE_string(addr, "127.0.0.1:12346", "ip:port");
DEFINE_string(client_config, "trpc_cpp_future.yaml", "");
DEFINE_uint32(count, 100, "");
DEFINE_bool(sync_call, false, "use sync interface or not");

std::atomic<uint64_t> g_succ = 0;
std::atomic<uint64_t> g_fail = 0;

void DoRoute(const std::shared_ptr<::trpc::test::route::ForwardServiceProxy>& prx) {
  ::trpc::test::helloworld::HelloRequest request;
  request.set_msg("future");

  auto context = ::trpc::MakeClientContext(prx);
  context->SetTimeout(1000);

  if (!FLAGS_sync_call) {
    auto fut =
        prx->AsyncRoute(context, request).Then([](::trpc::Future<::trpc::test::helloworld::HelloReply>&& rsp_fut) {
          if (rsp_fut.IsReady()) {
            ++g_succ;
            auto reply = rsp_fut.GetValue0();
          } else {
            ++g_fail;
            auto exception = rsp_fut.GetException();
            std::cout << "status:" << exception.what() << std::endl;
          }

          return ::trpc::MakeReadyFuture<>();
        });
    ::trpc::future::BlockingGet(std::move(fut));
  } else {
    ::trpc::test::helloworld::HelloReply reply;
    ::trpc::Status status = prx->Route(context, request, &reply);

    if (status.OK()) {
      ++g_succ;
    } else {
      ++g_fail;
      std::cout << "status:" << status.ToString() << std::endl;
    }
  }
}

void DoParallelRoute(const std::shared_ptr<::trpc::test::route::ForwardServiceProxy>& prx) {
  ::trpc::test::helloworld::HelloRequest request;
  request.set_msg("future");

  auto context = ::trpc::MakeClientContext(prx);
  context->SetTimeout(1000);

  if (!FLAGS_sync_call) {
    auto fut = prx->AsyncParallelRoute(context, request)
                   .Then([](::trpc::Future<::trpc::test::helloworld::HelloReply>&& rsp_fut) {
                     if (rsp_fut.IsReady()) {
                       ++g_succ;
                       auto reply = rsp_fut.GetValue0();
                     } else {
                       ++g_fail;
                       auto exception = rsp_fut.GetException();
                       std::cout << "status:" << exception.what() << std::endl;
                     }

                     return ::trpc::MakeReadyFuture<>();
                   });
    ::trpc::future::BlockingGet(std::move(fut));
  } else {
    ::trpc::test::helloworld::HelloReply reply;
    ::trpc::Status status = prx->ParallelRoute(context, request, &reply);

    if (status.OK()) {
      ++g_succ;
    } else {
      ++g_fail;
      std::cout << "status:" << status.ToString() << std::endl;
    }
  }
}

int Run() {
  ::trpc::ServiceProxyOption option;

  option.name = FLAGS_target;
  option.codec_name = "trpc";
  option.network = "tcp";
  option.conn_type = "long";
  option.timeout = 1000;
  option.selector_name = "direct";
  option.target = FLAGS_addr;

  auto prx = ::trpc::GetTrpcClient()->GetProxy<::trpc::test::route::ForwardServiceProxy>(FLAGS_target, option);

  size_t begin_time = ::trpc::time::GetMilliSeconds();
  uint32_t k = 0;
  while (k < FLAGS_count) {
    DoRoute(prx);

    DoParallelRoute(prx);

    ++k;
  }

  size_t end_time = ::trpc::time::GetMilliSeconds();

  std::cout << "succ:" << g_succ << ", fail:" << g_fail << ", timecost(ms):" << (end_time - begin_time) << std::endl;

  return 0;
}

void ParseClientConfig(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::CommandLineFlagInfo info;
  if (GetCommandLineFlagInfo("client_config", &info) && info.is_default) {
    std::cerr << "start client with client_config, for example: " << argv[0]
              << " --client_config=/client/client_config/filepath" << std::endl;
    exit(-1);
  }

  std::cout << "FLAGS_target:" << FLAGS_target << std::endl;
  std::cout << "FLAGS_client_config:" << FLAGS_client_config << std::endl;

  int ret = ::trpc::TrpcConfig::GetInstance()->Init(FLAGS_client_config);
  if (ret != 0) {
    std::cerr << "load client_config failed." << std::endl;
    exit(-1);
  }
}

int main(int argc, char* argv[]) {
  ParseClientConfig(argc, argv);

  // If the business code is running in trpc pure client mode,
  // the business code needs to be running in the `RunInTrpcRuntime` function
  return ::trpc::RunInTrpcRuntime([]() { return Run(); });
}
