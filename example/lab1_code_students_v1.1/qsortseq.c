/* Sequential version of Quick sort */

#include <stdio.h>
#include <stdlib.h>

#define KILO (1024)
#define MEGA (1024*1024)
#define MAX_ITEMS (64*MEGA)
#define swap(v, a, b) {unsigned tmp; tmp=v[a]; v[a]=v[b]; v[b]=tmp;}

static int *v;

static void
print_array(void)
{
    int i;
    for (i = 0; i < MAX_ITEMS; i++)
        printf("%d ", v[i]);
    printf("\n");
}

static void
init_array(void)
{
    int i;
    v = (int *) malloc(MAX_ITEMS*sizeof(int));
    for (i = 0; i < MAX_ITEMS; i++)
        v[i] = rand();
}

static unsigned
partition(int *v, unsigned low, unsigned high, unsigned pivot_index)
{
    /* move pivot to the bottom of the vector */
    if (pivot_index != low)
        swap(v, low, pivot_index);

    pivot_index = low;
    low++;

    /* invariant:
     * v[i] for i less than low are less than or equal to pivot
     * v[i] for i greater than high are greater than pivot
     */

    /* move elements into place */
    while (low <= high) {
        if (v[low] <= v[pivot_index])
            low++;
        else if (v[high] > v[pivot_index])
            high--;
        else
            swap(v, low, high);
    }

    /* put pivot back between two groups */
    if (high != pivot_index)
        swap(v, pivot_index, high);
    return high;
}

static void
quick_sort(int *v, unsigned low, unsigned high)
{
    unsigned pivot_index;

    /* no need to sort a vector of zero or one element */
    if (low >= high)
        return;

    /* select the pivot value */
    pivot_index = (low+high)/2;

    /* partition the vector */
    pivot_index = partition(v, low, high, pivot_index);

    /* sort the two sub arrays */
    if (low < pivot_index)
        quick_sort(v, low, pivot_index-1);
    if (pivot_index < high)
        quick_sort(v, pivot_index+1, high);
}

int
main(int argc, char **argv)
{
    init_array();
    //print_array();
    quick_sort(v, 0, MAX_ITEMS-1);
    //print_array();
}
