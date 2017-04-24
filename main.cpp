#include"DFA.h"
#include"Reader.h"
#include"Worker.h"
#include"conshow.h"

using namespace std;

int main(int argc, char *argv[])
{
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
	Parser parser = Parser(reader);
	try
	{
		parser.test();
	}
	catch (exception &err)
	{
		con.showError(err.what());
	}
	delete reader;
	reader = nullptr;
	return 0;
}
