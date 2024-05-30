#include "pattern_matching.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


int pm_init(pm_t * pfsm){
    pfsm->zerostate = (pm_state_t*)malloc(sizeof(pm_state_t));
    if(!pfsm->zerostate)
        return -1;

    pfsm->zerostate->id=0;
    pfsm->zerostate->depth=0;
    pfsm->zerostate->fail=NULL;

    pfsm->zerostate->_transitions= (dbllist_t*)malloc(sizeof (dbllist_t));
    if(!pfsm->zerostate->_transitions) {
        free(pfsm->zerostate);
        pfsm->zerostate=NULL;
        return -1;
    }
    dbllist_init(pfsm->zerostate->_transitions);

    pfsm->zerostate->output= (dbllist_t*)malloc(sizeof (dbllist_t));
    if(!pfsm->zerostate->output) {
        dbllist_destroy(pfsm->zerostate->_transitions,DBLLIST_FREE_DATA);
        pfsm->zerostate->_transitions=NULL;
        free(pfsm->zerostate);
        pfsm->zerostate=NULL;
        return -1;
    }
    dbllist_init(pfsm->zerostate->output);

    pfsm->newstate=1;
    return 0;
}

int pm_addstring(pm_t * pfsm,unsigned char * to_add, _size_t n){
    pm_state_t* current = pfsm->zerostate;
    for(int i=0;i<n;i++){
        if(pm_goto_get(current,to_add[i])){
            current=pm_goto_get(current,to_add[i]);
        }
        else{
            pm_state_t* newState = (pm_state_t*)malloc(sizeof(pm_state_t));
            if(!newState)
                return -1;
            newState->id=pfsm->newstate++;
            newState->depth=current->depth+1;
            newState->fail=NULL;
            newState->_transitions = (dbllist_t*)malloc(sizeof(dbllist_t));
            if(! newState->_transitions) {
                free(newState);
                newState=NULL;
                return -1;
            }
            dbllist_init(newState->_transitions);
            newState->output = (dbllist_t*)malloc(sizeof(dbllist_t));
            if(! newState->output) {
                dbllist_destroy(newState->output,DBLLIST_FREE_DATA);
                newState->output=NULL;
                free(newState);
                newState=NULL;
                return -1;
            }
            dbllist_init(newState->output);
            printf("Allocating state %d\n",newState->id);
            if(0>pm_goto_set(current, to_add[i], newState)){
                pm_destroy(pfsm);
                return -1;
            }
            current = newState;
        }
    }
    char* to_add_string = (char*)malloc(sizeof(char)*(n+1));
    if(!to_add_string){
        pm_destroy(pfsm);
        return -1;
    }
    for(int i=0;i<n;i++) to_add_string[i]=to_add[i];
    to_add_string[n]='\0';

    if(0>dbllist_append(current->output,to_add_string)){
        pm_destroy(pfsm);
        return -1;
    }
    return 0;
}

int pm_makeFSM(pm_t * pfsm){
    dbllist_t* Q = (dbllist_t*) malloc(sizeof (dbllist_t));                 //Q store state_t*
    if(!Q){
        pm_destroy(pfsm);
        return -1;
    }
    dbllist_init(Q);                                                        //Q<-empty
    dbllist_node_t* current_node = dbllist_head(pfsm->zerostate->_transitions);
    if(!current_node){
        dbllist_destroy(Q,DBLLIST_LEAVE_DATA);
        return 0;
    }
    pm_labeled_edge_t* current_labeled_edge = dbllist_data(current_node);
    pm_state_t* current_state= current_labeled_edge->state;                 //init iterator vars

    while (current_node){                                                   //iter over all depth=1 states
        if(0>dbllist_append(Q, current_state)){
        pm_destroy(pfsm);
        return -1;
        }
        current_state->fail = pfsm->zerostate;
        current_node=dbllist_next(current_node);
        if(!current_node)
            break;
        current_labeled_edge = dbllist_data(current_node);
        current_state = current_labeled_edge->state;
    }

    while(dbllist_size(Q)>0) {
        pm_state_t* r = dbllist_data(dbllist_head(Q));
        dbllist_remove(Q,dbllist_head(Q),DBLLIST_LEAVE_DATA);
        current_node = dbllist_head(r->_transitions);
        char b;
        if(current_node) {
            current_labeled_edge = dbllist_data(current_node);
            current_state = current_labeled_edge->state;
            b = current_labeled_edge->label;
        }
 
        while (current_node) {
            if(0>dbllist_append(Q,current_state)){
                pm_destroy(pfsm);
                return -1;
            }
            pm_state_t* state = r->fail;
            if(state!=pfsm->zerostate)
                while (!pm_goto_get(state,b) && pm_goto_get(state,b)!=pfsm->zerostate) state = state->fail;
            if(pm_goto_get(state,b))
                current_state->fail = pm_goto_get(state,b);
            else
                current_state->fail = pfsm->zerostate;
            printf("Setting f(%u) = %u\n",current_state->id,current_state->fail->id);
            //union of s.output with f(s).output
            dbllist_node_t* to_add_node = dbllist_head(current_state->fail->output);
            while (to_add_node){
                char* to_add_str= (char*)malloc(sizeof(char)* (strlen(dbllist_data(to_add_node))+1));
                if(!to_add_str){
                    pm_destroy(pfsm);
                    return -1;
                }
                strcpy(to_add_str,dbllist_data(to_add_node));
                if(0>dbllist_append(current_state->output,to_add_str)){
                    pm_destroy(pfsm);
                    return -1;
                }
                to_add_node = dbllist_next(to_add_node);
            }
            current_node = dbllist_next(current_node);
            if (!current_node)
                break;
            current_labeled_edge = dbllist_data(current_node);
            current_state = current_labeled_edge->state;
            b = current_labeled_edge->label;
        }
    }
    dbllist_destroy(Q,DBLLIST_LEAVE_DATA);
    return 0;
}

int pm_goto_set(pm_state_t *from_state,unsigned char symbol,pm_state_t *to_state){
    pm_labeled_edge_t* to_add = (pm_labeled_edge_t*) malloc(sizeof (pm_labeled_edge_t));
    if(!to_add)
        return -1;
    to_add->label = symbol;
    to_add->state = to_state;
    dbllist_append(from_state->_transitions, to_add);
    printf("%d -> %c -> %d\n",from_state->id,symbol,to_state->id);
    return 0;
}

pm_state_t* pm_goto_get(pm_state_t *state,unsigned char symbol){
    if(state->_transitions->size==0)
        return NULL;
    dbllist_node_t* current = state->dbllist_head(_transitions);
    pm_labeled_edge_t* current_edge=dbllist_data(current);
    unsigned char to_compare = current_edge->label;
    while(current){
        if(to_compare==symbol) {
            return current_edge->state;
        }
        current= dbllist_next(current);
        if(!current)
            return NULL;
        current_edge=dbllist_data(current);
        to_compare=current_edge->label;
    }
}

dbllist_t* pm_fsm_search(pm_state_t * pstate, unsigned char * to_search, _size_t n){
    dbllist_t* matchs = (dbllist_t*)malloc(sizeof (dbllist_t));
    if(!matchs)
        return NULL;
    dbllist_init(matchs);
    pm_state_t* state = pstate;
    for(int j=0;j<n;j++){
        while(!pm_goto_get(state,to_search[j]) && state->fail) state=state->fail;
        state = pm_goto_get(state,to_search[j]);
        dbllist_node_t* current_node;
        if(!state) {
            state=pstate;
            continue;
        }
        current_node = dbllist_head(state->output);
        while(current_node){
            pm_match_t* newMatch= (pm_match_t*)malloc(sizeof (pm_match_t));
            if(!newMatch){
                dbllist_destroy(matchs,DBLLIST_FREE_DATA);
                return NULL;
            }
            newMatch->pattern=dbllist_data(current_node);  //the matched pattern
            newMatch->start_pos=j-strlen(newMatch->pattern)+1;  //where does it start in the text
            newMatch->end_pos=j+1; //where does it end in the text
            newMatch->fstate=state; //what is the final state
            if(0>dbllist_append(matchs,newMatch)){
                dbllist_destroy(matchs,DBLLIST_FREE_DATA);
                free(newMatch);
                return NULL;
            }
            current_node = dbllist_next(current_node);
        }
    }
    return matchs;
}
//private func

void destroy_states(pm_state_t* to_free){//recursive func to destroy and free state and all transitions
    while(dbllist_size(to_free->_transitions)>0){
        pm_labeled_edge_t* current_edge = dbllist_data(dbllist_head(to_free->_transitions));
        destroy_states(current_edge->state);
        dbllist_remove(to_free->_transitions,dbllist_head(to_free->_transitions),DBLLIST_FREE_DATA);
    }
    dbllist_destroy(to_free->output,DBLLIST_FREE_DATA);//
    to_free->output=NULL;
    dbllist_destroy(to_free->_transitions,DBLLIST_FREE_DATA);
    to_free->_transitions=NULL;
    if(to_free)
        free(to_free);
    to_free=NULL;
}

void pm_destroy(pm_t * to_destroy){
    destroy_states(to_destroy->zerostate);
}
