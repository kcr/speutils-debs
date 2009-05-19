#include <linkedlist.h>
#include <unistd.h>
#include <stdlib.h>
#include <malloc.h>

struct list_s *listInit()
{
    struct list_s *list;
    list = malloc(sizeof(struct list_s));
    list->first = NULL;
    list->last = NULL;
    list->current = NULL;
    list->entries = 0;
    return list;
}


int listAddEntry(void *entry,struct list_s *list) {

    struct list_entry_s *listentry = malloc(sizeof(struct list_entry_s));

    listentry->entry = entry;
    listentry->next = NULL;

    if (list->first == NULL) {
        list->first =listentry;
        list->last = listentry;
        listentry->previous = NULL;
    } else {
        listentry->previous = list->last;

        list->last->next = listentry;

        list->last = listentry;

    }
    list->entries++;
}


int listRemoveEntry(void *entry, struct list_s *list)
{
    int i = 0;

  //  void *next = listFirstEntry(list);
    list->current = list->first;
    do
    {
        //next = listNextEntry(list);
        if (list->current->entry == entry)
            break;

        if (list->current == list->last)
            return -1;

        list->current = list->current->next;
    } while(1);

    if (list->current->previous != NULL) {
        list->current->previous->next = list->current->next;
    }

    if (list->current == list->last)
        list->last = list->current->previous;

    if (list->current == list->first)
        list->first = list->current->next;

    struct list_entry_s *lentry = list->current;

    list->current = list->first;
    list->entries--;
    free(lentry);
    return 0;
}

int listEntries(struct list_s *list)
{
    return list->entries;
}

void *listFirstEntry(struct list_s *list)
{
    list->current = list->first;
    return list->first->entry;

}
void *listNextEntry(struct list_s *list) {
    list->current = list->current->next;
    if (list->current == NULL)
        return NULL;
    else
        return list->current->entry;
}