# A-POD Arduino Code

## PS2 Controller

### Common Controls

| Control     | Description
| ----------- | -------------
| **START**   | Turn bot on/off 
| **X**       | Toggle mode: `walk` -> `shift` -> `tilt`  
| **L1**      | Open Mandibles 
| **L2**      | Close Mandibles 
| **L2+LEFT** | Decrease gripper torque
| **L2+RIGHT**| Increase gripper torque
| **O**       | Toggle single leg mode 
| **[]**      | Toggle balance mode 
| **/\\**     | Move body 35mm from the ground and back. 
| **UP**      |  
| **DOWN**    |  
| **LEFT**    | Decrease speed with 50mS 
| **RIGHT**   | Increase speed with 50mS
| **R1+Left Stick** | Pan / Tilt Head
| **R1+Right Stick** | Rotate Head
| **R1+L3** | Center Head

### Walk Controls

| Control         | Mode 1 |
| --------------- | ------------- |
| **SELECT**      | Switch gaits |
| **R1**          | Toggle Double gait travel speed  |
| **R2**          | Toggle Double gait travel length |
| **Left Stick**  | Walk / Strafe |
| **Right Stick L-R** | Rotate |
| **Right Stick U-D** | Body up/down |


### Shift Controls

| Control       | Description |
| ------------- | ------------- |
| **Left Stick** | Shift body X/Z |
| **Right Stick** | Shift body Y and rotate body Y |


### Tilt Controls

| Control       | Description |
| ------------- | ------------- |
| **Left Stick** | Tilt body X/Z |
| **Right Stick** | Tilt body Y |


### Single Leg Controls

| Control         | Description |
| --------------- | ------------- |
| **SELECT**      | Switch legs |
| **Left Stick**  | Move Leg X/Z (relative) |
| **Right Stick** | Move Leg Y (absolute) |
| **R2**          | Hold/release leg position |
