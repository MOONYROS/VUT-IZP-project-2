#ifndef setcal_types_h
#define setcal_types_h

#define MAX_ITEM_LEN 30
#define MAX_LINES 1000
#define MAX_LINE_PARAMS 4
#define LINES_ALLOC_SIZE 3
#define UNIVERSUM_LINE 1

typedef struct _TWordListItem
{
    char *name;
    struct _TWordListItem *next;
} TWordListItem;

typedef struct _TRelationItem
{
    char *name1;
    char *name2;
    struct _TRelationItem *next;
} TRelationItem;

typedef struct _TCommand
{
    char *name;
    int op[MAX_LINE_PARAMS];
} TCommand;

typedef struct _TLine
{
    TWordListItem *set;
    TRelationItem *relation;
    TCommand *command;
} TLine;

int strInSet(TWordListItem *set, char *str);
void addSetItem(TWordListItem **pset, char *item);
void printWordList(TWordListItem *set);
void printSet(TWordListItem *set);
void printUniversum(TWordListItem *set);
int countElements(TWordListItem *set1);

#endif // setcal_types_h
