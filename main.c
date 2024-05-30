#include "slist.c"
#include "pattern_matching.c"

void print_matchs(dbllist_t* list){
    printf("____________match list______________\n");
    if(dbllist_size(list)==0){
        printf("matchs empty list\n");
        return;
    }
    dbllist_node_t* current_node = dbllist_head(list);
    pm_match_t* match = dbllist_data(current_node);
    while(current_node){
        printf("pattern: \"%s\" at (%d,%d) endNode is %u\n",match->pattern,match->start_pos,match->end_pos,match->fstate->id);
        current_node=dbllist_next(current_node);
        if(!current_node)
            break;
        match= dbllist_data(current_node);
    }
    printf("____________end list______________\n");
}
int main(){

    pm_t fsm;
    pm_init(&fsm);
    unsigned char txt[20]="abcddacabca";
    char test[4][4] = {"a","ab","cab","bca"};
    for(int i =0;i<4;i++)
        pm_addstring(&fsm,test[i], strlen(test[i]));
    pm_makeFSM(&fsm);
    dbllist_t* matchs= pm_fsm_search(fsm.zerostate,txt,strlen(txt));

    //print_matchs(matchs);
    printf("txt \"%s\"\n",txt);
    print_matchs(matchs);

    if(matchs)
        dbllist_destroy(matchs, DBLLIST_FREE_DATA);
    pm_destroy(&fsm);

    /*

    //pm_addstring(&fsm,"a",1);
    //printf("main: id of state 1 is %u\n",((pm_labeled_edge_t*)(fsm.zerostate->_transitions->head->data))->state->id);


    char to_add[4][4] = {"abc","bca","cab","acb"};
    for(int i=0;i<4;i++)
        pm_addstring(&fsm,to_add[i],strlen(to_add[i]));

    pm_labeled_edge_t* labeled_edge = dbllist_data(dbllist_head(fsm.zerostate->_transitions));
    pm_state_t* state = labeled_edge->state;
    print_string(state->output);

    char a[10]="hello";
    int n=strlen(a);
    char* string = (char*) malloc(sizeof (char)*(n+1));
    for(int i=0;i<n;i++) string[i]=a[i];
    string[n]='\0';
    free(string);
    dbllist_t* testList=(dbllist_t*) malloc(sizeof (dbllist_t));
    dbllist_init(testList);
    dbllist_append(testList,string);
    dbllist_remove(testList,dbllist_head(testList),DBLLIST_FREE_DATA);
    dbllist_destroy(testList,DBLLIST_FREE_DATA);
     */
    return 0;

}