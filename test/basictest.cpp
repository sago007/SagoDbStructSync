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

/*
 This test just ensures that the thing actually compiles.
 */
BOOST_AUTO_TEST_CASE(It_compiles) {
	sago::database::DbTable t;
	t.tablename = "MyTable";
	sago::database::DbColumn c;
	c.name = "name";
	c.type = SagoDbType::TEXT;
	c.length = 50;
	t.columns.push_back(c);
  {
    std::string pretty_json = JS::serializeStruct(t);
    std::cerr << pretty_json << "\n";
	}
}
