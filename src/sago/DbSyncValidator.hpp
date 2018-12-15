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

#pragma once

#include "dbsync.hpp"


namespace sago {
	namespace database {

		class DbValidationException : public DbException {
			using DbException::DbException;
		};

		class DbSyncValidator {
		public:
			std::vector<DbValidationException> validationErrors;

			void ValidateModel(const DbDatabaseModel& model);
			void ValidateTable(const DbTable& table);
			void ValidateColumn(const DbColumn& column, const DbTable& table);
			void ValidateForeignKey(const DbForeignKeyConstraint& constraint);
			void ValidateUniqueConstraint(const DbUniqueConstraint& constraint);
			void ValidateName(const std::string& name, const std::string& path_for_use_in_the_error_message);
			bool checkNamesCase = true;
			bool checkNamesChars = true;
			bool checkNamesLength = false;
			int nameMaxLength = 30;
		private:
			std::string schemaNameBeingValidated = "";
			void ValidateDuplicates();
		};

	} //namespace database
} //namespace sago