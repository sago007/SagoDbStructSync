#include <iostream>
#include <boost/program_options.hpp>
#include "sago/dbsync.hpp"
#include <cereal/archives/json.hpp>
#include <fstream>
#include "sago/DbSyncDbOracle.hpp"
#include "sago/DbSyncDbMySql.hpp"

using std::string;
using std::cout;
using std::vector;

#if 0
static void PrintVector(const std::vector<std::string>& v) {
	for (const auto& s : v) {
		cout << s << "\n";
	}
}

static void PrintColumn(const sago::database::DbColumn& c) {
	{
		cereal::JSONOutputArchive archive( cout );
		archive ( cereal::make_nvp("c",c));
	}
}

static void PrintTable(const sago::database::DbTable& t) {
	{
		cereal::JSONOutputArchive archive( cout );
		archive ( cereal::make_nvp("table",t));
	}
}
#endif

int main(int argc, const char* argv[])
{
	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "Print basic usage information to stdout and quits")
		("connectstring", boost::program_options::value<string>(), "A string for use with ")
		("schema,s", boost::program_options::value<string>(), "A string for use with ")
		("output-file,o", boost::program_options::value<string>(), "The output file. If blank stdout is used. If not set no output are generated")
		("input-file,i", boost::program_options::value<string>(), "The input file. If blank stdin is used. If not set no input is read")
	;
	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	boost::program_options::notify(vm);
	if (vm.count("help")) {
		cout << desc << "\n";
		return 1;
	}
	string connectstring = "mysql:database=dbsync_test;user=testuser;password=password;opt_reconnect=1";
	if (vm.count("connectstring")) {
		connectstring = vm["connectstring"].as<string>();
	}
	std::ifstream inputFileStream;
	std::istream* input = &std::cin;
	bool readInput = false;
	if (vm.count("input-file")) {
		readInput = true;
		string inputFileName = vm["input-file"].as<string>();
		if (inputFileName.length() > 0) {
			inputFileStream.open(inputFileName);
			input = &inputFileStream;
		}
	}
	std::ofstream outputFileStream;
	std::ostream* output = &std::cout;
	bool writeOutput = false;
	if (vm.count("output-file")) {
		writeOutput = true;
		string outputFileName = vm["output-file"].as<string>();
		if (outputFileName.length() > 0) {
			outputFileStream.open(outputFileName);
			output = &outputFileStream;
		}
	}
	std::string schema_name = "dbsync_test";
	if (vm.count("schema")) {
		schema_name = vm["schema"].as<string>();
	}
	std::shared_ptr<cppdb::session> db(new cppdb::session(connectstring));
	std::shared_ptr<sago::database::DbSyncDb> dbi;
	{
		dbi = std::shared_ptr<sago::database::DbSyncDb>(new DbSyncDbMySql(db,schema_name));
	}
	
#if 0
	sago::database::DbTable t;
	t.tablename = "MyTable";
	sago::database::DbColumn c;
	c.name = "name";
	c.type = sago::database::DbType::TEXT;
	c.length = 50;
	t.columns.push_back(c);
	{
		cereal::JSONOutputArchive archive( cout );
		archive ( cereal::make_nvp("total",t));
	}
	
	cout << "\n";
	cout << "SOME_TABLE: " << dbi->TableExists("SOME_TABLE") << "\n";
	cout << "my_test_table: " << dbi->TableExists("my_test_table") << "\n";
	cout << "Has coloumn \"name\": " << dbi->ColumnExists("my_test_table", "name") << "\n";
	cout << "Table names:\n"; 
	PrintVector(dbi->GetTableNames());
	cout << "ColoumNames: \n";
	PrintVector(dbi->GetColoumNamesFromTable("my_test_table"));
	cout << "Unique constraints: \n";
	PrintVector(dbi->GetUniqueConstraintNamesForTable("my_test_table"));
	cout << "Foreign keys: \n";
	PrintVector(dbi->GetForeignKeyNamesForTable("my_test_table"));
	cout << "Column:\n";
	PrintColumn(dbi->GetColumn("my_test_table","name"));
	cout << "Table:\n";
	PrintTable(dbi->GetTable("my_test_table"));
	cout << "\n\n";
#endif
	sago::database::DbDatabaseModel dbm;
	if (readInput) {
		{
			cereal::JSONInputArchive archive( *input );
			archive (cereal::make_nvp("dbm",dbm));
		}		
	}
	if (writeOutput) {
		dbm = sago::database::ExtractDataModel(*dbi);
		{
			cereal::JSONOutputArchive archive( *output );
			archive ( cereal::make_nvp("dbm",dbm));
		}
	}
#if 0
	std::string test_table_name = "table_to_create";
	if (!dbi->TableExists(test_table_name)) {
		sago::database::DbTable t;
		t.tablename = test_table_name;
		dbi->CreateTable(t);
	}
#endif
	return 0;
}
