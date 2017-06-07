#include"reader.h"
#include"lexer.h"
#include"conshow.h"
#include"parser.h"

using namespace std;

int main(int argc, char *argv[])
{
	int c = 0;
	Output con;
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
		con.error<<e.what()<<endl;
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
		con.error<<err.what()<<endl;
	}
	delete reader;
	reader = nullptr;
	system("pause");
	return 0;
}