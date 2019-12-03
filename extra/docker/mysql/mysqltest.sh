set -e

# Start the MySQL daemon in the background.
echo "Starting MySQL"
/usr/sbin/mysqld &
mysql_pid=$!

until mysqladmin ping >/dev/null 2>&1; do
  echo -n "."; sleep 0.2
done
echo "MySQL has been started"


#Do stuff here!
mysql -e "create database testdb"
mysql --database=testdb -e "CREATE TABLE IF NOT EXISTS tasks (
    task_id INT AUTO_INCREMENT PRIMARY KEY,
    title VARCHAR(255) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
)"

mysql --database=testdb -e "SELECT COLUMN_NAME,DATA_TYPE,CHARACTER_MAXIMUM_LENGTH,NUMERIC_PRECISION,NUMERIC_SCALE,IS_NULLABLE,COLUMN_DEFAULT IS NOT NULL,COLUMN_DEFAULT,EXTRA
				FROM INFORMATION_SCHEMA.COLUMNS
				WHERE table_schema = 'testdb' AND TABLE_NAME = 'tasks'"

cd /staging
./dbsync -o "" --connectstring "mysql:user=root;database=testdb" | tee /output.json

mysql -e "create database testdb2"

./dbsync -i "/output.json" --connectstring "mysql:user=root;database=testdb2"

mysql --database=testdb2 -e "SELECT COLUMN_NAME,DATA_TYPE,CHARACTER_MAXIMUM_LENGTH,NUMERIC_PRECISION,NUMERIC_SCALE,IS_NULLABLE,COLUMN_DEFAULT IS NOT NULL,COLUMN_DEFAULT,EXTRA
				FROM INFORMATION_SCHEMA.COLUMNS
				WHERE table_schema = 'testdb2' AND TABLE_NAME = 'tasks'"

echo "Shutting down MySQL"
# Tell the MySQL daemon to shutdown.
mysqladmin shutdown

# Wait for the MySQL daemon to exit.
wait $mysql_pid
echo "done"
