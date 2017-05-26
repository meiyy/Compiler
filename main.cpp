#include"DFA.h"
#include"reader.h"
#include"lexer.h"
#include"conshow.h"
#include"parser.h"
#include<tuple>

using namespace std;

int main(int argc, char *argv[])
{
	int c = 0;
	Conshow con;
	if (argc < 2)
	{
		con.showVersion();
		con.showMenu();
		exit(EXIT_FAILURE);
	}
	Reader *reader = nullptr;
	try
	{
		reader = new Reader(argv[1]);
	}
	catch (exception e)
	{
		con.showError(e.what());
		exit(EXIT_FAILURE);
	}
	Lexer lexer = Lexer(reader);
	try
	{
		Parser parser(lexer);
		parser.work();
	}
	catch (exception &err)
	{
		con.showError(err.what());
	}
	delete reader;
	reader = nullptr;
	return 0;
}
