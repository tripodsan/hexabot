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
| **UP**      | Body up 10 mm 
| **DOWN**    | Body down 10 mm 
| **LEFT**    | Decrease speed with 50mS 
| **RIGHT**   | Increase speed with 50mS 


### Walk Controls

| Control         | Mode 1 | Mode 2 |
| --------------- | ------------- | --------- |
| **SELECT**      | Switch gaits ||
| **R1**          | Toggle Double gait travel speed  ||
| **R2**          | Toggle Double gait travel length ||
| **Left Stick**  | Walk / Strafe | n/a |
| **Right Stick** | Rotate | Walk / rotate |


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
