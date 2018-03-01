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
                std::cout << "wrong  team index given. Cannot set team" << std::endl;
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
            std::cout << "cannot set team name to " << team << std::endl;
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

        tf::TransformBroadcaster br; 
      
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
        }

        void printReport()
        {
            cout << "My name is " << name << " and my team is " << getTeamName() << endl;
        }

        void move(void)
        {
            tf::Transform transform;
            transform.setOrigin( tf::Vector3(3, 6, 0.0) );
            tf::Quaternion q;
            q.setRPY(0, 0,M_PI );
            transform.setRotation(q);
            br.sendTransform(tf::StampedTransform(transform, ros::Time::now(), "world","pboucanova" ));
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
    
   
   if (My_player.green_team->playerBelongsToTeam("pboucanova")) {
       cout << "o pedro esta na equipa certa" << endl;
    }

    ros::Rate loop_rate(10);
     while (ros::ok()) {
         My_player.move();
         ros::spinOnce();
         loop_rate.sleep();
     }

    ros::spin();
}
#endif  
 
