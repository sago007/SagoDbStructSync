/*
  Its is under the MIT license, to encourage reuse by cut-and-paste.

  Copyright (c) 2022 Poul Sander

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

#pragma once

#include "dbsync_db.hpp"
#include <cppdb/frontend.h>

namespace sago {
	namespace database {

		class DbSyncDbSqlite : public sago::database::DbSyncDb {
		public:
			DbSyncDbSqlite(std::shared_ptr<cppdb::session>& sql);
			DbSyncDbSqlite(const DbSyncDbSqlite& orig);
			virtual ~DbSyncDbSqlite();

			virtual bool TableExists(const std::string& tablename) override;
			virtual bool ColumnExists(const std::string& tablename, const std::string& columnname) override;
			virtual bool UniqueConstraintExists(const std::string& tablename, const std::string& name) override;
			virtual bool ForeignKeyExists(const std::string& tablename, const std::string& name) override;
			virtual bool SchemaExists(const std::string& schemaname) override;

			virtual void CreateTable(const sago::database::DbTable& t) override;

			virtual void CreateUniqueConstraint(const sago::database::DbUniqueConstraint& c) override;
		private:
			std::shared_ptr<cppdb::session> sql;
		};

	} //namespace database
} //namespace sago


