/*
 * SAT.cpp
 * Created by Yujie Zhou
 * date: 2024-9-4
 */
#include "include/OptSAT.h"

int maxLiteralNum;

/*
 * 优化的DPLL算法进行SAT求解
 */
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

//int chooseLiteral(Formular formular, bool& is_negative){
//    int minNum = INT_MAX;
//    Clause* s = formular.root;
//    Clause* minClause = NULL;
//    while(s != NULL){
//        if(s->numLiteral < minNum) {
//            minNum = s->numLiteral;
//            minClause = s;
//        }
//        s = s->nextClause;
//    }
//    return minClause->firstLiteral->data;
//}

/*
 * 衡量文字的重要性，用于选择文字
 */
inline int value(LiteralNums a){
    return a.PositiveNum;
}

inline double valueMix(LiteralNums a){
    return (a.PositiveNum - a.NegativeNum)*0.9 + a.Num*0.1;
}

/*
 * 分支选择文字
 */
int chooseLiteral(Formular formular, bool& is_negative){
    if(formular.numClause/formular.numBoolen > 10){
        is_negative = formular.root->firstLiteral->is_negative;
        return formular.root->firstLiteral->data;
    }
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
    //double maxNum = 0;
    int maxNum = 0;
    for(int j = 1; j <= maxLiteralNum; j ++){
        if(value(litsNums[j]) > maxNum){
            maxNum = value(litsNums[j]);
            i = j;
        }
    }
    is_negative = litsNums[i].NegativeNum > litsNums[i].PositiveNum ? NEGATIVE : POSITIVE;
    free(litsNums);
    return i;
}

/*
 * 优化的DPLL算法核心
 */
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
                    s->numLiteral --;
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
                            s->numLiteral --;
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
                        s->numLiteral --;
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
    bool v_is_negative;
    int boolen_choose = chooseLiteral(formular, v_is_negative);
    v->data = boolen_choose;
    v->is_negative = v_is_negative;
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
        v->data = boolen_choose;
        v->is_negative = !v_is_negative;
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