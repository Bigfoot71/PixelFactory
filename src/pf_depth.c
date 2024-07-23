#include "pixelfactory/pf_depth.h"

int
pf_depth_equal(float dst, float src)
{
    return (src == dst);
}

int
pf_depth_not_equal(float dst, float src)
{
    return (src != dst);
}

int
pf_depth_less(float dst, float src)
{
    return (src < dst);
}

int
pf_depth_less_equal(float dst, float src)
{
    return (src <= dst);
}

int
pf_depth_greater(float dst, float src)
{
    return (src > dst);
}

int
pf_depth_greater_equal(float dst, float src)
{
    return (src >= dst);
}
