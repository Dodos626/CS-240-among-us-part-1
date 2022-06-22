/***************************************************************
 *                                                             *
 * file: main.cpp                                                *
 *                                                             *
 * @Author  Antonios Peris                         		   	   *
 * @Version 20-10-2020                             			   *
 * @email   csdp1196@csd.uoc.gr                                *
 *                                                             *
 * @brief   Main function for the needs of cs-240 project 2020 *
 *                                                             *
 ***************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <iostream>
#include "among_us.h"
using namespace std;
/***************************************************
 *                                                 *
 * file: among_us.cpp                              *
 *                                                 *
 * @Author  Antonios Peris                         *
 * @Version 20-10-2020                             *
 * @email   csdp1196@csd.uoc.gr                    *
 *                                                 *
 * @brief   Implementation of among_us.h           *
 *                                                 *
 ***************************************************
 */

/**
 * @brief Optional function to initialize data structures that 
 *        need initialization
 *
 * @return 1 on success
 *         0 on failure
 */
int initialize()
{
	// players
	players_head = new Players;
	players_head->pid = -1;
	players_head->is_alien = -1;
	players_head->next = players_head;
	players_head->prev = players_head;

	//tasks
	tasks_head = new Head_GL;
	tasks_head->tasks_count[0] = 0;
	tasks_head->tasks_count[1] = 0;
	tasks_head->tasks_count[2] = 0;
	tasks_head->head = NULL;

	//tasks completed
	tasks_stack = new Head_Completed_Task_Stack;
	tasks_stack->count = 0;
	tasks_stack->head = NULL;

	return 1;
}

/**
 * @brief Register player
 *
 * @param pid The player's id
 *
 * @param is_alien The variable that decides if he is an alien or not
 * @return 1 on success
 *         0 on failure
 */
int register_player(int pid, int is_alien)
{
	Tasks *task_sentinel = new Tasks;
	task_sentinel->next = task_sentinel;
	task_sentinel->difficulty = -1;
	task_sentinel->tid = -1;
	// for the first player
	if (players_head->next == players_head)
	{
		Players *to_add = new Players;
		to_add->evidence = 0;
		to_add->prev = players_head;
		to_add->next = players_head;
		to_add->pid = pid;
		to_add->is_alien = is_alien;
		to_add->tasks_head = task_sentinel;
		to_add->tasks_sentinel = task_sentinel;
		players_head->next = to_add;
		players_head->prev = to_add;
		print_players();
		return 1;
	}

	Players *next1 = players_head;
	Players *last = players_head;

	last=players_head->prev;

	


	//creates the new player
	Players *to_add = new Players;
	to_add->pid = pid;
	to_add->is_alien = is_alien;
	to_add->next = next1;
	to_add->prev = last;
	to_add->evidence = 0;
	to_add->tasks_head = task_sentinel;
	to_add->tasks_sentinel = task_sentinel;

	//puts it inbetween the end and the last
	last->next = to_add;
	next1->prev = to_add;

	print_players();
	return 1;
}

/**
 * @brief Insert task in the general task list
 *
 * @param tid The task id
 * 
 * @param difficulty The difficulty of the task
 *
 * @return 1 on success
 *         0 on failure
 */
int insert_task(int tid, int difficulty)
{
	//adds + 1 in the tasks_count meter
	tasks_head->tasks_count[difficulty - 1]++;

	//if i add the first task in the task list
	if (tasks_head->head == NULL)
	{
		Tasks *to_add = new Tasks;
		tasks_head->head = to_add;
		tasks_head->head->difficulty = difficulty;
		tasks_head->head->next = NULL;
		tasks_head->head->tid = tid;

		print_tasks();
		return 1;
	}
	Tasks *last = tasks_head->head;
	while (last->next != NULL)
	{
		if (difficulty < last->next->difficulty)
		{
			Tasks *to_add = new Tasks;
			to_add->difficulty = difficulty;
			to_add->tid = tid;
			to_add->next = last->next;
			last->next = to_add;
			print_tasks();
			return 1;
		}
		last = last->next;
	}
	Tasks *to_add = new Tasks;
	to_add->difficulty = difficulty;
	to_add->tid = tid;
	to_add->next = NULL;
	last->next = to_add;
	print_tasks();

	return 1;
}

/**
 * @brief Distribute tasks to the players
 * @return 1 on success
 *         0 on failure
 */
int distribute_tasks()
{
	Players *currentPlayer = players_head;
	Tasks *currentTask = tasks_head->head;
	int total_tasks = tasks_head->tasks_count[0] + tasks_head->tasks_count[1] + tasks_head->tasks_count[2];

	for (int i = 0; i < total_tasks; i++)
	{
		//check if current player is player sentinel or imposter
		while ((currentPlayer->is_alien == -1) || (currentPlayer->is_alien == 1))
		{
			currentPlayer = currentPlayer->next;
		}

		//check if no tasks are given
		if (currentPlayer->tasks_head == currentPlayer->tasks_sentinel)
		{
			Tasks *to_add = new Tasks;
			to_add->difficulty = currentTask->difficulty;
			to_add->tid = currentTask->tid;

			to_add->next = currentPlayer->tasks_sentinel;
			currentPlayer->tasks_sentinel->next = to_add;
			currentPlayer->tasks_head = to_add;
		}
		else
		{
			//in case there are other taks looks for the last and ads the
			//new at the end of the task list
			Tasks *to_add = new Tasks;
			to_add->difficulty = currentTask->difficulty;
			to_add->tid = currentTask->tid;

			to_add->next = currentPlayer->tasks_sentinel;

			currentPlayer->tasks_sentinel->next->next = to_add;
			currentPlayer->tasks_sentinel->next = to_add;
		}

		currentPlayer = currentPlayer->next;
		currentTask = currentTask->next;
	}
	print_double_list();

	return 1;
}

/**
 * @brief Implement Task
 *
 * @param pid The player's id
 *
 * @param difficulty The task's difficuly
 *
 * @return 1 on success
 *         0 on failure
 */
int implement_task(int pid, int difficulty)
{

	Players *top = players_head->next;
	//looks for player with pid
	while (top != players_head)
	{
		if (top->pid == pid)
		{
			break;
		}
		top = top->next;
	}
	if (top == players_head)
	{
		return 1;
	}

	//if impostor or something went wrong doesnt work
	if (top->is_alien != 0)
	{
		return 1;
	}
	if (top->tasks_head == top->tasks_sentinel)
	{
		return 1;
	}

	//if a task with same difficulty is found at top it's
	//immidiatly removed
	//if after the top comes the sentinel
	if (top->tasks_head->next == top->tasks_sentinel)
	{
		putstack(top->tasks_head);
		top->tasks_head = top->tasks_sentinel;
		top->tasks_sentinel->next = top->tasks_sentinel;
		print_double_list();
		return 1;
	}

	if (top->tasks_head->difficulty == difficulty)
	{
		Tasks *to_del = top->tasks_head;
		top->tasks_head = top->tasks_head->next;

		putstack(to_del);

		print_double_list();
		return 1;
	}

	Tasks *to_del = top->tasks_head;
	Tasks *prev_del = top->tasks_head;
	//looks if there is task with given difficulty

	while (to_del->next != top->tasks_sentinel)
	{

		//if it has same difficulty it deletes it from player
		//and pops it in stack of completed
		if (to_del->next->difficulty == -1)
		{
			break;
		}

		if (difficulty == to_del->difficulty)
		{
			prev_del->next = to_del->next;

			putstack(to_del);

			print_double_list();
			return 1;
		}
		prev_del = to_del;
		to_del = to_del->next;
	}
	//if no task is found with same difficulty is found
	//so it deletes the first but there is only one

	if (top->tasks_head->next == top->tasks_sentinel)
	{
		Tasks *to_del = top->tasks_head;
		top->tasks_head = top->tasks_head->next;
		top->tasks_sentinel->next = top->tasks_sentinel;
		putstack(to_del);

		print_double_list();
		return 1;
	}

	//if no task with same difficulty found
	//it just deletes the last one

	prev_del->next = to_del->next;
	putstack(to_del);
	top->tasks_sentinel->next = prev_del;

	print_double_list();

	return 1;
}

void putstack(Tasks *to_add)
{
	tasks_stack->count++;
	//if its the first node to enter
	if (tasks_stack->head == NULL)
	{
		tasks_stack->head = to_add;
		to_add->next = NULL;
	}
	else
	{
		to_add->next = tasks_stack->head;
		tasks_stack->head = to_add;
	}
}
Tasks *pop_ret_task()
{

	// if stack has only one node
	if (tasks_stack->head->next == NULL)
	{
		Tasks *ret = tasks_stack->head;
		tasks_stack->head = NULL;
		tasks_stack->count--;
		return ret;
	}

	//count of nodes in stack are -1
	tasks_stack->count--;
	//poped is the one to pop and return
	Tasks *poped = tasks_stack->head;
	tasks_stack->head = tasks_stack->head->next;
	poped->next = NULL;

	return poped;
}


/**
 * @brief Eject Player
 * 
 * @param pid The ejected player's id
 *
 * @return 1 on success
 *         0 on failure
 */
int eject_player(int pid)
{
	Players *pid_min;
	Players *top = players_head->next;

	int min = 0;
	while (top != players_head)
	{
		if ((top->pid != pid) && (top->is_alien == 0))
		{
			int current_task_num = 0;
			Tasks *current_task = top->tasks_head;
			while (current_task->difficulty != -1)
			{
				current_task_num++;
				current_task = current_task->next;
			}

			if (min == 0)
			{
				min = current_task_num;
				pid_min = top;
			}
			else if (min > current_task_num)
			{
				min = current_task_num;
				pid_min = top;
			}
		}
		top = top->next;
	}

	top = players_head->next;
	Players *to_eject;

	while (top != players_head)
	{
		if (top->pid == pid)
		{

			to_eject = top;
			break;
		}

		top = top->next;
	}

	//delete player
	to_eject->prev->next = to_eject->next;
	to_eject->next->prev = to_eject->prev;

	Tasks initiate;
	//final list
	Tasks *Result_list = &initiate;
	//list1 and list2 are merged and sorted
	Tasks *list1 = to_eject->tasks_head;
	Tasks *list2 = pid_min->tasks_head;

	//while none is at the sentinel
	while ((list1->difficulty != -1) && (list2->difficulty != -1))
	{
		//if list1 has bigger difficulty than list 2
		// result list takes list 1 node else list 2 node

		if (list1->difficulty < list2->difficulty)
		{
			Result_list->next = list1;
			list1 = list1->next;
		}
		else
		{
			Result_list->next = list2;
			list2 = list2->next;
		}
		if ((list1->difficulty == -1) || (list2->difficulty == -1))
		{
			Result_list = Result_list->next;
			break;
		}

		Result_list = Result_list->next;
	}

	//the list that didnt end goes into the rest of the
	// result list
	if (list1 == to_eject->tasks_sentinel)
	{
		Result_list->next = list2;
	}
	else
	{
		Result_list->next = list1;
		pid_min->tasks_sentinel = to_eject->tasks_sentinel;
	}

	pid_min->tasks_head = initiate.next;

	print_double_list();
	return 1;
}

/**
 * @brief Witness Eject Player
 *
 * @param pid_a The alien's pid
 * 
 * @param pid The crewmate's pid
 * 
 * @param number_of_witnesses The number of witnesses
 *
 * @return 1 on success
 *         0 on failure
 */
int witness_eject(int pid, int pid_a, int number_of_witnesses)
{
	Players *pid_min;
	Players *top = players_head->next;

	int min = 0;
	while (top != players_head)
	{
		if ((top->pid != pid) && (top->is_alien == 0))
		{
			int current_task_num = 0;
			Tasks *current_task = top->tasks_head;
			while (current_task->difficulty != -1)
			{
				current_task_num++;
				current_task = current_task->next;
			}

			if (min == 0)
			{
				min = current_task_num;
				pid_min = top;
			}
			else if (min > current_task_num)
			{
				min = current_task_num;
				pid_min = top;
			}
		}
		top = top->next;
	}

	top = players_head->next;
	Players *to_eject = NULL;

	while (top != players_head)
	{
		if (top->pid == pid)
		{

			to_eject = top;
			break;
		}

		top = top->next;
	}
	if (to_eject == NULL)
	{
		return 1;
	}

	//delete player
	to_eject->prev->next = to_eject->next;
	to_eject->next->prev = to_eject->prev;

	Tasks initiate;
	//final list
	Tasks *Result_list = &initiate;
	//list1 and list2 are meged and sorted
	Tasks *list1 = to_eject->tasks_head;
	Tasks *list2 = pid_min->tasks_head;
	print_double_list();
	//while none is at the sentinel
	while ((list1->difficulty != -1) && (list2->difficulty != -1))
	{
		//if list1 has bigger difficulty than list 2
		// result list takes list 1 node else list 2 node
		if (list1->difficulty < list2->difficulty)
		{
			Result_list->next = list1;
			list1 = list1->next;
		}
		else
		{
			Result_list->next = list2;
			list2 = list2->next;
		}
		if ((list1->difficulty == -1) || (list2->difficulty == -1))
		{
			Result_list = Result_list->next;
			break;
		}
		Result_list = Result_list->next;
	}

	//the list that didnt end goes into the rest of the
	// result list
	if (list1->difficulty == -1)
	{
		//Tasks *fix_sentinel = Result_list;
		//Tasks *fix_sentinel2 = Result_list;
		Result_list->next = list2;
		//FIND THE LAST NODE BEFORE SENTINEL
		//while (fix_sentinel->difficulty!=-1)
		//{
		//	fix_sentinel2=fix_sentinel;
		//	fix_sentinel=fix_sentinel->next;
		//}
		//fix_sentinel =Result_list;
		//FIND SENTINEL
		//while (fix_sentinel->difficulty!=-1)
		//{
		//	fix_sentinel=fix_sentinel->next;
		//}
		//FIX SENTINEL
		//fix_sentinel->next=fix_sentinel2;
	}
	else
	{
		Result_list->next = list1;
		//Tasks * fix_sentinel=Result_list;
		//Tasks * fix_sentinel2=Result_list;
		//find new sentinel and last node
		//while (fix_sentinel->difficulty!=-1)
		//{
		//	fix_sentinel2=fix_sentinel;
		//	fix_sentinel=fix_sentinel->next;
		//}
		//fix_sentinel->next=fix_sentinel2;
		pid_min->tasks_sentinel = to_eject->tasks_sentinel;
	}

	pid_min->tasks_head = initiate.next;

	Players *alienator = players_head->next;
	while (alienator != players_head)
	{
		if (alienator->pid == pid_a)
		{
			alienator->evidence = alienator->evidence + number_of_witnesses;
			break;
		}

		alienator = alienator->next;
	}

	print_double_list_sus();
	return 1;
}

/**
 * @brief Sabbotage
 *
 * @param pid The player's id
 *
 * @param number_of_tasks The number of tasks to be popped
 * 
 * @return 1 on success
 *         0 on failure
 */
int sabbotage(int pid, int number_of_tasks)
{
	//choosen_pid will find and keep track
	//of the choosen player with the pid
	Players *choosen_pid = players_head->next;

	//neo_the_one is the one that will take
	//the burden of tasks through the
	//algorithm given
	Players *neo_the_one = players_head;

	//find the player with given pid
	while (choosen_pid != players_head)
	{
		if (choosen_pid->pid == pid)
		{
			break;
		}

		choosen_pid = choosen_pid->next;
	}

	//find the player tasks/2 seats left of the pid-choosen
	neo_the_one = choosen_pid;
	for (int i = 0; i < (number_of_tasks / 2); i++)
	{
		neo_the_one = neo_the_one->prev;
		if (neo_the_one == players_head)
		{
			neo_the_one = neo_the_one->prev;
		}
	}
	//if neo is alien or sentinel go to the right
	//repeat till crewmate is found
	while (neo_the_one->is_alien != 0)
	{
		neo_the_one = neo_the_one->next;
	}

	for (int i = 0; i < number_of_tasks; i++)
	{
		Tasks *to_put = pop_ret_task();
		Tasks *list1 = neo_the_one->tasks_head;
		Tasks *list2 = neo_the_one->tasks_head;
		bool flag = true;
		//if the the player has no tasks
		if (list1 == neo_the_one->tasks_sentinel)
		{
			neo_the_one->tasks_head = to_put;
			to_put->next = neo_the_one->tasks_sentinel;
			neo_the_one->tasks_sentinel->next = to_put;
		}

		else
		{
			while (list1->difficulty < to_put->difficulty)
			{
				//if i have to put the task before the sentinel
				if (list1->next == neo_the_one->tasks_sentinel)
				{
					to_put->next = neo_the_one->tasks_sentinel;
					neo_the_one->tasks_sentinel->next = to_put;
					list1->next = to_put;
					flag = false;
				}
				list2 = list1;
				list1 = list1->next;
			}
			if (flag)
			{
				//if the player has only one task and i have to put it
				//before the first task
				if (list1 == neo_the_one->tasks_head)
				{
					neo_the_one->tasks_head = to_put;
					to_put->next = list1;
				}
				//if i have to put it between two nodes
				else
				{
					to_put->next = list1;
					list2->next = to_put;
				}
			}
		}
		//searches for the next canditate to be the one
		//while skipping sentinel and impostors
		neo_the_one = neo_the_one->next;
		while (neo_the_one->is_alien != 0)
		{
			neo_the_one = neo_the_one->next;
		}
	}

	print_double_list();

	return 1;
}

/**
 * @brief Vote
 *
 * @param pid The player's id
 * 
 * @param vote_evidence The vote's evidence
 *
 * @return 1 on success
 *         0 on failure
 */
int vote(int pid, int vote_evidence)
{

	Players *top = players_head->next;

	//adding the evidence
	while (top != players_head)
	{
		if (top->pid == pid)
		{
			top->evidence = top->evidence + vote_evidence;
		}

		top = top->next;
	}

	top = players_head->next;
	int max = top->evidence;
	Players *to_be_ejected = top;

	//searching for highest evidence target
	while (top != players_head)
	{
		if (top->evidence > max)
		{
			to_be_ejected = top;
			max = top->evidence;
		}

		top = top->next;
	}

	Vote_eject(to_be_ejected->pid);
	return 1;
}
int Vote_eject(int pid)
{
	Players *pid_min;
	Players *top = players_head->next;

	int min = 0;
	while (top != players_head)
	{
		if ((top->pid != pid) && (top->is_alien == 0))
		{
			int current_task_num = 0;
			Tasks *current_task = top->tasks_head;
			while (current_task->difficulty != -1)
			{
				current_task_num++;
				current_task = current_task->next;
			}

			if (min == 0)
			{
				min = current_task_num;
				pid_min = top;
			}
			else if (min > current_task_num)
			{
				min = current_task_num;
				pid_min = top;
			}
		}
		top = top->next;
	}

	top = players_head->next;
	Players *to_eject;

	while (top != players_head)
	{
		if (top->pid == pid)
		{

			to_eject = top;
			break;
		}

		top = top->next;
	}

	//delete player
	to_eject->prev->next = to_eject->next;
	to_eject->next->prev = to_eject->prev;

	Tasks initiate;
	//final list
	Tasks *Result_list = &initiate;
	//list1 and list2 are meged and sorted
	Tasks *list1 = to_eject->tasks_head;
	Tasks *list2 = pid_min->tasks_head;

	//while none is at the sentinel
	while ((list1 != to_eject->tasks_sentinel) && (list2 != pid_min->tasks_sentinel))
	{
		//if list1 has bigger difficulty than list 2
		// result list takes list 1 node else list 2 node

		if (list1->difficulty < list2->difficulty)
		{
			Result_list->next = list1;
			list1 = list1->next;
		}
		else
		{
			Result_list->next = list2;
			list2 = list2->next;
		}
		if ((list1->difficulty == -1) || (list2->difficulty == -1))
		{
			Result_list = Result_list->next;
			break;
		}

		Result_list = Result_list->next;
	}

	//the list that didnt end goes into the rest of the
	// result list
	if (list1 == to_eject->tasks_sentinel)
	{
		Result_list->next = list2;
	}
	else
	{
		Result_list->next = list1;
		pid_min->tasks_sentinel = to_eject->tasks_sentinel;
	}

	pid_min->tasks_head = initiate.next;
	print_double_list_sus();
	return 1;
}
/**
 * @brief Give Away Work
 *
 * @return 1 on success
 *         0 on failure
 */
int give_work()
{
	Players *pid_min;
	Players *top = players_head->next;
	cout << tasks_head->tasks_count[0] + tasks_head->tasks_count[1] + tasks_head->tasks_count[2] << endl;
	cout << tasks_stack->count << endl;
	if ((tasks_head->tasks_count[0] + tasks_head->tasks_count[1] + tasks_head->tasks_count[2]) == tasks_stack->count)
	{
		return 1;
	}

	int min = 0;
	//finds the min task player
	while (top != players_head)
	{
		if (top->is_alien == 0)
		{
			int current_task_num = 0;
			Tasks *current_task = top->tasks_head;
			while (current_task->difficulty != -1)
			{
				current_task_num++;
				current_task = current_task->next;
			}

			if (min == 0)
			{
				min = current_task_num;
				pid_min = top;
			}
			else if (min > current_task_num)
			{
				min = current_task_num;
				pid_min = top;
			}
		}
		top = top->next;
	}
	//finds the max task player
	top = players_head->next;
	Players *max_player;
	int max = -10;

	while (top != players_head)
	{
		if (top->is_alien == 0)
		{
			int current_task_num = 0;
			Tasks *current_task = top->tasks_head;
			while (current_task->difficulty != -1)
			{
				current_task_num++;
				current_task = current_task->next;
			}

			if (current_task_num > max)
			{
				max_player = top;
				max = current_task_num;
			}
		}
		top = top->next;
	}
	if (max_player == pid_min)
	{
		return 1;
	}

	Tasks *list1 = max_player->tasks_head;
	Tasks *tmp = max_player->tasks_head;

	for (int i = 0; i < max / 2; i++)
	{
		tmp = tmp->next;
	}
	max_player->tasks_head = tmp;

	tmp = list1;

	for (int i = 1; i < max / 2; i++)
	{
		tmp = tmp->next;
	}

	Tasks *new_sentinel = new Tasks;
	new_sentinel->difficulty = -1;
	new_sentinel->next = tmp;
	new_sentinel->tid = -1;
	//max_player->tasks_sentinel = new_sentinel;
	tmp->next = new_sentinel;

	Tasks initiate;
	//final list
	Tasks *Result_list = &initiate;
	//list1 and list2 are meged and sorted

	Tasks *list2 = pid_min->tasks_head;

	//while none is at the sentinel
	while ((list1->difficulty != -1) && (list2->difficulty != -1))
	{
		//if list1 has bigger difficulty than list 2
		// result list takes list 1 node else list 2 node
		if (list1->difficulty < list2->difficulty)
		{
			Result_list->next = list1;
			list1 = list1->next;
		}
		else
		{
			Result_list->next = list2;
			list2 = list2->next;
		}

		Result_list = Result_list->next;
	}
	//the list that didnt end goes into the rest of the
	// result list
	if (list1->difficulty == -1)
	{
		Result_list->next = list2;
	}
	else
	{
		Result_list->next = list1;
		pid_min->tasks_sentinel = new_sentinel;
	}

	pid_min->tasks_head = initiate.next;
	print_double_list();

	return 1;
}

/**
 * @brief Terminate
 *
 * @return 1 on success
 *         0 on failure
 */
int Terminate()
{
	Players *top = players_head->next;
	int alliens = 0;
	int ppl = 0;
	while (top != players_head)
	{
		if (top->is_alien == 0)
		{
			ppl++;
		}
		else
		{
			alliens++;
		}
		top = top->next;
	}
	if (ppl < alliens)
	{
		cout << "ALIENS WIN" << endl;
		return 1;
	}
	int total_tasks = tasks_head->tasks_count[0] +
					  tasks_head->tasks_count[1] +
					  tasks_head->tasks_count[2];
	if (total_tasks == tasks_stack->count)
	{
		cout << "Crewmates win" << endl;
		return 1;
	}
	if (alliens == 0)
	{
		cout << "Crewmates win" << endl;
		return 1;
	}

	cout << "game still in progress " << endl;

	return 1;
}

/**
 * @brief Print Players
 *
 * @return 1 on success
 *         0 on failure
 */
int print_players()
{
	Players *top = players_head->next;
	cout << "Players : ";
	while (top != players_head)
	{
		cout << "<" << top->pid << ":" << top->is_alien << "> ";
		top = top->next;
	}
	cout << endl;
	return 1;
}

/**
 * @brief Print Tasks
 *
 * @return 1 on success
 *         0 on failure
 */
int print_tasks()
{
	Tasks *top = tasks_head->head;
	cout << "Tasks : ";
	while (top != NULL)
	{
		cout << "<" << top->tid << ":" << top->difficulty << "> ";
		top = top->next;
	}
	cout << endl;
	return 1;
}
/**
 * @brief Print Stack
 *
 * @return 1 on success
 *         0 on failure
 */
int print_stack()
{
	
	if (tasks_stack->head == NULL)
	{
		cout << " Empty Stack" << endl;
	}
	else
	{
		Tasks *top = tasks_stack->head;
		cout << "Tasks Completed: ";
		print_tasks_recur(top);
		cout << endl;
	}

	return 1;
}
void print_tasks_recur(Tasks *p)
{

	if (p != NULL)
	{
		print_tasks_recur(p->next);
		cout << "<" << p->tid << ":" << p->difficulty << "> ";
	}

	
}
/**
 * @brief Print Players & Task List
 *
 * @return 1 on success
 *         0 on failure
 */
int print_double_list()
{
	Players *top = players_head->next;

	while (top != players_head)
	{

		cout << "Player " << top->pid << " ";
		Tasks *current_task = top->tasks_head;
		while (current_task->difficulty != -1)
		{

			cout << "<" << current_task->tid << "," << current_task->difficulty << "> ";
			current_task = current_task->next;
			if (current_task->tid == -1)
			{
				break;
			}
		}

		cout << endl;
		top = top->next;
	}

	return 1;
}
int print_double_list_sus()
{
	Players *top = players_head->next;
	while (top != players_head)
	{
		cout << "<Player " << top->pid << "," << top->evidence << "> ";
		Tasks *current_task = top->tasks_head;
		while (current_task->difficulty != -1)
		{
			cout << "<" << current_task->tid << "," << current_task->difficulty << "> ";
			current_task = current_task->next;
		}
		cout << endl;
		top = top->next;
	}

	return 1;
}
/**
 * @brief Free resources
 *
 * @return 1 on success
 *         0 on failure
 */

int free_all(void)
{
	//hahahahahhahahahhahahahahah
	return 1;
}

#define BUFFER_SIZE 1024 /* Maximum length of a line in input file */

/* Uncomment the following line to enable debugging prints 
 * or comment to disable it */
#define DEBUG
#ifdef DEBUG
#define DPRINT(...) fprintf(stderr, __VA_ARGS__);
#else /* DEBUG */
#define DPRINT(...)
#endif /* DEBUG */

/**
 * @brief The main function
 *
 * @param argc Number of arguments
 * @param argv Argument vector
 *
 * @return 0 on success
 *         1 on failure
 */
int main(int argc, char **argv)
{
	FILE *fin = NULL;
	char buff[BUFFER_SIZE], event = '\0';

	/* Check command buff arguments */
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <input_file> <>\n", argv[0]);
		return EXIT_FAILURE;
	}

	/* Open input file */
	if ((fin = fopen(argv[1], "r")) == NULL)
	{
		fprintf(stderr, "\n Could not open file: %s\n", argv[1]);
		perror("Opening test file\n");
		return EXIT_FAILURE;
	}

	/* Initializations */
	initialize();

	/* Read input file buff-by-buff and handle the events */
	while (fgets(buff, BUFFER_SIZE, fin))
	{

		DPRINT("\n>>> Event: %s", buff);

		switch (buff[0])
		{

		/* Comment */
		case '#':
			break;

		/* Register player
			 * P <pid><is_alien> */
		case 'P':
		{
			int pid;
			int is_alien;
			sscanf(buff, "%c %d %d", &event, &pid, &is_alien);
			DPRINT("%c %d %d\n", event, pid, is_alien);

			if (register_player(pid, is_alien))
			{
				DPRINT("P %d %d succeeded\n", pid, is_alien);
			}
			else
			{
				fprintf(stderr, "P %d %d failed\n", pid, is_alien);
			}

			break;
		}

		/* Insert task
			 * T <tid><difficulty>  */
		case 'T':
		{
			int tid, difficulty;

			sscanf(buff, "%c %d %d", &event, &tid, &difficulty);
			DPRINT("%c %d %d\n", event, tid, difficulty);

			if (insert_task(tid, difficulty))
			{
				DPRINT("%c %d %d succeded\n", event, tid, difficulty);
			}
			else
			{
				fprintf(stderr, "%c %d %d failed\n", event, tid, difficulty);
			}

			break;
		}

		/* Distribute Tasks
			 * D */
		case 'D':
		{
			sscanf(buff, "%c", &event);
			DPRINT("%c\n", event);

			if (distribute_tasks())
			{
				DPRINT("%c succeeded\n", event);
			}
			else
			{
				fprintf(stderr, "%c failed\n", event);
			}

			break;
		}

		/* Implement Task
			 * I <pid> <difficulty> */
		case 'I':
		{
			int pid, difficulty;

			sscanf(buff, "%c %d %d", &event, &pid, &difficulty);
			DPRINT("%c %d %d \n", event, pid, difficulty);

			if (implement_task(pid, difficulty))
			{
				DPRINT("%c %d %d succeeded\n", event, pid, difficulty);
			}
			else
			{
				fprintf(stderr, "%c %d %d failed\n", event, pid, difficulty);
			}

			break;
		}

		/* Eject Player
			 * E <pid>*/
		case 'E':
		{
			int pid;

			sscanf(buff, "%c %d", &event, &pid);
			DPRINT("%c %d\n", event, pid);

			if (eject_player(pid))
			{
				DPRINT("%c %d succeeded\n", event, pid);
			}
			else
			{
				fprintf(stderr, "%c %d failed\n", event, pid);
			}

			break;
		}

			/* Witness Ejection
                 * W <pid> <pid_a> <number_of_witnesses>
                 */
		case 'W':
		{
			int pid, pid_a, number_of_witnesses;

			sscanf(buff, "%c %d %d %d", &event, &pid, &pid_a, &number_of_witnesses);
			DPRINT("%c %d %d %d\n", event, pid, pid_a, number_of_witnesses);

			if (witness_eject(pid, pid_a, number_of_witnesses))
			{
				DPRINT("%c %d %d %d succeded\n", event, pid, pid_a, number_of_witnesses);
			}
			else
			{
				fprintf(stderr, "%c %d %d %d failed\n", event, pid, pid_a, number_of_witnesses);
			}

			break;
		}

		/* Sabbotage
			 * S <number_of_tasks><pid> */
		case 'S':
		{
			int pid, number_of_tasks;

			sscanf(buff, "%c %d %d\n", &event, &number_of_tasks, &pid);
			DPRINT("%c %d %d\n", event, number_of_tasks, pid);

			if (sabbotage(pid, number_of_tasks))
			{
				DPRINT("%c %d %d succeeded\n", event, number_of_tasks, pid);
			}
			else
			{
				fprintf(stderr, "%c %d %d failed\n", event, number_of_tasks, pid);
			}

			break;
		}

		/* Vote
			 * V <pid> <vote_evidence> */
		case 'V':
		{
			int pid, vote_evidence;

			sscanf(buff, "%c %d %d\n", &event, &pid, &vote_evidence);
			DPRINT("%c %d %d\n", event, pid, vote_evidence);

			if (vote(pid, vote_evidence))
			{
				DPRINT("%c %d %d succeeded\n", event, pid, vote_evidence);
			}
			else
			{
				fprintf(stderr, "%c %d %d failed\n", event, pid, vote_evidence);
			}

			break;
		}

		/* Give Away Work
			 * G */
		case 'G':
		{
			sscanf(buff, "%c", &event);
			DPRINT("%c\n", event);

			if (give_work())
			{
				DPRINT("%c succeeded\n", event);
			}
			else
			{
				fprintf(stderr, "%c failed\n", event);
			}

			break;
		}

		/* Terminate
			 * F */
		case 'F':
		{
			sscanf(buff, "%c", &event);
			DPRINT("%c\n", event);

			if (Terminate())
			{
				DPRINT("%c succeeded\n", event);
			}
			else
			{
				fprintf(stderr, "%c failed\n", event);
			}

			break;
		}
		/* Print Players
			 * X */
		case 'X':
		{
			sscanf(buff, "%c", &event);
			DPRINT("%c\n", event);

			if (print_players())
			{
				DPRINT("%c succeeded\n", event);
			}
			else
			{
				fprintf(stderr, "%c failed\n", event);
			}

			break;
		}
		/* Print Tasks
			 * Y */
		case 'Y':
		{
			sscanf(buff, "%c", &event);
			DPRINT("%c\n", event);

			if (print_tasks())
			{
				DPRINT("%c succeeded\n", event);
			}
			else
			{
				fprintf(stderr, "%c failed\n", event);
			}

			break;
		}
		/* Print Stack
			 * Z */
		case 'Z':
		{
			sscanf(buff, "%c", &event);
			DPRINT("%c\n", event);

			if (print_stack())
			{
				DPRINT("%c succeeded\n", event);
			}
			else
			{
				fprintf(stderr, "%c failed\n", event);
			}

			break;
		}
		/* Print Players & Tasks list
			 * F */
		case 'U':
		{
			sscanf(buff, "%c", &event);
			DPRINT("%c\n", event);

			if (print_double_list())
			{
				DPRINT("%c succeeded\n", event);
			}
			else
			{
				fprintf(stderr, "%c failed\n", event);
			}

			break;
		}
		/* Empty line */
		case '\n':
			break;

		/* Ignore everything else */
		default:
			DPRINT("Ignoring buff: %s \n", buff);

			break;
		}
	}

	free_all();
	return (EXIT_SUCCESS);
}
