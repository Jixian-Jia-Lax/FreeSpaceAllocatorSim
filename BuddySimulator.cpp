//
// Created by Jixian Jia on 2023-10-12.
//

#define SIZE 64  //the size of memory( in terms of blocks maybe)
#define HighestOrder 6

//notice 2^6=64 so our order ranges from 0 to 5. Higher order means larger block.
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
    auto *head = (node_t*)malloc(sizeof(node_t));
    head->order = HighestOrder;
    head->next = nullptr;
    head->prev = nullptr;
    head->available = true;
    return head;
}

void split(node_t *node){
    node_t *temp = node->next;
    auto *new_node =(node_t*) malloc(sizeof (node_t));
    node->order--;
    node->next = new_node;

    new_node->order = node->order;
    new_node->available = true;
    new_node->prev = node;
    new_node->next = temp;
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
 * return true if successfully allocated. return false otherwise.
 */
node_t *buddy_allocate(node_t *head, int size){
    if(head == nullptr)
        return nullptr;
    if (!head->available) {
        return buddy_allocate(head->next, size);
    }
    int order_required = find_order(size);
    if(head->order < order_required) //not enough space
        return buddy_allocate( head->next,size);
    else if (head-> order == order_required){ //perfect fit
        head->available = false;
        return head;
    }
    // split!
    else{
        split(head);
        return buddy_allocate(head,size);
    }

}

void coalesce(node_t *node){
    if (node == nullptr)
        return;

    if(node->prev != nullptr && node->prev->available && node->prev->order == node->order){
        node_t *temp = node->prev;
        node -> order ++;
        node ->prev = node ->prev->prev;
        free(temp);
        coalesce(node);
    }
    else if(node-> next!= nullptr && node->next->available && node->next->order == node->order){
        node_t *temp = node->next;
        node -> order ++;
        node ->next = node ->next->next;
        free(temp);
        coalesce(node);
    }

}

void buddy_free(node_t *node){
    node->available = true;
    coalesce(node);
}

void buddy_show(node_t *head){
    while(head){
        if(!head->available){
            for(int i =0; i< pow(2,head->order); i++) {
                std::cout << "*"; //means inuse
            }
            head = head->next;
        }
        else {
            for(int i =0; i< pow(2,head->order); i++) {
                std::cout << "["<<i<<"]"; //means inuse
            }
            head = head->next;
        }
    }
    std::cout << std::endl;
}

int main(){
    node_t *my_list = buddy_initialize(SIZE);
    buddy_show(my_list);

    node_t *job1 = buddy_allocate(my_list,2);
    buddy_show(my_list);

    buddy_free(job1);
    buddy_show(my_list);

    return 0;
}