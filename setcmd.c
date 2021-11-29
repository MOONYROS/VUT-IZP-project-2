#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "setcmd.h"

/** cmdEmpty tiskne a vraci true nebo false podle toho, jestli je mnozina definovana na radku A prazdna nebo neprazdna
 *
 * \param set1 je ukazatel na mnozinu
 * \return vraci true (1), pokud je mnozina prazdna, jinak false (0)
 *
 */
int cmdEmpty(TWordListItem *set1)
{
    if(set1 == NULL)
    {
        printf("true\n");
        return true;
    }
    else
    {
        printf("false\n");
        return false;
    }
}

/** cmdCard tiskne pocet prvku v mnozine set1
 *
 * \param set1 je ukazatel na mnozinu (radek), obsahuje ukazatel na sebe sama, dokud neni NULL, tak se pocitaji prvky
 * \param elementCount je promenna, do ktere se uklada pocet prvku v mnozine, pricitaji se, dokud set1 neni NULL
 *
 */
void cmdCard(TWordListItem *set1)
{
    printf("%d\n", countElements(set1));
}

/** cmdComplement tiskne mnozinovy doplnek k zadane mnozine
 *
 * \param set1 je ukazatel na danou mnozinu
 * \param universum je ukazatelm na mnozinu vsech prvku
 * \param *resSet je ukazatel na ukazatel na vyslednou mnozinu
 *
 */
void cmdComplement(TWordListItem *set1, TWordListItem *universum, TWordListItem **resSet) /// Petana
{
    assert(resSet != NULL);
    TWordListItem *setTmp = set1;

    while(universum != NULL)
    {
        int check = 0;
        while(setTmp != NULL)
        {
            if(strcmp(universum->name, setTmp->name) == 0) check = 1;
            setTmp = setTmp->next;
        }

        if(!check && resSet) addSetItem(resSet, universum->name);

        universum = universum->next;
        setTmp = set1;
    }
    printSet(*resSet);
}

/** cmdUnion tiske sjednoceni mnozin set1 a set2
 *
 * \param set1 je ukazatel na prvni mnozinu
 * \param set2 je ukazatel na druhou mnozinu
 * \param resSet je ukazatel na ukazatel, do ktereho je ulozena vysledna mnozina
 *
 */
void cmdUnion(TWordListItem *set1, TWordListItem *set2, TWordListItem **resSet) /// mikki
{
    assert(resSet != NULL);

    while(set1 != NULL)  // vsechno z mnoziny set1 vypiseme na vystup
    {
        addSetItem(resSet, set1->name);
        set1 = set1->next;
    }
    while(set2 != NULL)  // vsechno z mnoziny set2 pridame taky. duplicity se nevlozi
    {
        addSetItem(resSet, set2->name);
        set2 = set2->next;
    }
    printSet(*resSet);
}

/** cmdIntersect tiskne prunik mnozin set1 a set2
 *
 * \param set1 je ukazatel na prvni mnozinu
 * \param set2 je ukazatel na druhou mnozinu
 * \param resSet je ukazatel na ukazatel, do ktereho je ulozena vysledna mnozina
 *
 */
void cmdIntersect(TWordListItem *set1, TWordListItem *set2, TWordListItem **resSet)
{
    assert(resSet != NULL);

    TWordListItem *setTmp = set2;
    while(set1 != NULL)
    {
        while(setTmp != NULL)
        {
            if(strcmp(set1->name, setTmp->name) == 0)
            {
                if(resSet != NULL)
                {
                    addSetItem(resSet, set1->name);
                }
            }
            setTmp = setTmp->next;
        }
        set1 = set1->next;
        setTmp = set2;
    }
    printSet(*resSet);
}

/** cmdMinus tiskne rozdil mnozin set1 a set2
 *
 * \param set1 je ukazatel na prvni mnozinu
 * \param set2 je ukazatel na druhou mnozinu
 * \param resSet je ukazatel na ukazatel, do ktereho je ulozena vysledna mnozina
 *
 */
void cmdMinus(TWordListItem *set1, TWordListItem *set2, TWordListItem **resSet)
{
    assert(resSet != NULL);

    TWordListItem *setTmp = set2;
    while(set1 != NULL)
    {
        int isThere = 0;
        while(setTmp != NULL)
        {
            if(strcmp(set1->name, setTmp->name) == 0)
            {
                isThere = 1;
            }
            setTmp = setTmp->next;
        }
        if(isThere == 0)
        {
            if(resSet != NULL)
            {
                addSetItem(resSet, set1->name);
            }
        }
        set1 = set1->next;
        setTmp = set2;
    }
    printSet(*resSet);
}

/**
 * 
 * \param set1 je ukazatel na mnozinu 1
 * \param set2 je ukazatel na mnozinu 2
 * \param se1ElementNum je pocet prvku v mnozine 1
 * \param commonElementNum je pocet prvku, ktere ma mnozina 1 spolecne s mnozinou 2
 * \return true v pripade, ze pocet spolecnych prvku se rovna poctu prvku v mnozine 1
 * \return false kdykoliv jindy
 * V cyklech se postupne prochazeji obe dve mnoziny a dle toho se modifikuji vyse uvedene parametry
 * 
 */

int cmdSubseteq(TWordListItem *set1, TWordListItem *set2) /// Ondra
{
    int set1ElementNum = 0;
    int commonElementNum = 0;

    TWordListItem *tmpSet = set2;

    while (set1 != NULL)
    {
        set1ElementNum++;

        while (tmpSet != NULL)
        {
            if (strcmp(set1->name, tmpSet->name) == 0)
            {
                commonElementNum++;
            }

            tmpSet = tmpSet->next;
        }

        set1 = set1->next;
        tmpSet = set2;
    }

    if (commonElementNum == set1ElementNum)
    {
        return true;
    }
    else 
    {
        return false;
    }
}

/** cmdSubset kontroluje, zda je množina valstní (true) podmnozinou nebo ne (false)
 *
 * \param set1 je ukazatel na prvni mnozinu (pripadnou podmnozinu)
 * \param set2 je ukazatel na druhou mnozinu
 * \return true pokud je set1 vlastni podmnozinou, false pokud neni
 *
 */
int cmdSubset(TWordListItem *set1, TWordListItem *set2)    /// Petana
{
    if(!(cmdEquals(set1, set2))&&(cmdSubseteq(set1, set2))){
      printf("True\n");
      return true;
    }
    else{
      printf("False\n");
      return false;
    }
}

/** cmdEquals tiskne a vraci true nebo false, jestli jsou mnoziny rovny
 *
 * \param set1 ukazatel na prvni mnozinu
 * \param set2 ukazatel na druhou mnozinu
 * \return vraci true (1), pokud jsou si mnoziny rovny, jinak false (0)
 *
 */
int cmdEquals(TWordListItem *set1, TWordListItem *set2)    /// mikki - upravil MOONYROS
{
    TWordListItem *tmpSet;
    int isSame;
    if(countElements(set1) != countElements(set2))
    {
        printf("false\n");
        return false;
    }
    while(set1 != NULL)
    {
        isSame = 0;
        tmpSet = set2;
        while(tmpSet != NULL)
        {
            if(strcmp(set1->name, tmpSet->name) == 0)
            {
                isSame = 1;
                break;
            }
            tmpSet = tmpSet->next;
        }
        if(isSame == 0)
        {
            printf("false\n");
            return false;
        }
        set1 = set1->next;
    }
    printf("true\n");
    return true;
}
