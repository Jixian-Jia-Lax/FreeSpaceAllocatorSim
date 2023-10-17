//
// Created by Jixian Jia on 2023-10-12.
//

#define SIZE 1024  //the size of memory( in terms of blocks maybe)
#define HighestOrder 10

//notice 2^6=64 so our order ranges from 1 to 6. Higher order means larger block.
#include <iostream>
#include <cstdlib>
#include <cmath>


typedef struct node{
    int order;
    node *next;
    node *prev;
    bool available;
} node_t;


node_t* buddy_initialize(int size){
    auto *dummy_head = (node_t*)malloc(sizeof(node_t));
    auto *real_head = (node_t*)malloc(sizeof(node_t));

    dummy_head->order = -1;
    dummy_head->next = real_head;
    dummy_head->prev = nullptr;
    dummy_head->available = false;

    real_head -> order = HighestOrder;
    real_head->next = nullptr;
    real_head -> prev = dummy_head;
    real_head ->available = true;
    return dummy_head;
}

void split(node_t *node, int *counter){
    if (node->order == 0){
        return;
    }
    node_t *temp = node->next;
    auto *new_node =(node_t*) malloc(sizeof (node_t));
    node->order--;
    node->next = new_node;

    new_node->order = node->order;
    new_node->available = true;
    new_node->prev = node;
    new_node->next = temp;
    *counter++;

}

/*
 * find the correct order for a given size such that
 * 2^(order-1) < given_size <= 2^(order)
 */
int find_order(int size){
    int order = 0;
    while(pow(2,order) < size){
        order++;
    }
    return order;
}

/*
 * counter = number of comparisons + number of splits
 * return true if successfully allocated. return false otherwise.
 */
node_t *buddy_allocate(node_t *head, int size, int *counter){
    if(head == nullptr)
        return nullptr;
    if (!head->available) {
        *counter +=1;
        return buddy_allocate(head->next, size,counter);
    }
    int order_required = find_order(size);
    if(head->order < order_required) { //not enough space
        *counter +=1;
        return buddy_allocate(head->next, size, counter);
    }
    else if (head-> order == order_required){ //perfect fit
        head->available = false;
        *counter += 1;
        return head;
    }
    // split!
    else{
        split(head,counter);
        return buddy_allocate(head,size,counter);
    }

}

void coalesce(node_t *node,int *counter){
    if (node == nullptr) {
        *counter += 1;
        return;
    }

    if(node->prev != nullptr && node->prev->available && node->prev->order == node->order){
        node_t *temp = node->prev;
        node -> order += 1;
        node ->prev = node->prev->prev;
        if (node -> prev != nullptr)
            node -> prev -> next = node;
        free(temp);
        *counter += 1;

        coalesce(node,counter);
    }
    else if(node-> next!= nullptr && node->next->available && node->next->order == node->order){
        node_t *temp = node->next;
        node -> order +=1;

        node ->next = node ->next->next;
        if (node -> next != nullptr)
            node-> next -> prev = node;
        free(temp);
        *counter += 1;

        coalesce(node,counter);
    }

}

void buddy_free(node_t *node,int *counter){
    node->available = true;
    coalesce(node,counter);
}

void buddy_show(node_t *head){
    if (head->order == -1){
        std::cout << "HEAD -->";
        head = head -> next;
    }
    while(head){
        if(!head->available){
            std::cout << "[" << pow(2,head->order) <<" bytes in use, order =" << head->order << "] --> "; //means inuse

            head = head->next;
        }
        else {
            std::cout << "[" << pow(2,head->order) <<" bytes  free, order ="  << head->order << "] --> "; //means inuse
            head = head->next;
        }
    }
    std::cout <<"END" << std::endl;
}

int main(){
    node_t *my_list = buddy_initialize(SIZE);
    buddy_show(my_list);
    int counter = 0;


    node_t *job1 = buddy_allocate(my_list,200,&counter);
    node_t *job2 = buddy_allocate(my_list,40,&counter);
    node_t *job3 = buddy_allocate(my_list,10,&counter);

    buddy_free(job3,&counter);
    buddy_free(job2,&counter);
    buddy_free(job1,&counter);
    node_t *job4 = buddy_allocate(my_list,30,&counter);
    node_t *job5 = buddy_allocate(my_list,50,&counter);
    buddy_free(job5,&counter);
    node_t *job6 = buddy_allocate(my_list,10,&counter);
    node_t *job7 = buddy_allocate(my_list,75,&counter);

    buddy_show(my_list);

    std::cout << counter;

    return 0;
}