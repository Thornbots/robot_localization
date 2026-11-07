robot_localization
==================

robot_localization is a package of nonlinear state estimation nodes. The package was developed by Charles River Analytics, Inc.

Please see documentation here: http://wiki.ros.org/robot_localization

Thornbots fork
--------------

Purpose: a lockstep simulation (`sim`) must give identical runs, and the stock EKF's
measurement queue is a heap ordered by stamp only. Two measurements with the same stamp (the
pose and twist halves of one `nav_msgs/Odometry`, or messages from two odometry topics
stamped alike) pop in an order set by the shape of the heap, which depends on which callback
pushed first, a wall-time race. The estimates differ in the last bit (2e-16 in yaw), which
amplifies through the simulated robot.

- Upstream base: [`cra-ros-pkg/robot_localization`](https://github.com/cra-ros-pkg/robot_localization)
  tag `3.8.3` (`9596c40`), the version `ros-jazzy-robot-localization` ships in the robot image.
  `3.8.4` was not taken so the diff against the image stays one patch.
- Patch: parameter `deterministic_measurement_order` (default `false`). When `true`, equal
  stamps pop by (topic name, arrival number) instead of by heap shape. Default `false` runs
  upstream's comparator unchanged, so a robot that picks this package up behaves as stock.
  The diff is `measurement.hpp` (comparator and an arrival number), the assignments in
  `RosFilter::enqueueMeasurement` and `RosFilter::loadParams`, the install of a marker file
  `share/robot_localization/thornbots_fork` (the simulation refuses to start without it), and
  `test/test_measurement_order.cpp`.
- Patch, second option: `publish_queued_stamps` (default `false`) publishes a `std_msgs/Header` on
  `~/queued_stamps` (stamp of the measurement, `frame_id` its topic) each time a measurement is queued, so
  a lockstep simulation can hold its clock until the EKF has a measurement before its tick fires. A
  tie-break alone left tick membership to message timing (measured: identical inputs, different
  `/localization/odom`).
- Use: only the simulation sets it, through `sentry_localization`'s `ekf_deterministic_order`
  launch argument ([README](../sentry_localization/README.md#initial-field-position)).
  Building this package in the workspace overlay shadows the apt one, as for any overlay.
- Branches: `nightly` is the integration branch the workspace tracks; `main` is the promoted
  release. Keep the diff to upstream small, and rebase onto a later upstream tag rather than
  merging.
