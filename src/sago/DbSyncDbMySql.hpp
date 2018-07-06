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

#ifndef DBSYNCDBMYSQL_HPP
#define DBSYNCDBMYSQL_HPP

#include "dbsync_db.hpp"
#include <cppdb/frontend.h>


namespace sago {
	namespace database {

		class DbSyncDbMySql : public sago::database::DbSyncDb {
		public:
			DbSyncDbMySql(std::shared_ptr<cppdb::session>& sql, const std::string& schema);
			DbSyncDbMySql(const DbSyncDbMySql& orig);
			virtual ~DbSyncDbMySql() override;

			virtual bool TableExists(const std::string& tablename) override;
			virtual bool ColumnExists(const std::string& tablename, const std::string& columnname) override;
			virtual bool UniqueConstraintExists(const std::string& tablename, const std::string& name) override;
			virtual bool ForeignKeyExists(const std::string& tablename, const std::string& name) override;

			virtual std::vector<std::string> GetTableNames() override;
			virtual std::vector<std::string> GetColoumNamesFromTable(const std::string& tablename) override;
			virtual std::vector<std::string> GetUniqueConstraintNamesForTable(const std::string& tablename) override;
			virtual std::vector<std::string> GetForeignKeyNamesForTable(const std::string& tablename) override;

			virtual sago::database::DbColumn GetColumn(const std::string& tablename, const std::string& columnname) override;
			virtual sago::database::DbTable GetTable(const std::string& tablename) override;
			virtual sago::database::DbUniqueConstraint GetUniqueConstraint(const std::string& tablename, const std::string& name) override;
			virtual sago::database::DbForeignKeyConstraint GetForeignKeyConstraint(const std::string& tablename, const std::string& name) override;

			virtual void CreateTable(const sago::database::DbTable& t) override;
			virtual void CreateColumn(const std::string& tablename, const sago::database::DbColumn& c) override;
			virtual void CreateUniqueConstraint(const sago::database::DbUniqueConstraint& c) override;
			virtual void CreateForeignKeyConstraint(const sago::database::DbForeignKeyConstraint& c) override;
		private:
			std::shared_ptr<cppdb::session> sql;
			std::string schema;
			std::string sago_id = "SAGO_ID";
			std::string sago_prefix = "SAGO_";

		};

	} //namespace database
} //namespace sago 

#endif /* DBSYNCDBMYSQL_HPP */

