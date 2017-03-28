#define BOOST_TEST_MAIN
#if !defined( WIN32 )
#define BOOST_TEST_DYN_LINK
#endif
#include <boost/test/unit_test.hpp>
#include "../src/sago/dbsync.hpp"
#include "../src/sago/dbsync_db.hpp"
#include "../src/sago/DbSyncDbMySql.hpp"
#include <cereal/archives/json.hpp>

using std::string;
using std::cout;

static string connectstring = "mysql:database=dbsync_test;user=testuser;password=password;opt_reconnect=1";

static void PrintVector(const std::vector<std::string>& v) {
	for (const auto& s : v) {
		cout << s << "\n";
	}
}

static void PrintColumn(const sago::database::DbColumn& c) {
	{
		cereal::JSONOutputArchive archive(cout);
		archive(cereal::make_nvp("c", c));
	}
}

static void PrintTable(const sago::database::DbTable& t) {
	{
		cereal::JSONOutputArchive archive(cout);
		archive(cereal::make_nvp("table", t));
	}
}

/*
 This test just ensures that the thing actually compiles. 
 */
BOOST_AUTO_TEST_CASE(It_can_connect) {
	std::string schema_name = "dbsync_test";
	std::shared_ptr<cppdb::session> db(new cppdb::session(connectstring));
	std::shared_ptr<sago::database::DbSyncDb> dbi;
	{
		dbi = std::shared_ptr<sago::database::DbSyncDb>(new sago::database::DbSyncDbMySql(db, schema_name));
	}

	sago::database::DbTable t;
	t.tablename = "MyTable";
	sago::database::DbColumn c;
	c.name = "name";
	c.type = sago::database::DbType::TEXT;
	c.length = 50;
	t.columns.push_back(c);
	{
		cereal::JSONOutputArchive archive(cout);
		archive(cereal::make_nvp("total", t));
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
	PrintColumn(dbi->GetColumn("my_test_table", "name"));
	cout << "Table:\n";
	PrintTable(dbi->GetTable("my_test_table"));
	cout << "\n\n";
}