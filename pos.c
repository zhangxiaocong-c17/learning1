#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
char input[100]; // 用户输入
char *cmd;       // 切分后的用户输入
int count[100];
int mark[100];           // 用于标记一次扫描是否对某件商品改动，以决定是否显示
int success;             // 标记一次指令是否是已知指令
float pricetotal;        // 总价
float priceproduct[100]; // n个同种商品的总价
char name[3][9] = {"Cola", "Lollipop", "Noodles"};
void output();
void receipt();
void drop();
void newday();
void getitems();
void setprice();
void itemadd();
void itemdel();
char item[100]; // 形如"cola x1;"的商品详情
char product_code[3][4] = {"001", "002", "003"};
int daycount = 1;      // 天数
int serial_number = 1; // 序列号
char line[100];        // 读取文件一行的字符串
FILE *fp = NULL;       // 文件指针
time_t rawtime;        // 时间戳（距1970.1.1的秒数）
char *cell;            // 读取的一个格子
void read(int day);
int mode = 0; // 控制是否为管理员模式
void cashier();
void admin();
char password[] = {"admin123"}; // 设置密码
struct products                 // 声明结构类型
{
  char code[4];
  float price;
  char name[20];
  int stock;
}; // 声明结构类型
struct products product[100]; // 定义100个结构变量product[i]
int product_count = 0;        // 商品种类数
void savechanges();
void setstock();
void restock();

void setinfo() // 设置商品价格、名称等信息
{
  /*价格*/
  product[0].price = 3.5;
  product[1].price = 0.5;
  product[2].price = 6.5;
  /*编码*/
  strcpy(product[0].code, "001");
  strcpy(product[1].code, "002");
  strcpy(product[2].code, "003");
  /*名称*/
  strcpy(product[0].name, "Cola");
  strcpy(product[1].name, "Lollipop");
  strcpy(product[2].name, "Noodles");
}

void getinfo() // 从info.csv文件中读取商品价格、名称等信息
{
  fp = fopen("info.csv", "r");
  fgets(line, 25, fp);
  for (int i = 0; fgets(line, 100, fp) != NULL; i++) // 逐行读取并保存到结构体product[i]中
  {
    cell = strtok(line, "\n\t,"); // 名称
    strcpy(product[i].name, cell);
    cell = strtok(NULL, "\n\t,"); // 编码
    strcpy(product[i].code, cell);
    cell = strtok(NULL, "\n\t,"); // 价格
    product[i].price = atof(cell);
    cell = strtok(NULL, "\n\t,"); // 库存
    product[i].stock = atoi(cell);
    product_count++;
  }
}

int main()
{
  fp = fopen("sales.csv", "w+");
  fprintf(fp, "No.,Time,Items,Ament\n");
  fclose(fp);
  getinfo();
  while (1)
  {
    printf("> ");
    fgets(input, sizeof(input), stdin);
    cmd = strtok(input, " \t\n");
    if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "quit") == 0) // 退出程序
    {
      break;
    }
    else if (strcmp(cmd, "prices") == 0) // 查询价格与库存
    {
      printf("Item        No.   Pri. Stock\n"
             "----------------------------\n");
      for (int i = 0; i <= product_count - 1; i++)
      {
        printf("%-12s%-4s%6.2f %d\n", product[i].name, product[i].code, product[i].price, product[i].stock);
      }
      if (mode == 1)
      {
        printf("admin");
      }
    }
    else // 判断模式
    {
      if (mode == 0) // 收银员
      {
        cashier();
      }
      if (mode == 1) // 管理员
      {
        admin();
      }
    }
  }
  fclose(fp);
}
void cashier() // 店员模式
{
  if (strcmp(cmd, "admin") == 0) // 进入管理员模式
  {
    printf("Password:");
    fgets(input, sizeof(input), stdin);
    cmd = strtok(input, " \t\n");
    if (strcmp(cmd, password) == 0) // 校验密码
    {
      printf("Admin mode.\n");
      mode = 1;
    }
    else
    {
      printf("Incorrect password.\n");
    }
  }
  else if (strcmp(cmd, "print") == 0) // 打印小票
  {
    receipt();
  }
  else if (strcmp(cmd, "drop") == 0) // 清空记录
  {
    drop();
  }
  else if (strcmp(cmd, "checkout") == 0) // 结账
  {
    fp = fopen("sales.csv", "a+");
    time(&rawtime);
    localtime(&rawtime);
    struct tm *t = localtime(&rawtime);
    receipt();
    savechanges();
    getitems();
    fprintf(fp, "%d,%02d:%02d:%02d,%s,%.2f\n", serial_number, t->tm_hour, t->tm_min, t->tm_sec, item, pricetotal);
    serial_number++;
    drop();
    fclose(fp);
  }
  else if (strcmp(cmd, "newday") == 0) // 新的一天
  {
    newday();
  }
  else if (strcmp(cmd, "sales") == 0) // 查看记录
  {
    char *cmd_sales = strtok(NULL, " \n\t");
    if (cmd_sales == NULL)
    {
      read(daycount);
    }
    else
    {
      int cmd_sales_int = atoi(cmd_sales);
      read(cmd_sales_int);
    }
  }
  else
  {
    success = 0;
    while (cmd != NULL)
    {
      for (int i = 0; i <= product_count - 1; i++)
      {
        if (strcmp(cmd, product[i].code) == 0) // 增加商品
        {
          count[i]++;
          mark[i] = 1;
          success = 1;
        }
        if (cmd[0] == '-') // 减少商品
        {
          if (strcmp(cmd + 1, product[i].code) == 0)
          {
            count[i]--;
            mark[i] = 1;
            success = 1;
          }
        }
      }
      if (success == 0) // 命令不存在报错
      {
        printf("ERROR: code not found\n");
      }
      cmd = strtok(NULL, " \t\n");
    }
    output();
  }
}
void admin() // 管理员模式
{
  success = 0;
  if (strcmp(cmd, "back") == 0)
  {
    success = 1;
    printf("Bye.\n");
    mode = 0;
  }
  else
  {
    if (strcmp(cmd, "setprice") == 0)
    {
      setprice();
      success = 1;
    }
    else if (strcmp(cmd, "itemadd") == 0)
    {
      itemadd();
      success = 1;
    }
    else if (strcmp(cmd, "itemdel") == 0)
    {
      itemdel();
      success = 1;
    }
    else if (strcmp(cmd, "restock") == 0)
    {
      restock();
      success = 1;
    }
    else if (strcmp(cmd, "setstock") == 0)
    {
      setstock();
      success = 1;
    }
    if (success == 0) // 命令不存在报错
    {
      printf("ERROR: code not found\n");
    }
    printf("admin");
  }
}
void output() // 扫描时实时计算、输出价格
{
  for (int i = 0; i <= product_count - 1; i++)
  {
    if (count[i] > 0)
    {
      priceproduct[i] = product[i].price * count[i];
    }
    else if (count[i] == 0)
    {
      priceproduct[i] = 0;
    }
    else if (count[i] < 0)
    {
      count[i] = 0;
      printf("ERROR: count must not be negative number\n");
    }
    if (mark[i] == 1)
    {
      printf("%-9s%5.2f x%-2d=%.2f\n", product[i].name, product[i].price, count[i], priceproduct[i]);
    }
    mark[i] = 0;
  }
}
void receipt() // 计算输出总价
{
  pricetotal = 0;

  for (int i = 0; i <= product_count - 1; i++)
  {
    if (product[i].stock - count[i] >= 0)
    {
      pricetotal = pricetotal + priceproduct[i];
    }
  }
  printf("Receipt\n"
         "Item      Pri.  Qty Amount\n"
         "-------------------------\n");
  for (int i = 0; i <= product_count - 1; i++)
  {
    if (count[i] > 0 && product[i].stock - count[i] >= 0)
    {
      printf("%-9s%5.2f x%-2d=%.2f\n", product[i].name, product[i].price, count[i], priceproduct[i]);
      product[i].stock = product[i].stock - count[i];
    }
  }
  printf("-------------------------\n");
  printf("Total             =%.2f\n", pricetotal);
  for (int i = 0; i <= product_count - 1; i++)
  {
    if (count[i] > 0)
    {
      if (product[i].stock - count[i] < 0)
      {
        printf("ERROR: out of stock(%s):%d\n", product[i].name, product[i].stock);
      }
      if (product[i].stock - count[i] >= 0)
      {
        printf("remain(%s): %d\n", product[i].name, product[i].stock);
      }
    }
  }
}
void drop() // 清空记录
{
  for (int i = 0; i <= product_count - 1; i++)
  {
    count[i] = 0;
    priceproduct[i] = 0;
  }
  pricetotal = 0;
}
void newday() // 新的一天 保存记录 并初始化sales.csv
{
  fclose(fp);
  char filename[50];
  sprintf(filename, "sales/Day%d.csv", daycount);
  rename("sales.csv", filename);
  daycount++;
  serial_number = 1;
  printf("New day started. Today's sales records cleared.\n");
  fp = fopen("sales.csv", "w+");
  fprintf(fp, "No.,Time,Items,Ament\n");
  fclose(fp);
}
void getitems() // 获取物品信息
{
  item[0] = '\0';
  char countchar[10]; // 之后用于将商品数由int转化为char
  for (int i = 0; i <= product_count - 1; i++)
  {
    if (count[i] > 0)
    {
      strcat(item, product[i].name);
      strcat(item, " x");
      sprintf(countchar, "%d", count[i]); // 用于将商品数由int转化为char
      strcat(item, countchar);
      strcat(item, ";");
    }
  }
  if (item[0] == '\0')
  {
    strcat(item, "NULL");
  }
}
void read(int day) // 从文件中读取物品信息
{
  if (day == daycount) // 当日
  {
    fp = fopen("sales.csv", "r");
  }
  else // 其他日期的
  {
    char filename[50];
    sprintf(filename, "sales/Day%d.csv", day);
    fp = fopen(filename, "r");
  }
  float daily = 0;
  fgets(line, 22, fp); // 跳过第一行表头
  printf("Date: %d\n", day);
  printf("--------------------------------------\n");
  while (fgets(line, 100, fp) != NULL) // 逐行读取并输出到屏幕
  {
    cell = strtok(line, "\n\t,");
    printf("%-6s", cell); // 编号
    cell = strtok(NULL, "\n\t,");
    printf("%8s", cell); // 时间
    cell = strtok(NULL, "\n\t,");
    // for(int i = 0; cell[i] != 0; i++ )
    //{
    //   if (cell[i] == ';'){cell[i] = '\n';}
    // }
    printf("  %s", cell); // 数量
    cell = strtok(NULL, "\n\t,");
    printf("%s\n", cell); // 总价

    float cell_price = atof(cell);
    daily = daily + cell_price;
  }
  printf("--------------------------------------\n");
  printf("Daily: %.2f\n", daily);
}
void setprice() // 修改商品价格
{
  cmd = strtok(NULL, " \n\t");
  char cmd_code[10];
  strcpy(cmd_code, cmd);
  cmd = strtok(NULL, " \n\t");
  float cmd_price = atof(cmd);
  for (int i = 0; i <= product_count - 1; i++)
  {
    if (strcmp(cmd_code, product[i].code) == 0)
    {
      product[i].price = cmd_price;
      break;
    }
  }
  printf("Price updated.\n");
  for (int i = 0; i <= product_count - 1; i++)
  {
    if (strcmp(cmd_code, product[i].code) == 0)
    {
      printf("New price of %s(%s): %.2f\n", product[i].name, product[i].code, product[i].price);
      break;
    }
  }
  // printf("newprice of %s :%s\n",cmd_code, cmd);
  savechanges();
}
void itemadd() // 添加新商品
{
  /*之所以这里使用product.count，是因为数量恰好比最大编号多1
  在数量还没增加的时候，原有数量就是新的最大编号*/
  cmd = strtok(NULL, " \n\t");
  strcpy(product[product_count].code, cmd);
  cmd = strtok(NULL, " \n\t");
  strcpy(product[product_count].name, cmd);
  cmd = strtok(NULL, " \n\t");
  float cmd_price = atof(cmd);
  product[product_count].price = cmd_price;
  printf("%s(%s) added.\nPrice:%.2f\n", product[product_count].name, product[product_count].code, product[product_count].price);
  product_count++;
  savechanges();
}
void itemdel() // 删除商品
{
  cmd = strtok(NULL, " \n\t");
  for (int i = 0; i <= product_count - 1; i++)
  {
    if (strcmp(product[i].code, cmd) == 0)
    {
      printf("%s(%s) removed.\n", product[i].name, product[i].code);
      break;
    }
  }
  product_count--;
  for (int i = 0; i <= product_count - 1; i++)
  {
    if (strcmp(cmd, product[i].code) == 0)
    {
      for (int ii = i; ii <= product_count - 1; ii++)
      {
        strcpy(product[ii].name, product[ii + 1].name);
        strcpy(product[ii].code, product[ii + 1].code);
        product[ii].price = product[ii + 1].price;
      }
      break;
    }
  }
  savechanges();
}
void savechanges() // 保存对商品信息的改动
{
  fp = fopen("info.csv", "w");
  fprintf(fp, "Item,No.,Pri.,Stock\n");
  for (int i = 0; i <= product_count - 1; i++)
  {
    fprintf(fp, "%s,%s,%.2f,%d\n", product[i].name, product[i].code, product[i].price, product[i].stock);
  }
  fclose(fp);
}
void setstock() // 设置库存
{
  cmd = strtok(NULL, " \n\t");
  printf("Stock updated.\ncode:%s\n", cmd);
  char cmd_code[10];
  strcpy(cmd_code, cmd);
  cmd = strtok(NULL, " \n\t");
  int cmd_stock = atoi(cmd);
  printf("stock:%d\n", cmd_stock);
  for (int i = 0; i <= product_count - 1; i++)
  {
    if (strcmp(cmd_code, product[i].code) == 0)
    {
      product[i].stock = cmd_stock;
      break;
    }
  }
  savechanges();
}
void restock() // 增加库存
{
  cmd = strtok(NULL, " \n\t");
  printf("Stock updated.\ncode:%s\n", cmd);
  char cmd_code[10];
  strcpy(cmd_code, cmd);
  cmd = strtok(NULL, " \n\t");
  int cmd_stock = atoi(cmd);
  for (int i = 0; i <= product_count - 1; i++)
  {
    if (strcmp(cmd_code, product[i].code) == 0)
    {
      product[i].stock = product[i].stock + cmd_stock;
      printf("stock:%d\n", product[i].stock);
      break;
    }
  }
  savechanges();
}