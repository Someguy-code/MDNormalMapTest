#ifndef _MDPROFILER_H_

//Utility macros for usage with md-profiler
#define LABEL(name) asm volatile("mdp_label_" name "_%=: .global mdp_label_" name "_%=":);
#define MDP_FUNCTION_START(name) LABEL(name "_start");
#define MDP_FUNCTION_END(name)   LABEL(name "_end");

#endif //#ifndef _MDPROFILER_H_