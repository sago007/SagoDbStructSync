/*
  Its is under the MIT license, to encourage reuse by cut-and-paste.

  Copyright (c) 2018 Poul Sander

  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation files
  (the "Software"), to deal in the Software without restriction,
  including without limitation the rights to use, copy, modify, merge,
  publish, distribute, sublicense, and/or sell copies of the Software,
  and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#define BOOST_TEST_MAIN
#if !defined( WIN32 )
#define BOOST_TEST_DYN_LINK
#endif
#include <boost/test/unit_test.hpp>
#include "../src/sago/dbsync.hpp"
#include "../src/sago/dbsync_db.hpp"
#include "../src/sago/DbSyncDbMySql.hpp"

using std::string;
using std::cout;

static string connectstring = "mysql:database=dbsync_test;user=testuser;password=password;opt_reconnect=1";

static void PrintVector(const std::vector<std::string>& v) {
	for (const auto& s : v) {
		cout << s << "\n";
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
	c.type = SagoDbType::TEXT;
	c.length = 50;
	t.columns.push_back(c);

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
	cout << "\n\n";
}
