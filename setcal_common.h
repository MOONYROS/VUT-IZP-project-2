#ifndef setcal_types_h
#define setcal_types_h

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
    int op1;
    int op2;
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

#endif // setcal_types_h
