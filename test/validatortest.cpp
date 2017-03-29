#define BOOST_TEST_MAIN
#if !defined( WIN32 )
#define BOOST_TEST_DYN_LINK
#endif
#include <boost/test/unit_test.hpp>
#include "../src/sago/dbsync.hpp"
#include "../src/sago/DbSyncValidator.hpp"
#include <cereal/archives/json.hpp>


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