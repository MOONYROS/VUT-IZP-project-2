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

    while(universum != NULL)
    {
        if(!strInSet(set1, universum->name))
        {
            addSetItem(resSet, universum->name);
        }
        universum = universum->next;
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

    while(set1 != NULL)
    {
        if(strInSet(set2, set1->name))
        {
            addSetItem(resSet, set1->name);
        }
        set1 = set1->next;
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

    while(set1 != NULL)
    {
        if(strInSet(set2, set1->name) == 0)
        {
            addSetItem(resSet, set1->name);
        }
        set1 = set1->next;
    }
    printSet(*resSet);
}

/** cmdSubseteqNoPrint vraci true nebo false podle toho, jestli je mnozina set1 podmnozinou mnoziny set2
 *
 * \param set1 je ukazatel na mnozinu 1
 * \param set2 je ukazatel na mnozinu 2
 * \return vraci true (1), pokud je mnozina set1 podmnozinou mnoziny set1, jinak false (0)
 *
 */
int cmdSubseteqNoPrint(TWordListItem *set1, TWordListItem *set2) /// Ondra
{
    while (set1 != NULL)
    {
        if(strInSet(set2, set1->name) == 0)
        {
            return false;
        }
        set1 = set1->next;
    }
    return true;
}

/** cmdEqualsNoPrint vraci true nebo false, jestli jsou mnoziny rovny
 *
 * \param set1 ukazatel na prvni mnozinu
 * \param set2 ukazatel na druhou mnozinu
 * \return vraci true (1), pokud jsou si mnoziny rovny, jinak false (0)
 *
 */
int cmdEqualsNoPrint(TWordListItem *set1, TWordListItem *set2)
{
    if(countElements(set1) != countElements(set2))
    {
        return false;
    }
    while(set1 != NULL)
    {
        if(strInSet(set2, set1->name) == 0)
        {
            return false;
        }
        set1 = set1->next;
    }
    return true;
}

/** cmdSubseteq vraci a tiskne true nebo false podle toho, jestli je mnozina set1 podmnozinou mnoziny set2
 *
 * \param set1 je ukazatel na mnozinu 1
 * \param set2 je ukazatel na mnozinu 2
 * \return vraci true (1), pokud je mnozina set1 podmnozinou mnoziny set1, jinak false (0)
 *
 */
int cmdSubseteq(TWordListItem *set1, TWordListItem *set2) /// Ondra
{
    if(cmdSubseteqNoPrint(set1, set2))
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

/** cmdSubset kontroluje, zda je množina valstní (true) podmnozinou nebo ne (false)
 *
 * \param set1 je ukazatel na prvni mnozinu (pripadnou podmnozinu)
 * \param set2 je ukazatel na druhou mnozinu
 * \return true pokud je set1 vlastni podmnozinou, false pokud neni
 *
 */
int cmdSubset(TWordListItem *set1, TWordListItem *set2)    /// Petana
{
    if(!(cmdEqualsNoPrint(set1, set2))&&(cmdSubseteqNoPrint(set1, set2)))
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

/** cmdEquals tiskne a vraci true nebo false, jestli jsou mnoziny rovny
 *
 * \param set1 ukazatel na prvni mnozinu
 * \param set2 ukazatel na druhou mnozinu
 * \return vraci true (1), pokud jsou si mnoziny rovny, jinak false (0)
 *
 */
int cmdEquals(TWordListItem *set1, TWordListItem *set2)    /// mikki - upravil MOONYROS
{
    if(cmdEqualsNoPrint(set1, set2))
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
