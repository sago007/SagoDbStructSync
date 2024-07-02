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

#ifndef DBSYNC_DB_HPP
#define DBSYNC_DB_HPP

#include "dbsync.hpp"

namespace sago {
	namespace database {

		class DbSyncDb {
		public:

			virtual bool TableExists(const std::string& tablename) {
				return false;
			}

			virtual bool ColumnExists(const std::string& tablename, const std::string& columnname) {
				return false;
			}

			virtual bool UniqueConstraintExists(const std::string& tablename, const std::string& name) {
				return false;
			}

			virtual bool ForeignKeyExists(const std::string& tablename, const std::string& name) {
				return false;
			}

			virtual bool SchemaExists(const std::string& schemaname) {
				return false;
			}

			virtual std::vector<std::string> GetTableNames() {
				return std::vector<std::string>();
			}

			virtual std::vector<std::string> GetColoumNamesFromTable(const std::string& tablename) {
				return std::vector<std::string>();
			}

			virtual std::vector<std::string> GetUniqueConstraintNamesForTable(const std::string& tablename) {
				return std::vector<std::string>();
			}

			virtual std::vector<std::string> GetForeignKeyNamesForTable(const std::string& tablename) {
				return std::vector<std::string>();
			}

			virtual DbColumn GetColumn(const std::string& tablename, const std::string& columnname) {
				return DbColumn();
			}

			virtual DbTable GetTable(const std::string& tablename) {
				return DbTable();
			}

			virtual DbUniqueConstraint GetUniqueConstraint(const std::string& tablename, const std::string& name) {
				return DbUniqueConstraint();
			}

			virtual DbForeignKeyConstraint GetForeignKeyConstraint(const std::string& tablename, const std::string& name) {
				return DbForeignKeyConstraint();
			}

			/**
			 * Create a table in the database
			 * @param t The table to create
			 * @param foreign_keys All the foreign keys in the database. The list might contain foreign keys that are not for this table.
			*/
			virtual void CreateTable(const DbTable& t, const std::vector<DbForeignKeyConstraint>& foreign_keys) {
				throw DbException("Not implemented", "CreateTable not implemented", t.tablename, "");
			}

			virtual void CreateColumn(const std::string& tablename, const DbColumn& c) {
				throw DbException("Not implemented", "CreateColumn not implemented", tablename, "");
			}

			virtual void CreateUniqueConstraint(const DbUniqueConstraint& c) {
				throw DbException("Not implemented", "CreateUniqueConstraint not implemented", c.tablename, "");
			}

			virtual void CreateForeignKeyConstraint(const DbForeignKeyConstraint& c) {
				throw DbException("Not implemented", "CreateForeignKeyConstraint not implemented", c.tablename, "");
			}

			virtual ~DbSyncDb() {
			};
		};

		void ApplyDataModel(const DbDatabaseModel& model, DbSyncDb& db);
		DbDatabaseModel ExtractDataModel(DbSyncDb& db);
		DbDatabaseModel ExtractDataModelForTables(DbSyncDb& db, const std::vector<std::string>& table_names);

	}
}

#endif /* DBSYNC_DB_HPP */

