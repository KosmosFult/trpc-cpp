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

#include "trpc/admin/watch_handler.h"

#include <memory>
#include <utility>

#include "gtest/gtest.h"

#include "trpc/server/server_context.h"

namespace trpc::testing {

constexpr char kWatchDescription[] = "[POST /watch]   watch your private tasks";

TEST(TestWatchHandler, Test) {
  std::unique_ptr<AdminHandlerBase> h1 = std::make_unique<admin::WatchHandler>();
  EXPECT_EQ(kWatchDescription, h1->Description());

  http::HttpRequestPtr req = std::make_shared<http::HttpRequest>();
  http::HttpResponse reply;
  ServerContextPtr context;
  h1->Handle("", context, req, &reply);
  EXPECT_EQ("{\"errorcode\":\"0\",\"message\":\"watching unsupported\"}", reply.GetContent());
}

}  // namespace trpc::testing
