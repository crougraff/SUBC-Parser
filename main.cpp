/*

Name: Sakshi Mittal

UFID: 1909-3909
*/

#include <cstdlib>
#include <iostream>
#include <cstring>
#include <sstream>
#include <fstream>
#include <stack>

using namespace std;

typedef struct nodeType{
    std::string name;
    struct nodeType** child;
    int count;
} Node;

string tokens[] = {"(",")","*","+",",","-",".","..","/",":",":=",":=:",";","<",
                    "<=","<>","<char>","<identifier>","<integer>","<string>",
                    "=",">",">=","and","begin","case","chr","const","do","else","end",
                    "eof","exit","for","function","if","loop","mod","not","of",
                    "or","ord","otherwise","output","pool","pred","program","read","repeat",
                    "return","succ","then","type","until","var","while"};

stack<Node*> s;
ifstream in;
string nextToken;

void proc_Tiny();
void proc_Consts();
void proc_Const();
void proc_Types();
void proc_Type();
void proc_LitList();
void proc_SubProgs();
void proc_Fcn();
void proc_Params();
void proc_Dclns();
void proc_Dcln();
void proc_Body();
void proc_OutExp();
void proc_StringNode();
int proc_Caseclauses();
void proc_Caseclause();
void proc_ConstValue();
void proc_CaseExpression();
void proc_Statement();
int proc_OtherwiseClause();
void proc_ForStat();
void proc_Assignment();
void proc_ForExp();
void proc_Expression();
void proc_Factor();
void proc_Term();
void proc_Primary();
void proc_Name();
void print_tree(Node*, int);
int isValidToken(string str);
void discard_comments(char s);
void putback(string str);
int isNextToken(string str);
void buildTree(string name, int count);
int identifier();
int integer();
int character();
int stringnode();
int isNextTokenIdentifier();
int isNextTokenInteger();
int isNextTokenCharacter();



int main(int argc, char** argv) 
{
    string file;
	if(argc == 3)
	{
		if(string(argv[1]) == "-ast"){
			file = argv[2];  
	    	in.open(file.c_str());
	    	proc_Tiny();
	    	print_tree(s.top(),0);
		}
	}
	else if(argc == 2)
	{
		
	} 
	else
	{
		cout<<"Error"<<endl;
	}
    return 0;
}



void proc_Tiny()
{
    if(isNextToken("{"))
        discard_comments('}');
    if(isNextToken("#"))
        discard_comments('\n');
    if(isNextToken("program"))
        proc_Name();
        
    if(isNextToken(":"))
	{
        proc_Consts();
        proc_Types();
        proc_Dclns();
        proc_SubProgs();
        proc_Body();
        proc_Name();
    }
    
	if(isNextToken("."))
	{
        buildTree("program", 7);
	}
}

void proc_Consts()
{
    if(isNextToken("{"))
        discard_comments('}');
    if(isNextToken("#"))
        discard_comments('\n');
    if(isNextToken("const"))
	{
        int n = 1;
        proc_Const();
        while(isNextToken(","))
		{
            proc_Const();
            n++;
        }
        buildTree("consts", n);
    }
    else
	{
        buildTree("consts", 0);
    }
}

void proc_Const()
{
    proc_Name();
    
	if(!isNextToken("=")) 
		exit(1);
		
    proc_ConstValue();
    buildTree("const", 2);
}

void proc_Types()
{
    if(isNextToken("{"))
        discard_comments('}');
    if(isNextToken("#"))
        discard_comments('\n');
    if(isNextToken("type"))
	{
        int n = 1;
        proc_Type();
        if(!isNextToken(";")) 
			exit(1);
        
		while(isNextTokenIdentifier())
		{
            proc_Type();
            if(!isNextToken(";")) 
				exit(1);
            
			n++;
        }
        buildTree("types", n);
    }
    else
	{
        buildTree("types", 0);
    }
}

void proc_Type()
{
    proc_Name();
    
	if(!isNextToken("=")) 
		exit(1);
    
	proc_LitList();
    buildTree("type", 2);
}

void proc_LitList()
{
    if(!isNextToken("(")) 
		exit(1);
    
	int n = 1;
    proc_Name();
    while(isNextToken(","))
	{
        proc_Name();
        n++;
    }
    
	if(!isNextToken(")")) 
		exit(1);
    
	buildTree("lit", n);
}

void proc_SubProgs()
{
    if(isNextToken("{"))
        discard_comments('}');
    if(isNextToken("#"))
        discard_comments('\n');
    int n = 0;
    while(isNextToken("function"))
	{
        putback("function");
        proc_Fcn();
        n++;
    }
    buildTree("subprogs", n);
}

void proc_Fcn()
{
    if(isNextToken("function"))
	{
        proc_Name();
        
		if(!isNextToken("(")) 
			exit(1);
        
		proc_Params();
        
		if(!isNextToken(")")) 
			exit(1);
        
		if(!isNextToken(":")) 
			exit(1);
        
		proc_Name();
        
		if(!isNextToken(";")) 
			exit(1); 
        
		proc_Consts();
        proc_Types();
        proc_Dclns();
        proc_Body();
        proc_Name();
        
		if(!isNextToken(";")) 
			exit(1);
        
		buildTree("fcn", 8);
    }
}

void proc_Params()
{
    int n = 1;
    proc_Dcln();
    while(isNextToken(";"))
	{
        proc_Dcln();
        n++;
    }
    buildTree("params", n);
}

void proc_Dclns()
{
    if(isNextToken("{"))
        discard_comments('}');
    if(isNextToken("#"))
        discard_comments('\n');
    if(isNextToken("var"))
	{
        proc_Dcln();
        
		if(!isNextToken(";")) 
			exit(1);
        
		int n = 1;
        while(isNextTokenIdentifier())
		{
            proc_Dcln();
            
			if(!isNextToken(";")) 
				exit(1);
            
			n++;
        }
        buildTree("dclns", n);
    }
    else
	{
        buildTree("dclns",0);
    }
}

void proc_Dcln()
{
    int n = 1;
    proc_Name();
    while(isNextToken(","))
	{
        proc_Name();
        n++;
    }
    
	if(!isNextToken(":")) 
		exit(1);
    
	proc_Name();
    buildTree("var", n+1);
}

void proc_Body()
{
    if(isNextToken("{"))
        discard_comments('}');
    if(isNextToken("#"))
        discard_comments('\n');
    if(!isNextToken("begin")) 
		exit(1);
    
	int n = 1;
    if(isNextToken("{"))
        discard_comments('}');
    proc_Statement();
    if(isNextToken("#"))
        discard_comments('\n');
    while(isNextToken(";"))
	{
        proc_Statement();
        if(isNextToken("#"))
            discard_comments('\n');
        n++;
    }
    
    if(!isNextToken("end")) 
		exit(1);
    
	buildTree("block", n);
}

void proc_Statement()
{
    int n;
    if(isNextToken("{"))
        discard_comments('}');
    if(isNextToken("#"))
        discard_comments('\n');
    if(isNextToken("output"))
	{
        if(!isNextToken("(")) 
			exit(1);
        
		n = 1;
        proc_OutExp();
        while(isNextToken(","))
		{
            proc_OutExp();
            n++;
        }
        
		if(!isNextToken(")")) 
			exit(1);
        
		buildTree("output", n);
    }
    else if(isNextToken("if"))
	{
        n = 2;
        proc_Expression();
        
		if(!isNextToken("then"))
            exit(1);
            
        proc_Statement();
        if(isNextToken("else"))
		{
            proc_Statement();
            n++;
        }
        buildTree("if", n);
    }
    else if(isNextToken("while"))
	{
        proc_Expression();
        
		if(!isNextToken("do")) 
			exit(1);
        
		proc_Statement();
        buildTree("while", 2);
    }
    else if(isNextToken("repeat"))
	{
        n = 1;
        proc_Statement();
        while(isNextToken(";"))
		{
            proc_Statement();
            n++;
        }
        
		if(!isNextToken("until")) 
			exit(1);
        
		proc_Expression();
        buildTree("repeat", n+1);
    }
    else if(isNextToken("for"))
	{
        if(!isNextToken("(")) 
			exit(1);
        
		proc_ForStat();
        
		if(!isNextToken(";")) 
			exit(1);
        
		proc_ForExp();
        
		if(!isNextToken(";")) 
			exit(1);
        
		proc_ForStat();
        
		if(!isNextToken(")")) 
			exit(1);
        
		proc_Statement();
        buildTree("for", 4);
    }
    else if(isNextToken("loop"))
	{
        n = 1;
        proc_Statement();
        while(isNextToken(";"))
		{
            proc_Statement();
            n++;
        }
        
        if(!isNextToken("pool")) 
			exit(1);
        buildTree("loop", n);
    }
    else if(isNextToken("case"))
	{
        int n = 1;
        proc_Expression();
        
		if(!isNextToken("of")) 
			exit(1);
        
		n += proc_Caseclauses();
        n += proc_OtherwiseClause();
        
		if(!isNextToken("end")) 
			exit(1);
        
		buildTree("case", n);
    }
    else if(isNextToken("read"))
	{
        n = 1;
        
        if(!isNextToken("(")) 
			exit(1);
        
		proc_Name();
        while(isNextToken(","))
		{
            proc_Name();
            n++;
        }
        
		if(!isNextToken(")")) 
			exit(1);
        
		buildTree("read", n);
    }
    else if(isNextToken("exit"))
	{
        buildTree("exit", 0);
    }
    else if(isNextToken("return"))
	{
        proc_Expression();
        buildTree("return", 1);
    }
    else if(isNextToken("begin"))
	{
        putback("begin");
        proc_Body();
    }
    else if(isNextTokenIdentifier())
	{
        proc_Assignment();
    }
    else
	{
        if(isNextToken("{"))
		{
            discard_comments('}');
        }
        if(isNextToken("#"))
		{
            discard_comments('\n');
        }
        buildTree("<null>", 0);
    }
}

void proc_OutExp()
{
    if(stringnode())
	{
        buildTree("string", 1);
    }
    else
	{
        proc_Expression();
        buildTree("integer", 1);
    }
}

void proc_StringNode()
{
    if(stringnode());
    else
		exit(1);
}

void proc_ConstValue()
{
    if(integer());
    else if(character());

    else
	{
        proc_Name();
    }
}

int proc_Caseclauses()
{
    int n = 1;
    proc_Caseclause();
    if(isNextToken(";"))
	{}
    while(isNextTokenInteger() || isNextTokenCharacter() || isNextTokenIdentifier())
	{
        proc_Caseclause();
        if(isNextToken(";"))
		{
            n++;
        }
    }
    return n;
}

void proc_Caseclause()
{
    proc_CaseExpression();
    int n = 1;
    while(isNextToken(","))
	{
        proc_CaseExpression();
        n++;
    }
    if(isNextToken(":"))
	{
        proc_Statement();
        buildTree("case_clause", n + 1);
    }
    else
	{
        exit(1);
    }
}

void proc_CaseExpression()
{
    proc_ConstValue();
    if(isNextToken(".."))
	{
        proc_ConstValue();
        buildTree("..", 2);
    }
}

int proc_OtherwiseClause()
{
    if(isNextToken("otherwise"))
	{
        proc_Statement();
        buildTree("otherwise", 1);
        return 1;
    }
    return 0;
}

void proc_ForStat()
{
    if(isNextTokenIdentifier())
	{
        proc_Assignment();
    }
    else
	{
        buildTree("<null>", 0);
    }
}

void proc_Assignment()
{
    if(isNextToken("{"))
        discard_comments('}');
    if(isNextToken("#"))
        discard_comments('\n');
    proc_Name();
    if(isNextToken(":="))
	{
        proc_Expression();
        buildTree("assign", 2);
    }
    else if(isNextToken(":=:"))
	{
        proc_Name();
        buildTree("swap", 2);
    }
}

void proc_ForExp()
{
    
    if(isNextToken(";"))
	{
        putback(";");
        buildTree("true",0);
    }
    else
       proc_Expression(); 
}

void proc_Expression()
{
    proc_Term();
    if(isNextToken("<="))
	{
        proc_Term();
        buildTree("<=", 2);
    }
    else if(isNextToken("<>"))
	{
        proc_Term();
        buildTree("<>", 2);
    }
    else if(isNextToken("<"))
	{
        proc_Term();
        buildTree("<", 2);
    }
    else if(isNextToken(">="))
	{
        proc_Term();
        buildTree(">=", 2);
    }
    else if(isNextToken(">"))
	{
        proc_Term();
        buildTree(">", 2);
    }
    else if(isNextToken("="))
	{
        proc_Term();
        buildTree("=", 2);
    }
}

void proc_Term()
{
    proc_Factor();
    while(isNextToken("+") || isNextToken("-") || isNextToken("or"))
	{
        if(nextToken == "+")
		{
            proc_Factor();
            buildTree("+", 2);
        }
        else if(nextToken == "-")
		{
            proc_Factor();
            buildTree("-", 2);
        }
        else if(nextToken == "or")
		{
            proc_Factor();
            buildTree("or", 2);
        }
    }
}

void proc_Factor()
{
    proc_Primary();
    while(isNextToken("*") || isNextToken("/") || isNextToken("and") || isNextToken("mod"))
	{
        if(nextToken == "*")
		{
            proc_Primary();
            buildTree("*", 2);
        }
        else if(nextToken == "/")
		{
            proc_Primary();
            buildTree("/", 2);
        }
        else if(nextToken == "and")
		{
            proc_Primary();
            buildTree("and", 2);
        }
        else if(nextToken == "mod")
		{
            proc_Primary();
            buildTree("mod", 2);
        }
    }
}

void proc_Primary()
{
    if(isNextToken("-"))
	{
        proc_Primary();
        buildTree("-", 1);
    }
    else if(isNextToken("+"))
	{
        proc_Primary();
    }
    else if(isNextToken("not"))
	{
        proc_Primary();
        buildTree("not", 1);
    }
    else if(isNextToken("eof"))
	{
        buildTree("eof", 0);
    }
    else if(integer())
    {}
    else if(character())
    {}
    else if(isNextToken("succ"))
	{
        if(!isNextToken("(")) 
			exit(1);
        proc_Expression();
        if(!isNextToken(")")) 
			exit(1);
        buildTree("succ", 1);
    }
    else if(isNextToken("pred"))
	{
        if(!isNextToken("(")) 
			exit(1);
    
	    proc_Expression();
    
	    if(!isNextToken(")")) 
			exit(1);
    
	    buildTree("pred", 1);
    }
    else if(isNextToken("ord"))
	{
        if(!isNextToken("(")) 
			exit(1);
    
	    proc_Expression();
    
	    if(!isNextToken(")")) 
			exit(1);
    
	    buildTree("ord", 1);
    }
    else if(isNextToken("chr"))
	{
        if(!isNextToken("(")) 
			exit(1);
        
		proc_Expression();
        
		if(!isNextToken(")")) 
			exit(1);
        
		buildTree("chr", 1);
    }
    else if(isNextToken("("))
	{
        proc_Expression();
        isNextToken(")");
    }
    else
	{
        proc_Name();
        int n = 1;
        if(isNextToken("("))
		{
            proc_Expression();
            n++;
            while(isNextToken(","))
			{
                proc_Expression();
                n++;
            }
            isNextToken(")");
            buildTree("call", n);
        }
    }
    
}

void proc_Name()
{
    if(!identifier())
	{
        char ch = in.get();
        stringstream ss;
        string s;
        ss << ch;
        ss >> s;
        s = "Unexpected token '" + s + "'";
        exit(1);
    }
}

void print_tree(Node *node, int level)
{
    if(node == NULL)
        return;
    for(int i = 1; i <= level; i++)
        cout<<". ";
    
	cout<<node->name<<"("<<node->count<<")"<<endl;

    if(node->child == NULL)
        return;
    
    for(int i = 0; i < node->count; i++)
	{
        print_tree(node->child[i], level+1);
    }    
}

int isValidToken(string str)
{
    for(int i = 0; i < sizeof(tokens)/sizeof(tokens[0]); i++)
        if(tokens[i] == str)
            return i;
    return -1;
}

void discard_comments(char s)
{
    char ch;
    while((ch = in.get()) != s);
}

void putback(string str)
{
    int len = str.size();
    const char* ch = str.c_str();
    for(int i = len-1; i >= 0; i--)
        in.putback(ch[i]);
}

int isNextToken(string str)
{
    int c = 0;
    char ch;
    string token;
    const char *s = str.c_str();
    while(isspace(ch = in.peek()))
        in.get();
    
	if((ch = in.get()) != s[c++])
	{
        in.putback(ch);
        return 0;
    }
    token += ch;
    while((ch = in.get()) == s[c++])
	{
        token += ch;
    }
    in.putback(ch);
    
	if(token == str)
	{
        nextToken = token;
        return 1;
    }
    else
	{
        putback(token);
        return 0;
    }
}

void buildTree(string name, int count)
{    
    Node* n = new Node;
    n->name = name;
    n->count = count;
    n->child = new Node*[count];
    
    for(int i = count-1; i >= 0; i--)
	{
        Node* temp = s.top();
        s.pop();
        n->child[i] = temp;
    }
    s.push(n);
}

int identifier()
{
    int ch;
    string id;
    while(isspace(ch = in.peek()))
        in.get();
    if (!((ch = in.get()) == '_' || isalpha(ch)))
	{
        in.putback(ch);
        return 0;
    }
    do 
	{ 
        id += ch;
        ch = in.get();
    } while (isalpha(ch) || isdigit(ch) || ch == '_');
    
	in.putback(ch);
    
	if(isValidToken(id) >= 0)
	{
        putback(id);
        return 0;
    }
    nextToken = id;
    Node* temp = new Node;
    temp->name = id;
    temp->count = 0;
    temp->child = NULL;
    s.push(temp);
    buildTree("<identifier>", 1);
    return 1;
}

int isNextTokenIdentifier()
{
    int ch;
    string id;
    while(isspace(ch = in.peek()))
        in.get();
    if (!((ch = in.get()) == '_' || isalpha(ch)))
	{
        in.putback(ch);
        return 0;
    }
    do 
	{ 
        id += ch;
        ch = in.get();
    } while (isalpha(ch) || isdigit(ch) || ch == '_');
    
	in.putback(ch);
    putback(id);
    
	if(isValidToken(id) == -1)
        return 1;
    else 
		return 0;
}

int integer()
{
    int ch;
    string id;
    while(isspace(ch = in.peek()))
        in.get();
    ch = in.get();
    if (!isdigit(ch))
	{
        in.putback(ch);
        return 0;
    }
    do 
	{ 
        id += ch;
        ch = in.get();
    } while (isdigit(ch));
    
	in.putback(ch);
    nextToken = id;
    Node* temp = new Node;
    temp->name = id;
    temp->count = 0;
    temp->child = NULL;
    s.push(temp);
    buildTree("<integer>", 1);
    return 1;
}

int isNextTokenInteger()
{
    int ch;
    string id;
    while(isspace(ch = in.peek()))
        in.get();
    ch = in.get();
    if (!isdigit(ch))
	{
        in.putback(ch);
        return 0;
    }
    do 
	{ 
        id += ch;
        ch = in.get();
    } while (isdigit(ch));
    
	in.putback(ch);
    putback(id);
    nextToken = id;
    return 1;
}

int character()
{
    int ch;
    string id;
    while(isspace(ch = in.peek()))
        in.get();
    if (!((ch = in.get()) == '\''))
	{
        in.putback(ch);
        return 0;
    }
    if ((ch = in.get()) == '\'')
	{
        in.putback(ch);
        return 0;
    }
    else
        id += ch;
    
	if (!((ch = in.get()) == '\''))
	{
        in.putback(ch);
        in.putback(id.at(0));
        in.putback('\'');
        return 0;
    }
    nextToken = id;
    Node* temp = new Node;
    temp->name = "'" + id + "'";
    temp->count = 0;
    temp->child = NULL;
    s.push(temp);
    buildTree("<char>", 1);
    return 1;
}

int isNextTokenCharacter()
{
    int ch;
    string id;
    while(isspace(ch = in.peek()))
        in.get();
    if (!((ch = in.get()) == '\''))
	{
        in.putback(ch);
        return 0;
    }
    if ((ch = in.get()) == '\'')
	{
        in.putback(ch);
        return 0;
    }
    else
        id += ch;
    
    if (!((ch = in.get()) == '\''))
	{
        in.putback(ch);
        in.putback(id.at(0));
        in.putback('\'');
        return 0;
    }
    in.putback('\'');
    in.putback(id.at(0));
    in.putback('\'');
    nextToken = id;
    return 1;
}

int stringnode()
{
    int ch;
    string id;
    if (!((ch = in.get()) == '\"'))
	{
        in.putback(ch);
        return 0;
    }
    while ((ch = in.get()) != '\"') 
	{ 
        id += ch;
    }
    
	in.putback(ch);
    if (!((ch = in.get()) == '\"'))
	{
        in.putback(ch);
        in.putback(id.at(0));
        in.putback('\'');
        return 0;
    }
    
	Node* temp = new Node;
    temp->name = id;
    temp->count = 0;
    temp->child = NULL;
    s.push(temp);
    buildTree("<string>", 1);
    return 1;
}

