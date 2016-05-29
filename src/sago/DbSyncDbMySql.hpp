#ifndef DBSYNCDBMYSQL_HPP
#define DBSYNCDBMYSQL_HPP

#include "dbsync_db.hpp"
#include <cppdb/frontend.h>

class DbSyncDbMySql : public sago::database::DbSyncDb {
public:
	DbSyncDbMySql(std::shared_ptr<cppdb::session>& sql, const std::string& schema);
	DbSyncDbMySql(const DbSyncDbMySql& orig);
	virtual ~DbSyncDbMySql();
	
	virtual bool TableExists(const std::string& tablename) override;
	virtual bool ColumnExists(const std::string& tablename, const std::string& columnname) override;
	virtual bool UniqueConstraintExists(const std::string& tablename, const std::string& name) override;
	virtual bool ForeignKeyExists(const std::string& tablename, const std::string& name) override;
	
	virtual std::vector<std::string> GetTableNames() override;
	virtual std::vector<std::string> GetColoumNamesFromTable(const std::string& tablename) override;
	virtual std::vector<std::string> GetUniqueConstraintNamesForTable(const std::string& tablename) override;
	virtual std::vector<std::string> GetForeignKeyNamesForTable(const std::string& tablename) override;
	
	virtual sago::database::DbColumn GetColumn(const std::string& tablename, const std::string& columnname) override;
	virtual sago::database::DbTable GetTable(const std::string& tablename) override;
	virtual sago::database::DbUniqueConstraint GetUniqueConstraint(const std::string& tablename, const std::string& name) override;
	virtual sago::database::DbForeignKeyConstraint GetForeignKeyConstraint(const std::string& tablename, const std::string& name) override;
	
	virtual void CreateTable(const sago::database::DbTable& t) override;
	virtual void CreateColumn(const std::string& tablename, const sago::database::DbColumn& c) override;
	virtual void CreateUniqueConstraint(const sago::database::DbUniqueConstraint& c) override;
	virtual void CreateForeignKeyConstraint(const sago::database::DbForeignKeyConstraint& c) override;
private:
	std::shared_ptr<cppdb::session> sql;
	std::string schema;
	std::string sago_id = "SAGO_ID";
	std::string sago_prefix = "SAGO_";
	
};

#endif /* DBSYNCDBMYSQL_HPP */

