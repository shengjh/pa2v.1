#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>
bool check_parentheses(int p,int q);
bool is_exec(int type);
bool is_in_parent(int p,int index);
bool priority(int p,int q);
int find_op(int p,int q);
int eval(int p,int q);
bool is_point(int index);
bool is_minus(int index);

enum {
  TK_NOTYPE = 256, TK_EQ,
	TK_LEFT,TK_RIGHT,TK_HEX,TK_DEC,
	TK_REG_NUM,TK_MINUS,TK_POINT


  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {
  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},   					 // spaces
	{"\\(", TK_LEFT},						 	//left-paren
	{"\\)", TK_RIGHT},						//right-paren
	{"\\*", '*'},				 					// mul
	{"/", 	'/'},									// div
  {"\\+", '+'},        				 // plus
	{"-", '-'},				 					 // sub
  {"==", TK_EQ},        			 // equal
	{"\\$[a-zA-Z]*", TK_REG_NUM}, 	//register-num
	{"0[xX][0-9a-fA-F]*", TK_HEX}, //hex-number
	{"[1-9][0-9]*", TK_DEC} 			//dec-number
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

	
        switch (rules[i].token_type) {
					case TK_NOTYPE: break;
          default: {
						tokens[nr_token].type = rules[i].token_type;
						//myStrcpy(tokens[nr_token].str,substr_start,substr_len);
						sprintf(tokens[nr_token].str,"%.*s",substr_len,substr_start);
						nr_token++;
					}
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
	*success = true;
	for(int i=0;i<nr_token;i++){
		if(is_minus(i))
			tokens[i].type=TK_MINUS;
		else if(is_point(i))
			tokens[i].type=TK_POINT;
	}
  return eval(0,nr_token-1);

  return 0;
}

bool is_exec(int type){
	if(type=='+' || type=='-' || type=='*' || type=='/' || type==TK_EQ)
		return true;
	else return false;
}

bool is_point(int index){
	if(tokens[index].type == '*' && index == 0)
		return true;
	else if(tokens[index].type == '*' && is_exec(tokens[index-1].type))
		return true;
	else return false;
}

bool is_minus(int index){
	if(tokens[index].type == '-' && index == 0)
		return true;
	else if(tokens[index].type == '-' && is_exec(tokens[index-1].type))
		return true;
	else return false;

}

int find_op(int p,int q){
	int i=0;
	int tok[32];
	int k=0;
	int m=0;
	tok[0]=p;
	for(i=p;i<q;i++){
		if(is_exec(tokens[i].type) && !is_in_parent(p,i)){
			Log("i=%d",i);
			tok[k]=i;
			k++;
		}
	}
	m=tok[0];
	for(i=0;i<k;i++){
	Log("m=%d i=%d",m,tok[i]);
		if(priority(m,tok[i])) 
			m = tok[i];
	}
	Log("m=%d",m);
	assert(0);
	return m;
}

int eval(int p,int q){
	Log("p= %d   q=%d",p,q);
	if(p>q)
		assert(0);
	else if(p==q){
		if(tokens[p].type == TK_HEX || tokens[p].type == TK_DEC)
			return atoi(tokens[p].str);
		else return 0;
	}
	else if(check_parentheses(p,q) == true)
		return eval(p+1,q-1);
	else{
		int op = find_op(p,q);
		Log("op= %d =%d",op,tokens[op].type);
		int val1;
		if(op!=0)
			val1 = eval(p,op-1);
		else val1=0;
		int val2 = eval(op+1,q);
		
		switch (tokens[op].type){
			case '+': return val1+val2;
			case '*': return val1*val2;
			case '-':	return val1-val2;
			case '/': return val1/val2;
			case TK_EQ: return val1==val2?1:0;
			case TK_MINUS: return -1 * val2;
			case TK_POINT: return 0;
			default:assert(0);
		}
	}
}


bool priority(int p,int q){
	bool is_priority = true;
	if((tokens[p].type=='+' || tokens[p].type=='-') && (tokens[q].type=='*' || tokens[q].type=='/'))
		is_priority = false;
	else if(tokens[p].type==TK_EQ)
		is_priority = false;
	else if(is_exec(tokens[p].type) && (is_point(q) || is_minus(q)))
		is_priority = false;
	return is_priority;
		
}

bool is_in_parent(int p,int index){
	int num_left=0,num_right=0;
	for(int i=p;i<index;i++){
		if(tokens[i].type==TK_LEFT)
			num_left++;
		else if(tokens[i].type==TK_RIGHT)
			num_right++;
	}
	if(num_left>num_right)
		return true;
	else return false;
}

bool check_parentheses(int p,int q){
	int top = -1;
	if(!(tokens[p].type==TK_LEFT &&tokens[q].type==TK_RIGHT)) return false;
	else{
		p++;
		q--;
			while(p<=q){
			if(tokens[p].type==TK_LEFT){
				top++;
			}
			if(tokens[p].type==TK_RIGHT){
				if(top==-1) assert(0);
				else top--;
			}
			p++;
		}
		if(top != -1) assert(0);
		return true;
	}
}

