/**
 * @file setcal.cz
 * @author  Ondrej Lukasek <ondrej@lukasek.cz>
 * @version 0.1
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * https://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * Set calculator reads file, that describes sets and relations and calculates math operations and displays result.
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <time.h>

#include "setcal_common.h"
#include "setcmd.h"
#include "relcmd.h"

#define ERR_OK 0
#define ERR_NR_ARG 1
#define ERR_FOPEN 2
#define ERR_FCLOSE 3
//#define ERR_WRONG_CHAR 4
#define ERR_LAST_LINE_CHARS 5
#define ERR_MALLOC 6
//#define ERR_PROCESS_LINE 7
#define ERR_EMPTY_LINE 8
#define ERR_WRONG_COMMAND 9
#define ERR_WRONG_LINE_TYPE 10

#define PL_ERR_INVALID_CMD 11
#define PL_ERR_INVALID_CHAR 12
#define PL_ERR_TOO_LONG_ITEM 13
#define PL_ERR_SET_AFTER_COMMAND 14
#define PL_ERR_REL_AFTER_COMMAND 15
#define PL_ERR_UNIVERSE_NOT_LINE1 16
#define PL_ERR_BAD_CMD_PARAM 17
#define PL_ERR_MALLOC_CMD 18
#define PL_ERR_ITEM_NOT_IN_UNIVERSE 19
#define PL_ERR_ITEM_IN_COMMANDS 20
#define PL_ERR_ITEM_IN_SET_ALREADY 21
#define PL_ERR_REL_ITEM_MISSING 22
#define PL_ERR_MISSING_PARENTHESIS 23
#define PL_ERR_ITEMS_IN_RELATION_ALREADY 24


#define ERR_CMD_LINE_NOEX 1
#define ERR_CMD_NOEX 2

#define STR_WRONG_PARAM_LINE "ERROR: Chybny parametr prikazu (neexistujici radek?) na radku %d!\n"

#define JUMP(iline) \
{ \
    if(iline > lines) \
    { \
        fprintf(stderr, STR_WRONG_PARAM_LINE, ln + 1); \
        return ERR_CMD_LINE_NOEX; \
    } \
    if(iline > 0) \
    { \
        ln = iline - 2; \
    } \
}

/** \brief strInRelation otestuje, jestli je dvojice retezcu str1 a str2 prvkem relace rel
 *
 * \param rel je ukazatel na relaci
 * \param str1 je prvni prvek relace, ktery hledame
 * \param str2 je druhy prvek relace, ktery hledame
 * \return vraci 1, kdyz je dvojice v relaci nalezena, jinak 0
 *
 */
 /*
int strInRelation(TRelationItem *rel, char *str1, char *str2)
{
    while(rel != NULL)
    {
        if(strcmp(rel->name1, str1) == 0 && strcmp(rel->name2, str2) == 0 )
        {
            return 1;
        }
        rel = rel->next;
    }
    return 0;
}
*/

/** \brief freeWordList uvolni pamet z celeho seznamu retezcu, vcetne retezcu o kterych predpoklada, ze jsou alokovane dynamicky
 *
 * \param item je ukazatel na prvni prvek seznamu
 *
 */
void freeWordList(TWordListItem *item)
{
    TWordListItem *tmpItem;

    while(item != NULL)
    {
        tmpItem = item;
        item = item->next;
        free(tmpItem->name);
        free(tmpItem);
    }
}

/** \brief freeRelationList uvolni pamet z celeho seznamu TRelationItem, vcetne retezcu o kterych predpoklada, ze jsou alokovane dynamicky
 *
 * \param item je ukazatel na prvni prvek seznamu
 *
 */
void freeRelationList(TRelationItem *item)
{
    TRelationItem *tmpItem;

    while(item != NULL)
    {
        tmpItem = item;
        item = item->next;
        free(tmpItem->name1);
        free(tmpItem->name2);
        free(tmpItem);
    }
}

/** \brief hasOnlyEnLetters zkontorluje, jestli retezec obsahuje pouze mala a velka pismena anglicke abecedy
 *
 * \param str je retezec, ktery se ma zkontrolovat
 * \return vraci 1 pokud retezec obsahuje jen mala a velka pismena anglicke abecedy, jinak 0
 *
 */
int hasOnlyEnLetters(char *str)
{
    for(size_t i=0; i<strlen(str); i++)
    {
        if(str[i] < 'A' || (str[i] > 'Z' && str[i] < 'a') || str[i] > 'z')
        {
            return 0;
        }
    }
    return 1;
}

/** \brief strInArray hleda retezec v poli
 *
 * \param str je hledany retezec
 * \param arr[] je pole, ve kterem hledame retezec str
 * \return funkce vraci 1, pokud se retezec nachazi v poli, jinak vraci 0
 *
 */
int strInArray(char *str, char *arr[], int arrSize)
{
    for(int i = 0; i < arrSize; i++)
    {
        if(strcmp(arr[i], str) == 0)
        {
            return 1;
        }
    }
    return 0;
}

/** \brief strInKeywords zjisti, jesli se retezec str vyskytuje mezi klicovymi slovy "true" a "false"
 *
 * \param str je hledany retezec
 * \return vraci 1, kdyz je retezec mezi klicovymi slovy nalezen, jinak 0
 *
 */
int strInKeywords(char *str)
{
    char *keywords[] = {"true", "false"};

    return strInArray(str, keywords, sizeof(keywords)/sizeof(char*));
}

/** \brief strInSetCmds1 zjisti, jesli se retezec str vyskytuje mezi mnozinovymi prikazy s 1 operandem
 *
 * \param str je hledany retezec
 * \return vraci 1, kdyz je retezec mezi prikazy nalezen, jinak 0
 *
 */
int strInSetCmds1(char *str)
{
    char *setCmds1[] = {"empty", "card", "complement", "select"};

    return strInArray(str, setCmds1, sizeof(setCmds1)/sizeof(char*));
}

/** \brief strInSetCmds2 zjisti, jesli se retezec str vyskytuje mezi mnozinovymi prikazy se 2 operandy
 *
 * \param str je hledany retezec
 * \return vraci 1, kdyz je retezec mezi prikazy nalezen, jinak 0
 *
 */
int strInSetCmds2(char *str)
{
    char *setCmds2[] = {"union", "intersect", "minus", "subseteq", "subset", "equals"};

    return strInArray(str, setCmds2, sizeof(setCmds2)/sizeof(char*));
}

/** \brief strInRelCmds1 zjisti, jesli se retezec str vyskytuje mezi relacnimi prikazy s jednim operandem
 *
 * \param str je hledany retezec
 * \return vraci 1, kdyz je retezec mezi prikazy nalezen, jinak 0
 *
 */
int strInRelCmds1(char *str)
{
    char *relCmds1[] = {"reflexive", "symmetric", "antisymmetric", "transitive", "function",
                       "domain", "codomain", "closure_ref", "closure_sym", "closure_trans"};

    return strInArray(str, relCmds1, sizeof(relCmds1)/sizeof(char*));
}

/** \brief strInRelCmds3 zjisti, jesli se retezec str vyskytuje mezi relacnimi prikazy se tremi operandy
 *
 * \param str je hledany retezec
 * \return vraci 1, kdyz je retezec mezi prikazy nalezen, jinak 0
 *
 */
int strInRelCmds3(char *str)
{
    char *relCmds3[] = {"injective", "surjective", "bijective"};

    return strInArray(str, relCmds3, sizeof(relCmds3)/sizeof(char*));
}

/** \brief strInSetCreateCmds zjisti, jesli se retezec str vyskytuje mezi prikazy, ktere vytvareji mnozinu
 *
 * \param str je hledany retezec
 * \return vraci 1, kdyz je retezec mezi prikazy nalezen, jinak 0
 *
 */
int strInSetCreateCmds(char *str)
{
    char *setCreateCmds[] = {"complement", "union", "intersect", "minus", "domain", "codomain"};

    return strInArray(str, setCreateCmds, sizeof(setCreateCmds)/sizeof(char*));
}

/** \brief strInSetCreateCmds zjisti, jesli se retezec str vyskytuje mezi prikazy, ktere vytvareji mnozinu
 *
 * \param str je hledany retezec
 * \return vraci 1, kdyz je retezec mezi prikazy nalezen, jinak 0
 *
 */
int strInRelationCreateCmds(char *str)
{
    char *relCreateCmds[] = {"closure_ref", "closure_sym", "closure_trans"};

    return strInArray(str, relCreateCmds, sizeof(relCreateCmds)/sizeof(char*));
}

/** \brief printAllLines vytiskne vsechny seznamy ze vsech radku
 *
 * \param line je ukazatel na pole radku
 * \param lineNr je pocet platnych radku v poli
 *
 */
void printAllLines(TLine *line, int lineNr)
{
    for(int i=0; i<lineNr; i++)
    {
        if(line[i].content == 'S' && line[i].set != NULL)
        {
            if(i == 0)
            {
                printf("%4d: ", i+1);
                printUniversum(line[i].set);
            }
            else
            {
                printf("%4d: ", i+1);
                printSet(line[i].set);

            }
        }
        if(line[i].content == 'R' && line[i].relation != NULL)
        {
            printf("%4d: ", i+1);
            printRelation(line[i].relation);
        }
        if(line[i].command != NULL)
        {
            printf("%4d: ", i+1);
            if(strInSetCmds1(line[i].command->name) == 1 || strInRelCmds1(line[i].command->name) == 1)
            {
                printf("C %s %d\n", line[i].command->name, line[i].command->op[0]);
            }
            else
            {
                if(strInSetCmds2(line[i].command->name) == 1)
                {
                    printf("C %s %d %d\n", line[i].command->name, line[i].command->op[0], line[i].command->op[1]);
                }
                else
                {

                    if(strInRelCmds1(line[i].command->name) == 1)
                    {
                        printf("C %s %d\n", line[i].command->name, line[i].command->op[0]);
                    }
                    else
                    {
                        if(strInRelCmds3(line[i].command->name) == 1)
                        {
                            printf("C %s %d %d %d\n", line[i].command->name, line[i].command->op[0], line[i].command->op[1], line[i].command->op[2]);
                        }
                        else
                        {
                            printf("C Unknown command???\n");
                        }
                    }
                }
            }
        }
        if(line[i].set == NULL && line[i].relation == NULL && line[i].command == NULL)
        {
            printf("%4d: %c\n", i+1, line[i].content);
        }

    }
}

/** freeAllLines uvolni vsechny seznamy ze vsech radku a nakonec i pole radku
 *
 * \param line je ukazatel na pole radku
 * \param lineNr je pocet platnych radku v poli
 *
 */
void freeAllLines(TLine *line, int lineNr)
{
    for(int i = 0; i < lineNr; i++)
    {
        freeWordList(line[i].set);
        freeRelationList(line[i].relation);
        free(line[i].command);
    }
    free(line);
}

/** \brief processLine zpracuje tokeny z nacteneho radku
 *  unviversum, mnoziny a relace ulozi do prislusnych radku v poli line[]
 *  prikazy jsou pouze ulozeny, ale neprovedeny, protoze nemame zatim nactene vsechny radky a nevime tedy, zda existuji
 *
 * \param token je ukazatel na seznam tokenu (obsah vstupniho radku), nesmi byt NULL
 * \param lineNr je cislo radku ke kteremu vstupni tokeny priradit
 * \param wasCommand je ukazatel na promennou indikujici, ze jiz byl nacten nejaky prikaz 'C'
 * \param line je pole radku
 * \return vraci 0 pri uspesnem zpracovani, jinak kod chyby v pripade neuspechu
 *
 */
int processLine(TWordListItem *token, int lineNr, int *wasCommand, TLine *line)
{
    assert(token != NULL);
    if(strlen(token->name)!=1)
    {
        fprintf(stderr, "ERROR: Prilis dlouha definice typu radku!\n");
        return PL_ERR_INVALID_CMD;
    }
    switch(*(token->name))
    {
        case 'U':
            if(lineNr != UNIVERSUM_LINE)
            {
                fprintf(stderr, "ERROR: Univerzum neni na radku 1!\n");
                return PL_ERR_UNIVERSE_NOT_LINE1;
            }
            line[lineNr - 1].content = 'S';
            token=token->next;
            while(token != NULL)
            {
                if(hasOnlyEnLetters(token->name) == 0)
                {
                    fprintf(stderr, "ERROR: Vadny znak v prvku na radku %d. Prvky musi obsahovat jen mala a velka pismena anglicke abecedy!\n", lineNr);
                    return PL_ERR_INVALID_CHAR;
                }
                if(strInSetCmds1(token->name) == 1 || strInSetCmds2(token->name) == 1 || strInRelCmds1(token->name) == 1 || strInRelCmds3(token->name) == 1 || strInKeywords(token->name) == 1)
                {
                    fprintf(stderr, "ERROR: Prvek na radku %d je obsazeny v prikazech nebo klicovych slovech!\n", lineNr);
                    return PL_ERR_ITEM_IN_COMMANDS;
                }
                if(strInSet(line[UNIVERSUM_LINE - 1].set, token->name) == 1)
                {
                    fprintf(stderr, "ERROR: Prvek na radku %d uz je v mnozine obsazen!\n", lineNr);
                    return PL_ERR_ITEM_IN_SET_ALREADY;
                }
                addSetItem(&line[UNIVERSUM_LINE - 1].set, token->name);
                token = token->next;
            }
            printUniversum(line[0].set);
            break;
        case 'S':
            // printf("SetCMD\n");
            if(line[0].set == NULL)
            {
                fprintf(stderr, "ERROR: Univerzum nebylo definovano a mame mnozinu na radku %d!\n", lineNr);
                return PL_ERR_UNIVERSE_NOT_LINE1;
            }
            if(*wasCommand == 1)
            {
                fprintf(stderr, "ERROR: Mnozina na radku %d po prikazu Command!\n", lineNr);
                return PL_ERR_SET_AFTER_COMMAND;
            }
            line[lineNr - 1].content = 'S';
            token=token->next;
            while(token != NULL)
            {
                /* pismena anglicke abecedy se nemusi kontrolovat u mnoziny a relace, protoze by jinak stejne nemohlo byt v univerzu, kde se to kontroluje
                if(hasOnlyEnLetters(token->name) == 0)
                {
                    fprintf(stderr, "ERROR: Vadny znak v prvku. Prvky musi obsahovat jen mala a velka pismena anglicke abecedy!\n");
                    return PL_ERR_INVALID_CHAR;
                } */
                if(strInSet(line[0].set, token->name) == 0)
                {
                    fprintf(stderr, "ERROR: Prvek na radku %d neni obsazeny v Univerzu (vadne znaky?)!\n", lineNr);
                    return PL_ERR_ITEM_NOT_IN_UNIVERSE;
                }
                if(strInSet(line[lineNr - 1].set, token->name) == 1)
                {
                    fprintf(stderr, "ERROR: Prvek na radku %d je uz v mnozine obsazen!\n", lineNr);
                    return PL_ERR_ITEM_IN_SET_ALREADY;
                }
                addSetItem(&line[lineNr - 1].set, token->name);
                token = token->next;
            }
            printSet(line[lineNr - 1].set);
            break;
        case 'R':
            if(line[0].set == NULL)
            {
                fprintf(stderr, "ERROR: Univerzum nebylo definovano a mame relaci na radku %d!\n", lineNr);
                return PL_ERR_UNIVERSE_NOT_LINE1;
            }
            if(*wasCommand == 1)
            {
                fprintf(stderr, "ERROR: Relace na radku %d po prikazu Command!\n", lineNr);
                return PL_ERR_REL_AFTER_COMMAND;
            }
            line[lineNr - 1].content = 'R';
            char name1[MAX_ITEM_LEN + 1];
            char name2[MAX_ITEM_LEN + 1];
            token = token->next;
            while(token != NULL)
            {
                strcpy(name1, token->name);
                token = token->next;
                if(token == NULL)
                {
                    fprintf(stderr, "ERROR: Chybne definovana relace na radku %d!\n", lineNr);
                    return PL_ERR_REL_ITEM_MISSING;
                }
                strcpy(name2, token->name);
                token = token->next;
                if(strlen(name1) > 1 && name1[0] == '(')
                {
                    memmove(name1, name1 + 1, strlen(name1));
                }
                else
                {
                    fprintf(stderr, "ERROR: Chybne definovana relace na radku %d!\n", lineNr);
                    return PL_ERR_MISSING_PARENTHESIS;
                }
                if(strlen(name2) > 0 && name2[strlen(name2) - 1] == ')')
                {
                    name2[strlen(name2) - 1] = '\0';
                }
                else
                {
                    fprintf(stderr, "ERROR: Chybne definovana relace na radku %d!\n", lineNr);
                    return PL_ERR_MISSING_PARENTHESIS;
                }

                if(strInSet(line[0].set, name1) == 0 || strInSet(line[0].set, name2) == 0)
                {
                    fprintf(stderr, "ERROR: Prvek relace z radku %d neni obsazen v univerzu (vadne znaky?)!\n", lineNr);
                    return PL_ERR_ITEM_NOT_IN_UNIVERSE;
                }
                if(findRelXY(line[lineNr - 1].relation, name1, name2) != NULL)
                {
                    fprintf(stderr, "ERROR: Prvek na radku %d uz je v relaci obsazen!\n", lineNr);
                    return PL_ERR_ITEMS_IN_RELATION_ALREADY;
                }
                addRelationItem(&line[lineNr - 1].relation, name1, name2);
            }
            printRelation(line[lineNr - 1].relation);
            break;
        case 'C':
            line[lineNr - 1].content = 'C';
            *wasCommand = 1;
            token=token->next;
            int param = 0;
            int op[MAX_LINE_PARAMS];
            for(int i = 0; i < MAX_LINE_PARAMS; i++)
            {
                op[i] = 0;
            }
            char command[MAX_ITEM_LEN + 1];

            while(token != NULL)
            {
                switch(param)
                {
                    case 0:
                        strcpy(command, token->name);
                        break;
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                        for(int i = 0; i < strlen(token->name); i++)
                        {
                            if(token->name[i] < '0' || token->name[i] > '9')
                            {
                                fprintf(stderr, "ERROR: Parametr prikazu na radku %d neni cislo!\n", lineNr);
                                return PL_ERR_BAD_CMD_PARAM;
                            }
                        }
                        if(sscanf(token->name, "%d", &op[param - 1]) != 1)
                        {
                            fprintf(stderr, "ERROR: Parametr prikazu na radku %d neni cislo!\n", lineNr);
                            return PL_ERR_BAD_CMD_PARAM;
                        }
                        if(op[param - 1] < 1 || op[param - 1] > MAX_LINES)
                        {
                            fprintf(stderr, "ERROR: Parametr prikazu (radek) na radku %d je mimo rozsah 1-1000!\n", lineNr);
                            return PL_ERR_BAD_CMD_PARAM;
                        }
                        break;
                    default:
                        fprintf(stderr, "ERROR: Prikaz na radku %d ma vice nez 4 parametry!\n", lineNr);
                        return PL_ERR_INVALID_CMD;
                }
                param++;
                token=token->next;
            }
            line[lineNr-1].command = malloc(sizeof(TCommand));
            if(line[lineNr-1].command ==  NULL)
            {
                fprintf(stderr, "ERROR: Nelze alokovat pamet pro ulozeni prikazu!\n");
                exit(ERR_MALLOC);
            }
            line[lineNr-1].command->name = malloc(strlen(command)+1);
            if(line[lineNr-1].command->name ==  NULL)
            {
                fprintf(stderr, "ERROR: Nelze alokovat pamet pro ulozeni jmena prikazu!\n");
                exit(ERR_MALLOC);
            }
            strcpy(line[lineNr-1].command->name, command);
            for(int i = 0; i < MAX_LINE_PARAMS; i++)
            {
                line[lineNr-1].command->op[i] = op[i];
            }
            //printf("Command: %s %d %d\n", line[lineNr-1].command->name, line[lineNr-1].command->op1, line[lineNr-1].command->op2);
            if(strInSetCreateCmds(command) == 1)
            {
                line[lineNr-1].content = 'S';
            }
            if(strInRelationCreateCmds(command) == 1)
            {
                line[lineNr-1].content = 'R';
            }
            break;
        default:
            fprintf(stderr, "ERROR: Spatny prikaz na radku %d! Povolene prikazy jsou pouze U, S, R, C!\n", lineNr);
            return PL_ERR_INVALID_CMD;
    }
    return 0;
}

/** \brief skipWhitesFromFile preskoci bile znaky pri nacitani soubori
 *
 * \param fp je vstupni soubor
 * \param ch je ukazatel na posledni znak nacteny ze souboru (musi byt nacteny pred volanim)
 * \return funkce nic nevraci, ale v ch bude pri navratu prvni nebily znak na ktery narazi a tento znak bude uz nacteny ze souboru
 *
 */
void skipWhitesFromFile(FILE *fp, char *ch)
{
    while(*ch == ' ' || *ch == '\t')
    {
        *ch = fgetc(fp);
    }
}

/** \brief getLineFromFile precte radek ze vstupniho souboru a vrati ho jako seznam tokenu
 *
 * \param fp je vstupni soubor
 * \return funkce vraci seznam tokenu jako struct _TWordListItem
 *
 */
TWordListItem * getLineFromFile(FILE *fp)
{
    TWordListItem *tokenList = NULL;
    TWordListItem *tmpTokenList;
    int i = 0;
    char token[MAX_ITEM_LEN];

    char ch = fgetc(fp);
    //skipWhitesFromFile(fp, &ch);
    if(ch != 'U' && ch != 'S' && ch != 'R' && ch != 'C')
    {
        return NULL;
    }
    while(ch != EOF)
    {
        if(ch == ' ' || ch == '\t' || ch == '\n')
        {
            if(i == 0)
            {
                // mame konec radku, tak prazdny retezec uz pridavat nebudeme
                //fprintf(stderr, "ERROR: Prazdny token, mezery na konci radku!\n");
                return tokenList;
            }
            // novy token - ukoncit a dat na konec seznamu
            token[i] = '\0';
            // jestli je zatim prazdny seznam tokenu, musime ho zalozit jinak pridat na konec
            // musime drzet poradi na radku kvuli prikazum
            if(tokenList==NULL)
            {
                tokenList = malloc(sizeof(TWordListItem));
                if(tokenList == NULL)
                {
                    fprintf(stderr, "ERROR: Nepodarilo se alokovat dostatek mista v pameti.\n");
                    exit(ERR_MALLOC);
                }
                tmpTokenList = tokenList;
            }
            else
            {
                tmpTokenList = tokenList;
                while(tmpTokenList->next != NULL)
                {
                    tmpTokenList = tmpTokenList->next;
                }
                tmpTokenList->next = malloc(sizeof(TWordListItem));
                if(tmpTokenList->next == NULL)
                {
                    fprintf(stderr, "ERROR: Nepodarilo se alokovat dostatek mista v pameti.\n");
                    exit(ERR_MALLOC);
                }
                tmpTokenList = tmpTokenList->next;
            }
            // kontrola jestli se povedl zalozit novy token
            if( tmpTokenList==NULL )
            {
                fprintf(stderr, "ERROR: Nepodarilo se alokovat pamet pro novy token!\n");
                return NULL;
            }
            // zalozit retezec, opet zkontrolovat a naplnit daty
            tmpTokenList->name = malloc(strlen(token)+1);
            if( tmpTokenList == NULL )
            {
                fprintf(stderr, "ERROR: Nepodarilo se alokovat pamet pro retezec v novem tokenu!\n");
                exit(ERR_MALLOC);
            }
            strcpy(tmpTokenList->name, token);
            tmpTokenList->next = NULL;
            // jestli duvod konce tokenu byl konec radku, tak koncime, jinak preskocit mezery a vynulovat pocitadlo znaku, bude asi dalsi token
            if(ch == '\n')
            {
                return tokenList;
            }
            else
            {
                skipWhitesFromFile(fp, &ch);
                i = 0;
            }
        }
        else
        {
            // jen dalsi znak do aktualne nacitaneho tokenu
            token[i++] = ch;
            if(i > 30)
            {
                fprintf(stderr, "ERROR: Prilis dlouhy prvek na radku!\n");
                return NULL;
            }
            ch = fgetc(fp);
        }
    }

    if(tokenList != NULL)
    {
        fprintf(stderr, "ERROR: Neocekavany konec souboru (radek neni ukonceny \\n)!\n");
    }

    return tokenList;
}

/** \brief processCommands zpracuje prikazy na radcich
 *
 * \param fileName je jmeno vstupniho souboru
 * \param lines je pocet radku vstupniho souboru
 * \return vraci 0 pri uspesnem zpracovani, jinak kod chyby v pripade neuspechu
 *
 */
int processCommands(TLine *line, int lines)
{
    char command[MAX_ITEM_LEN];

    for(int ln = 0; ln < lines; ln++)
    {
        if(line[ln].command != NULL)
        {
            strcpy(command, line[ln].command->name);
            int operand1 = line[ln].command->op[0];
            if(operand1 == 0 || operand1 > lines)
            {
                fprintf(stderr, STR_WRONG_PARAM_LINE, ln + 1);
                return ERR_CMD_LINE_NOEX;
            }
            int operand2 = line[ln].command->op[1];
            if(operand2 == 0 || operand2 > lines)
            {
                if(strInSetCmds2(command) == 1 || strInRelCmds3(command) == 1) // druhy operand nas zajima jen pokud mame prikaz pro 2 operandy
                {
                    fprintf(stderr, STR_WRONG_PARAM_LINE, ln + 1);
                    return ERR_CMD_LINE_NOEX;
                }
            }
            int operand3 = line[ln].command->op[2];
            if(operand3 == 0 || operand3 > lines)
            {
                if(strInRelCmds3(command) == 1) // treti operand nas zajima jen pokud mame prikaz pro 3 operandy
                {
                    fprintf(stderr, STR_WRONG_PARAM_LINE, ln + 1);
                    return ERR_CMD_LINE_NOEX;
                }
            }
            int operand4 = line[ln].command->op[3];
            if(strInSetCmds1(command) == 1)
            {
                if(line[operand1 - 1].content != 'S')
                {
                    fprintf(stderr, "ERROR: Prikaz pro mnozinu na radku %d ukazuje na radek, na kterem neni mnozina!\n", ln + 1);
                    return ERR_WRONG_LINE_TYPE;
                }
                if(operand3 > 0 || operand4 > 0)
                {
                    fprintf(stderr, "ERROR: Prilis mnoho operandu na radku %d!\n", ln + 1);
                    return ERR_WRONG_COMMAND;
                }
            }
            if(strInSetCmds2(command) == 1)
            {
                if(line[operand1 - 1].content != 'S' || line[operand2 - 1].content != 'S')
                {
                    fprintf(stderr, "ERROR: Prikaz pro mnozinu na radku %d ukazuje na radek, na kterem neni mnozina!\n", ln + 1);
                    return ERR_WRONG_LINE_TYPE;
                }
                if(operand4 > 0)
                {
                    fprintf(stderr, "ERROR: Prilis mnoho operandu na radku %d!\n", ln + 1);
                    return ERR_WRONG_COMMAND;
                }
            }
            if(strInRelCmds1(command) == 1)
            {
                if(line[operand1 - 1].content != 'R')
                {
                    fprintf(stderr, "ERROR: Prikaz pro relaci na radku %d ukazuje na radek, na kterem neni relace!\n", ln + 1);
                    return ERR_WRONG_LINE_TYPE;
                }
                if(operand3 > 0 || operand4 > 0)
                {
                    fprintf(stderr, "ERROR: Prilis mnoho operandu na radku %d!\n", ln + 1);
                    return ERR_WRONG_COMMAND;
                }
            }
            if(strInRelCmds3(command) == 1 && (line[operand1 - 1].content != 'R'  || line[operand2 - 1].content != 'S' || line[operand3 - 1].content != 'S'))
            {
                fprintf(stderr, "ERROR: Prikaz pro relaci na radku %d ukazuje na radek, na kterem neni relace!\n", ln + 1);
                return ERR_WRONG_LINE_TYPE;
            }
            // Set commands
            if(strcmp(command, "empty") == 0)
            {
                if(!cmdEmpty(line[operand1-1].set))
                {
                    JUMP(operand2);
                }
            }
            else if(strcmp(command, "card") == 0)
            {
                cmdCard(line[operand1-1].set);
            }
            else if(strcmp(command, "complement") == 0)
            {
                cmdComplement(line[operand1-1].set, line[UNIVERSUM_LINE - 1].set, &line[ln].set);
            }
            else if(strcmp(command, "union") == 0)
            {
                cmdUnion(line[operand1-1].set, line[operand2-1].set, &line[ln].set);
            }
            else if(strcmp(command, "intersect") == 0)
            {
                cmdIntersect(line[operand1-1].set, line[operand2-1].set, &line[ln].set);
            }
            else if(strcmp(command, "minus") == 0)
            {
                cmdMinus(line[operand1-1].set, line[operand2-1].set, &line[ln].set);
            }
            else if(strcmp(command, "subseteq") == 0)
            {
                if(!cmdSubseteq(line[operand1-1].set, line[operand2-1].set))
                {
                    JUMP(operand3);
                }
            }
            else if(strcmp(command, "subset") == 0)
            {
                if(!cmdSubset(line[operand1-1].set, line[operand2-1].set))
                {
                    JUMP(operand3);
                }
            }
            else if(strcmp(command, "equals") == 0)
            {
                if(!cmdEquals(line[operand1-1].set, line[operand2-1].set))
                {
                    JUMP(operand3);
                }
            }
            else if(strcmp(command, "select") == 0)
            {
                if(operand2==0)
                {
                    fprintf(stderr, STR_WRONG_PARAM_LINE, ln + 1);
                    return ERR_CMD_LINE_NOEX;
                }
                if(!cmdSelect(line[operand1-1].set))
                {
                    JUMP(operand2);
                }
            }
            // Relation commands
            else if(strcmp(command, "reflexive") == 0)
            {
                if(!cmdReflexive(line[operand1-1].relation, line[UNIVERSUM_LINE - 1].set))
                {
                    JUMP(operand2);
                }
            }
            else if(strcmp(command, "symmetric") == 0)
            {
                if(!cmdSymmetric(line[operand1-1].relation))
                {
                    JUMP(operand2)
                }
            }
            else if(strcmp(command, "antisymmetric") == 0)
            {
                if(!cmdAntisymmetric(line[operand1-1].relation))
                {
                    JUMP(operand2)
                }
            }
            else if(strcmp(command, "transitive") == 0)
            {
                if(!cmdTransitive(line[operand1-1].relation))
                {
                    JUMP(operand2)
                }
            }
            else if(strcmp(command, "function") == 0)
            {
                if(!cmdFunction(line[operand1-1].relation))
                {
                    JUMP(operand2)
                }
            }
            else if(strcmp(command, "domain") == 0)
            {
                cmdDomain(line[operand1-1].relation, &line[ln].set);
            }
            else if(strcmp(command, "codomain") == 0)
            {
                cmdCodomain(line[operand1-1].relation, &line[ln].set);
            }
            else if(strcmp(command, "injective") == 0)
            {
                if(!cmdInjective(line[operand1-1].relation, line[operand2-1].set, line[operand3-1].set))
                {
                    JUMP(operand4);
                }
            }
            else if(strcmp(command, "surjective") == 0)
            {
                if(!cmdSurjective(line[operand1-1].relation, line[operand2-1].set, line[operand3-1].set))
                {
                    JUMP(operand4);
                }
            }
            else if(strcmp(command, "bijective") == 0)
            {
                if(!cmdBijective(line[operand1-1].relation, line[operand2-1].set, line[operand3-1].set))
                {
                    JUMP(operand4);
                }
            }
            else if(strcmp(command, "closure_ref") == 0)
            {
                cmdClosureRef(line[operand1-1].relation, line[UNIVERSUM_LINE - 1].set, &line[ln].relation);
            }
            else if(strcmp(command, "closure_sym") == 0)
            {
                cmdClosureSym(line[operand1-1].relation, &line[ln].relation);
            }
            else if(strcmp(command, "closure_trans") == 0)
            {
                cmdClosureTrans(line[operand1-1].relation, &line[ln].relation);
            }
            else
            {
                fprintf(stderr, "ERROR: Neznamy prikaz!\n");
                return ERR_WRONG_COMMAND;
            }
        }
    }
    return ERR_OK;
}

/** \brief processFile zpracuje vstupni soubor podle zadani
 *
 * \param fileName je jmeno vstupniho souboru
 * \return vraci 0 pri uspesnem zpracovani, jinak kod chyby v pripade neuspechu
 *
 */
int processFile(char *fileName)
{
    FILE *fp;
    fp = fopen(fileName, "r");

    if(fp != NULL)
    {
        int lineNr = 0;
        int allocatedLines = LINES_ALLOC_SIZE;
        int err;
        int wasCommand = 0;

        // definujeme a vytvorime vynulovane pole s informacemi pro max 1000 (MAX_LINES) radku vstupniho souboru
        TLine *line;
        line = malloc(sizeof(TLine) * allocatedLines);
        if(line == NULL)
        {
            fprintf(stderr, "ERROR: Nepodarilo se alokovat dostatek pameti pro pole radku!\n");
            exit(ERR_MALLOC);
        }
        memset(line, 0, sizeof(TLine) * allocatedLines);

        // prochazi cely soubor
        do
        {
            TWordListItem *token = getLineFromFile(fp);
            if(token != NULL)
            {
                if(feof(fp))
                {
                    // neco se nacetlo, ale jsme na konci souboru, takze radek nebyl ukonceny \n a to je chyba
                    freeAllLines(line, lineNr);
                    return ERR_LAST_LINE_CHARS;
                }
                lineNr++;
                if(lineNr>allocatedLines)
                {
                    allocatedLines += LINES_ALLOC_SIZE;
                    line = realloc(line, sizeof(TLine) * allocatedLines);
                    if(line == NULL)
                    {
                        fprintf(stderr, "ERROR: Nepodarilo se alokovat dostatek pameti pro pole radku!\n");
                        exit(ERR_MALLOC);
                    }
                    memset(line + allocatedLines - LINES_ALLOC_SIZE, 0, sizeof(TLine) * LINES_ALLOC_SIZE);
                }
                //printf("Nacteny radek = "); printSet(token);
                err = processLine(token, lineNr, &wasCommand, line);
                freeWordList(token);
                if( err != 0 )
                {
                    freeAllLines(line, lineNr);
                    return err;
                }
            }
            else
            {
                if(feof(fp))
                {
                    // nic se nenactelo, ale jsme uz na konci souboru, tak to je OK
                    break;
                }
                fprintf(stderr, "Chyba na radku %d nebo je radek prazdny!", lineNr + 1);
                freeAllLines(line, lineNr);
                return ERR_EMPTY_LINE;
            }
        } while (!feof(fp));

        if(fclose(fp) != 0)
        {
            fprintf(stderr, "Nepodarilo se zavrit vstupni soubor!");
            freeAllLines(line, lineNr);
            return ERR_FCLOSE;
        }

        processCommands(line, lineNr);

        printf("\n... a to je konec programu\ntohle mame v pameti radku:\n\n");
        printAllLines(line, lineNr);

        freeAllLines(line, lineNr);
    }
    else
    {
        fprintf(stderr, "Nepodarilo se otevrit vstupni soubor! Chyba %s\n", strerror(errno));
        return ERR_FOPEN;
    }
    return ERR_OK;
}

/** \brief main nacte argument (jmeno souboru) z prikazoveho radku
 *  a zavola processFile() na zpracovani souboru
 *
 * \param argc je pocet parametru na prikazove radce (vc. vlastniho spustitelneho souboru)
 * \param argv[] jsou parametry prikazove radky
 * \return vraci 0 pri uspesnem zpracovani, jinak kod chyby v pripade neuspechu
 *
 */
int main(int argc, char *argv[])
{
    if(argc == 2)
    {
        time_t t;
        srand((unsigned) time(&t));
        int result = processFile(argv[1]);
        if(result != 0)
        {
            //fprintf(stderr, "Chyba pri zpracovani vstupniho souboru!");
            return result;
        }
    }
    else
    {
        fprintf(stderr, "Spatny pocet vstupnich argumentu. Spustte program jako ./setcal FILE!");
        return ERR_NR_ARG;
    }

    return ERR_OK;
}
