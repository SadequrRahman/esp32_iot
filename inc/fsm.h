/*
 * fsm.h
 *
 *  Created on: 16 Nov 2019
 *      Author: Rabby
 */

#ifndef INC_FSM_H_
#define INC_FSM_H_

#include <stdint.h>


typedef struct FSM fsm_t;

/** return values */
typedef enum{
	FSM_RET_HANDLED, 		//< event was handled
	FSM_RET_IGNORED, 		//< event was ignored; not used in this implementation
	FSM_RET_TRANSITION, 		//< event was handled and a state transition occurred
	FSM_RET_MAX
}fsm_return_t;

/* signals used by the FSM */
typedef enum {
	FSM_TRANSITION_ENTRY,
	FSM_TRANSITION_EXIT
}fsm_transition_t;
/* base type for events*/
typedef struct {
	int signal;
	void *sigData;
}fsm_event_t;


typedef uint8_t fsm_retStatus;
/* Fsm state as function pointer */
typedef fsm_retStatus (*fsm_state_t)(fsm_t *, const fsm_event_t *);

/** base type for state machine */
struct  FSM{
	fsm_state_t state; /* current state */
};


/** dispatches events to state machine, called in application*/
inline static void fsm_dispatch(fsm_t* fsm, const fsm_event_t* event) {
	static fsm_event_t entryEvent = { .signal = FSM_TRANSITION_ENTRY };
	static fsm_event_t exitEvent = { .signal = FSM_TRANSITION_EXIT };
	fsm_state_t s = fsm->state;
	fsm_retStatus r = fsm->state(fsm, event);
	if (r == FSM_RET_TRANSITION) {
		s(fsm, &exitEvent); 				//< call exit action of last state
		fsm->state(fsm, &entryEvent); 		//< call entry action of new state
	}
}

/* sets and calls initial state of state machine */
inline static void fsm_init(fsm_t* fsm, fsm_state_t init) {
	fsm->state = init;
	//fsm_dispatch(fsm, (void*)0);
}

#endif /* INC_FSM_H_ */
