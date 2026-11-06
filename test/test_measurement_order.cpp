// Copyright 2026 Thornbots
// Licensed under the 3-clause BSD license of robot_localization (see LICENSE).

// Ordering of equal-stamp measurements in the filter's queue.

#include <algorithm>
#include <memory>
#include <numeric>
#include <queue>
#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "robot_localization/measurement.hpp"

namespace robot_localization
{
using Queue = std::priority_queue<MeasurementPtr, std::vector<MeasurementPtr>, Measurement>;

namespace
{
MeasurementPtr make(const std::string & topic, int64_t ns, uint64_t sequence)
{
  auto m = std::make_shared<Measurement>();
  m->topic_name_ = topic;
  m->time_ = rclcpp::Time(ns, RCL_ROS_TIME);
  m->sequence_ = sequence;
  return m;
}
}  // namespace

TEST(MeasurementOrder, DefaultOrdersByTimeOnly)
{
  Queue queue;  // upstream construction
  queue.push(make("b", 20, 0));
  queue.push(make("a", 10, 1));
  queue.push(make("c", 30, 2));
  EXPECT_EQ(queue.top()->time_.nanoseconds(), 10);
  queue.pop();
  EXPECT_EQ(queue.top()->time_.nanoseconds(), 20);
  queue.pop();
  EXPECT_EQ(queue.top()->time_.nanoseconds(), 30);
}

TEST(MeasurementOrder, EqualStampsPopIndependentOfPushOrder)
{
  Measurement comparator;
  comparator.order_ties_ = true;
  // (topic, stamp, sequence) in the expected pop order.
  const std::vector<MeasurementPtr> expected = {
    make("odom0_pose", 5, 0), make("odom0_twist", 5, 1), make("odom0_twist", 5, 7),
    make("odom1_pose", 5, 2), make("odom1_twist", 5, 3), make("odom0_pose", 9, 4)};
  std::vector<int> order(expected.size());
  std::iota(order.begin(), order.end(), 0);
  do {
    Queue queue(comparator);
    for (const int i : order) {
      queue.push(expected[i]);
    }
    for (const auto & want : expected) {
      ASSERT_FALSE(queue.empty());
      EXPECT_EQ(queue.top().get(), want.get());
      queue.pop();
    }
  } while (std::next_permutation(order.begin(), order.end()));
}
}  // namespace robot_localization

int main(int argc, char ** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
