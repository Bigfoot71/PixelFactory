#ifndef PF_DEPTH_H
#define PF_DEPTH_H

/* Functions Types */

typedef int (*pf_depth_test_fn)(float, float);

/* Default Depth Test Functions */

int
pf_depth_equal(float dst, float src);

int
pf_depth_not_equal(float dst, float src);

int
pf_depth_less(float dst, float src);

int
pf_depth_less_equal(float dst, float src);

int
pf_depth_greater(float dst, float src);

int
pf_depth_greater_equal(float dst, float src);

#endif //PF_DEPTH_H
