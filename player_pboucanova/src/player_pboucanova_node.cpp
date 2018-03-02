#include <iostream>
#include <vector>

//Boost includes
#include <boost/shared_ptr.hpp>

//Ros includes
#include <ros/ros.h>
#include <std_msgs/String.h>

//library includes
#include <rws2018_libs/team.h>

//geometric trans. includes
#include <tf/transform_broadcaster.h>
#include <rws2018_msgs/MakeAPlay.h>

#include <visualization_msgs/Marker.h>
#include <tf/transform_listener.h>


#define DEFAULT_TIME 0.05

using namespace ros;
using namespace tf;


using namespace std;

namespace rws_pboucanova
{
class Player
{
private:
       std::string team;                        

public:
    Player(std::string argin_name) { name = argin_name; }

    string name;
    
    //overload de funções -> ou seja duas ou mais funçõoes com o mesmo nome;
    int setTeamName(int team_index = 0)
    {
        switch (team_index) {
            case 0:
                return setTeamName("red");      break;
            case 1:
                return setTeamName("green");    break;
            case 2:
                return setTeamName("blue");     break;
            default:
                 ROS_WARN("wrong team index given. Cannot set team");
                //std::cout << "wrong  team index given. Cannot set team" << std::endl;
        }
    }
    
    int setTeamName(std::string arg_team)
    {
        // este team é o argumento da função
        if (arg_team=="red" || arg_team=="green"|| arg_team=="blue") {
            team = arg_team;
            return 1;
        }
        else
        {
           // std::cout << "cannot set team name to " << team << std::endl;
            return  0;
        }
    }

    std::string getTeamName(void){return team;}
    
};
              
             
class MyPlayer : public Player
{
    public:
        boost::shared_ptr <Team> red_team;
        boost::shared_ptr <Team> green_team;
        boost::shared_ptr <Team> blue_team;
      
        boost::shared_ptr <Team> my_team;
        boost::shared_ptr <Team> my_preys;
        boost::shared_ptr <Team> my_hunters;
        ros::NodeHandle n;        
        boost::shared_ptr<ros::Subscriber> sub;

        tf::TransformBroadcaster br; 
        tf::Transform T;
         boost::shared_ptr<ros::Publisher> pub;
       

         tf::TransformListener listener;

       // s::Publisher vis_pub = node_handle.advertise<visualization_msgs::Marker>( "visualization_marker", 0 );
        
        
        
        MyPlayer(std::string name, std::string team): Player(name)
        {
            red_team   = boost::shared_ptr <Team> (new Team("red"));  
            green_team = boost::shared_ptr <Team> (new Team("green"));
            blue_team  = boost::shared_ptr <Team> (new Team("blue"));
            

          

            setTeamName(team);
            printReport();

            if (red_team->playerBelongsToTeam(name)) {
                my_team = red_team;
                my_preys = green_team;
                my_hunters = blue_team;

                setTeamName("red");
            }
            else if (green_team->playerBelongsToTeam(name)) {
                 my_team =green_team;
                my_preys =blue_team;
                my_hunters =red_team;
                
                setTeamName("green");
            }

             else if (blue_team->playerBelongsToTeam(name)) {
                 my_team =blue_team;
                my_preys =red_team;
                my_hunters =green_team;

                setTeamName("blue");
               
            }  
             
             sub = boost::shared_ptr<ros::Subscriber> (new ros::Subscriber());
            *sub = n.subscribe("/make_a_play",100, &MyPlayer::move, this);
                                                                                                                
             pub=boost::shared_ptr<ros::Publisher> (new ros::Publisher());
            *pub = n.advertise<visualization_msgs::Marker>( "/bocas", 0 );                                     
            
            
            
            struct timeval t1;
            gettimeofday(&t1, NULL);
            srand(t1.tv_usec);
            double start_x = ((double) rand()/(double)RAND_MAX)* 10 -5;
            double start_y = ((double) rand()/(double)RAND_MAX)* 10 -5;
            printf("start_x=%f start_y=%f\n",start_x, start_y);
                      

            warp(start_x, start_y, M_PI/2);
        }

        void printReport()
        {
          //  cout << "My name is " << name << " and my team is " << getTeamName() << endl;
       
            //print em ros 
            
            ROS_INFO("My name is %s and my team is %s" ,name.c_str(), getTeamName().c_str() );
        
        }




        void warp (double x, double y, double alfa)
        {
            T.setOrigin(tf::Vector3(x, y, 0.0));
            tf::Quaternion q;
            q.setRPY(0,0,alfa);
            T.setRotation(q);

            br.sendTransform(tf::StampedTransform(T, ros::Time::now(), "world", "pboucanova"));
            ROS_INFO("Warning to x=%f y=%f a=%f", x, y, alfa);


        }
          double getAngleToPLayer(string other_player, double time_to_wait=DEFAULT_TIME)
        {                                                                          
          StampedTransform t; //The transform object                               
          //Time now = Time::now(); //get the time                                 
          Time now = Time(0); //get the latest transform received                  
                                                                                   
          try{                                                                     
            listener.waitForTransform("pboucanova", other_player, now, Duration(time_to_wait));
            listener.lookupTransform("pboucanova", other_player, now, t);           
          }                                                                        
          catch (TransformException& ex){                                          
            ROS_ERROR("%s",ex.what());                                             
            return NAN;                                                            
          }                                                                        
                                                                                   
          return atan2(t.getOrigin().y(), t.getOrigin().x());                      
        }   
        void move(const rws2018_msgs::MakeAPlay::ConstPtr& msg)
        {
            double x = T.getOrigin().x();
            double y = T.getOrigin().y();
            double a = 0;

            // AI PART
            //
   
          
           double dist = 6;
           double delta_alfa = getAngleToPLayer("tosorio");   
          //
          //double delta_alfa = M_PI/2;
           
            if (isnan(delta_alfa))                                                  
           delta_alfa = 0;      


           // CONSTRAINS PART
           //
           double dist_max = msg->dog;
           double dist_with_constrains;
           dist > dist_max ? dist = dist_max: dist = dist;

           double delta_alfa_max = M_PI/30;
           fabs(delta_alfa) > fabs(delta_alfa_max) ? delta_alfa = delta_alfa_max * delta_alfa / fabs(delta_alfa): delta_alfa = delta_alfa;



           tf::Transform my_move_T;
            my_move_T.setOrigin(tf::Vector3(dist, 0.0, 0.0));

            tf::Quaternion q1;
            q1.setRPY(0,0,delta_alfa);
            my_move_T.setRotation(q1);
            T = T * my_move_T;
            br.sendTransform(tf::StampedTransform(T,ros::Time::now(), "world", "pboucanova"));


            visualization_msgs::Marker marker;
marker.header.frame_id = "pboucanova";
marker.header.stamp = ros::Time();
marker.ns = "pboucanova";
marker.id = 0;
marker.type = visualization_msgs::Marker::TEXT_VIEW_FACING;
marker.action = visualization_msgs::Marker::ADD;

marker.pose.orientation.w = 1.0;
marker.scale.z = 0.3;
marker.color.a = 1.0; // Don't forget to set the alpha!
marker.color.r = 0.0;
marker.color.g = 1.0;
marker.color.b = 0.0;
marker.lifetime = ros::Duration(2);
    marker.text = "->->-<-<-<-<";
//only if using a MESH_RESOURCE marker type:
pub->publish( marker );


        }    
        
      
};
}// fim do namespace

//Exemplo basico para overloads
int somar(int a , int b){return a+b;}
double somar(double a, double b){return a+b;}



#if 1
int main(int argc, char ** argv)
{

    ros::init(argc, argv, "pboucanova");
                          
    ros::NodeHandle n;    
   
    rws_pboucanova::MyPlayer My_player("pboucanova", "green");
    
   
//   if (My_player.green_team->playerBelongsToTeam("pboucanova")) {
//       cout << "o pedro esta na equipa certa" << endl;
//    }

 //   ros::Rate loop_rate(10);
//     while (ros::ok()) {
//         My_player.move();
 //        ros::spinOnce();
 //        loop_rate.sleep();
  //   }

    ros::spin();
}
#endif 
