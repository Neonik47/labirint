#include "lab.struct.h"
//~ #include "room.h"

int main(){
	types_init();
	add_player("John");
	add_player("LARA");

room new=get_room("maps/default/1.map");

	//~ 
	//~ printf("%d\n",new.max_x);
	//~ printf("%d\n",new.max_y);
print_room(new);
return 0;
}
