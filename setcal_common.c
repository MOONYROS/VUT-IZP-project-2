#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "setcal_common.h"

/** strInSet otestuje, jestli je retezec str prvkem mnoziny set
 *
 * \param set je ukazatel na mnozinu
 * \param str je retezec, ktery hledame
 * \return vraci 1, kdyz je retezec v mnozine nalezen, jinak 0
 *
 */
int strInSet(TWordListItem *set, char *str)
{
    while(set != NULL)
    {
        if(strcmp(set->name, str) == 0)
        {
            return 1;
        }
        set = set->next;
    }
    return 0;
}

/** addSetItem prida prvek do mnoziny
 *
 * \param pset je ukazatel na ukazatel na mnozinu, nesmi byt NULL
 * \param item je retezec predstavujici prvek mnoziny
 *
 */
void addSetItem(TWordListItem **pset, char *item)
{
    assert(pset != NULL);

    if(strInSet(*pset, item) == 1)
    {
        return;
    }

    TWordListItem *next = *pset;
    *pset = malloc(sizeof(TWordListItem));
    (*pset)->name = malloc(strlen(item)+1);
    strcpy((*pset)->name, item);
    (*pset)->next = next;
}

/** printSet vytiskne prvky do mnoziny oddelene ' ' na stdout
 *
 * \param set je ukazatel na mnozinu
 *
 */
void printWordList(TWordListItem *set)
{
    while(set != NULL)
    {
        printf(" %s", set->name);
        set = set->next;
    }
    printf("\n");
}

void printSet(TWordListItem *set)
{
    printf("S");
    printWordList(set);
}

void printUniversum(TWordListItem *set)
{
    printf("U");
    printWordList(set);
}

/** countElements spocita prvky v mnozine set1
 *
 * \param set1 je ukazatel na mnozinu
 * \return vraci pocet prvku v mnozine
 *
 */
int countElements(TWordListItem *set1)
{
    int elementCount = 0; /**< elementCount pocita jednotlive prvky v set1 */
    while(set1 != NULL)
    {
        elementCount++;
        set1 = set1->next;
    }
    return elementCount;
}
