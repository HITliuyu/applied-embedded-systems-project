# README #

Shared Robovision project for AEP, TU Berlin 2015.

### Content ###

This project contains 2 main components:

* Image-Processor
* Replayer

The image-processor is the part of the project processing the images coming from the two in-field cameras. It process each frame in order to find and locate all the player markers contained in the image.
It is possible to log a specific part of a match for a later replay.

The replayer can replay any previously logged recording. It has settings for the replay speed.

It uses a simple data format, making it easy to manually create/edit logs for training purposes.

### Requirements ###

This project requires 2 USB cameras (preferably external ones) connected to the host computer.

The first camera should solely oversee the left side of the football pitch and the second one the right side.

### How to compile it ###

The project has been built using Eclipse.

It uses the following libraries:

- Aruco 1.2.5+ (http://sourceforge.net/projects/aruco/)
- OpenCV 2.4.10

Install both libraries in the system (tested on Ubuntu 14.04+).

The following 2 tutorials are a good guidance for installing ArUco and calibrate the camera(s).

- http://maztories.blogspot.de/2013/07/installing-aruco-augmented-reality.html
- http://maztories.blogspot.com.es/2013/07/camera-calibration-with-opencv.html (needed for getting the intrinsics files)

** Linker Flags: **

- -std=c++11 -lm


** Include paths (-I):**

- {path_to_opencv_installation}/include
- {path_to_aruco_installation}/utils
- {path_to_aruco_installation}/src


**Libraries (-l): **

* opencv_core
* pthread
* aruco
* opencv_imgproc
* opencv_highgui
* opencv_calib3d

**Library Search Path (-L):**

{path_to_opencv_installation}/lib

### How to run it ###

Copy the 'res' folder in the same folder of the executable.

Image-processor:
```
#!bash

./shared-robovision intrinsics_left.yml intrinsics_right.yml teams_info.yml [log_file.txt]
```

Replayer:
```
#!bash

./replayer log_file.txt teams_info.yml [time_between_frames_in_ms] [step]

```

**Config.h** contains the following variables for tweaking the application:

- **MARKER_SIZE      **: size of the marker (in meters)

- **PLAYER_RADIUS    **: size of the player in the map

- **ROBOT_HEIGHT     **: distance of the marker from the ground (in meters)

- **FIELD_HEIGHT     **: height of the field image (in pixels)

- **FIELD_WIDTH      **: width of the field image (in pixels)

- **CAMERA_HEIGHT    **: distance of the camera from the ground (in meters)

- **REAL_FIELD_HEIGHT**: height of the field (in meters)

- **REAL_FIELD_WIDTH **: width of the field (in meters)

- **DEBUG            **: run in debug mode

- **DEBUG_CAMERA     **: use only local cameras

### Team Data ###

Here is an example of a team.yml:

```
team_1=TU-Berlin
short_1=TUB
colour_1=255,255,0
players_1=792,4,192.168.0.1;849,1,192.168.0.2;1009,5,192.168.0.3
#
team_2=DragonKnights
short_2=DK
colour_2=0,0,255
players_2=466,6,192.168.0.4;650,1,192.168.0.5;354,10,192.168.0.6
##
```

`team_#`: full name of the team #

`short_#`: short name of the team # (shown in the field)

`colour_#`: colour of the team #

`players_#`: tuples of 'id,number,IP_Address' separated by a ';'.

`#`: team separator (optional)

`##`: end of file (optional)

The team file is required for correctly displaying the information of the players on the field and being able to communicate with the players.

**Note:** The program currently supports only 2 teams.
