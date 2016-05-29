#ifndef DBSYNCDBORACLE_HPP
#define DBSYNCDBORACLE_HPP

#include "dbsync_db.hpp"
#include <cppdb/frontend.h>

class DbSyncDbOracle : public sago::database::DbSyncDb {
public:
	DbSyncDbOracle(std::shared_ptr<cppdb::session>& sql);
	DbSyncDbOracle(const DbSyncDbOracle& orig);
	virtual ~DbSyncDbOracle();
	
	virtual bool TableExists(const std::string& tablename) override;
private:
	std::shared_ptr<cppdb::session> sql;
};

#endif /* DBSYNCDBORACLE_HPP */

