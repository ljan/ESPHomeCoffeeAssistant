# ESPHomeCoffeeAssistant

## GrinderComponent for ESPHome

The `GrinderComponent` is a custom ESPHome component that controls the behavior of a coffee grinder. It supports various modes such as single grind, double grind, manual grind, and saving custom grind times with a long press of the button. The component is highly customizable and can be easily integrated into ESPHome-based projects.

### Features

- **Single Click**: Starts a preset single grind cycle.
- **Double Click**: Starts a preset double grind cycle.
- **Long Press**: Starts a manual grind, which can be stopped by releasing the button.
- **Data Logging**: Logs grind duration in milliseconds and outputs to ESPHome logs.
- **Time Management**: Grind times (single/double) can be manually adjusted and saved for future use.

### Usage

#### Single Click

A short press on the button initiates a single grind cycle with the preset time. Press the button again during grinding to stop the process.

#### Double Click

A double click starts a double grind cycle. Press the button again during grinding to stop the process.

#### Long Press

Holding down the button starts a manual grind. The process will stop when the button is released.

#### Saving Grind Times

During a manual grind, holding and releasing the button will save the current grind time as the new default for either single or double grind, depending on the state.

### Logging

During operation, the component logs the grind duration and state changes to the ESPHome logs. This can be useful for monitoring performance or diagnosing issues.
