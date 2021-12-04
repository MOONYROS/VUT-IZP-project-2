#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "setcal_common.h"

/** \brief strInSet otestuje, jestli je retezec str prvkem mnoziny set
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

/** \brief addSetItem prida prvek do mnoziny
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

/** \brief printWordList vytiskne prvky do mnoziny oddelene ' ' na stdout
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

/** \brief printSet vytiskne danou mnozinu
 *
 * \param set je ukazatel na mnozinu
 *
 */
void printSet(TWordListItem *set)
{
    printf("S");
    printWordList(set);
}

/** \brief printUniversum vytiskne danou mnozinu
 *
 * \param set je ukazatel na mnozinu
 *
 */
void printUniversum(TWordListItem *set)
{
    printf("U");
    printWordList(set);
}

/** \brief countElements spocita prvky v mnozine set1
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

/** \brief addRelationItem prida prvky do relace
 *
 * \param prel je ukazatel na ukazatel na relaci, nesmi byt NULL
 * \param name1 je prvni prvek relace
 * \param name2 je druhy prvek relace
 *
 */
void addRelationItem(TRelationItem **prel, char *name1, char *name2)
{
    assert(prel != NULL);

    if(findRelXY(*prel, name1, name2) != NULL) /**< test, jestli dvojice uz je v relaci obsazena */
    {
        return;
    }

    TRelationItem *next = *prel;
    *prel = malloc(sizeof(TRelationItem));
    if(*prel == NULL)
    {
        fprintf(stderr, "ERROR: Nepodarilo se alokovat dostatek mista v pameti.\n");
        exit(ERR_MALLOC);
    }
    (*prel)->name1 = malloc(strlen(name1)+1);
    if((*prel)->name1 == NULL)
    {
        fprintf(stderr, "ERROR: Nepodarilo se alokovat dostatek mista v pameti.\n");
        exit(ERR_MALLOC);
    }
    strcpy((*prel)->name1, name1);
    (*prel)->name2 = malloc(strlen(name2)+1);
    if((*prel)->name2 == NULL)
    {
        fprintf(stderr, "ERROR: Nepodarilo se alokovat dostatek mista v pameti.\n");
        exit(ERR_MALLOC);
    }
    strcpy((*prel)->name2, name2);
    (*prel)->next = next;
}

/** \brief printRelation vytiskne vechny prvky relace na stdout
 *
 * \param rel je ukazatel na relaci
 *
 */
void printRelation(TRelationItem *rel)
{
    printf("R");
    while(rel != NULL)
    {
        printf(" (%s %s)", rel->name1, rel->name2);
        rel = rel->next;
    }
    printf("\n");
}

TRelationItem *findRelX(TRelationItem *rel, char *x)
{
    while(rel != NULL)
    {
        if(strcmp(rel->name1, x)==0)
        {
            return rel;
        }
        rel = rel->next;
    }
    return NULL;
}

TRelationItem *findRelY(TRelationItem *rel, char *y)
{
    while(rel != NULL)
    {
        if(strcmp(rel->name2, y)==0)
        {
            return rel;
        }
        rel = rel->next;
    }
    return NULL;
}

TRelationItem *findRelXY(TRelationItem *rel, char *x, char *y)
{
    while(rel != NULL)
    {
        if((strcmp(rel->name1, x)==0) && (strcmp(rel->name2, y)==0))
        {
            return rel;
        }
        rel = rel->next;
    }
    return NULL;
}

int countRelX(TRelationItem *rel, char *x)
{
    int cnt = 0;
    while(rel != NULL)
    {
        if(strcmp(rel->name1, x)==0)
        {
            cnt++;
        }
        rel = rel->next;
    }
    return cnt;
}

int countRelY(TRelationItem *rel, char *y)
{
    int cnt = 0;
    while(rel != NULL)
    {
        if(strcmp(rel->name2, y)==0)
        {
            cnt++;
        }
        rel = rel->next;
    }
    return cnt;
}
