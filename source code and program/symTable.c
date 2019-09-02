#include "def.h"

char *strcat0(char *s1,char *s2)
{
    static char result[10];
    strcpy(result,s1);
    strcat(result,s2);
    return result;
}

char *newAlias()
{
    static int no=1;
    char s[10];
    itoa(no++,s,10);
    return strcat0("v",s);
}

char *newLabel()
{
    static int no=1;
    char s[10];
    itoa(no++,s,10);
    return strcat0("label",s);
}

char *newTemp()
{
    static int no=1;
    char s[10];
    itoa(no++,s,10);
    return strcat0("temp",s);
}

//生成一条TAC代码的结点组成的双向循环链表，返回头指针
struct codenode *genIR(int op,struct opn opn1,struct opn opn2,struct opn result)
{
    struct codenode *h=(struct codenode *)malloc(sizeof(struct codenode));
    h->op=op;
    h->opn1=opn1;
    h->opn2=opn2;
    h->result=result;
    h->next=h->prior=h;
    return h;
}

//生成一条标号语句，返回头指针
struct codenode *genLabel(char *label)
{
    struct codenode *h=(struct codenode *)malloc(sizeof(struct codenode));
    h->op=LABEL;
    strcpy(h->result.id,label);
    h->next=h->prior=h;
    return h;
}

//生成GOTO语句，返回头指针
struct codenode *genGoto(char *label)
{
    struct codenode *h=(struct codenode *)malloc(sizeof(struct codenode));
    h->op=GOTO;
    strcpy(h->result.id,label);
    h->next=h->prior=h;
    return h;
}

//合并多个中间代码的双向循环链表，首尾相连
struct codenode *merge(int num,...)
{
    struct codenode *h1,*h2,*p,*t1,*t2;
    va_list ap;
    va_start(ap,num);
    h1=va_arg(ap,struct codenode *);
    while (--num>0)
    {
        h2=va_arg(ap,struct codenode *);
        if (h1==NULL)
            h1=h2;
        else if (h2)
        {
            t1=h1->prior;
            t2=h2->prior;
            t1->next=h2;
            t2->next=h1;
            h1->prior=t2;
            h2->prior=t1;
        }
    }
    va_end(ap);
    return h1;
}

//输出中间代码
void prnIR(struct codenode *head)
{
    char opnstr1[32],opnstr2[32],resultstr[32];

    struct codenode *h=head;
    do
    {
        if (h->opn1.kind==INT)
            sprintf(opnstr1,"#%d",h->opn1.const_int);
            //fprintf(fp,"li $t3,%d\n",h->opn1.const_int);
        if (h->opn1.kind==FLOAT)
            sprintf(opnstr1,"#%f",h->opn1.const_float);
        if (h->opn1.kind==ID)
            sprintf(opnstr1,"%s",h->opn1.id);
           // fprintf(fp,"lw $t1,%d($sp)\nmove $t3,$t1\n",h->opn1.offset);
        if (h->opn2.kind==INT)
            sprintf(opnstr2,"#%d",h->opn2.const_int);
          // fprintf(fp,"lw $t2,%d($sp)\n",h->opn2.offset);
        if (h->opn2.kind==FLOAT)
            sprintf(opnstr2,"#%f",h->opn2.const_float);
        if (h->opn2.kind==ID)
            sprintf(opnstr2,"%s",h->opn2.id);
           // fprintf(fp,"lw $t2,%d($sp)\n",h->opn2.offset);
        sprintf(resultstr,"%s",h->result.id);
        switch (h->op)
        {
        case ASSIGNOP:
            printf("  %s := %s\n",resultstr,opnstr1);
           //fprintf(fp,"sw $t3,%d($sp)\n",h->result.offset);
            break;
        case PLUS:
            printf("  %s := %s %c %s\n",resultstr,opnstr1, \
                   h->op==PLUS?'+':h->op==MINUS?'-':h->op==STAR?'*':'\\',opnstr2);
           // fprintf(fp,"add $t3,$t1,$t2\n");
            break;
        case MINUS:
            printf("  %s := %s %c %s\n",resultstr,opnstr1, \
                   h->op==PLUS?'+':h->op==MINUS?'-':h->op==STAR?'*':'\\',opnstr2);
           // fprintf(fp,"sub $t3,$t1,$t2\n");
            break;
        case STAR:
            printf("  %s := %s %c %s\n",resultstr,opnstr1, \
                   h->op==PLUS?'+':h->op==MINUS?'-':h->op==STAR?'*':'\\',opnstr2);
                 // fprintf(fp,"mul $t3,$t1,$t2\n");
                   break;
        case DIV:
            printf("  %s := %s %c %s\n",resultstr,opnstr1, \
                   h->op==PLUS?'+':h->op==MINUS?'-':h->op==STAR?'*':'\\',opnstr2);
             //  fprintf(fp,"mul $t3,$t1,$t2\ndiv $1,$t2\nmflo $t3\n");
            break;
        case FUNCTION:
            printf("\nFUNCTION %s :\n",h->result.id);
            break;
        case PARAM:
            printf("  PARAM %s\n",h->result.id);
            break;
        case LABEL:
            printf("LABEL %s :\n",h->result.id);
            break;
        case GOTO:
            printf("  GOTO %s\n",h->result.id);
            break;
        case JLE:
            printf("  IF %s <= %s GOTO %s\n",opnstr1,opnstr2,resultstr);
            //fprintf(fp,"blt $t1,$t2,%s\n",resultstr);
            break;
        case JLT:
            printf("  IF %s < %s GOTO %s\n",opnstr1,opnstr2,resultstr);
            //fprintf(fp,"ble $t1,$t2,%s\n",resultstr);
            break;
        case JGE:
            printf("  IF %s >= %s GOTO %s\n",opnstr1,opnstr2,resultstr);
           // fprintf(fp,"bge $t1,$t2,%s\n",resultstr);
            break;
        case JGT:
            printf("  IF %s > %s GOTO %s\n",opnstr1,opnstr2,resultstr);
           // fprintf(fp,"bgt $t1,$t2,%s",resultstr);
            break;
        case EQ:
            printf("  IF %s == %s GOTO %s\n",opnstr1,opnstr2,resultstr);
          // fprintf(fp,"beq $t1,$t2,%s",resultstr);
            break;
        case NEQ:
            printf("  IF %s != %s GOTO %s\n",opnstr1,opnstr2,resultstr);
           // fprintf(fp,"bne $t1,$t2,%s\n",resultstr);
            break;
        case ARG:
            printf("  ARG %s\n",h->result.id);
            break;
        case CALL:
            printf("  %s := CALL %s\n",resultstr, opnstr1);
            break;
        case RETURN:
            if (h->result.kind){
                printf("  RETURN %s\n",resultstr);
               // fprintf(fp,"move $v0,%d($sp)\n",h->result.offset);
            }
            else
            {
                 printf("  RETURN\n");
                 //fprintf(fp,"jr $ra");
            }

            break;

        }
        h=h->next;
    }
    while (h!=head);
}

void semantic_error(int line,char *msg1,char *msg2)
{
    //这里可以只收集错误信息，最后一次显示
    printf("在%d行,%s %s\n",line,msg1,msg2);
}
void prn_symbol()  //显示符号表
{
    int i=0;
    printf("%6s   %6s %6s  %6s %4s %6s\n","变量名","别 名","层 号","类  型","标记","偏移量");
    for(i=0; i<symbolTable.index; i++)
        printf("%6s %6s %6d  %6s %4c %6d\n",symbolTable.symbols[i].name,\
               symbolTable.symbols[i].alias,symbolTable.symbols[i].level,\
               symbolTable.symbols[i].type==INT?"int":(symbolTable.symbols[i].type==FLOAT?"float":"char"),\
               symbolTable.symbols[i].flag,symbolTable.symbols[i].offset);
}

int searchSymbolTable(char *name)
{
    int i;
    for(i=symbolTable.index-1; i>=0; i--)
        if (!strcmp(symbolTable.symbols[i].name, name))
            return i;
    return -1;
}

int fillSymbolTable(char *name,char *alias,int level,int type,char flag,int offset)
{
    //首先根据name查符号表，不能重复定义 重复定义返回-1
    int i;
    /*符号查重，考虑外部变量声明前有函数定义，
    其形参名还在符号表中，这时的外部变量与前函数的形参重名是允许的*/
    for(i=symbolTable.index-1; i>=0 && (symbolTable.symbols[i].level==level||level==0); i--)
    {
        if (level==0 && symbolTable.symbols[i].level==1)
            continue;  //外部变量和形参不必比较重名
        if (!strcmp(symbolTable.symbols[i].name, name))
            return -1;
    }
    //填写符号表内容
    strcpy(symbolTable.symbols[symbolTable.index].name,name);
    strcpy(symbolTable.symbols[symbolTable.index].alias,alias);
    symbolTable.symbols[symbolTable.index].level=level;
    symbolTable.symbols[symbolTable.index].type=type;
    symbolTable.symbols[symbolTable.index].flag=flag;
    symbolTable.symbols[symbolTable.index].offset=offset;
    return symbolTable.index++; //返回的是符号在符号表中的位置序号，中间代码生成时可用序号取到符号别名
}

//填写临时变量到符号表，返回临时变量在符号表中的位置
int fill_Temp(char *name,int level,int type,char flag,int offset)
{
    strcpy(symbolTable.symbols[symbolTable.index].name,"");
    strcpy(symbolTable.symbols[symbolTable.index].alias,name);
    symbolTable.symbols[symbolTable.index].level=level;
    symbolTable.symbols[symbolTable.index].type=type;
    symbolTable.symbols[symbolTable.index].flag=flag;
    symbolTable.symbols[symbolTable.index].offset=offset;
    return symbolTable.index++; //返回的是临时变量在符号表中的位置序号
}



int LEV=0;      //层号
int func_size;  //1个函数的活动记录大小
/*ExtDecList:  VarDec      {$$=$1;}
           | VarDec COMMA ExtDecList {$$=mknode(EXT_DEC_LIST,$1,$3,NULL,yylineno);}
           ;
*/
void ext_var_list(struct node *T)   //处理变量列表
{
    int rtn,num=1;
    switch (T->kind)
    {
    case EXT_DEC_LIST:
        T->ptr[0]->type=T->type;              //将类型属性向下传递变量结点
        T->ptr[0]->offset=T->offset;          //外部变量的偏移量向下传递
        T->ptr[1]->type=T->type;              //将类型属性向下传递变量结点
        T->ptr[1]->offset=T->offset+T->width; //外部变量的偏移量向下传递
        T->ptr[1]->width=T->width;
        ext_var_list(T->ptr[0]);
        ext_var_list(T->ptr[1]);
        T->num=T->ptr[1]->num+1;
        //在ExtDef再统计width
        break;
    case ID:
        rtn=fillSymbolTable(T->type_id,newAlias(),LEV,T->type,'V',T->offset);  //最后一个变量名
        if (rtn==-1)
            semantic_error(T->pos,T->type_id, "变量重复定义");
        else
            T->place=rtn;
        T->num=1;

        break;
    }
}

int  match_param(int i,struct node *T)
{
    int j,num=symbolTable.symbols[i].paramnum;
    int type1,type2;
    if (num==0 && T==NULL)
        return 1;
	int pos=T->pos;
    for (j=1; j<=num; j++)
    {
        if (T==NULL)
        {
            semantic_error(pos,"", "函数调用参数太少");
            return 0;
        }
        type1=symbolTable.symbols[i+j].type;  //形参类型
        type2=T->ptr[0]->type;
        if (type1!=type2)
        {
            semantic_error(T->pos,"", "函数调用参数类型不匹配");
            return 0;
        }
        T=T->ptr[1];
    }
    if (T)  //num个参数已经匹配完，还有实参表达式
    {
        semantic_error(T->pos,"", "函数调用参数太多");
        return 0;
    }
    return 1;
}

void boolExp(struct node *T)   //布尔表达式，参考文献[2]p84的思想
{
    struct opn opn1,opn2,result;
    int op;
    int rtn;
    if (T)
    {
        switch (T->kind)
        {
        case INT:
            if (T->type_int!=0)
                T->code=genGoto(T->Etrue);
            else
                T->code=genGoto(T->Efalse);
            T->width=0;
            break;
        case FLOAT:
            if (T->type_float!=0.0)
                T->code=genGoto(T->Etrue);
            else
                T->code=genGoto(T->Efalse);
            T->width=0;
            break;
        case ID:    //查符号表，获得符号表中的位置，类型送type
            rtn=searchSymbolTable(T->type_id);
            if (rtn==-1)
                semantic_error(T->pos,T->type_id, "变量未定义");
            if (symbolTable.symbols[rtn].flag=='F')
                semantic_error(T->pos,T->type_id, "将函数名当做变量使用");
            else
            {
                opn1.kind=ID;
                strcpy(opn1.id,symbolTable.symbols[rtn].alias);
                opn1.offset=symbolTable.symbols[rtn].offset;
                opn2.kind=INT;
                opn2.const_int=0;
                result.kind=ID;
                strcpy(result.id,T->Etrue);
                T->code=genIR(NEQ,opn1,opn2,result);
                T->code=merge(2,T->code,genGoto(T->Efalse));
            }
            T->width=0;
            break;
        case RELOP: //处理关系运算表达式,2个操作数都按基本表达式处理
            T->ptr[0]->offset=T->ptr[1]->offset=T->offset;
            Exp(T->ptr[0]);
            T->width=T->ptr[0]->width;
            Exp(T->ptr[1]);
            if (T->width<T->ptr[1]->width)
                T->width=T->ptr[1]->width;
            opn1.kind=ID;
            strcpy(opn1.id,symbolTable.symbols[T->ptr[0]->place].alias);
            opn1.offset=symbolTable.symbols[T->ptr[0]->place].offset;
            opn2.kind=ID;
            strcpy(opn2.id,symbolTable.symbols[T->ptr[1]->place].alias);
            opn2.offset=symbolTable.symbols[T->ptr[1]->place].offset;
            result.kind=ID;
            strcpy(result.id,T->Etrue);
            if (strcmp(T->type_id,"<")==0)
                op=JLT;
            else if (strcmp(T->type_id,"<=")==0)
                op=JLE;
            else if (strcmp(T->type_id,">")==0)
                op=JGT;
            else if (strcmp(T->type_id,">=")==0)
                op=JGE;
            else if (strcmp(T->type_id,"==")==0)
                op=EQ;
            else if (strcmp(T->type_id,"!=")==0)
                op=NEQ;
            T->code=genIR(op,opn1,opn2,result);
            T->code=merge(4,T->ptr[0]->code,T->ptr[1]->code,T->code,genGoto(T->Efalse));
            break;
        case AND:
        case OR:
            if (T->kind==AND)
            {
                strcpy(T->ptr[0]->Etrue,newLabel());
                strcpy(T->ptr[0]->Efalse,T->Efalse);
            }
            else
            {
                strcpy(T->ptr[0]->Etrue,T->Etrue);
                strcpy(T->ptr[0]->Efalse,newLabel());
            }
            strcpy(T->ptr[1]->Etrue,T->Etrue);
            strcpy(T->ptr[1]->Efalse,T->Efalse);
            T->ptr[0]->offset=T->ptr[1]->offset=T->offset;
            boolExp(T->ptr[0]);
            T->width=T->ptr[0]->width;
            boolExp(T->ptr[1]);
            if (T->width<T->ptr[1]->width)
                T->width=T->ptr[1]->width;
            if (T->kind==AND)
                T->code=merge(3,T->ptr[0]->code,genLabel(T->ptr[0]->Etrue),T->ptr[1]->code);
            else
                T->code=merge(3,T->ptr[0]->code,genLabel(T->ptr[0]->Efalse),T->ptr[1]->code);
            break;
        case NOT:
            strcpy(T->ptr[0]->Etrue,T->Efalse);
            strcpy(T->ptr[0]->Efalse,T->Etrue);
            boolExp(T->ptr[0]);
            T->code=T->ptr[0]->code;
            break;
        }
    }
}


void Exp(struct node *T)
{
    //处理基本表达式，参考文献[2]p82的思想
    int rtn,num,width;
    struct node *T0;
    struct opn opn1,opn2,result;
    if (T)
    {
        switch (T->kind)
        {
        case ID:    //查符号表，获得符号表中的位置，类型送type
            rtn=searchSymbolTable(T->type_id);
            if (rtn==-1)
                semantic_error(T->pos,T->type_id, "变量未定义");
            if (symbolTable.symbols[rtn].flag=='F')
                semantic_error(T->pos,T->type_id, "将函数名当做变量使用");
            else
            {
                T->place=rtn;       //结点保存变量在符号表中的位置
                T->code=NULL;       //标识符不需要生成TAC
                T->type=symbolTable.symbols[rtn].type;
                T->offset=symbolTable.symbols[rtn].offset;
                T->width=0;   //未再使用新单元
            }
            break;
        case INT:
            T->place=fill_Temp(newTemp(),LEV,T->type,'T',T->offset); //为整常量生成一个临时变量
            T->type=INT;
            opn1.kind=INT;
            opn1.const_int=T->type_int;
            result.kind=ID;
            strcpy(result.id,symbolTable.symbols[T->place].alias);
            result.offset=symbolTable.symbols[T->place].offset;
            //生成 result(newAlias)=opn1;
            T->code=genIR(ASSIGNOP,opn1,opn2,result);
            T->width=4;
            break;
        case CHAR:
            T->place=fill_Temp(newTemp(),LEV,T->type,'T',T->offset); //为字符常量生成一个临时变量
            T->type=CHAR;
            opn1.kind=CHAR;
            opn1.const_int=T->type_int;
            result.kind=ID;
            strcpy(result.id,symbolTable.symbols[T->place].alias);
            result.offset=symbolTable.symbols[T->place].offset;
            T->code=genIR(ASSIGNOP,opn1,opn2,result);
            T->width=1;
            break;
        case FLOAT:
            T->place=fill_Temp(newTemp(),LEV,T->type,'T',T->offset);   //为浮点常量生成一个临时变量
            T->type=FLOAT;
            opn1.kind=FLOAT;
            opn1.const_float=T->type_float;
            result.kind=ID;
            strcpy(result.id,symbolTable.symbols[T->place].alias);
            result.offset=symbolTable.symbols[T->place].offset;
            T->code=genIR(ASSIGNOP,opn1,opn2,result);
            T->width=4;
            break;
        case ASSIGNOP:
            
                Exp(T->ptr[0]);
                T->ptr[1]->offset = T->offset;
                Exp(T->ptr[1]);
               
                if(T->ptr[0]->type != T->ptr[1]->type)
                {
                    semantic_error(T->pos, T->type_id, "赋值运算两边类型不匹配");
                }
                else
                {
                    Exp(T->ptr[0]); //处理左值，例中仅为变量
                    T->ptr[1]->offset = T->offset;
                    Exp(T->ptr[1]);
                    T->type = T->ptr[0]->type;
                    T->width = T->ptr[1]->width;
                    T->code = merge(2, T->ptr[0]->code, T->ptr[1]->code);
                    opn1.kind = ID;
                    strcpy(opn1.id, symbolTable.symbols[T->ptr[1]->place].alias); //右值一定是个变量或临时变量
                    opn1.offset = symbolTable.symbols[T->ptr[1]->place].offset;
                    result.kind = ID;
                    strcpy(result.id, symbolTable.symbols[T->ptr[0]->place].alias);
                    result.offset = symbolTable.symbols[T->ptr[0]->place].offset;
                    T->code = merge(2, T->code, genIR(ASSIGNOP, opn1, opn2, result));
                }
          
            break;
        //不允许在非控制语句中出现&& ||等运算符,控制语句的exp都由boolexp处理了，这里直接报错
        case AND:  
        case OR:    
        case RELOP: 
            semantic_error(T->pos,T->type_id, "不支持布尔值和其他数据类型的转换");
            break;
        case COMPASSIGN:
            T->ptr[0]->offset=T->offset;
            // 要求第一个子exp是ID，需要做检查


            if (T->ptr[0]->kind!=ID )
            {
                semantic_error(T->pos,T->type_id, "复合赋值操作需要一个左值");
            }

            int type = (!strcmp(T->type_id,"+="))? PLUS:(!strcmp(T->type_id,"-="))?MINUS:(!strcmp(T->type_id,"*="))?STAR:DIV;
            // 将当前的节点替换成一个ASSIGNOP节点
            // x=x+exp

            struct node * temp_op = mknode(type,T->ptr[0],T->ptr[1],NULL,T->pos);
            strcpy(temp_op->type_id,(!strcmp(T->type_id,"+="))? "PLUS":(!strcmp(T->type_id,"-="))?"MINUS":(!strcmp(T->type_id,"*="))?"STAR":"DIV");
            T->parent->ptr[T->seq_num]=mknode(ASSIGNOP,T->ptr[0],temp_op,NULL,T->pos);
            strcpy(T->type_id,"ASSIGNOP");
            T=T->parent->ptr[T->seq_num];
            T->code=NULL;
            Exp(T);//调用时生成code
            // prnIR(T->code);
            // display(T,T->level);
            break;
        case PLUS:
        case MINUS:
        case STAR:
        case DIV:
            T->ptr[0]->offset=T->offset;
            Exp(T->ptr[0]);
            T->ptr[1]->offset=T->offset+T->ptr[0]->width;
            Exp(T->ptr[1]);
            if (T->ptr[0]->type!=T->ptr[1]->type)
            {
                semantic_error(T->pos,T->type_id, "运算操作左右类型不一致");
            }
            //判断T->ptr[0]，T->ptr[1]类型是否正确，可能根据运算符生成不同形式的代码，给T的type赋值
            //下面的类型属性计算，没有考虑错误处理情况
            if (T->ptr[0]->type==FLOAT || T->ptr[1]->type==FLOAT)
                T->type=FLOAT,T->width=T->ptr[0]->width+T->ptr[1]->width+4;
            else
                T->type=INT,T->width=T->ptr[0]->width+T->ptr[1]->width+4;
            T->place=fill_Temp(newTemp(),LEV,T->type,'T',T->offset+T->ptr[0]->width+T->ptr[1]->width);
            opn1.kind=ID;
            strcpy(opn1.id,symbolTable.symbols[T->ptr[0]->place].alias);
            opn1.type=T->ptr[0]->type;
            opn1.offset=symbolTable.symbols[T->ptr[0]->place].offset;
            opn2.kind=ID;
            strcpy(opn2.id,symbolTable.symbols[T->ptr[1]->place].alias);
            opn2.type=T->ptr[1]->type;
            opn2.offset=symbolTable.symbols[T->ptr[1]->place].offset;
            result.kind=ID;
            strcpy(result.id,symbolTable.symbols[T->place].alias);
            result.type=T->type;
            result.offset=symbolTable.symbols[T->place].offset;
            T->code=merge(3,T->ptr[0]->code,T->ptr[1]->code,genIR(T->kind,opn1,opn2,result));
            //  T->width=T->ptr[0]->width+T->ptr[1]->width+(T->type==INT?4:8);
            break;
        case NOT:
            semantic_error(T->pos,T->type_id, "不支持布尔值和其他数据类型的转换");
            break;
        //只支持前置自增自减
        case PLUSONE:
        case MINUSONE:
            T->ptr[0]->offset=T->offset;
            // 要求子exp是ID，需要做检查
            if (T->ptr[0]->kind!=ID)
            {
                semantic_error(T->pos,T->type_id, "自增自减操作需要一个左值");
                break;
            }
            Exp(T->ptr[0]); //处理ID语句，从符号表查找变量类型等
            if ((T->ptr[0]->type)!=INT )
            {
                semantic_error(T->pos,T->type_id, "只有整型数据支持自增自减操作");
                break;
            }
            else
                T->width=T->ptr[0]->width+4;

            // 将当前的节点替换成一个ASSIGNOP节点
            // x=x+1
            struct node * temp1 = mknode(INT,NULL,NULL,NULL,T->pos);
            temp1->type_int=1;
            T->type=INT;
            struct node * temp_add = mknode((T->kind==PLUSONE)? PLUS:MINUS,T->ptr[0],temp1,NULL,T->pos);
            strcpy(temp_add->type_id,(T->kind==PLUSONE)?"PLUS":"MINUS");

            T->parent->ptr[T->seq_num]=mknode(ASSIGNOP,T->ptr[0],temp_add,NULL,T->pos);
            strcpy(T->type_id,"ASSIGNOP");
            T=T->parent->ptr[T->seq_num];
            T->code=NULL;
            Exp(T);//调用时，将临时变量 INT 1 加入符号表
            // prnIR(T->code);
            // display(T,T->level);
            break;

        case UMINUS:
            T->ptr[0]->offset=T->offset;
            // 要求子exp是计算，需要做检查
            if (T->ptr[0]->kind!=ID)
            {
                semantic_error(T->pos,T->type_id, "单运算符减操作需要一个左值");
                break;
            }
            Exp(T->ptr[0]);
            if (T->ptr[0]->type==FLOAT )
                T->type=FLOAT,T->width=T->ptr[0]->width+4;
            else if(T->ptr[0]->type==CHAR )
            {
                semantic_error(T->pos,T->type_id, "字符型数据不能为负值");
                break;
            }
            else
                T->type=INT,T->width=T->ptr[0]->width+4;

            T->place=fill_Temp(newTemp(),LEV,T->type,'T',T->offset+T->ptr[0]->width);
            opn1.kind=ID;
            strcpy(opn1.id,symbolTable.symbols[T->ptr[0]->place].alias);
            opn1.type=T->ptr[0]->type;
            opn1.offset=symbolTable.symbols[T->ptr[0]->place].offset;
            result.kind=ID;
            strcpy(result.id,symbolTable.symbols[T->place].alias);
            result.type=T->type;
            result.offset=symbolTable.symbols[T->place].offset;
            T->code=merge(2,T->ptr[0]->code,genIR(UMINUS,opn1,opn2,result));
            break;
       case FUNC_CALL: //根据T->type_id查出函数的定义，如果语言中增加了实验教材的read，write需要单独处理一下
            rtn = searchSymbolTable(T->type_id);
            if (rtn == -1)
            {
                semantic_error(T->pos, T->type_id, "函数未定义");
                break;
            }
            if (symbolTable.symbols[rtn].flag != 'F')
            {
                semantic_error(T->pos, T->type_id, "不是一个函数");
                break;
            }
            T->type = symbolTable.symbols[rtn].type;
            width = T->type == INT ? 4 : (T->type==FLOAT ? 4 : 1); //存放函数返回值的单数字节数
            if (T->ptr[0])
            {
                T->ptr[0]->offset = T->offset;
                Exp(T->ptr[0]);                      //处理所有实参表达式求值，及类型
                T->width = T->ptr[0]->width + width; //累加上计算实参使用临时变量的单元数
                T->code = T->ptr[0]->code;
            }
            else
            {
                T->width = width;
                T->code = NULL;
            }
            match_param(rtn, T->ptr[0]); //处理所以参数的匹配
                //处理参数列表的中间代码
            T0 = T->ptr[0];
            while (T0)
            {
                result.kind = ID;
                strcpy(result.id, symbolTable.symbols[T0->ptr[0]->place].alias);
                result.offset = symbolTable.symbols[T0->ptr[0]->place].offset;
                T->code = merge(2, T->code, genIR(ARG, opn1, opn2, result));
                T0 = T0->ptr[1];
            }
            T->place = fill_Temp(newTemp(), LEV, T->type, 'T', T->offset + T->width - width);
            opn1.kind = ID;
            strcpy(opn1.id, T->type_id); //保存函数名
            opn1.offset = rtn;           //这里offset用以保存函数定义入口,在目标代码生成时，能获取相应信息
            result.kind = ID;
            strcpy(result.id, symbolTable.symbols[T->place].alias);
            result.offset = symbolTable.symbols[T->place].offset;
            T->code = merge(2, T->code, genIR(CALL, opn1, opn2, result)); //生成函数调用中间代码
            break;
        case ARGS: //此处仅处理各实参表达式的求值的代码序列，不生成ARG的实参系列
            T->ptr[0]->offset = T->offset;
            Exp(T->ptr[0]);
            T->width = T->ptr[0]->width;
            T->code = T->ptr[0]->code;
            if (T->ptr[1])
            {
                T->ptr[1]->offset = T->offset + T->ptr[0]->width;
                Exp(T->ptr[1]);
                T->width += T->ptr[1]->width;
                T->code = merge(2, T->code, T->ptr[1]->code);
            }
            break;
        }
    }
}


void semantic_Analysis(struct node *T)
{
    //对抽象语法树的先根遍历,按display的控制结构修改完成符号表管理和语义检查和TAC生成（语句部分）
    int rtn,num,width;
    struct node *T0;
    struct opn opn1,opn2,result;
    if (T)
    {
        switch (T->kind)
        {
        case EXT_DEF_LIST:
            if (!T->ptr[0])
                break;
            T->ptr[0]->offset=T->offset;
            semantic_Analysis(T->ptr[0]);    //访问外部定义列表中的第一个
            T->code=T->ptr[0]->code;
            if (T->ptr[1])
            {
                T->ptr[1]->offset=T->ptr[0]->offset+T->ptr[0]->width;
                semantic_Analysis(T->ptr[1]); //访问该外部定义列表中的其它外部定义
                T->code=merge(2,T->code,T->ptr[1]->code);
            }
            break;
//ExtDef:   Specifier ExtDecList SEMI   {$$=mknode(EXT_VAR_DEF,$1,$2,NULL,yylineno);}
        case EXT_VAR_DEF:   //处理外部说明,将第一个孩子(TYPE结点)中的类型送到第二个孩子的类型域
            T->type=T->ptr[1]->type=(!strcmp(T->ptr[0]->type_id,"int"))?INT:(!strcmp(T->ptr[0]->type_id,"char")) ? CHAR: FLOAT;
            T->ptr[1]->offset=T->offset;        //这个外部变量的偏移量向下传递
            T->ptr[1]->width=(T->type==CHAR)?1:4;  //将一个变量的宽度向下传递

            ext_var_list(T->ptr[1]);     //处理外部变量说明中的标识符序列 ExtDecList
            T->width=((T->type==CHAR)?1:4)* T->ptr[1]->num;  //计算这个外部变量说明的宽度
            T->code=NULL;             //这里假定外部变量不支持初始化
            break;
        case FUNC_DEF:      //填写函数定义信息到符号表
            T->ptr[1]->type=!strcmp(T->ptr[0]->type_id,"int")?INT:FLOAT;//获取函数返回类型送到含函数名、参数的结点
            T->width=0;     //函数的宽度设置为0，不会对外部变量的地址分配产生影响
            T->offset=DX;   //设置局部变量在活动记录中的偏移量初值
            semantic_Analysis(T->ptr[1]); //处理函数名和参数结点部分，这里不考虑用寄存器传递参数
            T->offset+=T->ptr[1]->width;   //用形参单元宽度修改函数局部变量的起始偏移量
            T->ptr[2]->offset=T->offset;
            strcpy(T->ptr[2]->Snext,newLabel());  //函数体语句执行结束后的位置属性
            semantic_Analysis(T->ptr[2]);         //处理函数体结点
            //计算活动记录大小,这里offset属性存放的是活动记录大小，不是偏移
            symbolTable.symbols[T->ptr[1]->place].offset=T->offset+T->ptr[2]->width;
            T->code=merge(3,T->ptr[1]->code,T->ptr[2]->code,genLabel(T->ptr[2]->Snext));          //函数体的代码作为函数的代码
            break;
        case FUNC_DEC:      //根据返回类型，函数名填写符号表
            rtn=fillSymbolTable(T->type_id,newAlias(),LEV,T->type,'F',0);//函数不在数据区中分配单元，偏移量为0
            if (rtn==-1)
            {
                semantic_error(T->pos,T->type_id, "函数重复定义");
                break;
            }
            else
                T->place=rtn;
            result.kind=ID;
            strcpy(result.id,T->type_id);
            result.offset=rtn;
            T->code=genIR(FUNCTION,opn1,opn2,result);     //生成中间代码：FUNCTION 函数名
            T->offset=DX;   //设置形式参数在活动记录中的偏移量初值
            if (T->ptr[0])   //判断是否有参数
            {
                T->ptr[0]->offset=T->offset;
                semantic_Analysis(T->ptr[0]);  //处理函数参数列表
                T->width=T->ptr[0]->width;
                symbolTable.symbols[rtn].paramnum=T->ptr[0]->num;
                T->code=merge(2,T->code,T->ptr[0]->code);  //连接函数名和参数代码序列
            }
            else
                symbolTable.symbols[rtn].paramnum=0,T->width=0;
            break;
        case PARAM_LIST:    //处理函数形式参数列表
            T->ptr[0]->offset=T->offset;
            semantic_Analysis(T->ptr[0]);
            if (T->ptr[1])
            {
                T->ptr[1]->offset=T->offset+T->ptr[0]->width;
                semantic_Analysis(T->ptr[1]);
                T->num=T->ptr[0]->num+T->ptr[1]->num;        //统计参数个数
                T->width=T->ptr[0]->width+T->ptr[1]->width;  //累加参数单元宽度
                T->code=merge(2,T->ptr[0]->code,T->ptr[1]->code);  //连接参数代码
            }
            else
            {
                T->num=T->ptr[0]->num;
                T->width=T->ptr[0]->width;
                T->code=T->ptr[0]->code;
            }
            break;
        case  PARAM_DEC:
            rtn=fillSymbolTable(T->ptr[1]->type_id,newAlias(),1,T->ptr[0]->type,'P',T->offset);
            if (rtn==-1)
                semantic_error(T->ptr[1]->pos,T->ptr[1]->type_id, "参数名重复定义");
            else
                T->ptr[1]->place=rtn;
            T->num=1;       //参数个数计算的初始值
            T->width=T->ptr[0]->type==CHAR?1:4;  //参数宽度
            result.kind=ID;
            strcpy(result.id, symbolTable.symbols[rtn].alias);
            result.offset=T->offset;
            T->code=genIR(PARAM,opn1,opn2,result);     //生成：FUNCTION 函数名
            break;
        case COMP_STM:
            LEV++;
            //设置层号加1，并且保存该层局部变量在符号表中的起始位置在symbol_scope_TX
            symbol_scope_TX.TX[symbol_scope_TX.top++]=symbolTable.index;
            T->width=0;
            T->code=NULL;
            if (T->ptr[0])
            {
                T->ptr[0]->offset=T->offset;
                semantic_Analysis(T->ptr[0]);  //处理该层的局部变量DEF_LIST
                T->width+=T->ptr[0]->width;
                T->code=T->ptr[0]->code;
            }
            if (T->ptr[1])
            {
                T->ptr[1]->offset=T->offset+T->width;
                strcpy(T->ptr[1]->Snext,T->Snext);  //S.next属性向下传递
                semantic_Analysis(T->ptr[1]);       //处理复合语句的语句序列
                T->width+=T->ptr[1]->width;
                T->code=merge(2,T->code,T->ptr[1]->code);
            }
            //prn_symbol();       //c在退出一个符合语句前显示的符号表
            LEV--;    //出复合语句，层号减1
            symbolTable.index=symbol_scope_TX.TX[--symbol_scope_TX.top]; //删除该作用域中的符号
            break;
        case DEF_LIST:
            T->code=NULL;
            if (T->ptr[0])
            {
                T->ptr[0]->offset=T->offset;
                semantic_Analysis(T->ptr[0]);   //处理一个局部变量定义
                T->code=T->ptr[0]->code;
                T->width=T->ptr[0]->width;
            }
            if (T->ptr[1])
            {
                T->ptr[1]->offset=T->offset+T->ptr[0]->width;
                semantic_Analysis(T->ptr[1]);   //处理剩下的局部变量定义
                T->code=merge(2,T->code,T->ptr[1]->code);
                T->width+=T->ptr[1]->width;
            }
            break;
        case VAR_DEF://处理一个局部变量定义,将第一个孩子(TYPE结点)中的类型送到第二个孩子的类型域
            //类似于上面的外部变量EXT_VAR_DEF，换了一种处理方法
            T->code=NULL;
            T->ptr[1]->type=(!strcmp(T->ptr[0]->type_id,"int"))?INT:(!strcmp(T->ptr[0]->type_id,"char")) ? CHAR:FLOAT;  //确定变量序列各变量类型
            T0=T->ptr[1]; //T0为变量名列表子树根指针，对ID、ASSIGNOP类结点在登记到符号表，作为局部变量
            num=0;
            T0->offset=T->offset;
            T->width=0;
            width=T->ptr[1]->type==CHAR?1:4;  //一个变量宽度
            while (T0)    //处理所以DEC_LIST结点
            {
                num++;
                T0->ptr[0]->type=T0->type;  //类型属性向下传递
                if (T0->ptr[1])
                    T0->ptr[1]->type=T0->type;
                T0->ptr[0]->offset=T0->offset;  //类型属性向下传递
                if (T0->ptr[1])
                    T0->ptr[1]->offset=T0->offset+width;
                if (T0->ptr[0]->kind==ID)
                {
                    rtn=fillSymbolTable(T0->ptr[0]->type_id,newAlias(),LEV,T0->ptr[0]->type,'V',T->offset+T->width);//此处偏移量未计算，暂时为0
                    if (rtn==-1)
                        semantic_error(T0->ptr[0]->pos,T0->ptr[0]->type_id, "变量重复定义");
                    else
                        T0->ptr[0]->place=rtn;
                    T->width+=width;
                }
                else if (T0->ptr[0]->kind==ASSIGNOP)
                {
                    rtn=fillSymbolTable(T0->ptr[0]->ptr[0]->type_id,newAlias(),LEV,T0->ptr[0]->type,'V',T->offset+T->width);//此处偏移量未计算，暂时为0
                    if (rtn==-1)
                        semantic_error(T0->ptr[0]->ptr[0]->pos,T0->ptr[0]->ptr[0]->type_id, "变量重复定义");
                    else
                    {
                        T0->ptr[0]->place=rtn;
                        T0->ptr[0]->ptr[1]->offset=T->offset+T->width+width;
                        Exp(T0->ptr[0]->ptr[1]);
                        opn1.kind=ID;
                        strcpy(opn1.id,symbolTable.symbols[T0->ptr[0]->ptr[1]->place].alias);
                        result.kind=ID;
                        strcpy(result.id,symbolTable.symbols[T0->ptr[0]->place].alias);
                        T->code=merge(3,T->code,T0->ptr[0]->ptr[1]->code,genIR(ASSIGNOP,opn1,opn2,result));
                    }
                    T->width+=width+T0->ptr[0]->ptr[1]->width;
                }
                T0=T0->ptr[1];
            }
            break;
        case STM_LIST:
            if (!T->ptr[0])
            {
                T->code=NULL;    //空语句序列
                T->width=0;
                break;
            }
            if (T->ptr[1]) //2条以上语句连接，生成新标号作为第一条语句结束后到达的位置
                strcpy(T->ptr[0]->Snext,newLabel());
            else     //语句序列仅有一条语句，S.next属性向下传递
                strcpy(T->ptr[0]->Snext,T->Snext);
            T->ptr[0]->offset=T->offset;
            semantic_Analysis(T->ptr[0]);
            T->code=T->ptr[0]->code;
            T->width=T->ptr[0]->width;
            if (T->ptr[1])      //2条以上语句连接,S.next属性向下传递
            {
                strcpy(T->ptr[1]->Snext,T->Snext);
                T->ptr[1]->offset=T->offset+T->ptr[0]->width; //顺序结构顺序分配单元方式
                semantic_Analysis(T->ptr[1]);
                //序列中第1条为表达式语句，返回语句，复合语句时，第2条前不需要标号
                if (T->ptr[0]->kind==RETURN ||T->ptr[0]->kind==EXP_STMT ||T->ptr[0]->kind==COMP_STM)
                    T->code=merge(2,T->code,T->ptr[1]->code);
                else
                    T->code=merge(3,T->code,genLabel(T->ptr[0]->Snext),T->ptr[1]->code);
                T->width+=T->ptr[1]->width;//顺序结构共享单元方式

            }
            break;
        case IF_THEN:
            strcpy(T->ptr[0]->Etrue,newLabel());  //设置条件语句真假转移位置
            strcpy(T->ptr[0]->Efalse,T->Snext);
            T->ptr[0]->offset=T->offset;
            boolExp(T->ptr[0]);
            T->width=T->ptr[0]->width;
            T->ptr[1]->offset=T->offset+T->ptr[0]->width;
            strcpy(T->ptr[1]->Snext,T->Snext);
            semantic_Analysis(T->ptr[1]);      //if子句
            T->width+=T->ptr[1]->width;
            T->code=merge(3,T->ptr[0]->code, genLabel(T->ptr[0]->Etrue),T->ptr[1]->code);
            break;  //控制语句都还没有处理offset和width属性
        case IF_THEN_ELSE:
            strcpy(T->ptr[0]->Etrue,newLabel());  //设置条件语句真假转移位置
            strcpy(T->ptr[0]->Efalse,newLabel());
            //???
            T->ptr[0]->offset=T->offset;
            boolExp(T->ptr[0]);      //条件，要单独按短路代码处理
            T->width=T->ptr[0]->width;
            //???
            T->ptr[1]->offset=T->offset+T->ptr[0]->width;
            strcpy(T->ptr[1]->Snext,T->Snext);
            semantic_Analysis(T->ptr[1]);      //if子句
            //???
            T->width+=T->ptr[1]->width;
            T->ptr[2]->offset=T->ptr[1]->offset+T->ptr[1]->width;
            strcpy(T->ptr[2]->Snext,T->Snext);
            semantic_Analysis(T->ptr[2]);      //else子句
            T->width+=T->ptr[2]->width;
            T->code=merge(6,T->ptr[0]->code,genLabel(T->ptr[0]->Etrue),T->ptr[1]->code,\
                          genGoto(T->Snext),genLabel(T->ptr[0]->Efalse),T->ptr[2]->code);
            break;
        case WHILE:
            strcpy(T->ptr[0]->Etrue,newLabel());  //子结点继承属性的计算
            strcpy(T->ptr[0]->Efalse,T->Snext);
            T->ptr[0]->offset=T->offset;
            boolExp(T->ptr[0]);      //循环条件，要单独按短路代码处理
            T->width=T->ptr[0]->width;
            //???
            T->ptr[1]->offset=T->offset+T->ptr[0]->width;
            strcpy(T->ptr[1]->Snext,newLabel());
            semantic_Analysis(T->ptr[1]);      //循环体
            //if (T->width<T->ptr[1]->width)
              //  T->width=T->ptr[1]->width;
            T->width+=T->ptr[1]->width;
            T->code=merge(5,genLabel(T->ptr[1]->Snext),T->ptr[0]->code, \
                          genLabel(T->ptr[0]->Etrue),T->ptr[1]->code,genGoto(T->ptr[1]->Snext));
            break;
        case EXP_STMT:
            T->ptr[0]->offset=T->offset;
            semantic_Analysis(T->ptr[0]);
            T->code=T->ptr[0]->code;
            T->width=T->ptr[0]->width;
            break;
        case RETURN:
            if (T->ptr[0])
            {
                T->ptr[0]->offset=T->offset;
                Exp(T->ptr[0]);
                num=symbolTable.index;
                do
                    num--;
                while (symbolTable.symbols[num].flag!='F');
                if (T->ptr[0]->type!=symbolTable.symbols[num].type)
                {
                    semantic_error(T->pos, "返回值类型错误","");
                    T->width=0;
                    T->code=NULL;
                    break;
                }
                T->width=T->ptr[0]->width;
                result.kind=ID;
                strcpy(result.id,symbolTable.symbols[T->ptr[0]->place].alias);
                result.offset=symbolTable.symbols[T->ptr[0]->place].offset;
                T->code=merge(2,T->ptr[0]->code,genIR(RETURN,opn1,opn2,result));
            }
            else
            {
                T->width=0;
                result.kind=0;
                T->code=genIR(RETURN,opn1,opn2,result);
            }
            break;
        case ID:
        case INT:
        case FLOAT:
        case ASSIGNOP:
        case PLUSONE:
        case MINUSONE:
        case COMPASSIGN:
        case AND:
        case OR:
        case RELOP:
        case PLUS:
        case MINUS:
        case STAR:
        case DIV:
        case NOT:
        case UMINUS:
        case FUNC_CALL:
            Exp(T);          //处理基本表达式
            break;
        }
    }
}


void objectCode(struct codenode *head)
{
    char opnstr1[32],opnstr2[32],resultstr[32];
    struct codenode *h=head,*p;
    int i;
    FILE *fp;
    fp=fopen("objectcode.s","w");
    fprintf(fp,".data\n");
    fprintf(fp,"_Prompt: .asciiz \"Enter an integer:  \"\n");
    fprintf(fp,"_ret: .asciiz \"\\n\"\n");
    fprintf(fp,".globl main\n");
    fprintf(fp,".text\n");
    fprintf(fp,"read:\n");
    fprintf(fp,"  li $v0,4\n");
    fprintf(fp,"  la $a0,_Prompt\n");
    fprintf(fp,"  syscall\n");
    fprintf(fp,"  li $v0,5\n");
    fprintf(fp,"  syscall\n");
    fprintf(fp,"  jr $ra\n");
    fprintf(fp,"write:\n");
    fprintf(fp,"  li $v0,1\n");
    fprintf(fp,"  syscall\n");
    fprintf(fp,"  li $v0,4\n");
    fprintf(fp,"  la $a0,_ret\n");
    fprintf(fp,"  syscall\n");
    fprintf(fp,"  move $v0,$0\n");
    fprintf(fp,"  jr $ra\n");
    do {
        switch (h->op) {
            case ASSIGNOP:
                        if (h->opn1.kind==INT)
                            fprintf(fp, "  li $t3, %d\n", h->opn1.const_int);
                        else {
                            fprintf(fp, "  lw $t1, %d($sp)\n", h->opn1.offset);
                            fprintf(fp, "  move $t3, $t1\n");
                            }
                        fprintf(fp, "  sw $t3, %d($sp)\n", h->result.offset);
                        break;
            case PLUS:
            case MINUS:
            case STAR:
            case DIV:
                       fprintf(fp, "  lw $t1, %d($sp)\n", h->opn1.offset);
                       fprintf(fp, "  lw $t2, %d($sp)\n", h->opn2.offset);
                       if (h->op==PLUS)       fprintf(fp, "  add $t3,$t1,$t2\n");
                       else if (h->op==MINUS) fprintf(fp, "  sub $t3,$t1,$t2\n");
                            else if (h->op==STAR)  fprintf(fp, "  mul $t3,$t1,$t2\n");
                                 else  {fprintf(fp, "  div $t1, $t2\n");
                                        fprintf(fp, "  mflo $t3\n");
                                        }
                        fprintf(fp, "  sw $t3, %d($sp)\n", h->result.offset);
                        break;
            case FUNCTION:
                        fprintf(fp, "\n%s:\n", h->result.id);
                        if (!strcmp(h->result.id,"main"))
                            fprintf(fp, "  addi $sp, $sp, -%d\n",symbolTable.symbols[h->result.offset].offset);
                        break;
            case PARAM:
                        break;
            case LABEL: fprintf(fp, "%s:\n", h->result.id);
                        break;
            case GOTO:  fprintf(fp, "  j %s\n", h->result.id);
                        break;
            case JLE:
            case JLT:
            case JGE:
            case JGT:
            case EQ:
            case NEQ:
                        fprintf(fp, "  lw $t1, %d($sp)\n", h->opn1.offset);
                        fprintf(fp, "  lw $t2, %d($sp)\n", h->opn2.offset);
                        if (h->op==JLE) fprintf(fp, "  ble $t1,$t2,%s\n", h->result.id);
                        else if (h->op==JLT) fprintf(fp, "  blt $t1,$t2,%s\n", h->result.id);
                        else if (h->op==JGE) fprintf(fp, "  bge $t1,$t2,%s\n", h->result.id);
                        else if (h->op==JGT) fprintf(fp, "  bgt $t1,$t2,%s\n", h->result.id);
                        else if (h->op==EQ)  fprintf(fp, "  beq $t1,$t2,%s\n", h->result.id);
                        else                 fprintf(fp, "  bne $t1,$t2,%s\n", h->result.id);
                        break;
            case ARG:
                        break;
            case CALL:  if (!strcmp(h->opn1.id,"read")){
                            fprintf(fp, "  addi $sp, $sp, -4\n");
                            fprintf(fp, "  sw $ra,0($sp)\n");
                            fprintf(fp, "  jal read\n");
                            fprintf(fp, "  lw $ra,0($sp)\n");
                            fprintf(fp, "  addi $sp, $sp, 4\n");
                            fprintf(fp, "  sw $v0, %d($sp)\n",h->result.offset);
                            break;
                            }
                        if (!strcmp(h->opn1.id,"write")){
                            fprintf(fp, "  lw $a0, %d($sp)\n",h->prior->result.offset);
                            fprintf(fp, "  addi $sp, $sp, -4\n");
                            fprintf(fp, "  sw $ra,0($sp)\n");
                            fprintf(fp, "  jal write\n");
                            fprintf(fp, "  lw $ra,0($sp)\n");
                            fprintf(fp, "  addi $sp, $sp, 4\n");
                            break;
                            }

                        for(p=h,i=0;i<symbolTable.symbols[h->opn1.offset].paramnum;i++)
                                p=p->prior;

                        fprintf(fp, "  move $t0,$sp\n");
                        fprintf(fp, "  addi $sp, $sp, -%d\n", symbolTable.symbols[h->opn1.offset].offset);
                        fprintf(fp, "  sw $ra,0($sp)\n");
                        i=h->opn1.offset+1;
                        while (symbolTable.symbols[i].flag=='P')
                            {
                            fprintf(fp, "  lw $t1, %d($t0)\n", p->result.offset);
                            fprintf(fp, "  move $t3,$t1\n");
                            fprintf(fp, "  sw $t3,%d($sp)\n",  symbolTable.symbols[i].offset);
                            p=p->next; i++;
                            }
                        fprintf(fp, "  jal %s\n",h->opn1.id);
                        fprintf(fp, "  lw $ra,0($sp)\n");
                        fprintf(fp, "  addi $sp,$sp,%d\n",symbolTable.symbols[h->opn1.offset].offset);
                        fprintf(fp, "  sw $v0,%d($sp)\n", h->result.offset);
                        break;
            case RETURN:fprintf(fp, "  lw $v0,%d($sp)\n",h->result.offset);
                        fprintf(fp, "  jr $ra\n");
                        break;

        }
    h=h->next;
    } while (h!=head);
fclose(fp);
}

void semantic_Analysis0(struct node *T)
{
    symbolTable.index=0;
    fillSymbolTable("read","",0,INT,'F',4);
    symbolTable.symbols[0].paramnum=0;//read的形参个数
    fillSymbolTable("write","",0,INT,'F',4);
    symbolTable.symbols[1].paramnum=1;
    fillSymbolTable("x","",1,INT,'P',12);
    symbol_scope_TX.TX[0]=0;  //外部变量在符号表中的起始序号为0
    symbol_scope_TX.top=1;
    T->offset=0;              //外部变量在数据区的偏移量

    semantic_Analysis(T);
    //prnIR(T->code);
    objectCode(T->code);
}