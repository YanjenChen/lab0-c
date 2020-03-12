#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (q) {
        q->head = NULL;
        q->tail = NULL;
        q->size = 0;
    }
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    /* Do nothing if queue is NULL */
    if (!q) {
        return;
    }
    /* Remove first list element until queue is empty */
    list_ele_t *tmp = q->head;
    while (tmp) {
        q->head = tmp->next;
        free(tmp->value);
        free(tmp);
        tmp = q->head;
    }
    /* Free queue structure */
    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    /* Return false when queue is NULL or could not allocate space */
    list_ele_t *newh;
    char *news;
    const int slen = strlen(s);
    newh = malloc(sizeof(list_ele_t));
    news = malloc(sizeof(char) * (slen + 1));
    if (!q || !newh || !news) {
        return false;
    }
    /* Copy string value and manuly add \0 to buffer end */
    strncpy(news, s, slen);
    news[slen] = '\0';
    /* Modify tail if insert first element to queue */
    if (!(q->head)) {
        q->tail = newh;
    }
    newh->next = q->head;
    newh->value = news;
    q->head = newh;
    q->size++;
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    /* Return false when queue is NULL or could not allocate space */
    list_ele_t *newh;
    char *news;
    const int slen = strlen(s);
    newh = malloc(sizeof(list_ele_t));
    news = malloc(sizeof(char) * (slen + 1));
    if (!q || !newh || !news) {
        return false;
    }
    /* Copy string value and manuly add \0 to buffer end */
    strncpy(news, s, slen);
    news[slen] = '\0';
    newh->value = news;
    /* Modify head if insert first element to queue */
    if (!(q->head)) {
        q->head = newh;
    } else {
        q->tail->next = newh;
    }
    newh->next = NULL;
    q->tail = newh;
    q->size++;
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (!q || !(q->head)) {
        return false;
    }
    list_ele_t *tmp = q->head;
    q->head = tmp->next;
    /* Modify tail if remove last element in queue */
    if (!(tmp->next)) {
        q->tail = NULL;
    }
    /* Copy to sp if specified */
    const size_t slen = strlen(tmp->value);
    if (sp) {
        strncpy(sp, tmp->value, slen >= bufsize ? bufsize - 1 : slen);
        sp[slen >= bufsize ? bufsize - 1 : slen] = '\0';
    }
    /* Free memory of removed element */
    free(tmp->value);
    free(tmp);
    q->size--;
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    return q ? q->size : 0;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (!q || !(q->head)) {
        return;
    }
    list_ele_t *current, *prev, *tmp;
    current = q->head;
    prev = NULL;
    q->tail = q->head;
    while (current) {
        tmp = current->next;
        current->next = prev;
        prev = current;
        current = tmp;
    }
    q->head = prev;
}

/*
 * Split list to half
 */
static void split_list(list_ele_t *head, list_ele_t **front, list_ele_t **back)
{
    list_ele_t *fast, *slow;
    slow = head;
    fast = head->next;
    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }
    *front = head;
    *back = slow->next;
    slow->next = NULL;
    return;
}

/*
 * Merge given list by nature order
 * Recursive funciton call will trigger stackoverflow,
 * use loop instead.
 */
static list_ele_t *merge(list_ele_t *a, list_ele_t *b)
{
    if (!a)
        return b;
    else if (!b)
        return a;
    list_ele_t *head, *tmp;
    if (strcmp(a->value, b->value) <= 0) {
        head = a;
        a = a->next;
    } else {
        head = b;
        b = b->next;
    }
    tmp = head;
    while (a && b) {
        /* if (strnatcmp(a->value, b->value) == -1) { */
        if (strcmp(a->value, b->value) <= 0) {
            tmp->next = a;
            a = a->next;
        } else {
            tmp->next = b;
            b = b->next;
        }
        tmp = tmp->next;
    }
    if (!a)
        tmp->next = b;
    else if (!b)
        tmp->next = a;
    return head;
}

/*
 * Merge sort
 */
static void merge_sort(list_ele_t **head_ref)
{
    list_ele_t *head = *head_ref;
    list_ele_t *a, *b;
    if (!head || !(head->next)) {
        return;
    }
    split_list(head, &a, &b);
    merge_sort(&a);
    merge_sort(&b);
    *head_ref = merge(a, b);
    return;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    /* this will cause segementation falut */
    /* strnatcmp("a", "b"); */
    if (!q || !(q->head)) {
        return;
    }
    merge_sort(&(q->head));
    /* Update tail */
    list_ele_t *tmp;
    for (tmp = q->head; tmp->next != NULL; tmp = tmp->next) {
    };
    q->tail = tmp;
    return;
}
