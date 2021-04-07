/*
 * postfix.c
 *
 *  Data Structures, Homework #5
 *  Department of Computer Science at Chungbuk National University
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX_STACK_SIZE 30
#define MAX_EXPRESSION_SIZE 30


/* stack 내에서 우선순위, lparen = 0 가장 낮음 */
typedef enum{
	lparen = 0,  /* ( 왼쪽 괄호 */
	rparen = 9,  /* ) 오른쪽 괄호*/
	times = 7,   /* * 곱셈 */
	divide = 6,  /* / 나눗셈 */
	plus = 5,    /* + 덧셈 */
	minus = 4,   /* - 뺄셈 */
	operand = 1 /* 피연산자 */
} precedence; // enum을 통해 stack 안에서의 연산자 우선순위를 지정

char infixExp[MAX_EXPRESSION_SIZE];   	/* infix expression을 저장하는 배열, char형 배열이다. */
char postfixExp[MAX_EXPRESSION_SIZE];	/* postfix로 변경된 문자열을 저장하는 배열, char형 배열이다. */
char postfixStack[MAX_STACK_SIZE];	/* postfix로 변환을 위해 필요한 스택, char형 배열이다. */
int evalStack[MAX_STACK_SIZE];		/* 계산을 위해 필요한 스택, int형 배열이다. */

int postfixStackTop = -1;  /* postfixStack용 top */
int evalStackTop = -1;	   /* evalStack용 top */

int evalResult = 0;	   /* 계산 결과를 저장 */

void postfixPush(char x); // postfix 변환 위한 stack에 요소를 push하는 함수, 매개변수로 요소 값 받음
char postfixPop(); // postfix 변환 위한 stack에서 요소를 pop하는 함수, pop한 요소 값을 반환함.
void evalPush(int x); // 후위표기식의 연산을 위한 stack에 요소를 push하는 함수, 매개변수로 요소 값 받음
int evalPop(); // 후위포기식의 연산을 위한 stack에서 요소를 pop하는 함수, pop한 요소 값을 반환함.
void getInfix(); // 변환할 infix 형식의 식을 입력받는 함수이다.
precedence getToken(char symbol); // 매개변수로 받은 값에 대해 알맞는 enum값 반환함
precedence getPriority(char x); // 매개변수로 받은 값에 대해 연산 우선순위에 따라 알맞는 enum값 반환
void charCat(char* c); // postfix 변환을 위해 기존 식의 연산자/피연산자를 변경해 재나열하는 함수  
void toPostfix(); // postfix 변환을 위해 실행하는 함수
void debug(); // 프로그램 실행 과정 중의 infix 식, postfix 식, 연산 결과값, postfixStack의 값 등을 확인하는 디버그 함수
void reset(); // 입력을 다시 받을 수 있게 초기화하는 함수
void evaluation(); // postfix로 변환된 식의 결과값을 연산하는 함수

int main()
{
	char command; // 메뉴 선택갑 저장하는 변수 선언
	printf("----- [손영재] [2018038080] -----\n");


	do{
		printf("----------------------------------------------------------------\n");
		printf("               Infix to Postfix, then Evaluation               \n");
		printf("----------------------------------------------------------------\n");
		printf(" Infix=i,   Postfix=p,  Eval=e,   Debug=d,   Reset=r,   Quit=q \n");
		printf("----------------------------------------------------------------\n");

		printf("Command = ");
		scanf(" %c", &command); // 명령 입력

		switch(command) { // 입력한 명령값에 따라 알맞은 함수 실행
		case 'i': case 'I':
			getInfix();
			break;
		case 'p': case 'P':
			toPostfix();
			break;
		case 'e': case 'E':
			evaluation();
			break;
		case 'd': case 'D':
			debug();
			break;
		case 'r': case 'R':
			reset();
			break;
		case 'q': case 'Q':
			break;
		default:
			printf("\n       >>>>>   Concentration!!   <<<<<     \n"); // 메뉴 이외 값 입력시 오류메세지 출력
			break;
		}

	}while(command != 'q' && command != 'Q'); // q 입력시까지 반복 실행

	return 1;
}

void postfixPush(char x)
{
    postfixStack[++postfixStackTop] = x;  // postfix 변환 위한 stack에 push하는 함수, 전달받은 x를 stack 요소로 push함
}

char postfixPop()
{
    char x; // postfix 변환 위한 stack에서 pop하는 함수, pop한 char형 값을 반환함
    if(postfixStackTop == -1) // stack이 비어있을 경우 NULL 반환
        return '\0';
    else {
    	x = postfixStack[postfixStackTop--];
    }
    return x;
}

void evalPush(int x)
{
    evalStack[++evalStackTop] = x; // 후위표기식 연산 위한 stack에 push하는 함수, 전달받은 x를 stack 요소로 push함
}

int evalPop()
{
    if(evalStackTop == -1) // 후위표기식 연산 위한 stack에서 pop하는 함수, pop한 char형 값을 반환함
        return -1; // stack이 비어있을 경우 NULL 반환
    else
        return evalStack[evalStackTop--];
}

/**
 * infix expression을 입력받는다.
 * infixExp에는 입력된 값을 저장한다.
 */
void getInfix()
{
    printf("Type the expression >>> ");
    scanf("%s",infixExp); // 입력받은 infix 식을 저장한다
}

precedence getToken(char symbol)
{
	switch(symbol) { // 매개변수로 받은 symbol에 대해 알맞은 enum값 / 피연산자 여부 반환한다.
	case '(' : return lparen;
	case ')' : return rparen;
	case '+' : return plus;
	case '-' : return minus;
	case '/' : return divide;
	case '*' : return times;
	default : return operand;
	}
}

precedence getPriority(char x) // 우선순위 판별을 위해 getToken함수를 호출한다.
{
	return getToken(x);
}

/**
 * 문자하나를 전달받아, postfixExp에 추가
 */
void charCat(char* c)
{
	if (postfixExp == '\0')
		strncpy(postfixExp, c, 1);
	else
		strncat(postfixExp, c, 1);
}

/**
 * infixExp의 문자를 하나씩 읽어가면서 stack을 이용하여 postfix로 변경한다.
 * 변경된 postfix는 postFixExp에 저장된다.
 */
void toPostfix()
{
	/* infixExp의 문자 하나씩을 읽기위한 포인터 */
	char *exp = infixExp;
	char x; /* 문자하나를 임시로 저장하기 위한 변수 */
	precedence token; // 연산자 우선순위 저장하는  변수 token 선언

	/* exp를 증가시켜가면서, 문자를 읽고 postfix로 변경 */
	while(*exp != '\0') // 기존 식의 모든 문자에 대해 postfix로 변환할 때까지
	{
		x = *exp; // 변환 전 식의 연산자/피연산자 하나를 저장하는 변수 선언 및 대입 
		token = getToken(x); // token에 x의 우선순위를 구하여 대입
		if(token == operand) // x가 피연산자일 경우
			charCat(&x); // charCat 함수를 통해 postfix 변환식에 그대로 붙임
		else if (token == rparen) // x가 우괄호일 경우
		{
			precedence stacktoken = getToken(postfixStack[postfixStackTop]); // 현재 postfix 변환 위한 stack의 요소의 연산 우선순위 구하여
			while(stacktoken != lparen) // stack 요소가 좌괄호가 아닐 때까지
			{
				char c = postfixPop(); // char형 변수 c를 선언하여 stack에서 pop한 값 저장
				charCat(&c); // pop한 값을 charCat 함수를 통해 postfix 변환식에 그대로 붙임
				stacktoken = getToken(postfixStack[postfixStackTop]); // 좌괄호 찾을 때까지 반복
			}
			postfixPop(); // 왼쪽 괄호 버림
		}
		else
		{
			if(getPriority(postfixStack[postfixStackTop]) >= (getPriority(x) ? getPriority(x) : 10)) // stack의 top 요소가 incoming 요소보다 연산 우선순위가 높거나 같을 경우
			{
			while(getPriority(postfixStack[postfixStackTop]) >= (getPriority(x) ? getPriority(x) : 10)) // stack top 요소 우선순위가 더 높은 동안
				{
					char c = postfixPop(); // stack에서 요소 pop해 char형 변수 c에 저장
					charCat(&c); // pop한 값을 charCat 함수를 통해 postfix 변환식에 그대로 붙임
				}
				postfixPush(x); // incoming 요소 연산 우선순위가 더 높아지면 incoming 요소 x를 stack에 push함
			}
			else if (getPriority(postfixStack[postfixStackTop]) < (getPriority(x) ? getPriority(x) : 10)) // stack의 top 요소가 incoming보다 연산 우선순위가 낮은 경우
				postfixPush(x); //  incoming 요소 x를 stack에 push함
		}

		exp++; // 기존 식의 다음 연산자/피연산자를 처리하기 위해 exp++
	}

	while(1) // postfix 변환 위한 stack에 남아있는 요소가 있을 때까지
	{
		char temp = postfixPop(); // char형 변수 temp에 stack에서 pop한 값 저장
		if(temp == '\0') // stack이 비어있을 경우
			break; // 반복문 탈출
		else charCat(&temp); // 아닐 경우 pop한 값 charCat 함수 통해 postfix 변환식에 그대로 붙임
	}

	printf("%s\n",postfixExp); // 변환이 완료된 postfix 식을 출력
	printf("\n");
	/* 필요한 로직 완성 */

}
void debug() // 디버깅을 위한 infix 식, postfix 식, 후위연산자로 변환한 식 연산결과와 postfix을 위한 stack 요소값 출력
{
	printf("\n---DEBUG\n");
	printf("infixExp =  %s\n", infixExp);
	printf("postExp =  %s\n", postfixExp);
	printf("eval result = %d\n", evalResult);

	printf("postfixStack : ");
	for(int i = 0; i < MAX_STACK_SIZE; i++)
		printf("%c  ", postfixStack[i]);

	printf("\n");

}

void reset() // 새로운 infix 식의 입력을 다시 받을 수 있게 초기화하는 함수
{
	infixExp[0] = '\0';  // infix 식 저장 배열 초기화
	postfixExp[0] = '\0'; // postfix 식 저장 배열 초기화

	for(int i = 0; i < MAX_STACK_SIZE; i++)
		postfixStack[i] = '\0'; // postfix 변환 위한 stack 배열의 모든 요소값 초기화
         
	postfixStackTop = -1; // postfix 변환 위한 stack의 top 위치 초기화
	evalStackTop = -1; // 후위표기식 연산 위한 stack의 top 위치 초기화
	evalResult = 0; // 연산 결과값 초기화
}

void evaluation()
{
	/* postfixExp, evalStack을 이용한 계산 */
	char *exp = postfixExp; // 변환된 후위표기식의 연산자/피연산자 하나를 순서대로 저장
	char x; /* 문자하나를 임시로 저장하기 위한 변수 */
	precedence token; // 연산자 우선순위 저장하는  변수 token 선언
	int op1, op2; // 피연산자 2개를 각각 저장할 int형 변수 선언

	while(*exp != '\0') // 후위표기식의 모든 연산자/피연산자에 대해 계산을 끝낼 때까지
	{
		x = *exp; // 임시 저장변수 x에 exp의 값 저장
		token = getToken(x); // x의 연산 우선순위 구함
		if(token == operand) // x가 피연산자일 경우
			evalPush(x-'0'); // int형 변환을 위해 '0'을 뺀 값을 연산 위한 stack에 push
		else // x가 연산자일 경우
		{
			op2 = evalPop(); // 두번째 피연산자값을 pop하여 저장
			op1 = evalPop(); // 첫번째 피연산자값을 pop하여 저장
			switch(token) // 연산자 종류에 따라
			{
				case plus: evalPush(op1+op2); break; // 알맞는 연산을 수행하여 결과값을 연산 위한 stack에 push
				case minus: evalPush(op1-op2); break;
				case times: evalPush(op1*op2); break;
				case divide: evalPush(op1/op2); break;
				default: break;
			}
		}

		exp++; // 식의 다음 연산자/피연산자를 처리하기 위해 exp++
	}
	evalResult = evalPop(); // 연산이 완료되어 최종적으로 stack에 남은 값을 pop하여 결과 저장
	printf("%d\n", evalResult); // 연산 결과값 출력
}

