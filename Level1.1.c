#include <stdio.h>
#include <string.h>
char input[100]; 
char *cmd;//没搞懂指针是什么先照葫芦画瓢吧
int main()
{

 while(1){
    fgets(input,sizeof(input),stdin);
    cmd = strtok(input," \t\n");
    if(strcmp(cmd,"exit") == 0 || strcmp(cmd,"quit") == 0 ){
        break;//python后遗症注意不要写成or
    }//退出程序
    else  if(strcmp(cmd,"prices") == 0){
        printf("Item     No. Pri.\n",
            "-----------------\n",
            "Cola     001 3.50\n"
            "Lollipop 002 0.50\n"
            "Noodles  003 6.00");//暂时不知道为什么不能一组双引号全部囊括 但是vscode标红了我就顺了它
    }    
    while(cmd != NULL){
        if(strcmp(cmd,"001") == 0){
          printf("Cola   3.50");
    }
        else if(strcmp(cmd,"002") == 0){
          printf("Lollipop   0.50");
    }
        else if(strcmp(cmd,"003") == 0){
         printf("Noodles   6.00");
    }
    cmd = strtok(input," \t\n");
    }

 }  
}