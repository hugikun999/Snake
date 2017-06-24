#include "8052.h" 
#include <stdlib.h>

code char table[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};	//Port2
code char round[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};	//Port0
code char lose[8] = {0x79, 0x76, 0x09, 0x00, 0x77, 0x00, 0xe7, 0x00};
char show_byte[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
int snake_body[20] = {28, 27, 26, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
int snake_length = 3, ran_count = 0, direction = 3, speed = 120; //direction: 0->up 1->down 2->left 3->right 
int food_pos;
void Game();
void set_food();
int check_food(int tmp_pos);
int pre_move();
int eat_food(int next);
void show();
void clear_all();
void read_snake();
void read_food();
void move(int next);
void show_end();
void delay(int t);

void main()
{
	P1 = 0x00;		//make port1 output
	P2 = 0x00; 		//make port2 ouput
	P0 = 0xFF; 		//make port3 input
	IE = 0x81;		//make INT0 enable
	TCON = 0x01;
	set_food();
	show();
	Game();
	show_end();
}

void set_food()
{
	int check = 0, tmp_pos;

	//TODO: generate random variable
	if(ran_count++ > 100)
		ran_count = 0;
	tmp_pos = snake_body[0] * snake_body[1] * snake_body[2];

	do{
		tmp_pos	= ((tmp_pos * (ran_count++)) % 64 )+1;
		if(tmp_pos < 65 && tmp_pos > 0)
			check = check_food(tmp_pos);
		else
			check = 0;
	}while(!check);
	
}

int check_food(int tmp_pos)
{
	int x;
	for(x = 0; x < snake_length; x++) {
		if(snake_body[x] == tmp_pos)
			return 0;
	}
	
	/*if(tmp_pos > 64 || tmp_pos < 1)
		return 0;*/
	food_pos = tmp_pos;
	return 1;
}

void Game()
{
	int next, result; 
	while(1){
		next = pre_move();
		if(next == -1)
			break;
		result = eat_food(next);
		move(next);
		
		if(result == 1){
			set_food();
		}
		
		show();
	}
}

int pre_move()
{
	int head, x;
	
	if(snake_length > 5)
		speed = 100;
	else if(snake_length > 7)
		speed = 75;
	else if(snake_length > 9)
		speed = 40;
	else if(snake_length > 12)
		speed = 15;
	else if(snake_length > 15)
		speed = 5;
		
	head = snake_body[0];
	switch(direction){
		case 0:
			if(head <= 8)
				return -1;
			head = head - 8;
			break;
			
		case 1:
			head = head + 8;
			break;
			
		case 2:
			if(head % 8 == 1)
				return -1;
			head = head - 1;
			break;
			
		case 3:
			if(head % 8 == 0)
				return -1;
			head = head + 1;
			break;
	}
	
	if(head > 64)
		return -1;
	
	for(x = 0; x < snake_length; x++){
		if(head == snake_body[x])
			return -1;
	}
	
	return head;
}

int eat_food(int next){
	if(next != food_pos)
		return 0;
	else{
		snake_length++;
		return 1;
	}
}

void move(int next)
{
	int tmp, x;	
	
	for(x = 0; x < snake_length;x++){
		tmp = snake_body[x];
		snake_body[x] = next;
		next = tmp;
	}
}

void show()
{
	
	int x, y = 0;
	clear_all();
	read_snake();
	read_food();
	while (y < 25) {
		for (x = 0; x < 8; x++){
			P1 = 0x00;
			P2 = 0x00;
			P1 = round[7-x];	//TODO: modify port number
			P2 = show_byte[7-x];
			delay(speed);
		}
		y++;
	}
	clear_all();
}

void clear_all()
{
	int x;
	for (x = 0; x < 8; x++){
		show_byte[x] = 0xff;
	}
}

void read_snake()
{
	int k, snake_pos, x, y;
	for (k = 0; k < snake_length; k++) {
		snake_pos = snake_body[k];
		x = snake_pos % 8;
		if(x == 0)
			x = 8;
		y = (snake_pos - x) / 8;
		show_byte[y] = show_byte[y] - table[x-1];
	}
}

void read_food()
{
	int x, y;
	x = food_pos % 8;
	if(x == 0)
		x = 8;
	y = (food_pos - x) / 8;
	show_byte[y] = show_byte[y] - table[x-1];
}

void delay(int t)
{
	int x;
	for(x=0; x < t; x++);
}

void show_end()
{
	int x;
	while (1) {
		for (x = 0; x < 8; x++){
			P1 = 0x00;
			P2 = 0x00;
			P1 = round[7-x];	//TODO: modify port number
			P2 = lose[7-x];
			delay(10);
		}
	}
}

void interrupt0() interrupt 0
{
	if(P0_3 == 0) {
		if(direction != 1)
			direction = 0;
	}
	else if(P0_1 == 0) {
		if(direction != 0)
			direction = 1;
	}
	else if(P0_0 == 0) {
		if(direction != 3)
			direction = 2;  
	}
	else if(P0_2 == 0) {
		if(direction != 2)
			direction = 3;
	}
}
