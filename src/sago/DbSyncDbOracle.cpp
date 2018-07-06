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

#include "DbSyncDbOracle.hpp"


namespace sago {
	namespace database {

		DbSyncDbOracle::DbSyncDbOracle(std::shared_ptr<cppdb::session>& sql) : sql(sql) {
		}

		DbSyncDbOracle::DbSyncDbOracle(const DbSyncDbOracle& orig) {
		}

		DbSyncDbOracle::~DbSyncDbOracle() {
		}

		bool DbSyncDbOracle::TableExists(const std::string& tablename) {
			cppdb::result res = *sql << "SELECT 1 FROM all_tables WHERE OWNER = sys_context('userenv','current_schema') AND TABLE_NAME = ?" << tablename;
			if (res.next()) {
				return true;
			}
			return false;
		}

	} //namespace database
} //namespace sago