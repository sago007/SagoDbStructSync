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

#include "dbsync_db.hpp"

namespace sago {
	namespace database {

		void ApplyDataModel(const DbDatabaseModel& model, DbSyncDb& db) {
			for (const DbTable& t : model.tables) {
				db.CreateTable(t);
			}
			for (const DbUniqueConstraint& uc : model.unique_constraints) {
				db.CreateUniqueConstraint(uc);
			}
			for (const DbForeignKeyConstraint& fk : model.foreign_keys) {
				db.CreateForeignKeyConstraint(fk);
			}
		}

		DbDatabaseModel ExtractDataModel(DbSyncDb& db) {
			DbDatabaseModel ret;
			std::vector<std::string> tablenames = db.GetTableNames();
			ret = ExtractDataModelForTables(db, tablenames);
			return ret;
		}

		DbDatabaseModel ExtractDataModelForTables(DbSyncDb& db, const std::vector<std::string>& table_names) {
			DbDatabaseModel ret;
			std::cerr << "Extracting\n";
			for (const std::string& s : table_names) {
				ret.tables.push_back(db.GetTable(s));
				std::vector<std::string> unique_constraint_names = db.GetUniqueConstraintNamesForTable(s);
				std::vector<std::string> unique_foreign_keys_names = db.GetForeignKeyNamesForTable(s);
				for (const std::string& un : unique_constraint_names) {
					ret.unique_constraints.push_back(db.GetUniqueConstraint(s, un));
				}
				for (const std::string& fn : unique_foreign_keys_names) {
					ret.foreign_keys.push_back(db.GetForeignKeyConstraint(s, fn));
				}
			}
			return ret;
		}

	}
}