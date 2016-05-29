#include <iostream>
#include <boost/program_options.hpp>
#include "sago/dbsync.hpp"
#include <cereal/archives/json.hpp>
#include "sago/DbSyncDbOracle.hpp"
#include "sago/DbSyncDbMySql.hpp"

using std::string;
using std::cout;
using std::vector;

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

int main(int argc, const char* argv[])
{
	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "Print basic usage information to stdout and quits")
		("connectstring", boost::program_options::value<string>(), "A string for use with ")
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
	
	std::shared_ptr<cppdb::session> db(new cppdb::session(connectstring));
	DbSyncDbMySql dbi(db, "dbsync_test");
	cout << "\n";
	cout << "SOME_TABLE: " << dbi.TableExists("SOME_TABLE") << "\n";
	cout << "my_test_table: " << dbi.TableExists("my_test_table") << "\n";
	cout << "Has coloumn \"name\": " << dbi.ColumnExists("my_test_table", "name") << "\n";
	cout << "Table names:\n"; 
	PrintVector(dbi.GetTableNames());
	cout << "ColoumNames: \n";
	PrintVector(dbi.GetColoumNamesFromTable("my_test_table"));
	cout << "Unique constraints: \n";
	PrintVector(dbi.GetUniqueConstraintNamesForTable("my_test_table"));
	cout << "Foreign keys: \n";
	PrintVector(dbi.GetForeignKeyNamesForTable("my_test_table"));
	cout << "Column:\n";
	PrintColumn(dbi.GetColumn("my_test_table","name"));
	cout << "Table:\n";
	PrintTable(dbi.GetTable("my_test_table"));
	cout << "\n\n";
	sago::database::DbDatabaseModel dbm = sago::database::ExtractDataModel(dbi);
	{
		cereal::JSONOutputArchive archive( cout );
		archive ( cereal::make_nvp("dbm",dbm));
	}
	std::string test_table_name = "table_to_create";
	if (!dbi.TableExists(test_table_name)) {
		sago::database::DbTable t;
		t.tablename = test_table_name;
		dbi.CreateTable(t);
	}
	return 0;
}
