#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
char input[100]; // 用户输入
char *cmd;       // 切分后的用户输入
int count[3];
float price[3];
int mark[3];           // 用于标记一次扫描是否对某件商品改动，以决定是否显示
int success;           // 标记一次指令是否是已知指令
float pricetotal;      // 总价
float priceproduct[3]; // n个同种商品的总价
char name[3][9] = {"Cola", "Lollipop", "Noodles"};
void output();
void receipt();
void drop();
void newday();
void getitems();
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

void setprice() // 设置商品价格
{
  price[0] = 3.5;
  price[1] = 0.5;
  price[2] = 6.5;
}
int main()
{
  fp = fopen("sales.csv", "w+");
  fprintf(fp, "No.,Time,Items,Ament\n");
  fclose(fp);
  setprice();
  while (1)
  {
    printf(">");
    fgets(input, sizeof(input), stdin);
    cmd = strtok(input, " \t\n");
    if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "quit") == 0) // python后遗症注意不要写成or
    {
      break;
    } // 退出程序
    else if (strcmp(cmd, "prices") == 0) // 查询价格
    {
      printf("Item     No. Pri.\n"
             "-----------------\n"
             "Cola     001 3.50\n"
             "Lollipop 002 0.50\n"
             "Noodles  003 6.00\n");
    }
    else
    {
      if (mode == 0)
      {
        void cashier();
      }
      if (mode == 1)
      {
        void admin();
      }
    }
  }
  fclose(fp);
}
void cashier() // 店员模式
{
  if (strcmp(cmd, "admin") == 0)
  {
    printf("Password:");
    fgets(input, sizeof(input), stdin);
    cmd = strtok(input, " \t\n");
    if (strcmp(cmd, password) == 0)
    {
      printf("\nAdmin mode.\n");
      mode = 1;
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
      for (int i = 0; i <= 2; i++)
      {
        if (strcmp(cmd, product_code[i]) == 0) // 增加商品
        {
          count[i]++;
          mark[i] = 1;
          success = 1;
        }
        if (cmd[0] == '-') // 减少商品
        {
          if (strcmp(cmd + 1, product_code[i]) == 0)
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
void output() // 扫描时实时计算、输出价格
{
  for (int i = 0; i <= 2; i++)
  {
    if (count[i] > 0)
    {
      priceproduct[i] = price[i] * count[i];
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
      printf("%-9s%5.2f x%-2d=%.2f\n", name[i], price[i], count[i], priceproduct[i]);
    }
    mark[i] = 0;
  }
}
void receipt() // 计算输出总价
{
  pricetotal = priceproduct[0] + priceproduct[1] + priceproduct[2];
  printf("Receipt\n"
         "Item      Pri.  Qty Amount\n"
         "-------------------------\n");
  for (int i = 0; i <= 2; i++)
  {
    if (count[i] > 0)
    {
      printf("%-9s%5.2f x%-2d=%.2f\n", name[i], price[i], count[i], priceproduct[i]);
    }
  }
  printf("-------------------------\n");
  printf("Total             =%.2f\n", pricetotal);
}
void drop() // 清空记录
{
  for (int i = 0; i <= 2; i++)
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
  for (int i = 0; i <= 2; i++)
  {
    if (count[i] > 0)
    {
      strcat(item, name[i]);
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