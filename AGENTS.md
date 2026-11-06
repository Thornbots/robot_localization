# robot_localization (Thornbots fork)

Follow [workspace rules](../AGENTS.md) and [CI](../docs/CI.md).
Keep the diff to upstream small; [README](README.md#thornbots-fork) names the base and patch.
Every change is opt-in by parameter with the default identical to upstream, so a robot
build behaves as the apt package. Only `sentry_localization`'s simulation launch enables it.

## Testing

`colcon test --packages-select robot_localization` in your own agent container; building
the package in the overlay shadows the apt one (Docker skill).
