#include <queue>
#include <utility>
#include <algorithm>
#include "movement.h"
#include "peripherals.h"


using std::pair;
using std::queue;
using std::make_pair;
using std::vector;

class Floodfill{
public:
	void mainfunc();

	Movement m;
private:
	class Node{
	public:
	    bool walls[4] = {false, false, false, false};
	    int value2 = 0;
	    int value = 0;
	    bool initilized = false;
	    bool visited = false;
	    int x, y;
	    void init(Direction facing, bool frontWall, bool leftWall, bool rightWall)
	    {
	        walls[facing] = frontWall;
	        walls[(facing+1)%4] = leftWall;
	        walls[(facing+2)%4] = false;
	        walls[(facing+3)%4] = rightWall;
	    }
	    Node(){
	    }
	};

	char directions[] = {'e', 'n', 'w', 's', 'u'};
	enum Direction{
	    EAST,
	    NORTH,
	    WEST,
	    SOUTH,
	    UNINITILIZED
	};
	int MAZE_SIZE = 16;

	int getTurnDirection(int current, int target){		// function will return:
	 	return((((current-target)+5)%4)-1);				// 0 no turn
	}													// +1 turn right
									                    // -1 turn left
									                    // 2 turn backwards
	void turnByString(char* movements);	// char can be L, R, F depending on the type of movement required
										// You can modify this function to accomodate diagonal movements
};