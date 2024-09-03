#include "include/OptSAT.h"

int maxLiteralNum;

void test3(){
    printf("---OptSAT---\n");
}

Answer OptDPLLSolution(Formular& formular){
    Answer ans;
    ans.numBoolen = formular.numBoolen;
    ans.state = (int*) malloc(sizeof(int) * (formular.numBoolen + 1));
    for(int i = 0; i <= formular.numBoolen; i ++)
        ans.state[i] = UNKNOWN;
    ans.solved = false;
    maxLiteralNum = formular.numBoolen;
    OptDPLL(formular, ans);
    destroyFormular(formular);
    return ans;
}

inline int value(LiteralNums a){
    return a.Num;
}


int chooseLiteral(Formular formular){
    LiteralNums *litsNums = (LiteralNums*) malloc(sizeof(LiteralNums) * (maxLiteralNum + 1));
    for(int i = 0; i <= maxLiteralNum; i ++){
        litsNums[i].LiteralName = i;
        litsNums[i].Num = 0;
    }
    Clause* s = formular.root;
    while(s){
        Literal* p = s->firstLiteral;
        while(p){
            litsNums[p->data].Num ++;
            if(p->is_negative)
                litsNums[p->data].NegativeNum ++;
            else
                litsNums[p->data].PositiveNum ++;
            p = p->next;
        }
        s = s->nextClause;
    }
    int i = 0;
    int maxNum = 0;
    for(int j = 1; j <= maxLiteralNum; j ++){
        if(value(litsNums[j]) > maxNum){
            maxNum = value(litsNums[j]);
            i = j;
        }
    }
    free(litsNums);
    return i;
}

void OptDPLL(Formular &formular, Answer& ans){
    Clause* unitClause;
    maxLiteralNum = maxLiteralNum > formular.numBoolen ? maxLiteralNum : formular.numBoolen;
    while(unitClause = UnitClauseLeft(formular)){
        int data = unitClause->firstLiteral->data;
        bool is_negative = unitClause->firstLiteral->is_negative;
        ans.state[data] = is_negative ? NEGATIVE : POSITIVE;
        Clause *s = formular.root;

        //依据单子句规则简化公式
        while(s){
            //如果子句为空则公式不可满足
            if(!s->firstLiteral){
                ans.solved = false;
                return;
            }

            //如果子句中存在单子句则删除该子句
            //当子句第一个变元为单子句时
            if(s->firstLiteral->data == data && s->firstLiteral->is_negative == is_negative){
                Clause* p = s;
                s = s->nextClause;
                destroyClause(formular, p);
            }
                //当子句第一个变元为相反的单子句时
            else if(s->firstLiteral->data == data && s->firstLiteral->is_negative != is_negative) {
                while(s->firstLiteral && s->firstLiteral->data == data && s->firstLiteral->is_negative != is_negative){
                    pLiteral p = s->firstLiteral;
                    s->firstLiteral = p->next;
                    free(p);
                }
                if(s->firstLiteral == NULL){
                    ans.solved = false;
                    return;
                }
                bool next = false;
                if(s->firstLiteral == NULL){
                    ans.solved = false;
                    return;
                } else{
                    pLiteral p = s->firstLiteral;
                    while(p->next){
                        if(p->next->data == data && p->next->is_negative == is_negative){
                            Clause* delClause = s;
                            s = s->nextClause;
                            next = true;
                            destroyClause(formular, delClause);
                            delClause = NULL;
                            p = NULL;
                            break;
                        }
                        else if(p->next->data == data && p->next->is_negative != is_negative){
                            pLiteral delLiteral = p->next;
                            p->next = delLiteral->next;
                            free(delLiteral);
                        }
                        else
                            p = p->next;
                    }
                    if(!next)
                        s = s->nextClause;
                }
            }
            else{
                pLiteral p = s->firstLiteral;
                bool next = false;
                while(p->next){
                    if(p->next->data == data && p->next->is_negative == is_negative){
                        Clause* delClause = s;
                        s = s->nextClause;
                        next = true;
                        destroyClause(formular, delClause);
                        delClause = NULL;
                        p = NULL;
                        break;
                    }
                    else if(p->next->data == data && p->next->is_negative != is_negative){
                        pLiteral delLiteral = p->next;
                        p->next = delLiteral->next;
                        free(delLiteral);
                    }
                    else
                        p = p->next;
                }
                if(!next)
                    s = s->nextClause;
            }
            if(formular.root == NULL){
                ans.solved = true;
                return;
            }
        }
        formular.numBoolen --;
    }

    //选取变元v
    pLiteral v = (pLiteral) malloc(sizeof(Literal));
    v->data = chooseLiteral(formular);
    v->is_negative = false;
    v->next = NULL;
    Clause* clause = createClause(v);
    addClause(formular, formular.root, clause);
    Formular newFormular = copyFormular(formular);
    DPLL(newFormular, ans);
    if(ans.solved)
        return;
    else{
        destroyClause(formular, clause);
        v = (pLiteral) malloc(sizeof(Literal));
        v->data = formular.root->firstLiteral->data;
        v->is_negative = true;
        v->next = NULL;
        clause = createClause(v);
        addClause(formular, formular.root, clause);
        destroyFormular(newFormular);
        newFormular = copyFormular(formular);
        DPLL(newFormular, ans);
        if(ans.solved) {
            destroyFormular(newFormular);   //若有问题则删除
            return;
        }
        else{
            ans.solved = false;
            destroyFormular(newFormular);   //若有问题则删除
            return;
        }
    }
}