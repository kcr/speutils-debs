#ifndef __LINKEDLIST_H
#define __LINKEDLIST_H

struct list_entry_s {
    struct list_entry_s *previous;
    struct list_entry_s *next;
    void *entry;
};

struct list_s {
    struct list_entry_s *first;
    struct list_entry_s *last;
    struct list_entry_s *current;
    int entries;
};

struct list_s *listInit();

int listAddEntry(void *entry,struct list_s *list);
int listRemoveEntry(void *entry,struct list_s *list);
void *listFirstEntry(struct list_s *list);
void *listNextEntry(struct list_s *list);


#endif