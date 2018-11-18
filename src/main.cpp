#include "ros/ros.h"
#include <geometry_msgs/Twist.h>
#include <std_msgs/String.h>
#include <std_srvs/Empty.h>
#include <hector_uav_msgs/EnableMotors.h>
#include <boost/thread.hpp>

void stop(ros::Publisher& publisher)
{
    if(publisher.getNumSubscribers() > 0)
    {
        geometry_msgs::Twist velocity;
        publisher.publish(velocity);
    }
}

void takeoff(ros::Publisher& publisher)
{
    ROS_INFO("takeoff start\n");
    ros::Rate loop_rate(2);
    geometry_msgs::Twist velocity;
    velocity = geometry_msgs::Twist();
    velocity.linear.z = 1.0;
    loop_rate.sleep();
    ROS_INFO("Linear: x=%f, y=%f, z=%f\n", velocity.linear.x,
             velocity.linear.y, velocity.linear.z);
    publisher.publish(velocity);
    //loop_rate.sleep();
    //stop(publisher);
}

class QuadrotorControl
{
private:
    ros::ServiceClient quadrotorOneMotor, quadrotorTwoMotor;
    ros::NodeHandle nodeHandle;
    ros::Publisher velocityPublisher1, velocityPublisher2;
    geometry_msgs::Twist velocity;
public:
    QuadrotorControl(ros::NodeHandle& nh) : nodeHandle(nh)
    {
        quadrotorOneMotor = nodeHandle.serviceClient<hector_uav_msgs::EnableMotors>("uav1/enable_motors");
        quadrotorTwoMotor = nodeHandle.serviceClient<hector_uav_msgs::EnableMotors>("uav2/enable_motors");
    }
    void EnableMotors()
    {
        ros::Rate rate(0.25);
        hector_uav_msgs::EnableMotors motor1, motor2;
        motor1.request.enable = 1;
        motor2.request.enable = 1;
        rate.sleep();
        if(quadrotorOneMotor.call(motor1) && quadrotorTwoMotor.call(motor2))
        {
            ROS_INFO("Success\n");
        } else{
            ROS_INFO("Fatal\n");
        }
    }
    void Takeoff()
    {
        ROS_INFO("takeoff start\n");
        ros::Rate rate(0.25);
        velocityPublisher1 = nodeHandle.advertise<geometry_msgs::Twist>("uav1/cmd_vel", 10);

        velocity = geometry_msgs::Twist();
        velocity.linear.z = 1.0;
        rate.sleep();
        velocityPublisher1.publish(velocity);
        rate.sleep();
        Flysquare();
        //Forward();
        //Stop();


    }

    void Stop()
    {
        if(velocityPublisher1.getNumSubscribers() > 0)
        {
            velocity = geometry_msgs::Twist();
            velocityPublisher1.publish(velocity); // sending stop message
        }
    }

    void Forward()
    {

    }

    void Flysquare()
    {
        ros::Rate loop_rate(0.5);
        ROS_INFO("Dispatching flysquare action.");
        // forward
        velocity = geometry_msgs::Twist();
        velocity.linear.x = 1.0;
        velocityPublisher1.publish(velocity);
        loop_rate.sleep();

        // right
        velocity = geometry_msgs::Twist();
        velocity.linear.y = 1.0;
        velocityPublisher1.publish(velocity);
        loop_rate.sleep();

        // backward
        velocity = geometry_msgs::Twist();
        velocity.linear.x = -1.0;
        velocityPublisher1.publish(velocity);
        loop_rate.sleep();

        // left
        velocity = geometry_msgs::Twist();
        velocity.linear.y = -1.0;
        velocityPublisher1.publish(velocity);
        loop_rate.sleep();

        Stop();
        ROS_INFO("Action flysquare dispatched.");
    }
};

void task1()
{

}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "quadrotor_flight");
    ROS_INFO("start...\n");
    ros::NodeHandle nodeHandle;
    QuadrotorControl quad(nodeHandle);
    quad.EnableMotors();
    quad.Takeoff();
/*
    ros::ServiceClient motor_on;
    ros::Publisher velocityPublisher;
    velocityPublisher = nodeHandle.advertise<geometry_msgs::Twist>("cmd_vel", 10);
    hector_uav_msgs::EnableMotors motor;
    motor.request.enable = 1;
    motor_on = nodeHandle.serviceClient<hector_uav_msgs::EnableMotors>("uav1/enable_motors");
    if(motor_on.call(motor))
    {
        ROS_INFO("Success. Response = %u", motor.response.success);
    } else{
        ROS_INFO("Fatal. Response = %u", motor.response.success);
        return 1;
    }
    takeoff(velocityPublisher);
*/
    return 0;
}
