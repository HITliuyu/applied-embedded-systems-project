import argparse
from time import sleep
from naoqi import ALProxy

def main(robotIP, x, y, theta, PORT=9559):
    motionProxy  = ALProxy("ALMotion", robotIP, PORT)
    postureProxy = ALProxy("ALRobotPosture", robotIP, PORT)

    # Wake up robot
    # Maybe not needed
    motionProxy.wakeUp()

    # Send robot to Pose Init
    # Maybe not needed
    postureProxy.goToPosture("StandInit", 0.5)

    # Example showing the moveTo command
    # The units for this command are meters and radians
    current_head_angle = motionProxy.getAngles("HeadPitch", True)
    print "Command angles <True>:"
    print str(current_head_angle)
    print ""

    motionProxy.moveTo(x, y, theta - current_head_angle[0])

    # Will block until move Task is finished
    ########
    # NOTE #
    ########
    # If moveTo() method does nothing on the robot,
    # read the section about walk protection in the
    # Locomotion control overview page.

    # Go to rest position
#    motionProxy.rest()

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--ip", type=str, default="127.0.0.1",
                        help="Robot ip address")
    parser.add_argument("--port", type=int, default=9559,
                        help="Robot port number")
    parser.add_argument("--x", type=float, default=0.0, help="X distance in meters")
    parser.add_argument("--y", type=float, default=0.0, help="Y distance in meters")
    parser.add_argument("--t", type=float, default=0.0, help="Theta angle in radians")

    args = parser.parse_args()
    main(args.ip, args.x, args.y, args.t, args.port)
