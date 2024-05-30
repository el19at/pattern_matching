# pattern matching
slist.c
containig implementayion of linked list declared in slist.h
pattern_matching.c
containig implementayion of pattern matching program declared in pattern_matching.h
additional info:
how to use:
	declare fsm
	call pm_init(fsm)
	add all pattern(repeat for each pattern): using pm_addstring(fsm, pattern, pattern_len) 
	call pm_makeFSM(fsm)
	declare list(for matchs)
	list = pm_fsm_search(fsm.zerostate,txt,strlen(txt))
	call dbllist_destroy(list, FREE_DATA)
	call pm_destroy(fsm)

By Elya Athlan
