#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#define N_LIST 10
int original_list[] = {7, 12, 19, 3, 18, 4, 2, 6, 15, 8};
int sorted_list[N_LIST];

struct timeval start, end;
#define usec_elapsed(s, e) (1000000 * ((e).tv_sec - (s).tv_sec) + ((e).tv_usec - (s).tv_usec))

// Struct for passing sorting arguments
typedef struct
{
    int *data;
    int size;
    char id[32];
} SortArg;

void print_list(const char *id, const char *msg, int *list, int n)
{
    printf("%s %s:", id, msg);
    for (int i = 0; i < n; i++)
    {
        printf(" %d", list[i]);
    }
    printf("\n");
}

int *listncopy(int *dst, int *src, int n)
{
    for (int i = 0; i < n; i++)
        dst[i] = src[i];
    return dst;
}

void bubble_sort(int *arr, int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

void *do_sort(void *arg)
{
    SortArg *sarg = (SortArg *)arg;
    print_list(sarg->id, "Sub-Old", sarg->data, sarg->size);
    gettimeofday(&start, NULL);
    bubble_sort(sarg->data, sarg->size);
    gettimeofday(&end, NULL);
    print_list(sarg->id, "Sub-New", sarg->data, sarg->size);
    printf("%s spent %ld usec\n", sarg->id, usec_elapsed(start, end));
    pthread_exit(NULL);
}

void *do_merge(void *arg)
{
    gettimeofday(&start, NULL);
    int *first = (int *)arg;
    int mid = N_LIST / 2;
    int i = 0, j = mid, k = 0;
    while (i < mid && j < N_LIST)
    {
        if (first[i] < first[j])
            sorted_list[k++] = first[i++];
        else
            sorted_list[k++] = first[j++];
    }
    while (i < mid)
        sorted_list[k++] = first[i++];
    while (j < N_LIST)
        sorted_list[k++] = first[j++];
    gettimeofday(&end, NULL);
    printf("A1115513#M merge spent %ld usec\n", usec_elapsed(start, end));
    pthread_exit(NULL);
}

int main()
{
    int mylist[N_LIST];
    listncopy(mylist, original_list, N_LIST);

    print_list("A1115513-M", "All-Old", original_list, N_LIST);

    pthread_t t0, t1, tm;
    SortArg arg0 = {mylist, N_LIST / 2, "A1115513#0"};
    SortArg arg1 = {mylist + N_LIST / 2, N_LIST / 2, "A1115513#1"};

    pthread_create(&t0, NULL, do_sort, &arg0);
    pthread_create(&t1, NULL, do_sort, &arg1);

    pthread_join(t0, NULL);
    pthread_join(t1, NULL);

    pthread_create(&tm, NULL, do_merge, mylist);
    pthread_join(tm, NULL);

    print_list("A1115513-M", "All-New", sorted_list, N_LIST);

    return 0;
}
