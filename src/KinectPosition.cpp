#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include <tf/transform_listener.h>
#include <geometry_msgs/Twist.h>
#include "nav_msgs/Odometry.h"
#include <cmath>

using namespace std;


//global gains
double lambda = 2; 			// translational  gain @ 1.5
double phi = 3.5; 			//angular gain @ 2


int main(int argc, char** argv){
	ros::init(argc, argv, "my_tf_listener");

	ros::NodeHandle node;
// publish twist message
   ros::Publisher cmd_pub = node.advertise<geometry_msgs::Twist>("RosAria/cmd_vel", 10);
   // subscribe odometry message (Motion capture system)
	tf::TransformListener listener;

	ros::Rate rate(100.0);
	while (node.ok()){
		tf::StampedTransform transform_msg;
		try{
			listener.lookupTransform("/camera_depth_frame", "/torso_1", ros::Time(0), transform_msg);
		}
		catch (tf::TransformException ex){
			ROS_ERROR("%s",ex.what());
		}
		//establish an x and y coordinate
		double Object_x = transform_msg.getOrigin().x();
		double Object_y = transform_msg.getOrigin().y();
		double Dist =  sqrt( pow(Object_x, 2) + pow(Object_y, 2));
		printf("x->%f \n",Object_x);
		printf("y->%f \n",Object_y);
		printf("D->%f\n",Dist);
		
		//establish a translational and angular velocity
		double T_vel = (Dist - 1)*lambda;
		double A_vel = atan2(Object_y, Object_x) * phi;
		printf("v->%f \n",T_vel);
		printf("w->%f \n",A_vel);
		
		 //Specify the velocity
		   geometry_msgs::Twist base_cmd;
		if (Dist <= 1.8 && Dist >= .6 && Object_y > -0.5 && Object_y < 0.5)
			{
				base_cmd.linear.x = T_vel; //translational velocity
				base_cmd.linear.y = 0;
				base_cmd.linear.z = 0;
				base_cmd.angular.x = 0;
				base_cmd.angular.y = 0;
				base_cmd.angular.z = A_vel; // rotational velocity
			}
		else
			{
				base_cmd.linear.x = 0; //translational velocity
				base_cmd.linear.y = 0;
				base_cmd.linear.z = 0;
				base_cmd.angular.x = 0;
				base_cmd.angular.y = 0;
				base_cmd.angular.z = 0; // rotational velocity
			}
		
		
		cmd_pub.publish(base_cmd);
		ros::spinOnce();
		rate.sleep();
	}
	return 0;
};
