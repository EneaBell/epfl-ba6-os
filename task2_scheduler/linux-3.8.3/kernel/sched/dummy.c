/*
 * Dummy scheduling class, mapped to last five levels of SCHED_NORMAL policy
 */

#include "sched.h"

/*
 * Timeslice and age threshold are represented in jiffies. Default timeslice
 * is 100ms. Both parameters can be tuned from /proc/sys/kernel.
 */

#define DUMMY_TIMESLICE		(100 * HZ / 1000)
#define DUMMY_AGE_THRESHOLD	(3 * DUMMY_TIMESLICE)

unsigned int sysctl_sched_dummy_timeslice = DUMMY_TIMESLICE;
static inline unsigned int get_timeslice()
{
	return sysctl_sched_dummy_timeslice;
}

unsigned int sysctl_sched_dummy_age_threshold = DUMMY_AGE_THRESHOLD;
static inline unsigned int get_age_threshold()
{
	return sysctl_sched_dummy_age_threshold;
}

/*
 * Init
 */

void init_dummy_rq(struct dummy_rq *dummy_rq, struct rq *rq)
{
	INIT_LIST_HEAD(&dummy_rq->queue);
}

/*
 * Helper functions
 */

static inline struct task_struct *dummy_task_of(struct sched_dummy_entity *dummy_se)
{
	return container_of(dummy_se, struct task_struct, dummy_se);
}

static inline void _enqueue_task_dummy(struct rq *rq, struct task_struct *p)
{
	struct sched_dummy_entity *dummy_se = &p->dummy_se;
        struct list_head *queue = &rq->dummy.queue;
        list_add_tail(&dummy_se->run_list, queue);
}

static inline void _dequeue_task_dummy(struct task_struct *p)
{
	struct sched_dummy_entity *dummy_se = &p->dummy_se;
        list_del_init(&dummy_se->run_list);
}

/*
 * Scheduling class functions to implement
 */

static void enqueue_task_dummy(struct rq *rq, struct task_struct *p, int flags)
{
	_enqueue_task_dummy(rq, p);
	inc_nr_running(rq);
}

static void dequeue_task_dummy(struct rq *rq, struct task_struct *p, int flags)
{
	_dequeue_task_dummy(p);
	dec_nr_running(rq);
}

static void yield_task_dummy(struct rq *rq)
{
}

static void check_preempt_curr_dummy(struct rq *rq, struct task_struct *p, int flags) {
  // If the current task is less important than p, we ask to put p on the processor.
	if (p->prio > rq->curr->prio) resched_task(p);
}

static struct task_struct *pick_next_task_dummy(struct rq *rq)
{
	struct dummy_rq *dummy_rq = &rq->dummy;
	struct sched_dummy_entity *next;

	if (!list_empty(&dummy_rq->queue)) {
		next = list_first_entry(&dummy_rq->queue, struct sched_dummy_entity, run_list);
		return dummy_task_of(next);
	} else {
		return NULL;
	}
}

static void put_prev_task_dummy(struct rq *rq, struct task_struct *prev)
{
}

static void set_curr_task_dummy(struct rq *rq)
{
}

static void task_tick_dummy(struct rq *rq, struct task_struct *curr, int queued) {
  
}

/*
 * Called when the scheduling class changes (due to the change in priority).
 */
static void switched_from_dummy(struct rq *rq, struct task_struct *p) {
  
}

/*
 * Called when the scheduling class changes (due to the change in priority).
 */
static void switched_to_dummy(struct rq *rq, struct task_struct *p) {
  
  if (!p->on_rq) return;
  
  /*
   * kick off the schedule if running, otherwise just see
   * if we can still preempt the current task.
   */
	if (rq->curr == p)
		resched_task(rq->curr);
	else
		check_preempt_curr_dummy(rq, p, 0);
}

/*
 * Called when the priority changes, but NOT the scheduling class (rt_mutex_setprio, sched_setscheduler).
 */
static void prio_changed_dummy(struct rq *rq, struct task_struct *p, int oldprio) {
  
	if (!p->on_rq) return;                    // If the given task is not in our list, quit.
  
  if (rq->curr == p) {                      // If the given task in parameter is running
		if (p->prio > oldprio) resched_task(p); // If we have a higher priority with the given task, we kick out the one currently using the processor and we put the new one in place.
  } else                                    // If not running, check if its value is greater than the current running task.
		check_preempt_curr_dummy(rq, p, 0); 
}

static unsigned int get_rr_interval_dummy(struct rq *rq, struct task_struct *p) {
	return get_timeslice();
}

/*
 * Scheduling class
 */

const struct sched_class dummy_sched_class = {
	.next		    = &idle_sched_class,
	.enqueue_task       = enqueue_task_dummy,
	.dequeue_task       = dequeue_task_dummy,
	.yield_task         = yield_task_dummy,

	.check_preempt_curr = check_preempt_curr_dummy,

	.pick_next_task     = pick_next_task_dummy,
	.put_prev_task      = put_prev_task_dummy,

	.set_curr_task      = set_curr_task_dummy,
	.task_tick          = task_tick_dummy,

	.switched_from      = switched_from_dummy,
	.switched_to        = switched_to_dummy,
	.prio_changed       = prio_changed_dummy,

	.get_rr_interval    = get_rr_interval_dummy,
};

