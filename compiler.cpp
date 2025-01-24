#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<algorithm>
#include <stack>

using namespace std;

//lexer will do the tokenization....... 
//means:

//store abc = 1 ; // tokens: (keyword,store), (identifier,abc), (operator,assignment), (literal,1), (brackets,{)

struct Token{
	string type;
	string value;
	
	Token(string t,string v):type(t),value(v){
	}
};

struct operation{
	 string Operator;
	 vector<Token> Oprerands;
};

struct node{
	string type;
	string value;
	vector<Token> expression;
	vector<Token> condition;
	
	vector<node> childrens; // [declaration,assignment,declaration]
};

enum StatementType{
	 declaration,
	 assignment,
	 condition,
};
  
class Lexer{
		private:
		 vector<string> KEYWORDS = {"store","if"}; // only 2 keywords for now.
		 vector<string> OPERATORS = {"=","+","-","<",">","=="};
		 vector<string> BRACKETS = {"(",")","{","}"};
		 string END = ";";
	public:
	vector<Token> tokens;
	bool invalidToken = false;
	string invalidTokenString = "";

	
	bool fetchTokens(string filePath){
		ifstream file;
		file.open("hello_world.txt");
		string word;
	
		while(file>>word){
			if(this->is_keyword(word)){
				this->tokens.push_back(Token("keyword",word));
				continue;
			}
			if(this->is_operator(word)){
				this->tokens.push_back(Token("operator",word));
				continue;
			}
			if(this->is_brackets(word)){
				this->tokens.push_back(Token("bracket",word));
				continue;
			}
			if(this->is_identifier(word)){
				this->tokens.push_back(Token("identifier",word));
				continue;
			}
			if(this->is_literal(word)){
				this->tokens.push_back(Token("literal",word));
				continue;
			}
			if(word == ";")
				{
					this->tokens.push_back(Token("end","end"));
					continue;
				}
			
			// if none of these matches it means.
			this->invalidToken = true;
			this->invalidTokenString = word;
			this->display_tokenError(word);
			return false;
			
		}
		
		return true;
	}
	
	private:
	bool isToken(vector<string> tokens_arr,string word){
		int i;
		for(i=0;i<tokens_arr.size();i++){
			if(tokens_arr[i] == word)
				return true;
		}
		return false;
	}
	
	bool is_keyword(string word){
		return isToken(this->KEYWORDS,word);
	}	
	
	bool is_operator(string word){
		return isToken(this->OPERATORS,word);
	}
	
	bool is_brackets(string word){
		return isToken(this->BRACKETS,word);
	}
	
	bool is_identifier(string word){
		int i;
		for(i=0;i<word.length();i++){
			if(i==0){
				if( !(word[i]>='a' && word[i]<='z') || (word[i]>='A' && word[i]<='Z' )  ){
					return false;
				}
			}
			else{
			if( !((word[i]>='a' && word[i]<='z') || (word[i]>='A' && word[i]<='Z' ) ||  (word[i]>='0' && word[i]<='9' ) )   ){
				return false;	
				}
			}
		}
		return true;
	}
	 // if the token is not identifier it means its a literal. but what is literal? -> a number, a character. a decimal, a string
	 //lets only validate a number as a literal.
	bool is_literal(string word){
		
		int i;
		for(i=0;i<word.length();i++){
			if(!(word[i]>='0' && word[i]<='9')){
				return false;
			}
		}
		return true;
	}

	void display_tokenError(string word){
		cout<<"Invalid Token! : "<<word<<endl;
		
	}
	public:
	void display_tokens(){
		if(this->invalidToken){
			this->display_tokenError(this->invalidTokenString);
			return;
		}
		int i=0;
		while(i<this->tokens.size()){
			cout<<"("<<tokens[i].type<<","<<tokens[i].value<<")"<<endl;
			i+=1;
		}
		
	}
	
};

/* (keyword,store)
(identifier,abc)
(end,end) 
(identifier,abc)
(operator,=)
(literal,10)
(end,end)
(identifier,abs)
(operator,=)
(literal,100)
(operator,+)
(literal,234)
(end,end)  */

struct statementLine{
	int statementNo;
	vector<Token> Data;
		
};
class Parser {
	private:
	vector<vector<Token>> STATEMENT_PATTERNS = 
	// DECLARATION 
	{{Token("keyword","store"),Token("identifier","*"),Token("end","end")}, 
	//ASSIGNEMENT
	{Token("identifier","*"),Token("operator","="),Token("expression","*"),Token("end","end")},
	//IF ELSE.
	{Token("keyword","if"),Token("bracket","("),Token("condition","*"),Token("bracket",")"),Token("bracket","{"), Token("body","*"), Token("bracket","}"),Token("end","end")} };
	public:
	statementLine expression;
	statementLine condition;
	vector<Token> TokenArray;
	int current_executing_statement = 0;
	string current_statement_type = "";
	
	
	Parser(vector<Token> TokenArray){
		this->TokenArray = TokenArray;
		cout<<"parser got the tokens"<<endl;
	}
	
	
	vector<node> parse(int ptr=0,string condition="None",vector<node> tempAST={}){ // starting point
	     cout<<"starting at"<<ptr<<endl;
		if(condition=="None" && ptr >= TokenArray.size()){
			cout<<"returning the value"<<endl;
					for(int i=0;i<tempAST.size();i++){
			for(int j=0;j<tempAST[i].expression.size();j+=1){
				cout<<tempAST[i].expression[j].value<<endl;
			}
		}
			return tempAST;
		}
		else if(condition!="None" && condition == TokenArray[ptr].value){
			cout<<"not herer"<<endl;
			return tempAST;
		}
		if(ptr>=TokenArray.size())
		{
			cout<<"heree"<<endl;
		
			return {};
		}
			
		vector<Token> statement = this->fetchStatement(ptr);
		vector<node> bodyAST={};

		if(statement.size()>0){
			cout<<"fetched the statement"<<endl;
			int flag =0;
			if(this->isAssignment(statement)){
				flag+=1;
			}
			else if((this->isDeclaration(statement))){
				flag+=1;	
			}
			else{
				if(condition == "None")
					{
					bodyAST = this->isCondition(ptr,statement);
					if(bodyAST.size()>0){
						flag+=1;
						
					}
					else{
						cout<<"ErrorBru "<<endl;
					}
				}
			}
			if(!flag)
				return {};
				
			cout<<"after fetching the movement of the pointer"<<ptr<<endl;
		}
		else{
			cout<<"Error happend at line no"<<this->current_executing_statement+1<<endl;
			return {};
		}
		// assignmetn should return the ast.
		
		
		node temp=this->constructAST(bodyAST,statement);
		if(temp.type == "None"){
			return {};
		}
		cout<<"pushing back..."<<endl<<endl;
		tempAST.push_back(temp);

		cout<<"end of temp"<<endl;

		this->current_executing_statement+=1;

		return parse(ptr+statement.size(),condition,tempAST);
		
	}
	
	vector<Token> fetchStatement(int current_pointer){
		int i = current_pointer;
		vector<Token> statement;
		if(i==this->TokenArray.size()){
			 vector<Token> blank;
			 return blank;
		}
		bool conditional = this->TokenArray[i].value == "if"?true:false;
		cout<<"conditional"<<conditional<<endl;
		 
		while( i<this->TokenArray.size()){
			if(conditional == true){
				if(this->TokenArray[i].value=="}")
					break;
			}
			else if(this->TokenArray[i].type=="end")
			{
				break;
			}
			cout<<"fetching token"<<endl;	
			statement.push_back(TokenArray[i]);
			i+=1;
		}
		
		if(i<this->TokenArray.size()) // this means we got the end statement;
			{
				statement.push_back(TokenArray[i]);	
				return statement;
			}
		else{
	 		vector<Token> blank;
			 return blank;
		}
		
	}
	
	int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    return 0;
}

bool isOperator(const string& value) {
    return value == "+" || value == "-";
}

vector<Token> infixToPrefix(vector<Token> expression) {
    stack<vector<Token>> operands;  
    stack<Token> operators;        

    for (const auto token : expression) {
        if (token.type == "identifier" || token.type == "literal") {
            operands.push({token});
        } else if (token.type == "operator") {
            operators.push(token);
        }
    }

    while (!operators.empty()) {
        Token op = operators.top();
        operators.pop();

        auto rightOperand = operands.top(); operands.pop();
        auto leftOperand = operands.top(); operands.pop();

        vector<Token> combined = {op};
        combined.insert(combined.end(), leftOperand.begin(), leftOperand.end());
        combined.insert(combined.end(), rightOperand.begin(), rightOperand.end());

        operands.push(combined);
    }

    return operands.top();
}

	
	node constructAST(vector<node> ast={}, vector<Token> statement={} ){
		node AST;
		AST.type = "None";
		 if(this->current_statement_type == "declaration"){
		 	cout<<"running the declaration ast!"<<endl;
		 	AST.type = "declaration";
		 	AST.value = statement[1].value;
		 	return AST;
		 	cout<<"delcaration ast ="<<AST.value<<endl;
		}
		else if(this->current_statement_type == "assignment"){
			cout<<"running the assignment ast"<<endl;
			AST.type = "assignment";
			AST.value = statement[0].value;
			if(!this->expression.Data.size()){
				cout<<"No expression found for the assignment!"<<endl;
				return {};
			}
			// 1+2+3
			vector<Token> prefix= this->infixToPrefix(this->expression.Data);
			cout<<"prefix:"<<endl;
			for(int i=0;i<prefix.size();i++){
				cout<<prefix[i].value<<endl;
			}
			AST.expression = prefix;
			return AST;
		}
		else{
			cout<<"into making the conditional statement ast"<<endl;
			AST.type = "conditional";
			AST.value = "if";
			if(!this->condition.Data.size()){
				cout<<"No expression found for the condition!"<<endl;
				return {};
			}
			// 1+2+3
			vector<Token> prefix= this->infixToPrefix(this->condition.Data);
			cout<<"prefix for condition:"<<endl;
			for(int i=0;i<prefix.size();i++){
				cout<<prefix[i].value<<endl;
			}
			AST.condition = prefix;
			cout<<"ast"<<endl;

			AST.childrens = ast;
			return AST;
		}
		
		return AST;
		
	};
	
	
	int evaluateTheExpression(int ptr,vector<Token> statement){
			vector<Token> expression;
			statementLine exp;
			exp.statementNo = this->current_executing_statement;

			while(statement[ptr].type!="end"){
				 if(!(statement[ptr].type=="operator" || statement[ptr].type=="identifier" || statement[ptr].type=="literal")){ //1 = literal
				 	return false;
				 }
				 else{
				 	if(statement[ptr].type == "operator" && statement[ptr].value == "="){
				 		return false;
					 }
					 expression.push_back(Token(statement[ptr].type,statement[ptr].value));
				 }
				 
				 ptr+=1;
			}
			if(ptr==statement.size()){
				return false;
			}
			exp.Data = expression;
			this->expression=exp;
			
			return ptr;
		
		// ( a+bc+c )
	}
	
	int validCondition(vector<Token> condition,int ptr){
			vector<Token> conditional;
			statementLine exp;
			exp.statementNo = this->current_executing_statement;
			
			cout<<"ptr value"<<ptr<<endl;
			
		while(condition[ptr].value!=")" && ptr<condition.size()){
			cout<<"checking::"<<condition[ptr].type<<endl;
			
			if( condition[ptr].type == "operator"){
				if(!(condition[ptr].value ==">" || condition[ptr].value == "<" || condition[ptr].value== "==")){
					return false;
				}
			
			}
			else if(condition[ptr].type == "literal" || condition[ptr].type == "identifier"){
				 cout<<"its is"<<condition[ptr].type<<endl;	
				 		}
			else{
				return false;
			}
			conditional.push_back(Token(condition[ptr].type,condition[ptr].value));

			ptr+=1;
		}
		
		if(ptr==condition.size()){
			return false;
		}
		
		exp.Data = conditional;
		this->condition=exp;
		return ptr;
	}
	
	bool isDeclaration(vector<Token> statement){
		int i;
		for(i=0;i<statement.size();i++){
			if(statement[i].type == "identifier" && this->STATEMENT_PATTERNS[0][i].type =="identifier"){
				continue;
			}
		 	if( !(statement[i].type == this->STATEMENT_PATTERNS[0][i].type  &&  statement[i].value == this->STATEMENT_PATTERNS[0][i].value) ){
		 		return false;
			 } 
		}
		this->current_statement_type ="declaration";
		return true;
	}
	

	bool isAssignment(vector<Token> statement){	
	cout<<"int the assignment"<<endl;
	for(int i=0;i<statement.size();i++){
		if(STATEMENT_PATTERNS[1][i].type == "expression"){
				int res = evaluateTheExpression(i,statement);
				if (res==false){
					cout<<"result is false expression is invalid!"<<endl;
					return false;
				}
				cout<<"result is"<<res<<endl;
				i = res;
				
			}
		 else if(statement[i].type != this->STATEMENT_PATTERNS[1][i].type){
		 		return false;
			 } 
		}
		this->current_statement_type = "assignment";
		return true;
		
	}
	
	vector<node> isCondition(int pointer,vector<Token> statement){
		cout<<"into the conditional: statment"<<pointer<<endl;
		vector<node> body;
		for(int i=0;i<statement.size();i++){
			cout<<statement[i].value<<endl;
		}
		
		int conditionSize = 0;
		int k = 0;
		
	for(int i=0;i<statement.size();i++,k++){
		cout<<"i and k"<<statement[i].value<<STATEMENT_PATTERNS[2][k].value<<endl;
		if(STATEMENT_PATTERNS[2][k].type == "condition"){
			int res = this->validCondition(statement,i);
				if (res==false){
					return {};
				}
			 cout<<"its is a valid condition"<<endl;
			cout<<"conditionSize:"<<conditionSize<<endl;

				i=res-1; // if condition is valid then add +1 for body and +1 for closing parenthesis.
				continue; 
			}
		else if(STATEMENT_PATTERNS[2][k].type == "body"){
			cout<<"parsing the body i and condition size"<<i<<conditionSize;
			body = this->parse(i+pointer,"}",{});
			if(body.size()<1){
				cout<<"found error in the if's body"<<endl;
			}
			cout<<"body parsed!:"<<endl;
			
			for(int i=0;i<body.size();i++){
				cout<<"body:"<<body[i].type<<" "<<body[i].value<<endl;
			}
			
			i = statement.size()-2;
			
		}
		 else if(statement[i].type != this->STATEMENT_PATTERNS[2][k].type){
		 		return {};
			 } 
		}
		this->current_statement_type = "conditional";
		cout<<"hogya kya bro ?"<<endl;
		return body;

	}

};

int main(){
Lexer lexer;
lexer.fetchTokens("hello_world.txt");
lexer.display_tokens();

Parser parser(lexer.tokens);
vector<node> tempAST = parser.parse();
cout<<"printing the ast tree"<<endl;
	for(int i=0;i<tempAST.size();i++){
			for(int j=0;j<tempAST[i].expression.size();j+=1){
				cout<<tempAST[i].expression[j].value<<endl;
			}
		}

}


