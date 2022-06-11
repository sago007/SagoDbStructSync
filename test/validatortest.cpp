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
#include "../src/sago/DbSyncValidator.hpp"


BOOST_AUTO_TEST_CASE(Validate_Name) {
	sago::database::DbSyncValidator validator;
	//Valid name
	validator.ValidateName("MyTable", "/path/to/error");
	
	//Valid name with lower case, upper case, number and underscore
	validator.ValidateName("My_Table5", "/path/to/error");
	
	//Name may not start with a number
	BOOST_CHECK_THROW(validator.ValidateName("1MyTable", "/path/to/error"), sago::database::DbValidationException);
	
	//Illigal char "."
	BOOST_CHECK_THROW(validator.ValidateName("my.table", "/path/to/error"), sago::database::DbValidationException);
	
	//Too long for some databases (Oracle)
	BOOST_CHECK_THROW(validator.ValidateName("A_TABLE_THAT_IS_WAY_TO_LONG_TO_FIT_IN_DATABASES_WITH_A_30_CHARS_LIMIT", "/path/to/error"), sago::database::DbValidationException);
}