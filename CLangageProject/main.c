/*
 * @Author       : LeeJc02
 * @Email        : 18081278716@stumail.ysu.edu.cn
 * @Date         : 2023-11-04 14:06:09
 * @LastEditTime : 2023-12-08 16:42:43
 * @LastEditors  : LeeJc02
 * @Description  : YSU-计算机程序设计A三级项目:手机销售信息管理系统
 * @FilePath     : \C-Project\Project.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

// 用宏提高代码复用性, 更易于维护
#define PHONE_MODEL_LENGTH 40      // 存储  手机型号  字符串长度
#define PHONE_SYSTEM_LENGTH 40     // 存储  手机系统  字符串长度
#define PHONE_ORDER_CODE_LENGTH 20 // 存储  订单号    字符串长度
#define TIME_NOW_LENGTH 40         // 存储  销售时间  字符串长度
#define USER_NAME_LENGTH 40        // 存储  用户名    字符串长度
#define USER_NUMBER_LENGTH 20      // 存储  用户号码  字符串长度
#define LIST_NAME_LENGTH 40        // 存储  链表名    字符串长度
#define STAMP_HEAD printf("======================%s========================\n\n", list->listName);
#define STAMP_FORMAT printf("UserName\tUserPhone\tModel\tSystem\tTime\t\tOrderCode\n");
#define STAMP_LINE_ONE printf("---------------------------------------------------------------------------\n");
#define STAMP_LINE_TWO printf("===========================================================================\n");
#define STAMP_LINE_THREE printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
#define STAMP_VERSION printf("                                                   version: 1.0.0\n");
#define STAMP_AUTHOR printf("                                                   Author: LeeJc02 from YSU\n");

// 存储销售的手机参数和销售信息
typedef struct SaleData {
    char model[PHONE_MODEL_LENGTH];          // 存储手机型号
    char system[PHONE_SYSTEM_LENGTH];        // 存储手机系统
    char orderCode[PHONE_ORDER_CODE_LENGTH]; // 存储订单号
    char time[TIME_NOW_LENGTH];              // 存储销售时间
} SaleData;

// 将SaleData和下一级指针抽象为新的一个节点并存储在UserNode之下
typedef struct SaleNode
{
    SaleData data;
    struct SaleNode *bro;
} SaleNode;

// 存储销售的用户信息
typedef struct UserData
{
    char userName[USER_NAME_LENGTH];     // 存储用户名
    char userNumber[USER_NUMBER_LENGTH]; // 存储用户手机号
} UserData;

// 将UserData和下一级指针抽象为新的一个节点并存储在DataList之下作为主链表
typedef struct UserNode
{
    UserData data;         // 存储用户信息
    SaleNode *sale;        // 该UserNode下的销售信息子链表
    struct UserNode *next; // 指向下一个UserNode节点
} UserNode;

// 存储链表名链表头作为链表的开始
typedef struct DataList
{
    UserNode *head;                  // 指向主链表的头节点
    char listName[LIST_NAME_LENGTH]; // 存储链表名
} DataList;

// const用于构造函数内部方式误操作修改固定的数据
UserNode *createUser(const char *userName, const char *userPhone);                                         // 新建一个UserNode节点并初始化
SaleNode *createSale(const char *model, const char *system, int type, const char *Date, const char *Code); // 新建一个SaleNode节点并根据传入的type参数进行不同初始化
UserNode *searchUser(const DataList *list, const char *userPhone);                                         // 查找所查询的UserNode节点, 并返回该节点的前一个节点
SaleNode *searchSale(const UserNode *userNode, const char *orderCode);                                     // 查找所查询的SaleNode节点, 并返回该节点的前一个节点
DataList *readToFile(char *listName, const char *dest);                                                    // 从文件中读取数据并初始化为链表
void printNode(const UserNode *userNode, const SaleNode *saleNode);                                        // 打印当前节点数据
void printList(const DataList *list);                                                                      // 打印整条链表数据
void deleteUser(UserNode *before);                                                                         // 删除传入节点的下一个UserNode并释放内存
void deleteSale(SaleNode *before);                                                                         // 删除传入节点的下一个SaleNode并释放内存
int askJudge(int type);                                                                                    // 各种操作前询问是否执行, 确认返回为1, 取消返回为0
void initInter(DataList *list);                                                                            // 初始化系统主菜单
void addModule(DataList *list);                                                                            // 实现添加一个销售信息的模块
void searchModule(const DataList *list);                                                                   // 实现查询一个用户信息的模块
void deleteModule(DataList *list);                                                                         // 实现删除一个销售信息的模块
void changeModule(DataList *list);                                                                         // 实现修改一个销售信息的模块
void randCode(char *code);                                                                                 // 随机生成11位的订单号
void getTimeNow(char *time);                                                                               // 获取当前时间
void saveToFile(const DataList *list, const char *dest);                                                   // 保存数据并写入csv文件中
void freeList(DataList *list);                                                                             // 手动释放链表内存

int main()
{
    DataList *list = NULL;
    char listName[LIST_NAME_LENGTH] = "YSU-Mobile Phone Sales System"; // 链表名
    char fileName[LIST_NAME_LENGTH] = "saleData.csv";                  // 存储数据文件名
    list = readToFile(listName, fileName);                             // 从文件中读取存储的数据
    initInter(list);                                                   // 初始化用户界面
    saveToFile(list, fileName);                                        // 保存数据并写入文件中
    freeList(list);                                                    // 手动释放链表内存
    return 0;
}

/*
 * UserNode *createUser(char *, char *)
 * - 新建一个UserNode节点并初始化
 */
UserNode *createUser(const char *userName, const char *userPhone)
{
    UserNode *userNode = (UserNode *)malloc(sizeof(UserNode));
    memset(userNode, 0, sizeof(UserNode)); // 初始化该UserNode节点, 防止内存栈中残留数据造成数据影响, 下述所有memset函数同理
    strcpy(userNode->data.userName, userName);
    strcpy(userNode->data.userNumber, userPhone);
    userNode->sale = NULL;
    userNode->next = NULL;
    return userNode; // 返回所创造的该UserNode节点
}

/*
 * SaleNode *createSale(char *, char *, int, char *, char *)
 * - 新建一个SaleNode节点并根据传入的type参数进行不同初始化
 */
SaleNode *createSale(const char *model, const char *system, int type, const char *Date, const char *Code)
{
    char time[TIME_NOW_LENGTH];
    char code[PHONE_ORDER_CODE_LENGTH];
    memset(time, 0, sizeof(time));
    memset(code, 0, sizeof(code));

    if (!type) // 通过传入状态参数type的值, 判定是否需要生成时间和订单号
    {
        getTimeNow(time); // 表示新建销售订单, 获取当前时间
        randCode(code);   // 随机生成订单号
    }
    else
    {
        strcpy(time, Date); // 仅是修改该销售订单, 无需生成时间和订单号
        strcpy(code, Code);
    }

    SaleNode *saleNode = (SaleNode *)malloc(sizeof(SaleNode));
    memset(saleNode, 0, sizeof(SaleNode));
    strcpy(saleNode->data.orderCode, code);
    strcpy(saleNode->data.model, model);
    strcpy(saleNode->data.system, system);
    strcpy(saleNode->data.time, time);
    saleNode->bro = NULL;
    return saleNode; // 返回所创造的该SaleNode节点
}

/*
 * UserNode *searchUser(DataList *, char *)
 * - 查找所查询的UserNode节点, 并返回该节点的前一个节点
 * - 确保UserNode的节点数大于1
 */
UserNode *searchUser(const DataList *list, const char *userPhone)
{
    UserNode *userNode = list->head;
    while (userNode->next != NULL) // 遍历整个链表进行数据匹配
    {
        if (strcmp(userNode->next->data.userNumber, userPhone) == 0)
            return userNode; // 匹配成功则返回查询节点的上一个节点
        userNode = userNode->next;
    }
    return NULL; // 匹配失败则返回NULL表示查询为空
}

/*
 * SaleNode *searchSale(UserNode *, char *)
 * - 查找所查询的SaleNode节点, 并返回该节点的前一个节点
 * - 确保该UserNode下SaleNode的节点数大于1
 */
SaleNode *searchSale(const UserNode *userNode, const char *orderCode)
{
    SaleNode *saleNode = userNode->sale;
    while (saleNode->bro != NULL) // 遍历该UserNode下的所有SaleNode节点进行数据匹配
    {
        if (strcmp(saleNode->bro->data.orderCode, orderCode) == 0)
            return saleNode; // 匹配成功则返回查询节点的上一个节点
        saleNode = saleNode->bro;
    }
    return NULL; // 匹配失败则返回NULL表示查询为空
}

/*
 * DataList *readToFile(char *, char *)
 * - 从文件中读取数据并初始化为链表
 */
DataList *readToFile(char *listName, const char *dest)
{
    DataList *list = (DataList *)malloc(sizeof(DataList)); // 初始化链表
    memset(list, 0, sizeof(DataList));
    strcpy(list->listName, listName);
    list->head = NULL;
    UserNode *userNode = NULL;
    SaleNode *saleNode = NULL;

    FILE *file = fopen(dest, "r"); // 先用r只读模式, 若文件存在则读取
    if (file == NULL)              // 若文件打开失败则不存在该文件
    {
        file = fopen(dest, "w+"); // 用w+读写模式生成一个新的文件用于存储信息
        if (file == NULL)         // 若还是打开失败则报错
        {
            fprintf(stderr, " - File open failed!\n");
            return NULL;
        }
        return list;
    }

    char userName[USER_NAME_LENGTH];
    char userPhone[USER_NUMBER_LENGTH];
    char model[PHONE_MODEL_LENGTH];
    char system[PHONE_SYSTEM_LENGTH];
    char orderCode[PHONE_ORDER_CODE_LENGTH];
    char time[TIME_NOW_LENGTH];
    memset(userName, 0, USER_NAME_LENGTH); // 全是为了防止内存栈中残留数据造成数据影响
    memset(userPhone, 0, USER_NUMBER_LENGTH);
    memset(model, 0, PHONE_MODEL_LENGTH);
    memset(system, 0, PHONE_SYSTEM_LENGTH);
    memset(orderCode, 0, PHONE_ORDER_CODE_LENGTH);
    memset(time, 0, TIME_NOW_LENGTH);

    // 用fscanf函数进行文件读取操作, 读取到文件末尾时返回EOF
    while (fscanf(file, "%s %s %s %s %s %s", userName, userPhone, model, system, time, orderCode) != EOF)
    {
        UserNode *flag = NULL;
        if (list->head == NULL) // 通过链表是否为空判断是否为第一个节点进行新建节点操作
            flag = NULL;
        else if (list->head->next == NULL && strcmp(list->head->data.userNumber, userPhone) == 0) // 若链表只有一个节点且为一个用户的多个销售数据
            flag = list->head;
        else // 若链表有多个节点则进行查找是否是一个用户的多个销售信息
        {
            flag = searchUser(list, userPhone); // 此时链表节点数一定大于1, 查询是否存在该用户
            if (flag != NULL)                   // 如果返回为NULL则表示未查询到用户, 否则返回查询到的节点的上一个节点
                flag = flag->next;
        }

        if (flag == NULL) // 若不存在该用户则新建一个节点
        {
            if (list->head == NULL) // 判定是否为第一个节点
            {
                list->head = createUser(userName, userPhone);
                userNode = list->head;
            }
            else
            {
                userNode->next = createUser(userName, userPhone);
                userNode = userNode->next;
            }
            userNode->sale = createSale(model, system, 1, time, orderCode);
        }
        else // 若用户存在则在该用户下新建一个销售信息节点
        {
            saleNode = flag->sale;
            while (saleNode->bro != NULL)
                saleNode = saleNode->bro;
            saleNode->bro = createSale(model, system, 1, time, orderCode);
        }
    }

    if (feof(file)) // 判断是否读取到文件末尾, 若是则关闭文件
        fclose(file);
    return list;
}

/*
 * void printNode(UserNode *, SaleNode *)
 * - 打印当前节点数据
 */
void printNode(const UserNode *userNode, const SaleNode *saleNode)
{
    printf("%s\t\t%s\t%s\t%s\t%s\t%s\n", userNode->data.userName, userNode->data.userNumber, saleNode->data.model,
           saleNode->data.system, saleNode->data.time, saleNode->data.orderCode);
    STAMP_LINE_THREE
}

/*
 * void printList(DataList *)
 * - 打印整条链表数据
 */
void printList(const DataList *list)
{
    system("cls"); // 清屏操作
    STAMP_HEAD     // 界面初始化
        printf("                          All sale information!\n");
    STAMP_LINE_ONE

    if (list->head == NULL) // 空链表则警告并返回
        printf(" - [Warning] There is no information!\n");
    else
    {
        STAMP_FORMAT
        STAMP_LINE_THREE
        UserNode *userNode = list->head;
        SaleNode *saleNode = NULL;
        while (userNode != NULL) // 遍历所有主链表和子链表以打印所有数据
        {
            saleNode = userNode->sale;
            while (saleNode != NULL)
            {
                printNode(userNode, saleNode);
                saleNode = saleNode->bro;
            }
            userNode = userNode->next;
        }
        printf(" - [Notice] Print successfully!\n"); // 提示信息
    }

    printf(" - Please enter to return the main menu!\n"); // 返回主菜单提示信息
    getchar();
}

/*
 * void deleteUser(UserNode *)
 * - 删除传入节点的下一个UserNode并释放内存
 * - 确保UserNode节点数大于1
 */
void deleteUser(UserNode *before)
{
    UserNode *userNode = before->next;
    before->next = userNode->next;
    free(userNode);
}

/*
 * void saveToFile(DataList *, char *)
 * - 删除传入节点的下一个SaleNode并释放内存
 * - 确保该UserNode下SaleNode节点数大于1
 */
void deleteSale(SaleNode *before)
{
    SaleNode *saleNode = before->bro;
    before->bro = saleNode->bro;
    free(saleNode);
}

/*
 * int askJudge(int)
 * - 各种操作前询问是否执行, 确认返回为1, 取消返回为0
 */
int askJudge(int type)
{
    char choice; // select the type you are operating on
    switch (type)
    {
    case 0:
        printf(" - [Notice] Are you sure to add the information? [y/n]: ");
        break;
    case 1:
        printf(" - [Notice] Are you sure to delete the information? [y/n]: ");
        break;
    case 2:
        printf(" - [Notice] Are you sure to change the information? [y/n]: ");
        break;
    default:
        printf(" - [Warning] Please enter the correct number!\n");
        return -1; // 错误传参, 异常返回
    }

    scanf("%c", &choice);
    getchar(); // 清除scanf函数输入后参与在输入缓冲区的一个换行符, 下述所有scanf函数后的getchar函数同理
    if (choice == 'y')
        return 1;
    return 0;
}

/*
 * void initInter(DataList *)
 * - 初始化系统主菜单
 */
void initInter(DataList *list)
{
    while (1) // 循环使用主菜单, 直到用户退出并保存
    {
        system("cls"); // 清屏操作
        STAMP_HEAD     // 界面初始化
            printf("               Welcome to %s!\n", list->listName);
        STAMP_LINE_ONE

        char choice; // 操作选项
        printf(" - [a] Show all the information.\n");
        printf(" - [b] Add a new information.\n");
        printf(" - [c] Search the information.\n");
        printf(" - [d] Delete the information.\n");
        printf(" - [e] Change the information.\n");
        printf(" - [z] Save and exit the system.\n");

        STAMP_LINE_TWO
        STAMP_VERSION
        STAMP_AUTHOR
        STAMP_LINE_TWO

        printf(" - Please enter your selection: ");
        scanf("%c", &choice);
        getchar();
        switch (choice)
        {
        case 'a':
            printList(list); // 打印操作
            break;
        case 'b':
            addModule(list); // 添加操作
            break;
        case 'c':
            searchModule(list); // 查询操作
            break;
        case 'd':
            deleteModule(list); // 删除操作
            break;
        case 'e':
            changeModule(list); // 修改操作
            break;
        case 'z':
            printf(" - Thank you for using the %s!\n", list->listName);
            return; // 退出系统
        default:
            printf(" - [Warning] Enter error, please re-enter!\n"); // 错误输入提醒
            getchar();                                              // 停顿等待用户回车刷新界面重新输入, 下述所有非scanf函数后的getchar函数同理
            break;
        }
    }
}

/*
 * void addModule(DataList *)
 * - 实现添加一个销售信息的模块
 */
void addModule(DataList *list)
{
    system("cls"); // 清屏操作
    STAMP_HEAD     // 界面初始化
        printf("                       Add a new sale information!\n");
    STAMP_LINE_ONE

    char userName[USER_NAME_LENGTH];
    char userPhone[USER_NUMBER_LENGTH];
    char model[PHONE_MODEL_LENGTH];
    char system[PHONE_SYSTEM_LENGTH];
    memset(userName, 0, USER_NAME_LENGTH);
    memset(userPhone, 0, USER_NUMBER_LENGTH);
    memset(model, 0, PHONE_MODEL_LENGTH);
    memset(system, 0, PHONE_SYSTEM_LENGTH);

    printf(" - Please enter the user name: ");
    scanf("%s", userName);
    getchar();
    printf(" - Please enter the user phone: ");
    scanf("%s", userPhone);
    getchar();
    printf(" - Please enter the phone model: ");
    scanf("%s", model);
    getchar();
    printf(" - Please enter the phone system: ");
    scanf("%s", system);
    getchar();

    if (!askJudge(0)) // 确认操作
    {
        printf(" - [Warning] Operation canceled!\n");
        printf(" - Please enter to return the main menu!\n");
        getchar();
        return;
    }

    UserNode *userNode = NULL;
    if (list->head == NULL) // 若链表为空则新建一个节点
    {
        list->head = createUser(userName, userPhone);
        list->head->sale = createSale(model, system, 0, NULL, NULL);
    }
    else if (list->head->next == NULL && strcmp(list->head->data.userNumber, userPhone) == 0) // 若链表只有一个节点且为一个用户的多个销售数据
    {
        SaleNode *tempSale = list->head->sale;
        while (tempSale->bro != NULL)
            tempSale = tempSale->bro;
        tempSale->bro = createSale(model, system, 0, NULL, NULL);
    }
    else // 若链表有多个节点则进行查找是否是一个用户的多个销售信息
    {
        userNode = searchUser(list, userPhone);
        if (userNode == NULL) // 若不存在该用户则新建一个节点
        {
            UserNode *tempUser = list->head;
            while (tempUser->next != NULL) // 遍历DataList下的主链表
                tempUser = tempUser->next;
            tempUser->next = createUser(userName, userPhone);
            tempUser->next->sale = createSale(model, system, 0, NULL, NULL);
        }
        else // 若用户存在则在该用户下新建一个销售信息节点
        {
            userNode = userNode->next;
            SaleNode *tempSale = userNode->sale;
            while (tempSale->bro != NULL) // 遍历UserNode下的子链表
                tempSale = tempSale->bro;
            tempSale->bro = createSale(model, system, 0, NULL, NULL);
        }
    }

    printf(" - [Notice] Add successfully!\n");
    printf(" - Please enter to return the main menu!\n");
    getchar();
}

/*
 * void searchModule(DataList *)
 * - 实现查询一个用户信息的模块
 */
void searchModule(const DataList *list)
{
    system("cls"); // 清屏操作
    STAMP_HEAD     // 界面初始化
        printf("                      Search the sale information!\n");
    STAMP_LINE_ONE

    char userPhone[USER_NUMBER_LENGTH];
    printf(" - Please enter the user phone: ");
    scanf("%s", userPhone); // 输入用户手机号进行遍历查询
    getchar();

    UserNode *userNode = NULL;
    if (list->head == NULL) // 空链表则未查询到
        userNode = NULL;
    else if (list->head->next == NULL && strcmp(list->head->data.userNumber, userPhone) == 0) // 若链表只有一个节点且为一个用户的多个销售数据
        userNode = list->head;
    else // 若链表有多个节点则进行查找是否是一个用户的多个销售信息
    {
        userNode = searchUser(list, userPhone);
        if (userNode != NULL)
            userNode = userNode->next;
    }

    if (userNode == NULL) // 若不存在该用户则警告并返回
    {
        printf(" - [Warning] The information does not exist!\n");
        printf(" - Please enter to return the main menu!\n");
        getchar();
        return;
    }

    SaleNode *saleNode = userNode->sale;
    STAMP_LINE_ONE
    STAMP_FORMAT
    STAMP_LINE_THREE

    while (saleNode != NULL) // 遍历该用户下的所有销售信息并打印
    {
        printNode(userNode, saleNode);
        saleNode = saleNode->bro;
    }

    printf(" - [Notice] Search successfully!\n");
    printf(" - Please enter to return the main menu!\n");
    getchar();
}

/*
 * void deleteModule(DataList *)
 * - 实现删除一个销售信息的模块
 */
void deleteModule(DataList *list)
{
    system("cls"); // 清屏操作
    STAMP_HEAD     // 界面初始化
        printf("                 Delete the sale information!\n");
    STAMP_LINE_ONE

    char userPhone[USER_NUMBER_LENGTH];
    char orderCode[PHONE_ORDER_CODE_LENGTH];
    memset(userPhone, 0, USER_NUMBER_LENGTH);
    memset(orderCode, 0, PHONE_ORDER_CODE_LENGTH);
    printf(" - Please enter the user phone: ");
    scanf("%s", userPhone); // 输入用户手机号进行遍历查询
    getchar();

    UserNode *userNode = NULL;
    UserNode *userBefore = NULL; // 用于记录查询节点的上一个节点
    if (list->head == NULL)      // 空链表则未查询到
        userNode = NULL;
    else if (list->head->next == NULL && strcmp(list->head->data.userNumber, userPhone) == 0) // 若链表只有一个节点且为一个用户的多个销售数据
        userNode = list->head;
    else // 若链表有多个节点则进行查找是否是一个用户的多个销售信息
    {
        userBefore = searchUser(list, userPhone);
        if (userBefore != NULL)
            userNode = userBefore->next;
    }

    if (userNode == NULL) // 若不存在该用户则警告并返回
    {
        printf(" - [Warning] The information does not exist!\n");
        printf(" - Please enter to return the main menu!\n");
        getchar();
        return;
    }

    SaleNode *saleNode = userNode->sale;
    STAMP_FORMAT
    STAMP_LINE_THREE
    while (saleNode != NULL) // 遍历该用户下的所有销售信息并打印
    {
        printNode(userNode, saleNode);
        saleNode = saleNode->bro;
    }

    if (userNode->sale->bro == NULL) // 若该用户只有一个销售信息则直接删除该用户节点 (UserNode下的SaleNode一定大于0)
    {
        if (!askJudge(1)) // 确认操作
        {
            printf(" - [Warning] Operation canceled!\n");
            printf(" - Please enter to return the main menu!\n");
            getchar();
            return;
        }

        if (userBefore == NULL) // 若该用户为第一个节点则直接删除
        {
            list->head = NULL;
            free(userNode);
        }
        else // 若该用户不是第一个节点则传入前一个节点删除该节点
            deleteUser(userBefore);
    }
    else // 若该用户有多个销售信息则输入订单号进行二次查询
    {
        printf(" - Please enter the order code: ");
        scanf("%s", orderCode);
        getchar();

        if (strcmp(orderCode, userNode->sale->data.orderCode) == 0) // 若订单号与第一个SaleNode匹配
            saleNode = userNode->sale;
        else // 若订单号与第一个SaleNode不匹配则进行二次查询
            saleNode = searchSale(userNode, orderCode);

        if (saleNode == NULL) // 若不存在该销售信息则警告并返回
        {
            printf(" - [Warning] The information does not exist!\n");
            printf(" - Please enter to return the main menu!\n");
            getchar();
            return;
        }

        STAMP_LINE_ONE
        STAMP_FORMAT
        STAMP_LINE_THREE
        printNode(userNode, saleNode); // 打印所查询到的销售信息

        if (!askJudge(1)) // 确认操作
        {
            printf(" - [Warning] Operation canceled!\n");
            printf(" - Please enter to return the main menu!\n");
            getchar();
            return;
        }

        if (strcmp(orderCode, saleNode->data.orderCode) == 0) // 若订单号与第一个SaleNode匹配
        {
            userNode->sale = saleNode->bro;
            free(saleNode);
        }
        else // 若订单号与第一个SaleNode不匹配则传入前一个节点删除该节点
            deleteSale(saleNode);
    }

    printf(" - [Notice] Delete successfully!\n");
    printf(" - Please enter to return the main menu!\n");
    getchar();
}

/*
 * void changeModule(DataList *)
 * - 实现修改一个销售信息的模块
 */
void changeModule(DataList *list)
{
    system("cls"); // 清屏操作
    STAMP_HEAD     // 界面初始化
        printf("                       Change the sale information!\n");
    STAMP_LINE_ONE

    char orderCode[PHONE_ORDER_CODE_LENGTH];
    char userName[USER_NAME_LENGTH];
    char userPhone[USER_NUMBER_LENGTH];
    char model[PHONE_MODEL_LENGTH];
    char system[PHONE_SYSTEM_LENGTH];
    memset(userName, 0, USER_NAME_LENGTH);
    memset(userPhone, 0, USER_NUMBER_LENGTH);
    memset(model, 0, PHONE_MODEL_LENGTH);
    memset(system, 0, PHONE_SYSTEM_LENGTH);
    memset(orderCode, 0, PHONE_ORDER_CODE_LENGTH);

    printf(" - Please enter the user phone: ");
    scanf("%s", userPhone); // 输入用户手机号进行遍历查询
    getchar();
    UserNode *userNode = NULL;

    if (list->head == NULL) // 空链表则未查询到
        userNode = NULL;
    else if (list->head->next == NULL && strcmp(list->head->data.userNumber, userPhone) == 0) // 若链表只有一个节点且为一个用户的多个销售数据
        userNode = list->head;
    else // 若链表有多个节点则进行查找是否是一个用户的多个销售信息
    {
        userNode = searchUser(list, userPhone);
        if (userNode != NULL)
            userNode = userNode->next;
    }

    if (userNode == NULL) // 若不存在该用户则警告并返回
    {
        printf(" - [Warning] The information does not exist!\n");
        printf(" - Please enter to return the main menu!\n");
        getchar();
        return;
    }

    SaleNode *saleNode = userNode->sale;
    STAMP_FORMAT
    STAMP_LINE_THREE
    while (saleNode != NULL) // 遍历该用户下的所有销售信息并打印
    {
        printNode(userNode, saleNode);
        saleNode = saleNode->bro;
    } // as with the deleteModule

    if (userNode->sale->bro == NULL) // 若该用户只有一个销售信息则直接修改该用户节点 (UserNode下的SaleNode一定大于0)
    {
        printf(" - Please enter the user name: "); // 重新输入节点信息
        scanf("%s", userName);
        getchar();
        printf(" - Please enter the phone model: ");
        scanf("%s", model);
        getchar();
        printf(" - Please enter the phone system: ");
        scanf("%s", system);
        getchar();

        if (!askJudge(2)) // 确认操作
        {
            printf(" - [Warning] Operation canceled!\n");
            printf(" - Please enter to return the main menu!\n");
            getchar();
            return;
        }

        char *readDate = (char *)malloc(sizeof(char) * TIME_NOW_LENGTH);
        getTimeNow(readDate);                      // 获取当前时间
        strcpy(userNode->data.userName, userName); // 覆盖性修改节点信息
        strcpy(userNode->sale->data.model, model);
        strcpy(userNode->sale->data.system, system);
        strcpy(userNode->sale->data.time, readDate);
    }
    else // 若该用户有多个销售信息则输入订单号进行二次查询
    {
        printf(" - Please enter the order code: ");
        scanf("%s", orderCode);
        getchar();

        if (strcmp(orderCode, userNode->sale->data.orderCode) == 0) // 若订单号与第一个SaleNode匹配
            saleNode = userNode->sale;
        else // 若订单号与第一个SaleNode不匹配则进行二次查询
            saleNode = searchSale(userNode, orderCode)->bro;

        if (saleNode == NULL) // 若不存在该销售信息则警告并返回
        {
            printf(" - [Warning] The information does not exist!\n");
            printf(" - Please enter to return the main menu!\n");
            getchar();
            return;
        }

        STAMP_LINE_ONE
        STAMP_FORMAT
        STAMP_LINE_THREE
        printNode(userNode, saleNode); // 打印所查询到的销售信息

        printf(" - Please enter the new user name: "); // 重新输入节点信息
        scanf("%s", userName);
        getchar();
        printf(" - Please enter the new phone model: ");
        scanf("%s", model);
        getchar();
        printf(" - Please enter the new phone system: ");
        scanf("%s", system);
        getchar();

        if (!askJudge(2)) // 确认操作
        {
            printf(" - [Warning] Operation canceled!\n");
            printf(" - Please enter to return the main menu!\n");
            getchar();
            return;
        }

        char *readDate = (char *)malloc(sizeof(char) * TIME_NOW_LENGTH);
        memset(readDate, 0, TIME_NOW_LENGTH);
        getTimeNow(readDate);                      // 获取当前时间
        strcpy(userNode->data.userName, userName); // 覆盖性修改节点信息
        strcpy(userNode->data.userNumber, userPhone);
        strcpy(saleNode->data.model, model);
        strcpy(saleNode->data.system, system);
        strcpy(userNode->sale->data.time, readDate);
    }

    printf(" - [Notice] Change successfully!\n");
    printf(" - Please enter to return the main menu!\n");
    getchar();
}

/*
 * char *randCode()
 * - 随机生成11位的订单号
 */
void randCode(char *code)
{
    struct timeval tv;                           // sys/time.h中自带的结构体, 存储毫秒级的时间间隔
    gettimeofday(&tv, NULL);                     // 系统调用初始化tv结构体
    srand(tv.tv_sec * 1000 + tv.tv_usec / 1000); // 初始化随机种子, 比srand((unsigned)time(NULL))更加精确
    for (int i = 0; i < 11; ++i)
        code[i] = rand() % 10 + '0'; // 生成精度不高的伪随机数
}

/*
 * void getTimeNow(char *)
 * - 获取当前时间
 */
void getTimeNow(char *Time)
{
    time_t current_time;
    struct tm *time_info;                 // sys/time.h中自带的结构体, 用于存储当前所有单位的时间
    time(&current_time);                  // 获取time_t类型的时间戳
    time_info = localtime(&current_time); // 将时间戳转换为当前时间
    snprintf(Time, TIME_NOW_LENGTH, "%d-%02d-%02d", time_info->tm_year + 1900, time_info->tm_mon + 1,
             time_info->tm_mday); // 将结构体中的当前时间转换为年月日的字符串
}

/*
 * void saveToFile(DataList *, char *)
 * - 保存数据并写入csv文件中
 */
void saveToFile(const DataList *list, const char *dest)
{
    FILE *file = fopen(dest, "w+"); // 用w+读写模式进行文件写入
    if (file == NULL)               // 若文件打开失败则报错
    {
        printf(" - File open failed!\n");
        return;
    }

    UserNode *userNode = list->head;
    SaleNode *saleNode = NULL;
    while (userNode != NULL) // 遍历所有主链表和子链表以写入所有数据
    {
        saleNode = userNode->sale;
        while (saleNode != NULL)
        {
            // 用fprintf函数进行文件写入操作, 写入到文件末尾时返回EOF
            fprintf(file, "%s %s %s %s %s %s\n", userNode->data.userName, userNode->data.userNumber,
                    saleNode->data.model, saleNode->data.system, saleNode->data.time, saleNode->data.orderCode);
            saleNode = saleNode->bro; // write data to a file
        }
        userNode = userNode->next;
    }

    if (feof(file)) // 判断是否写入到文件末尾, 若是则关闭文件
        fclose(file);
}

/*
 * void freeList(DataList *)
 * - 手动释放所有链表节点的内存
 */
void freeList(DataList *list)
{
    UserNode *tempUser = list->head;
    if (tempUser == NULL) // 空链表则直接释放链表节点内存
    {
        free(list);
        return;
    }

    while (tempUser != NULL) // 遍历所有主链表的UserNode节点并释放内存
    {
        SaleNode *tempSale = tempUser->sale;
        while (tempSale != NULL) // 遍历所有子链表的SaleNode节点并释放内存
        {
            SaleNode *tempNextSale = tempSale->bro; // 先获取下一级节点地址
            free(tempSale);
            tempSale = tempNextSale;
        }

        UserNode *tempNextUser = tempUser->next; // 先获取下一级节点地址
        free(tempUser);
        tempUser = tempNextUser;
    }
    free(list); // 最后释放链表节点内存
}