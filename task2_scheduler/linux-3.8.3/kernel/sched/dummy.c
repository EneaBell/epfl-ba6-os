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
 * Macros for debugging
 */

//#define DUMMY_DEBUG
//#define DUMMY_DEBUG_QUEUE

/*
 * Init
 */

void init_dummy_rq(struct dummy_rq *dummy_rq, struct rq *rq)
{
	int i;
	for (i=0; i<DUMMY_QUEUE_SIZE; ++i) {
		INIT_LIST_HEAD(&dummy_rq->queues[i]);
	}
	dummy_rq->dummy_timeslice = 0;
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
	struct dummy_rq *dummy_rq = &rq->dummy;
	struct sched_dummy_entity *dummy_se = &p->dummy_se;
	struct list_head *queue = &dummy_rq->queues[p->prio - DUMMY_PRIORITY_MIN];

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
#ifdef DUMMY_DEBUG_QUEUE
	printk(KERN_ALERT "sched_dummy: enqueue: %d\n", p->pid);
#endif

	p->dummy_age_tick = 0;
	_enqueue_task_dummy(rq, p);
	inc_nr_running(rq);
}

static void dequeue_task_dummy(struct rq *rq, struct task_struct *p, int flags)
{
#ifdef DUMMY_DEBUG_QUEUE
	printk(KERN_ALERT "sched_dummy: dequeue: %d\n", p->pid);
#endif

	_dequeue_task_dummy(p);
	dec_nr_running(rq);
}

static void requeue_task_dummy(struct rq *rq, struct task_struct *p, int flags)
{
#ifdef DUMMY_DEBUG_QUEUE
	printk(KERN_ALERT "sched_dummy: requeue: %u with prio %u\n", p->pid, p->prio);
#endif

	dequeue_task_dummy(rq, p, flags);
	enqueue_task_dummy(rq, p, flags);
	resched_task(p);
}

static void yield_task_dummy(struct rq *rq)
{
#ifdef DUMMY_DEBUG
	printk(KERN_ALERT "sched_dummy: yield_task_dummy()\n");
#endif

	requeue_task_dummy(rq, rq->curr, 0);
}

static void check_preempt_curr_dummy(struct rq *rq, struct task_struct *p, int flags) {
#ifdef DUMMY_DEBUG
	printk(KERN_ALERT "sched_dummy: check_preempt_curr_dummy()\n");
#endif
	// If the current task is less important than p, we ask to put p on the processor.
	if (p->prio > rq->curr->prio) resched_task(p);
}

static struct task_struct *pick_next_task_dummy(struct rq *rq)
{
	struct dummy_rq *dummy_rq = &rq->dummy;
	struct sched_dummy_entity *next;
	struct task_struct *p;
	struct list_head *queue;
	int i;

	for (i=0; i<DUMMY_QUEUE_SIZE; ++i) {
		queue = &dummy_rq->queues[i];
		if (!list_empty(queue)) {
			next = list_first_entry(queue, struct sched_dummy_entity, run_list);
			p = dummy_task_of(next);
			p->dummy_age_tick = 0;

#ifdef DUMMY_DEBUG
			printk(KERN_ALERT "sched_dummy: pick_next_task_dummy() -> queue %u -> %u with prio %u\n", i, p->pid, p->prio);
#endif
			return p;
		}
	}
#ifdef DUMMY_DEBUG
	printk(KERN_ALERT "sched_dummy: pick_next_task_dummy() -> NULL \n");
#endif
	return NULL;
}

static void put_prev_task_dummy(struct rq *rq, struct task_struct *prev)
{
	// Nothing to do :o)
}

static void set_curr_task_dummy(struct rq *rq)
{
	// Nothing to do :o)
}

static void task_tick_dummy(struct rq *rq, struct task_struct *curr, int queued)
{
	struct dummy_rq *dummy_rq = &rq->dummy;
	struct sched_dummy_entity *next;
	struct list_head *pos, *q; // used inside the foreach_safe
	struct task_struct *p;
	int i;

	for (i=1; i<DUMMY_QUEUE_SIZE; ++i) { // All priority queue except the lowest
		list_for_each_safe(pos, q, &dummy_rq->queues[i]) {
			next = list_entry(pos, struct sched_dummy_entity, run_list);
			p = dummy_task_of(next);
			if (p->dummy_age_tick++ > get_age_threshold()) {
				p->dummy_age_tick = 0;
#ifdef DUMMY_DEBUG
				printk(KERN_ALERT "sched_dummy: task_tick_dummy -> age %u to queue %u\n", p->pid, (i-1));
#endif

				list_move_tail(pos, &dummy_rq->queues[i-1]);
				resched_task(p);
			}
		}
	}

	if (dummy_rq->dummy_timeslice++ >= get_timeslice()) {
#ifdef DUMMY_DEBUG
		printk(KERN_ALERT "sched_dummy: task_tick_dummy -> timeslice %u at %u\n", curr->pid, dummy_rq->dummy_timeslice);
#endif

		requeue_task_dummy(rq, curr, 0);
		dummy_rq->dummy_timeslice = 0;
	}
}

/*
 * Called when the scheduling class changes (due to the change in priority).
 */
static void switched_from_dummy(struct rq *rq, struct task_struct *p) {
#ifdef DUMMY_DEBUG
	printk(KERN_ALERT "sched_dummy: switched_from_dummy(%u)\n", p->pid);
#endif
	// Nothing to do in our case.
}

/*
 * Called when the scheduling class changes (due to the change in priority).
 */
static void switched_to_dummy(struct rq *rq, struct task_struct *p) {
#ifdef DUMMY_DEBUG
	printk(KERN_ALERT "sched_dummy: switched_to_dummy(%u)\n", p->pid);
#endif
  
	if (!p->on_rq) return;
  
	if (rq->curr == p)                // If the task that is from another scheduling class is running, kick it.
		resched_task(rq->curr);
	else                              // Otherwise, check if the task has a better priority than the currently running task.
		check_preempt_curr_dummy(rq, p, 0);
}

/*
 * Called when the priority changes, but NOT the scheduling class (rt_mutex_setprio, sched_setscheduler).
 */
static void prio_changed_dummy(struct rq *rq, struct task_struct *p, int oldprio) {
#ifdef DUMMY_DEBUG
	printk(KERN_ALERT "sched_dummy: prio_changed_dummy(%u, %u)\n", p->pid, oldprio);
#endif
  
	if (!p->on_rq) return;                    // If the given task is not in our list, quit.
  
	if (rq->curr == p) {                      // If the given task in parameter is running
		if (p->prio > oldprio) resched_task(p); // If we have a higher priority with the given task, we kick out the one currently using the processor and we put the new one in place.
	} else                                    // If not running, check if its value is greater than the current running task.
		check_preempt_curr_dummy(rq, p, 0); 
}

static unsigned int get_rr_interval_dummy(struct rq *rq, struct task_struct *p) {
#ifdef DUMMY_DEBUG
	printk(KERN_ALERT "sched_dummy: get_rr_interval_dummy(%u)\n", p->pid);
#endif
	return get_timeslice();
}

/*
 * Scheduling class
 */

const struct sched_class dummy_sched_class = {
	.next		    	= &idle_sched_class,
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

