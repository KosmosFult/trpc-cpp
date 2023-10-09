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

#include "trpc/admin/cpu_profiler_handler.h"

#include <memory>
#include <utility>

#include "gtest/gtest.h"

#include "trpc/server/server_context.h"

namespace trpc::testing {

class TestCpuProfilerHandler : public ::testing::Test {
 public:
};

TEST_F(TestCpuProfilerHandler, Check) {
  http::HttpResponse reply;
  ServerContextPtr context;

  std::unique_ptr<AdminHandlerBase> h1 = std::make_unique<admin::CpuProfilerHandler>();
  h1->Handle("", context, std::make_shared<http::HttpRequest>(), &reply);
  std::cout << reply.GetContent() << std::endl;

  EXPECT_EQ(
      "{\"errorcode\":-1,\"message\":\"please recomplile with -DTRPC_ENABLE_PROFILER and link "
      "libtcmalloc_and_profiler.a\"}",
      reply.GetContent());

  http::HttpResponse reply2;
  std::unique_ptr<AdminHandlerBase> h2 = std::make_unique<admin::WebCpuProfilerHandler>();
  h2->Handle("", context, std::make_shared<http::HttpRequest>(), &reply2);
  EXPECT_EQ("alert(please recomplile with -DTRPC_ENABLE_PROFILER "
            "and link libtcmalloc_and_profiler.a)",
            reply2.GetContent());

  http::HttpResponse reply3;
  std::unique_ptr<AdminHandlerBase> h3 = std::make_unique<admin::WebCpuProfilerDrawHandler>();
  h3->Handle("", context, std::make_shared<http::HttpRequest>(), &reply3);
  EXPECT_EQ("alert(please recomplile with -DTRPC_ENABLE_PROFILER "
            "and link libtcmalloc_and_profiler.a)",
            reply3.GetContent());
}

}  // namespace trpc::testing
