#include <stdio.h>
#include <string.h>
char input[100];
char *cmd; // 没搞懂指针是什么先照葫芦画瓢吧
int count[3];
float price[3];
int mark[3];//用于标记一次扫描是否对某件商品改动，以决定是否显示
float pricetotal;
float priceproduct[3];
char name[3][9] = {"Cola", "Lollipop", "Noodles"};
void output();
void receipt();
void drop();
void setprice()
{
  price[0] = 3.5;
  price[1] = 0.5;
  price[2] = 6.5;
}
int main()
{
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
    else if (strcmp(cmd, "prices") == 0)
    {
      printf("Item     No. Pri.\n"
             "-----------------\n"
             "Cola     001 3.50\n"
             "Lollipop 002 0.50\n"
             "Noodles  003 6.00\n"); // 暂时不知道为什么不能一组双引号全部囊括 但是vscode标红了我就顺了它
    }
    else if (strcmp(cmd, "print") == 0)
    {
      receipt();
    }
    else if (strcmp(cmd, "drop") == 0)
    {
      drop();
    }
    else if (strcmp(cmd, "checkout") == 0)
    {
      receipt();
      drop();
    }
    else
    {
      while (cmd != NULL)
      {
        if (strcmp(cmd, "001") == 0)
        {
          count[0]++;
          mark[0] = 1;
        }
        else if (strcmp(cmd, "002") == 0)
        {
          count[1]++;
          mark[1] = 1;
        }
        else if (strcmp(cmd, "003") == 0)
        {
          count[2]++;
          mark[2] = 1;
        }
        else if (strcmp(cmd, "-001") == 0)
        {
          count[0]--;
          mark[0] = 1;
        }
        else if (strcmp(cmd, "-002") == 0)
        {
          count[1]--;
          mark[1] = 1;
        }
        else if (strcmp(cmd, "-003") == 0)
        {
          count[2]--;
          mark[2] = 1;
        }
        else if (strcmp(cmd, "prices") != 0)
        {
          printf("ERROR: code not found\n");
        }
        cmd = strtok(NULL, " \t\n");
      }
      output();
    }
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