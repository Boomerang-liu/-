// ============================================================
// 客户消费积分管理系统
// 功能：客户管理、消费积分计算、折扣优惠管理
// 数据结构：单链表 + 二进制文件存储
// 作者：课程设计
// 日期：2026-07-06
// ============================================================

#include <stdio.h>      // 标准输入输出库（printf, scanf, fopen等）
#include <stdlib.h>     // 标准库（malloc, free, system等）
#include <string.h>     // 字符串处理库（strcpy, strcmp等）
#include <time.h>       // 时间库（用于记录消费时间）

#define FILENAME "customers.dat"  // 数据存储文件名（二进制文件）

// ============================================================
// 客户结构体定义
// 用于存储单个客户的所有信息
// ============================================================
typedef struct Customer {
    int id;                     // 客户编号（主键，自动递增）
    char name[20];              // 客户姓名
    char phone[15];             // 联系电话
    double totalAmount;         // 累计消费总金额
    int points;                 // 当前积分（1元=1积分）
    double discount;            // 折扣率（如：0.95表示95折）
    struct Customer *next;      // 链表指针，指向下一个客户节点
} Customer;

// ============================================================
// 函数声明（提前声明所有函数，方便调用）
// ============================================================
void welcome();                 // 显示主菜单
void loadData(Customer **head); // 从文件加载数据到链表
void saveData(Customer *head);  // 将链表数据保存到文件
void addCustomer(Customer **head);      // 添加新客户
void modifyCustomer(Customer *head);    // 修改客户信息
void deleteCustomer(Customer **head);   // 删除客户
void searchCustomer(Customer *head);    // 查询客户信息
void consume(Customer *head);           // 消费结算（更新积分和折扣）
void updatePointsAndDiscount(Customer *c, double amount); // 更新积分和折扣
void displayAll(Customer *head);        // 显示所有客户
void freeList(Customer *head);          // 释放链表内存
int getNextId(Customer *head);          // 获取下一个可用ID
void displayDiscountRule();             // 显示折扣规则

// ============================================================
// 主函数 - 程序入口
// ============================================================
int main() {
    Customer *head = NULL;   // 链表头指针，初始为空
    
    // 1. 程序启动时，从文件加载已有数据
    loadData(&head);
    
    int choice;  // 存储用户菜单选择
    
    // 2. 主循环：显示菜单并处理用户操作，直到选择退出
    do {
        welcome();              // 显示主菜单
        printf("请选择操作: ");
        scanf("%d", &choice);   // 读取用户选择
        getchar();              // 消耗回车符，防止影响后续输入
        
        // 3. 根据用户选择执行对应功能
        switch(choice) {
            case 1: 
                addCustomer(&head);   // 添加客户
                break;
            case 2: 
                modifyCustomer(head); // 修改客户信息
                break;
            case 3: 
                deleteCustomer(&head);// 删除客户
                break;
            case 4: 
                searchCustomer(head); // 查询客户
                break;
            case 5: 
                consume(head);        // 消费结算
                break;
            case 6: 
                displayAll(head);     // 显示所有客户
                break;
            case 7:
                displayDiscountRule();// 显示折扣规则
                break;
            case 0:
                // 4. 退出前保存数据并释放内存
                saveData(head);
                freeList(head);
                printf("数据已保存，感谢使用！\n");
                break;
            default: 
                printf("输入有误，请重新选择！\n");
        }
    } while(choice != 0);  // 选择0时退出循环
    
    return 0;  // 程序正常结束
}

// ============================================================
// 显示主菜单界面
// ============================================================
void welcome() {
    system("cls");  // 清屏（Windows系统）
    // system("clear"); // Linux/Mac系统使用此命令
    
    printf("===================================\n");
    printf("  客户消费积分管理系统\n");
    printf("===================================\n");
    printf("1. 添加客户\n");
    printf("2. 修改客户信息\n");
    printf("3. 删除客户\n");
    printf("4. 查询客户\n");
    printf("5. 消费结算（积分更新）\n");
    printf("6. 显示所有客户\n");
    printf("7. 查看折扣规则\n");
    printf("0. 退出系统\n");
    printf("===================================\n");
}

// ============================================================
// 显示折扣规则说明
// ============================================================
void displayDiscountRule() {
    printf("\n========= 折扣规则 =========\n");
    printf("积分 0-500     ：无折扣（100%%）\n");
    printf("积分 501-1000  ：98折（0.98）\n");
    printf("积分 1001-2000 ：95折（0.95）\n");
    printf("积分 2001-5000 ：9折（0.90）\n");
    printf("积分 5000以上  ：85折（0.85）\n");
    printf("============================\n");
    printf("说明：每消费1元积1分\n");
    system("pause");
}

// ============================================================
// 从二进制文件加载数据到链表
// 参数：head - 链表头指针的地址（双重指针）
// ============================================================
void loadData(Customer **head) {
    FILE *fp = fopen(FILENAME, "rb");  // 以二进制读模式打开文件
    if (!fp) {  // 如果文件不存在或打开失败，直接返回
        return;
    }
    
    Customer temp;  // 临时变量，用于读取每个客户数据
    // 循环读取文件中的每个客户记录
    // fread返回值：成功读取的记录数，这里每次读1个
    while (fread(&temp, sizeof(Customer), 1, fp)) {
        // 为新客户分配内存
        Customer *new = (Customer*)malloc(sizeof(Customer));
        *new = temp;          // 复制数据到新节点
        new->next = *head;    // 头插法：新节点指向原头节点
        *head = new;          // 更新头指针为新节点
    }
    
    fclose(fp);  // 关闭文件
}

// ============================================================
// 将链表数据保存到二进制文件
// 参数：head - 链表头指针
// ============================================================
void saveData(Customer *head) {
    FILE *fp = fopen(FILENAME, "wb");  // 以二进制写模式打开文件
    if (!fp) {  // 文件打开失败
        printf("文件保存失败！\n");
        return;
    }
    
    Customer *p = head;  // 从链表头开始遍历
    while (p) {  // 遍历所有节点
        // 将当前节点数据写入文件
        fwrite(p, sizeof(Customer), 1, fp);
        p = p->next;  // 移动到下一个节点
    }
    
    fclose(fp);  // 关闭文件
}

// ============================================================
// 获取下一个可用的客户ID（自动递增）
// 参数：head - 链表头指针
// 返回值：当前最大ID + 1
// ============================================================
int getNextId(Customer *head) {
    int maxId = 0;
    Customer *p = head;
    while (p) {  // 遍历所有节点
        if (p->id > maxId) {  // 找到最大ID
            maxId = p->id;
        }
        p = p->next;
    }
    return maxId + 1;  // 返回最大ID+1作为新ID
}

// ============================================================
// 添加新客户
// 参数：head - 链表头指针的地址
// ============================================================
void addCustomer(Customer **head) {
    // 1. 为新客户分配内存
    Customer *new = (Customer*)malloc(sizeof(Customer));
    new->next = NULL;  // 新节点的next初始化为NULL
    
    // 2. 自动生成客户ID
    new->id = getNextId(*head);
    printf("客户ID: %d\n", new->id);
    
    // 3. 输入客户基本信息
    printf("请输入姓名: ");
    scanf("%s", new->name);
    printf("请输入电话: ");
    scanf("%s", new->phone);
    
    // 4. 初始化积分相关数据
    new->totalAmount = 0;   // 累计消费金额初始为0
    new->points = 0;        // 积分初始为0
    new->discount = 1.0;    // 折扣率初始为1.0（无折扣）
    
    // 5. 头插法插入链表
    new->next = *head;
    *head = new;
    
    printf("添加成功！\n");
    system("pause");  // 暂停，让用户看到提示
}

// ============================================================
// 修改客户信息（姓名、电话）
// 参数：head - 链表头指针
// ============================================================
void modifyCustomer(Customer *head) {
    // 1. 检查链表是否为空
    if (!head) {
        printf("暂无客户数据！\n");
        system("pause");
        return;
    }
    
    int id;
    printf("请输入要修改的客户ID: ");
    scanf("%d", &id);
    
    // 2. 查找指定ID的客户
    Customer *p = head;
    while (p && p->id != id) {  // 遍历链表查找
        p = p->next;
    }
    
    // 3. 未找到客户
    if (!p) {
        printf("未找到该客户！\n");
        system("pause");
        return;
    }
    
    // 4. 显示当前信息
    printf("当前信息：姓名:%s 电话:%s\n", p->name, p->phone);
    
    // 5. 修改姓名（允许直接回车保留原值）
    printf("请输入新姓名（直接回车保留原值）: ");
    char temp[20];
    getchar();  // 消耗之前输入的回车符
    fgets(temp, sizeof(temp), stdin);  // 读取一行（包括空格）
    temp[strcspn(temp, "\n")] = '\0';   // 去掉末尾的换行符
    
    if (strlen(temp) > 0) {  // 如果输入了内容
        strcpy(p->name, temp);  // 更新姓名
    }
    
    // 6. 修改电话（允许直接回车保留原值）
    printf("请输入新电话（直接回车保留原值）: ");
    fgets(temp, sizeof(temp), stdin);
    temp[strcspn(temp, "\n")] = '\0';
    
    if (strlen(temp) > 0) {  // 如果输入了内容
        strcpy(p->phone, temp);  // 更新电话
    }
    
    printf("修改成功！\n");
    system("pause");
}

// ============================================================
// 删除客户
// 参数：head - 链表头指针的地址
// ============================================================
void deleteCustomer(Customer **head) {
    // 1. 检查链表是否为空
    if (!*head) {
        printf("暂无客户数据！\n");
        system("pause");
        return;
    }
    
    int id;
    printf("请输入要删除的客户ID: ");
    scanf("%d", &id);
    
    // 2. 查找要删除的客户（同时记录前驱节点）
    Customer *p = *head;     // 当前节点
    Customer *prev = NULL;   // 前驱节点
    
    while (p && p->id != id) {
        prev = p;      // 保存前驱节点
        p = p->next;   // 移动到下一个节点
    }
    
    // 3. 未找到客户
    if (!p) {
        printf("未找到该客户！\n");
        system("pause");
        return;
    }
    
    // 4. 删除节点（分两种情况）
    if (prev) {  // 如果要删除的不是头节点
        prev->next = p->next;  // 前驱节点指向当前节点的后继
    } else {  // 如果要删除的是头节点
        *head = p->next;  // 头指针指向下一个节点
    }
    
    free(p);  // 释放被删除节点的内存
    printf("删除成功！\n");
    system("pause");
}

// ============================================================
// 查询客户详细信息
// 参数：head - 链表头指针
// ============================================================
void searchCustomer(Customer *head) {
    // 1. 检查链表是否为空
    if (!head) {
        printf("暂无客户数据！\n");
        system("pause");
        return;
    }
    
    int id;
    printf("请输入要查询的客户ID: ");
    scanf("%d", &id);
    
    // 2. 查找指定ID的客户
    Customer *p = head;
    while (p && p->id != id) {
        p = p->next;
    }
    
    // 3. 未找到客户
    if (!p) {
        printf("未找到该客户！\n");
        system("pause");
        return;
    }
    
    // 4. 输出客户的所有信息
    printf("========= 客户信息 =========\n");
    printf("客户ID: %d\n", p->id);
    printf("姓名: %s\n", p->name);
    printf("电话: %s\n", p->phone);
    printf("累计消费: %.2f 元\n", p->totalAmount);
    printf("当前积分: %d 分\n", p->points);
    printf("折扣率: %.0f%%\n", p->discount * 100);  // 转换为百分数显示
    printf("============================\n");
    system("pause");
}

// ============================================================
// 更新客户积分和折扣率（核心业务逻辑）
// 参数：c - 客户指针，amount - 本次消费金额
// ============================================================
void updatePointsAndDiscount(Customer *c, double amount) {
    // 1. 更新累计消费金额
    c->totalAmount += amount;
    
    // 2. 更新积分（每消费1元积1分）
    // 注意：这里用(int)强制转换，只取整数部分
    c->points += (int)amount;
    
    // 3. 根据积分更新折扣率（阶梯折扣）
    // 积分越高，折扣越大（数值越小表示折扣越多）
    if (c->points > 5000) {          // 积分5000以上
        c->discount = 0.85;          // 85折
    } else if (c->points > 2000) {   // 积分2001-5000
        c->discount = 0.90;          // 9折
    } else if (c->points > 1000) {   // 积分1001-2000
        c->discount = 0.95;          // 95折
    } else if (c->points > 500) {    // 积分501-1000
        c->discount = 0.98;          // 98折
    } else {                         // 积分0-500
        c->discount = 1.0;           // 无折扣
    }
}

// ============================================================
// 消费结算功能（核心功能）
// 参数：head - 链表头指针
// ============================================================
void consume(Customer *head) {
    // 1. 检查链表是否为空
    if (!head) {
        printf("暂无客户数据！\n");
        system("pause");
        return;
    }
    
    int id;
    double amount;  // 消费金额
    
    printf("请输入客户ID: ");
    scanf("%d", &id);
    
    // 2. 查找指定ID的客户
    Customer *p = head;
    while (p && p->id != id) {
        p = p->next;
    }
    
    // 3. 未找到客户
    if (!p) {
        printf("未找到该客户！\n");
        system("pause");
        return;
    }
    
    // 4. 显示当前折扣信息
    printf("当前折扣: %.0f%%\n", p->discount * 100);
    printf("请输入消费金额: ");
    scanf("%lf", &amount);
    
    // 5. 计算实际应付金额（应用折扣）
    double actualPay = amount * p->discount;
    
    // 6. 更新积分和折扣（注意：积分按原价计算，不是按折扣后）
    updatePointsAndDiscount(p, amount);
    
    // 7. 显示结算信息
    printf("========= 结算信息 =========\n");
    printf("原价: %.2f 元\n", amount);
    printf("折扣率: %.0f%%\n", p->discount * 100);
    printf("折扣后: %.2f 元\n", actualPay);
    printf("节省: %.2f 元\n", amount - actualPay);  // 计算节省金额
    printf("本次积分: %d 分\n", (int)amount);
    printf("总积分: %d 分\n", p->points);
    printf("当前折扣: %.0f%%\n", p->discount * 100);
    printf("累计消费: %.2f 元\n", p->totalAmount);
    printf("============================\n");
    system("pause");
}

// ============================================================
// 显示所有客户信息（列表形式）
// 参数：head - 链表头指针
// ============================================================
void displayAll(Customer *head) {
    // 1. 检查链表是否为空
    if (!head) {
        printf("暂无客户数据！\n");
        system("pause");
        return;
    }
    
    // 2. 打印表头（格式化输出）
    printf("\n========= 所有客户信息 =========\n");
    printf("%-5s %-10s %-12s %-10s %-8s %-8s\n", 
           "ID", "姓名", "电话", "累计消费", "积分", "折扣");
    printf("----------------------------------------\n");
    
    // 3. 遍历链表并打印每个客户
    Customer *p = head;
    while (p) {
        printf("%-5d %-10s %-12s %-10.2f %-8d %-8.0f%%\n", 
               p->id, p->name, p->phone, p->totalAmount, 
               p->points, p->discount * 100);
        p = p->next;  // 移动到下一个节点
    }
    printf("====================================\n");
    system("pause");
}

// ============================================================
// 释放链表所有节点占用的内存
// 参数：head - 链表头指针
// ============================================================
void freeList(Customer *head) {
    Customer *p;  // 临时指针
    while (head) {        // 循环直到链表为空
        p = head;         // 保存当前节点
        head = head->next; // 头指针后移
        free(p);          // 释放当前节点内存
    }
}

// ============================================================
// 程序结束
// ============================================================