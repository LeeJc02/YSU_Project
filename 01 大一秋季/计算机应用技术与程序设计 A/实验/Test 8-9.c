/*
 * @Author: JC 407981034@qq.com
 * @Date: 2023-10-09 20:09:54
 * @LastEditTime: 2023-10-20 09:59:01
 * @LastEditors: JC
 * @Description: 会员管理信息系统之会员信息的输入、输出
 * @FilePath: 实验八/九
 */

#include <stdio.h>
#include <stdlib.h>/*调用system函数*/
#include <malloc.h>/*动态分配内存*/
#include <string.h>

/*使用宏定义, 提高代码复用*/
#define PERSON_NAME_LENGTH 21     /*用户名字长度*/
#define PERSON_ID_LENGTH 18       /*用户ID长度*/
#define PERSON_NICKNAME_LENGTH 21 /*用户昵称长度*/
#define PRINT_HEAD printf("=========== YSU-VIP Management System ===========\n");
#define PRINT_LINE_ONE printf("=================================================\n");
#define PRINT_LINE_TWO printf("-------------------------------------------------\n");
#define PRINT_LINE_THREE printf("+++++++++++++++++++++++++++++++++++++++++++++++++\n");


/*VIP节点:用于存储一个用户的信息*/
typedef struct VIP{
    char name[PERSON_NAME_LENGTH];         /*用户名字*/
    char id[PERSON_ID_LENGTH];             /*用户ID*/
    char nickname[PERSON_NICKNAME_LENGTH]; /*用户昵称*/
} VIP;

/*VIPNode节点:用于存储一个VIP节点和下一级节点指针*/
typedef struct VIPNode{
    VIP item;             /*存储该VIPNode节点的用户信息*/
    struct VIPNode *next; /*作为链表节点指向下一节点的尾指针*/
} VIPNode;

/*VIPList节点:用于存储VIPNode初始头节点*/
typedef struct VIPList{
    int length;    /*链表长度*/
    VIPNode *head; /*指向不存储数据的头节点,作为链表的开始*/
} VIPList;

void Init();               /*初始化用户界面*/
void AddVIP();             /*添加用户信息*/
void PrintVIP();           /*输出用户信息*/
void SearchVIP();          /*查找用户信息*/
void ModifyVIP();          /*修改用户信息*/
void DeleteVIP();          /*删除用户信息*/
VIPList *vipSystem = NULL; /*全局变量, 用于存储VIPList初始头节点*/


int main() {
    vipSystem = (VIPList *)malloc(sizeof(VIPList));
    VIPNode *p = (VIPNode *)malloc(sizeof(VIPNode));
    vipSystem->head = p; 
    p->next = NULL;        /*指向一个不存储数据的头节点作为链表的开始*/
    vipSystem->length = 0; /*初始化链表长度为0*/
    Init();                /*初始化用户界面,打开系统*/
    return 0;
}


/**
 * @description: 初始化用户界面函数
 * @return {*}
 */
void Init() {                
    system("cls"); /*清屏上一个函数残留界面*/
    PRINT_HEAD
    printf("     Welcome to YSU-VIP Management System!\n");
    PRINT_LINE_TWO
    printf(" - [a] Add a new user information\n");
    printf(" - [b] Show all user information\n");
    printf(" - [c] Search for user information\n");
    printf(" - [d] Modify user information\n");
    printf(" - [e] Delete user information\n");
    printf(" - [z] Exit this system\n");
    PRINT_LINE_ONE
    printf("                             version: 1.0.0\n");
    printf("                             by: LeeJc02 from YSU\n");
    PRINT_LINE_ONE

    char choice;
    do { /*输入其他将视为无效输入,需重复输入*/
        printf(" - Please enter your selection:");
        scanf("%c", &choice);
        getchar(); /*所有的scanf下的getchar都是为了清除缓冲区的回车符, 防止重复录入出现覆盖*/
        switch (choice) {
        case 'a':   AddVIP();       break;  /*添加用户信息*/
        case 'b':   PrintVIP();     break;  /*输出用户信息*/
        case 'c':   SearchVIP();    break;  /*查找用户信息*/
        case 'd':   ModifyVIP();    break;  /*修改用户信息*/
        case 'e':   DeleteVIP();    break;  /*删除用户信息*/
        case 'z':   printf(" - Exit this system\n");
                    return;                 /*退出Init函数, 表明退出系统, 程序结束*/
        default:    printf(" - [Warning]: Input error, please re-enter!\n");
        }
    } while(!(choice>='a' && choice<='e') && choice!='z');
}


/**
 * @description: 添加用户信息函数
 * @return {*}
 */
void AddVIP() {                 
    system("cls"); /*清屏上一个函数残留界面*/
    VIPNode *p = (VIPNode *)malloc(sizeof(VIPNode));
    p->next = NULL; /*创建节点并录入信息, 以尾指针指向NULL作为链表的结尾*/
    PRINT_HEAD
    printf("          Add a new user information!\n");
    PRINT_LINE_TWO

    printf(" - Please enter the user's name:");
    scanf("%s", p->item.name); /*录入新的用户信息进行覆盖*/
    getchar();
    printf(" - Please enter the user's ID:");
    scanf("%s", p->item.id);
    getchar();
    printf(" - Please enter the user's nickname:");
    scanf("%s", p->item.nickname);
    getchar();

    VIPNode *q = (VIPNode *)malloc(sizeof(VIPNode));
    q = vipSystem->head; /*由头节点进行遍历链表*/
    while (q->next != NULL)
        q = q->next; /*找到链表的尾节点,在尾节点进行数据添加*/
    q->next = p;
    printf(" - Add successfully!\n");
    vipSystem->length++; /*添加节点成功, 链表长度+1*/

    printf(" - Press enter to return to the main menu!\n");
    getchar();
    Init(); /*重复调用, 回到主菜单*/
}


/**
 * @description: 输出所有的用户信息
 * @return {*}
 */
void PrintVIP() {                  
    system("cls");              /*清屏上一个函数残留界面*/
    PRINT_HEAD
    printf("           All user information\n");
    PRINT_LINE_TWO
    VIPNode *p = (VIPNode *)malloc(sizeof(VIPNode));
    p = vipSystem->head->next;  /*从头节点下的第一个数据节点开始遍历*/

    printf(" - Order\tName\tID\t\tNickname\n");
    int order = 1;              /*order用于链表计数*/
    while (p != NULL) {         /*遍历链表进行输出所有节点*/
        PRINT_LINE_TWO
        printf(" - %-3d\t\t%s\t%s\t\t%s\n", order++, p->item.name, p->item.id, p->item.nickname);
        p = p->next;
    }
    PRINT_LINE_THREE
    printf(" - Press any key to return to the main menu!\n");
    getchar();
    Init(); /*重复调用, 回到主菜单*/
}


/**
 * @description: 查找用户信息
 * @return {*}
 */
void SearchVIP() {                 
    system("cls"); /*清屏上一个函数残留界面*/
    PRINT_HEAD
    printf("         Search for user information\n");
    PRINT_LINE_TWO
    VIPNode *p = (VIPNode *)malloc(sizeof(VIPNode));
    p = vipSystem->head->next;

    printf(" - Please enter the user's name:");
    char name[PERSON_NAME_LENGTH];
    scanf("%s", name);
    getchar();

    int order = 1;
    while (p != NULL) { /*遍历链表进行查找*/
        if (strcmp(p->item.name, name) == 0) { /*匹配username,匹配成功输出该节点信息*/
            PRINT_LINE_THREE
            printf(" - Order\tName\tID\t\tNickname\n");
            PRINT_LINE_THREE
            printf(" - %-3d\t\t%s\t%s\t\t%s\n", order++, p->item.name, p->item.id, p->item.nickname);
             PRINT_LINE_ONE
            break;
        }       
        p = p->next;
    }

    if (p == NULL) { /*p遍历到NULL则说明未找到则输出提示信息*/
        printf(" - [Warning]: No such user information!\n"); 
        char choice;
        do {
            printf(" - [y] Search user information continue\n");
            printf(" - [n] Return to the main menu\n");
            PRINT_LINE_TWO
            printf(" - Please enter your selection:");
            scanf("%c", &choice);
            getchar();
            if (choice == 'n')          break;
            else if (choice == 'y')     SearchVIP(); /*重新调用SearchVIP函数, 以便继续查找*/
            else    printf(" - [Warning]: Input error, please re-enter!\n");
        } while (choice != 'y' && choice != 'n');
    }

    printf(" - Press any key to return to the main menu!\n");
    getchar();
    Init(); /*重复调用, 回到主菜单*/
}


/**
 * @description: 修改用户信息
 * @return {*}
 */
void ModifyVIP() {               
    system("cls"); /*清屏上一个函数残留界面*/
    PRINT_HEAD
    printf("             Modify user information\n");
    PRINT_LINE_TWO
    VIPNode *p = (VIPNode *)malloc(sizeof(VIPNode));
    p = vipSystem->head->next;

     printf(" - Please enter the user's name:");
    char name[PERSON_NAME_LENGTH];
    scanf("%s", name);
    getchar();
    
    int order = 1;
    while (p != NULL) { /*同SearchVIP函数, 遍历链表进行查找并进行数据修改*/
        if (strcmp(p->item.name, name) == 0) {
            PRINT_LINE_THREE
            printf(" - Order\tName\tID\t\tNickname\n");
            PRINT_LINE_THREE
            printf(" - %-3d\t\t%s\t%s\t\t%s\n", order++, p->item.name, p->item.id, p->item.nickname);
            PRINT_LINE_ONE /*查找到数据先进行输出, 再进行覆盖性输入*/

            printf(" - Please enter the user's new name:");
            scanf("%s", p->item.name);
            getchar();
            printf(" - Please enter the user's new ID:");
            scanf("%s", p->item.id);
            getchar();
            printf(" - Please enter the user's new nickname:");
            scanf("%s", p->item.nickname);
            getchar();
            printf(" - Modify successfully!\n");
            break; /*找到则跳出循环*/
        }       
        p = p->next;
    }

    if (p == NULL) { /*未能找到则进行二次操作*/
        printf(" - [Warning]: No such user information!\n"); 
        char choice;
        do {
            printf(" - [y] Search user information continue\n");
            printf(" - [n] Return to the main menu\n");
            PRINT_LINE_TWO
            printf(" - Please enter your selection:");
            scanf("%c", &choice);
            getchar();
            if (choice == 'n')          break;
            else if (choice == 'y')     ModifyVIP(); /*重新调用ModifyVIP函数, 以便继续查找*/
            else    printf(" - [Warning]: Input error, please re-enter!\n");
        } while (choice != 'y' && choice != 'n');        
    }

    printf(" - Press enter to return to the main menu!\n");
    getchar();
    Init(); /*重复调用, 回到主菜单*/
}


/**
 * @description: 删除用户信息
 * @return {*}
 */
void DeleteVIP() {                  
    system("cls"); /*清屏上一个函数残留界面*/
    PRINT_HEAD
    printf("             Delete user information\n");
    PRINT_LINE_TWO
    printf(" - Please enter the user's name:");

    char name[PERSON_NAME_LENGTH];
    scanf("%s", name);
    getchar();
    VIPNode *p = (VIPNode *)malloc(sizeof(VIPNode));
    VIPNode *q = (VIPNode *)malloc(sizeof(VIPNode));
    p = vipSystem->head->next;
    q = vipSystem->head; /*q用于记录p的前一个节点, 以便删除p节点*/

    int order = 1;
    while (p != NULL) {
        if (strcmp(p->item.name, name) == 0) {
            PRINT_LINE_THREE
            printf(" - Order\tName\tID\t\tNickname\n");
            PRINT_LINE_THREE
            printf(" - %-3d\t\t%s\t%s\t\t%s\n", order++, p->item.name, p->item.id, p->item.nickname);
            PRINT_LINE_ONE /*同SearchVIP函数, 对数据进行遍历查找并打印*/

            printf(" - Are you sure you want to delete this user information?\n");
            printf(" - [y] Yes, I'm sure!\n");
            printf(" - [n] No, I'm not sure!\n");

            char choice;
            do {
                printf(" - Please enter your selection:");
                scanf("%c", &choice);
                getchar();
                switch (choice) {
                case 'y':
                    q->next = p->next; /*删除p节点*/
                    printf(" - Delete successfully!\n");
                    vipSystem->length--;
                    break;
                case 'n':   break;
                default:    printf(" - [Warning]: Input error, please re-enter!\n");
                }
            } while (choice != 'y' && choice != 'n');
            break;
        }

        p = p->next;
        q = q->next;
    }

    if (p == NULL) {
        printf(" - [Waning]: No such user information!\n"); /*未找到则输出提示信息*/
        char choice;
        do {
            printf(" - [y] Search user information continue\n");
            printf(" - [n] Return to the main menu\n");
            PRINT_LINE_TWO
            printf(" - Please enter your selection:");
            scanf("%c", &choice);
            getchar();
            if (choice == 'n')          break;
            else if (choice == 'y')     DeleteVIP(); /*重新调用DeleteVIP函数, 以便继续查找*/
            else    printf(" - [Warning]: Input error, please re-enter!\n");
        } while (choice != 'y' && choice != 'n');
    }

    printf(" - Press enter to return to the main menu!\n");
    getchar();
    Init(); /*重复调用, 回到主菜单*/
}