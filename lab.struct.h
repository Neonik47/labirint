#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define TRUE 1 
#define FALSE 0

#define FLOOR     ' '
#define IN        '1'
#define OUT       '2'
#define WALL      'w'
#define FIREWALL  'W'
#define MONEY     'm'
#define HEALTH    'h'
#define GRENADE   'g'
#define GRENADE_A 'G'
#define BOMB      'b'
#define BOMB_A    'B'
#define MINE      'x'
#define DOOR_C    'c'
#define DOOR_O    'o'
#define DOOR_K    'k'

#define MAX_TYPES 30
int n_types=0;

#define MAX_PLAYERS 2
int n_players=0;

#define MAX_X 50
#define MAX_Y 50

char field[MAX_Y][MAX_X];

typedef struct block{
  char type;
  int across;
  int move;
  int health;
  int money;
  int destroy;
  int pick;
}block;

block blocks[MAX_TYPES];

void add_type(char type,int across,int move,int health,int money,int destroy,int pick){
  block new;
  new.type=type;
  new.across=across;
  new.move=move;
  new.health=health;
  new.money=money;
  new.destroy=destroy;
  new.pick=pick;
  if (n_types==MAX_TYPES){
    printf("ERR: max numb of types");
  }else
    blocks[n_types++]=new;
}

block f_block(char type){
	int i;
	for(i=0;i<n_types;i++)
		if(blocks[i].type==type)return blocks[i];
	return f_block(FLOOR);
}

//          type,      across,   move,   h,     m, dest,   pick
void types_init(){
  add_type(  FLOOR,      TRUE,  FALSE,   0,    1, FALSE, FALSE);
  add_type(  IN,         TRUE,  FALSE,   0,    1, FALSE, FALSE);
  add_type(  OUT,        TRUE,  FALSE,   0, 1000, FALSE, FALSE);
  add_type(  WALL,      FALSE,  FALSE,   0,    0,  TRUE, FALSE);
  add_type(  FIREWALL,  FALSE,  FALSE,   0,    0, FALSE, FALSE);
  add_type(  MONEY,      TRUE,  FALSE,   0,  100, FALSE,  TRUE);
  add_type(  HEALTH,     TRUE,  FALSE,  30,    1, FALSE,  TRUE);
  add_type(  GRENADE,    TRUE,  FALSE,   0,    1, FALSE,  TRUE);
  add_type(  GRENADE_A,  TRUE,  FALSE, -30, -500, FALSE, FALSE);
  add_type(  BOMB,       TRUE,  FALSE,   0,    0, FALSE,  TRUE);
  add_type(  BOMB_A,     TRUE,  FALSE, -50, -1000, FALSE, FALSE);
  add_type(  MINE,       TRUE,  FALSE, -70, -1500,  TRUE, FALSE);
  add_type(  DOOR_C,    FALSE,  FALSE,   0,    1, FALSE, FALSE);
  add_type(  DOOR_O,     TRUE,  FALSE,   0,    1, FALSE, FALSE);
  add_type(  DOOR_K,     TRUE,  FALSE,   0,    1, FALSE,  TRUE);
}

typedef struct player{
	char name[20];
	int health;
	int money;
	int bombs;
	int keys;
	int grenades;
	int flags;
}player;

player players[MAX_PLAYERS];

void add_player(char name[20]){
	player new;
	#bzero(new.name,20);
        memset(new.name,'\0',20);
	strcpy(new.name, name);
	new.health=100;
	new.money=0;
	new.bombs=0;
	new.keys=1;
	new.grenades=0;
	new.flags=5;
	if (n_players==MAX_PLAYERS){
		printf("ERR: max numb of players\n");
	}else
		players[n_players++]=new;
}

typedef struct room{
	int max_x;//j
	int max_y;//i
	char map[MAX_Y][MAX_X];
}room;

room get_room(char *filename){
	FILE *pFile = fopen(filename, "r");
	room new;
	new.max_x=-1;
	new.max_y=-1;
	if(pFile == NULL){
		printf("CAN'T OPEN FILE \"%s\"\n",filename);
		return new;
	}
	
	char line[MAX_Y];
	new.max_x=0;
	new.max_y=0;
	while (1){
		bzero(line,MAX_X);
		fgets(line,MAX_X,pFile);
		if(feof(pFile) || new.max_y == MAX_Y)
			break;
		bzero(new.map[new.max_y], MAX_Y);
		strcpy(new.map[new.max_y++], line);
		if (new.max_x < strlen(line)-1) new.max_x=strlen(line)-1;
	}
	fclose(pFile);
	return new;
}

void print_room(room r){
	int i,j;
	for (i=0;i<r.max_y;i++){
		for(j=0;j<r.max_x;j++){
			printf("%c",r.map[i][j]);
		}
		printf("\n");
	}
}

int axy(int n,room r){
	if(n>0) return n;
return 0;
}

int bx (int n, room r){
	if (n<r.max_x - 1) return n;
return r.max_x-1;
}

int by (int n, room r){
	if (n<r.max_y - 1) return n;
return r.max_y-1;
}

int destroy(int y, int x, room r){
	if (x>=0 && x<=r.max_x && y>=0 && y<=r.max_y)
		if(f_block(r.map[y][x]).destroy) return 1;
	return 0;
}

int is_type(char type, int y, int x, room r){
	if(x>=0 && x<=r.max_x && y>=0 && y<=r.max_y)
		if(f_block(r.map[y][x]).type==type) return 1;
	return 0;
}

void motion(room r, int pl_index){
	char kbd_key;
	scanf("%c",&kbd_key);
	int dx=0;int dy=0;
	if(kbd_key=='w') d_y=-1;
	else if(kbd_key=='a') d_x=-1;
	else if(kbd_key=='s') d_y=1;
	else if(kbd_key=='d') d_x=1;
	else action(kbd_key, r, pl_index);
	player p=players[pl_index];
	block b=f_block(r.map[p.y+dy][p.x+dx]);
	if(b.type==DOOR_C && p.keys){
		r.map[p.y+dy][p.x+dx]=DOOR_O;
		p.keys--;
		b=f_block(DOOR_O);
	}
	if(!b.across) break;
	p.y+=dy;p.x+=dx;
	p.health+=b.health;
	p.money+=b.money;
	if(b.pick){
		if(b.type==KEY) p.keys++;
		else if(b.type==GRENADE) p.grenades++;
		else if(b.type==BOMB) p.bombs++;
		r.map[p.y][p.x]=FLOOR;
	}
	player[pl_index]=p;
}

void action(char kbd_key, room r, int pl_index){
	player p=players[pl_index];
	if(kbd_key=='b')
		if(p.bombs && r.map[p.y][p.x]==FLOOR){r.map[p.y][p.x]=BOMB_A;p.bombs--;}
	else if(kbd_key=='e'){
		int x,y,x1,y1,x2,y2,i;
		x1=axy(p.x-5,r);x2=bx(p.x+5,r);
		y1=axy(p.y-5,r);y2=by(p.y+5,r);
!!!d		for(x=x1;x<x2;x++){
			if(is_type(BOMB_A,y,x,r){
				for(i=x-2;i<=x+2;i++) if(destroy(y,i,r)) r.map[y][i]=FLOOR;
				for(i=y-2;i<=y+2;i++) if(destroy(i,x,r)) r.map[i][x]=FLOOR;
				if(destroy(y-1,x-1,r)) r.map[y-1][x-1]=FLOOR;
				if(destroy(y-1,x+1,r)) r.map[y-1][x+1]=FLOOR;
				if(destroy(y+1,x-1,r)) r.map[y+1][x-1]=FLOOR;
				if(destroy(y+1,x+1,r)) r.map[y+1][x+1]=FLOOR;
			}
		}
	}
}
