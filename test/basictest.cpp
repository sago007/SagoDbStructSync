#define BOOST_TEST_MAIN
#if !defined( WIN32 )
    #define BOOST_TEST_DYN_LINK
#endif
#include <boost/test/unit_test.hpp>
#include "../src/sago/dbsync.hpp"
#include <cereal/archives/json.hpp>

/*
 This test just ensures that the thing actually compiles. 
 */
BOOST_AUTO_TEST_CASE( It_compiles ) {
	sago::database::DbTable t;
	t.tablename = "MyTable";
	sago::database::DbColumn c;
	c.name = "name";
	c.type = sago::database::DbType::TEXT;
	c.length = 50;
	t.columns.push_back(c);
	{
		cereal::JSONOutputArchive archive( std::cerr );
		archive ( cereal::make_nvp("total",t));
	}
}