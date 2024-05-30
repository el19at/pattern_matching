/*
 before commit:
 replace the pttern matching include with list include
 delete all print functions
 */
#include "slist.h"
#include <stdlib.h>
#include <stdio.h>

void dbllist_init(dbllist_t * to_init){
    dbllist_head(to_init)=NULL;
    dbllist_tail(to_init)=NULL;
    dbllist_size(to_init)=0;
}
int dbllist_append(dbllist_t * list,void * data){
    dbllist_node_t* to_append = (dbllist_node_t*)malloc(sizeof(dbllist_node_t));
    if(to_append==NULL)
        return -1;
    dbllist_next(to_append)=NULL;
    if(dbllist_size(list)==0){
        dbllist_tail(list)=to_append;
        dbllist_head(list)=to_append;
    }
    else{
        dbllist_next(dbllist_tail(list))=to_append;
        dbllist_prev(to_append)=dbllist_tail(list);
        dbllist_tail(list)=to_append;
    }
    dbllist_data(to_append)=data;
    dbllist_size(list)++;
    return 0;
}
int dbllist_prepend(dbllist_t *list,void *data){
    dbllist_node_t* to_append = (dbllist_node_t*)malloc(sizeof(dbllist_node_t));
    if(to_append==NULL)
        return -1;
    if(dbllist_tail(list)==NULL){
        dbllist_tail(list)=to_append;
        dbllist_head(list)=to_append;
    }
    else{
        dbllist_prev(dbllist_head(list))=to_append;
        dbllist_next(to_append)=dbllist_head(list);
        dbllist_head(list)=to_append;
    }
    dbllist_size(list)++;
    dbllist_data(to_append)=data;
    return 0;
}

int dbllist_remove(dbllist_t *list, dbllist_node_t* to_remove,dbllist_destroy_t flag){
    if(dbllist_size(list)==0){
        return -1;
    }
    dbllist_node_t* current= dbllist_head(list);
    if(current==to_remove){//head case
        dbllist_head(list)=dbllist_next(dbllist_head(list));
        dbllist_next(to_remove)=NULL;
        dbllist_prev(to_remove)=NULL;
        if(flag==DBLLIST_FREE_DATA && to_remove){
            free(dbllist_data(to_remove));
            dbllist_data(to_remove)=NULL;
            }
        free(to_remove);
        to_remove=NULL;
        list->size--;
        return 0;
    }
    while(current->next!=NULL){//in list 
        if(current==to_remove){
            dbllist_next(dbllist_prev(to_remove))=dbllist_next(to_remove);
            dbllist_prev(dbllist_next(to_remove))=dbllist_prev(to_remove);
            dbllist_next(to_remove)=NULL;
            dbllist_prev(to_remove)=NULL;
            if(flag==DBLLIST_FREE_DATA && to_remove){
                free(dbllist_data(to_remove));
                dbllist_data(to_remove)=NULL;
            }
            free(to_remove);
            to_remove=NULL;
            dbllist_size(list)--;
            return 0;
        }
        current=dbllist_next(current);
    }
    if(current==to_remove){//tail case
        dbllist_tail(list)=dbllist_prev(dbllist_tail(list));
        dbllist_next(dbllist_tail(list))=NULL;
        dbllist_prev(to_remove)=NULL;
        dbllist_next(to_remove)=NULL;
        if(flag==DBLLIST_FREE_DATA && to_remove){
            free(dbllist_data(to_remove));
            dbllist_data(to_remove)=NULL;
        }
        free(to_remove);
        to_remove=NULL;
        dbllist_size(list)--;
        return 0;
    }   
    return -1;//the node not found in the list
}
void dbllist_destroy(dbllist_t* list,dbllist_destroy_t flag){
    while(dbllist_size(list)>0) dbllist_remove(list,dbllist_head(list),flag);
    free(list);
}
//private func
void dbllist_concate(dbllist_t* concated, dbllist_t* to_add){
    dbllist_tail(concated)=dbllist_head(to_add);
    dbllist_size(concated)+=dbllist_size(to_add);
    dbllist_tail(concated)=dbllist_tail(to_add);
}
