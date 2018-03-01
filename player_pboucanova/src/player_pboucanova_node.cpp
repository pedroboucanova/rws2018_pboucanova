#include <iostream>


using namespace std;

namespace rws_pboucanova
{


class Player
{
private:
       std::string team;                        

public:
    Player(std::string argin_name) { this->name = argin_name; }
    
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
    
    int setTeamName(std::string team)
    {
        // este team é o argumento da função
        if (team=="red" || team=="green"|| team=="blue") {
            this->team = team;
            return 1;
        }
        else
        {
            std::cout << "cannot set team name to " << team << std::endl;
            return  0;
        }
    }

    std::string getTeamName(void){return team;}
    std::string name;
};

class MyPlayer : public Player
{
    public:
        MyPlayer(std::string name, std::string team): Player(name)
        {
            setTeamName(team);
        }
};
}

//Exemplo basico para overloads
int somar(int a , int b){return a+b;}
double somar(double a, double b){return a+b;}



#if 1
int main()
{
    rws_pboucanova::MyPlayer My_player("pboucanova", "green");

    std::cout << "my_player.name is " << My_player.name << std::endl;
    std::cout << "team is " << My_player.getTeamName() << std::endl;
}
#endif  
   

#if 0
int main()
{
    myPlayer my_player("pboucanova", "green");


    std::cout << "my_player.name is " << my_player.name << std::endl;
    std::cout << "team is " << my_player.getTeamName() << std::endl;
}
#endif  
 

#if 0
int main()
{
    Player player("pboucanova");
    player.setTeamName("red");
    player.setTeamName(2);

    std::cout << "player.name is " << player.name << std::endl;
    std::cout << "team is " << player.getTeam() << std::endl;
}
#endif  

#if 0
int main()
{
   std::string player_name = "pboucanova";

    Player player(player_name);
    
    
    std::cout << "Created an instance of class player with public name " << player.name << std::endl;
}

#endif
#if 0  /**Parte 1*/

int main()
{
    std::cout << "Hello world" << std::endl;
    return 1;
}

#endif

