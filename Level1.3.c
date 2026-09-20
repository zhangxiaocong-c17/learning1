#include <stdio.h>
#include <string.h>
#include <time.h>
char input[100];
char *cmd; // 没搞懂指针是什么先照葫芦画瓢吧
int count[3];
float price[3];
int mark[3]; // 用于标记一次扫描是否对某件商品改动，以决定是否显示
int success; // 标记一次指令是否是已知指令
float pricetotal;
float priceproduct[3];
char name[3][9] = {"Cola", "Lollipop", "Noodles"};
void output();
void receipt();
void drop();
void newday();
void getitems();
char item[100]; 
char product_code[3][4] = {"001", "002", "003"};
int daycount = 1;
int serial_number = 1;
FILE *fp = NULL;
time_t rawtime;

void setprice() //设置商品价格
{
  price[0] = 3.5;
  price[1] = 0.5;
  price[2] = 6.5;
}
int main()
{
  fp = fopen("sales.csv","w+");
  fprintf(fp,"No.,Time,Items,Ament\n");
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
      time(&rawtime);
      localtime(&rawtime);
      struct tm *t = localtime(&rawtime); 
      receipt();
      getitems();
      fprintf(fp,"%d,%02d:%02d:%02d,%s,%.2f\n",serial_number,t->tm_hour,t->tm_min,t->tm_sec,item,pricetotal);
      serial_number++;
      drop();
      fclose(fp);
      fp = fopen("sales.csv","a+");
    }
    else if (strcmp(cmd, "newday") == 0 ) //新的一天
    {
      newday();
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
  fclose(fp);
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
void newday() //新的一天 保存记录 并初始化sales.csv
{
  fclose(fp);
  char filename[50];
  sprintf(filename,"sales/Day%d.csv",daycount);
  rename("sales.csv",filename);
  daycount++;
  printf("New day started. Today's sales records cleared.\n");
  fp = fopen("sales.csv","w+");
  fprintf(fp,"No.,Time,Items,Ament\n");
}
void getitems() //获取物品信息
{
  for(int i = 0; i <= 2 && count[i] > 0 ; i++ )
  {
    char countchar[10]; // 之后用于将商品数由int转化为char
    strcpy(name[i],item);
    strcat("x",item);
    sprintf(countchar, "%d", count[i]); // 用于将商品数由int转化为char
    strcat(countchar,item);
    if(i != 2)
    {
      strcat(";",item);
    }
  }
}