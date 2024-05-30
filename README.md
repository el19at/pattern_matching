# pattern matching
slist.c<br />
containig implementayion of linked list declared in slist.h<br />
pattern_matching.c<br />
containig implementayion of pattern matching program declared in pattern_matching.h<br />
additional info:<br />
how to use:<br />
	declare fsm<br />
	call pm_init(fsm)<br />
	add all pattern(repeat for each pattern): using pm_addstring(fsm, pattern, pattern_len) <br />
	call pm_makeFSM(fsm)<br />
	declare list(for matchs)<br />
	list = pm_fsm_search(fsm.zerostate,txt,strlen(txt))<br />
	call dbllist_destroy(list, FREE_DATA)<br />
	call pm_destroy(fsm)<br />

By Elya Athlan
