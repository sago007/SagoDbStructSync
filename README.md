# SagoDbStructSync
Program designed to synchronize database model/structure between databases

## Rationale
Traditional relational databases are a perfect fit in many cases. In cases were high integrity is needed databases are often the best choice.
Unfortunately many traditional databases were designed in a time were the concept of fast development iterations and distributed development was introduced.

Updating an SQL DB is quite hard to automate the update process. Most tools involve writing manual commands that needs to be executed in a specific order and only once.

Modern NoSQL databases have no trouble with modern development practices and are therefore very popular. The problem with most widespread NoSQL databases is that they are optimized for scalability instead of integrity.

SagoDbStructSync is my attempt to create a small program/library that for trivial cases can automate the update task for SQL databases, so that the database choice is based on having the best tool for the job and not based on the database fitting the development method.

```
{
    "dbm": {
        "tables": [
            {
                "tablename": "my_movies",
                "columns": [
                    {
                        "name": "movie_id",
                        "type": "NUMBER",
                        "length": 20,
                        "scale": 0,
                        "nullable": false
                    },
                    {
                        "name": "title",
                        "type": "TEXT",
                        "length": 50,
                        "scale": 0,
                        "nullable": false,
                        "hasDefaultValue": false,
                        "defaultValue": ""
                    }
                ]
            }
        ],
        "unique_constraints": [
            {
                "name": "PRIMARY",
                "tablename": "my_movies",
                "columns": [
                    "movie_id"
                ]
            }
        ],
        "foreign_keys": []
    }
}
```

## License
The source is under the MIT license. Be aware that the final product is under the GPLv2 if compiled against the MySQL library.
