#include <stdio.h>
#include <string.h>
char input[100]; 
char *cmd;//没搞懂指针是什么先照葫芦画瓢吧
int count[3];
float price[3];
float pricetotal;
float priceproduct[3];
char name[3][8]={"Cola","Lollipop","Noodles"};
void output();
void setprice(){
    price[0]=3.5;
    price[1]=0.5;
    price[2]=6.5;
}
int main()
{
 setprice();
 while(1){
    printf(">");
    fgets(input,sizeof(input),stdin);
    cmd = strtok(input," \t\n");
    if(strcmp(cmd,"exit") == 0 || strcmp(cmd,"quit") == 0 ){
        break;//python后遗症注意不要写成or
    }//退出程序
    else  if(strcmp(cmd,"prices") == 0){
        printf("Item     No. Pri.\n"
            "-----------------\n"
            "Cola     001 3.50\n"
            "Lollipop 002 0.50\n"
            "Noodles  003 6.00\n");//暂时不知道为什么不能一组双引号全部囊括 但是vscode标红了我就顺了它
    }
    else{
        while(cmd != NULL){
        if(strcmp(cmd,"001") == 0){
          count[0]++;
        }
        else if(strcmp(cmd,"002") == 0){
          count[1]++;
        }
        else if(strcmp(cmd,"003") == 0){
          count[2]++;
        }
        else if(strcmp(cmd,"-001") == 0){
          count[0]--;
        }
        else if(strcmp(cmd,"-002") == 0){
          count[1]--;
        }
        else if(strcmp(cmd,"-003") == 0){
          count[2]--;
        }
        else if(strcmp(cmd,"prices") != 0){
            printf("ERROR: code not found\n");
        }
        cmd = strtok(NULL," \t\n");
        }
        output();
    }    


 }  
}
void output(){//扫描时实时输出价格
    for(int i =  0; i <= 2; i++){
        if(count[i] != 0){
        priceproduct[i] = price[i] * count[i];
        printf("%-9s%5.2f x%2d=%.2f\n",name[i],price[i],count[i],priceproduct[i]);
    }
    }
    
}