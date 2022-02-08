/**
 * @file sql.h
 * @brief Contains SQL database module
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <experimental/propagate_const>
#include <memory>
#include <string>
#include <string_view>

namespace klib {

class SqlQuery;

/**
 * @brief Represents a column of data in a row
 */
class Column {
  friend class SqlQuery;

 public:
  Column(const Column &) = delete;
  Column(Column &&) = delete;
  Column &operator=(const Column &) = delete;
  Column &operator=(Column &&) = delete;

  /**
   * @brief Destructor
   */
  ~Column();

  /**
   * @brief Determine whether the data is of null type
   * @return Returns true if it is of null type
   */
  [[nodiscard]] bool is_null() const;

  /**
   * @brief Return the 32 bit integer value of the column
   * @return 32 bit integer value
   */
  [[nodiscard]] std::int32_t as_int32() const;

  /**
   * @brief Return the 64 bit integer value of the column
   * @return 64 bit integer value
   */
  [[nodiscard]] std::int64_t as_int64() const;

  /**
   * @brief Return the double value of the column
   * @return Double value
   */
  [[nodiscard]] double as_double() const;

  /**
   * @brief Return the text value of the column
   * @return Text value
   */
  [[nodiscard]] std::string as_text() const;

  /**
   * @brief Return the blob value of the column
   * @return Blob value
   * @note Automatically use Zstandard algorithm to decompress
   */
  [[nodiscard]] std::string as_blob() const;

 private:
  Column(SqlQuery &sql_query, std::int32_t index);

  class ColumnImpl;
  std::experimental::propagate_const<std::unique_ptr<ColumnImpl>> impl_;
};

class SqlDatabase;

/**
 * @brief Represents a SQL statement
 */
class SqlQuery {
  friend class SqlDatabase;
  friend class Column::ColumnImpl;

 public:
  /**
   * @brief Initialize a SqlQuery with the given database instance
   * @param db: Database instance
   */
  explicit SqlQuery(SqlDatabase &db);

  SqlQuery(const SqlQuery &) = delete;
  SqlQuery(SqlQuery &&) = delete;
  SqlQuery &operator=(const SqlQuery &) = delete;
  SqlQuery &operator=(SqlQuery &&) = delete;

  /**
   * @brief Destructor
   */
  ~SqlQuery();

  /**
   * @brief Destroy the SQL statement saved in the object
   * @see SqlDatabase::vacuum()
   */
  void finalize();

  /**
   * @brief Compile the given SQL statement
   * @param sql: SQL statement
   */
  void prepare(std::string_view sql);

  /**
   * @brief Bind a 32 bit integer value to a parameter "?" in the SQL prepared
   * statement
   * @param index: Serial number (starting from 1)
   * @param value: The value to bind
   */
  void bind(std::int32_t index, std::int32_t value);

  /**
   * @brief Bind a 64 bit integer value to a parameter "?" in the SQL prepared
   * statement
   * @param index: Serial number (starting from 1)
   * @param value: The value to bind
   */
  void bind(std::int32_t index, std::int64_t value);

  /**
   * @brief Bind a double value to a parameter "?" in the SQL prepared statement
   * @param index: Serial number (starting from 1)
   * @param value: The value to bind
   */
  void bind(std::int32_t index, double value);

  /**
   * @brief Bind a text value to a parameter "?" in the SQL prepared statement
   * @param index: Serial number (starting from 1)
   * @param value: The value to bind
   */
  void bind(std::int32_t index, const std::string &value);

  /**
   * @brief Bind a blob value to a parameter "?" in the SQL prepared statement
   * @param index: Serial number (starting from 1)
   * @param value: The value to bind
   * @param size: The size of the value to be bound
   * @note Automatically use Zstandard algorithm to compress
   */
  void bind(std::int32_t index, const char *value, std::size_t size);

  /**
   * @brief Execute a one-step query with no expected result and reset the
   * statement to make it ready for a new execution
   * @return The number of rows modified, inserted or deleted by the most
   * recently completed INSERT, UPDATE or DELETE statement
   */
  std::int32_t exec();

  /**
   * @brief Execute a step of the prepared query to fetch one row of results
   * @return If another row is ready, return true; if the query is completed,
   * return false
   */
  [[nodiscard]] bool next();

  /**
   * @brief Get the name of the specified column in the result
   * @param index: The serial number of the column (starting from 0)
   * @return The name of the specified column in the result
   */
  [[nodiscard]] std::string get_column_name(std::int32_t index);

  /**
   * @brief Return the column data specified by its index
   * @param index: The serial number of the column (starting from 0)
   * @return The column data
   */
  [[nodiscard]] Column get_column(std::int32_t index);

 private:
  class SqlQueryImpl;
  std::experimental::propagate_const<std::unique_ptr<SqlQueryImpl>> impl_;
};

class SqlDatabase {
  friend SqlQuery::SqlQueryImpl;

 public:
  /**
   * @brief Database open mode
   * @note ReadWrite mode will automatically create a file if the database file
   * does not exist
   */
  enum OpenMode { ReadOnly, ReadWrite };

  /**
   * @brief Open the database
   * @param db_name: Database name
   * @param open_mode: Database open mode
   * @param password: Database password
   */
  SqlDatabase(const std::string &db_name, OpenMode open_mode,
              const std::string &password);

  SqlDatabase(const SqlDatabase &) = delete;
  SqlDatabase(SqlDatabase &&) = delete;
  SqlDatabase &operator=(const SqlDatabase &) = delete;
  SqlDatabase &operator=(SqlDatabase &&) = delete;

  /**
   * @brief Destructor
   */
  ~SqlDatabase();

  /**
   * @brief Begin transaction
   */
  void transaction();

  /**
   * @brief Commit changes
   */
  void commit();

  /**
   * @brief Rollback changes
   */
  void rollback();

  /**
   * @brief Rebuild the database file, repacking it into a minimal amount of
   * disk space
   */
  void vacuum();

  /**
   * @brief Determine whether the table exists
   * @param table_name: Table name
   * @return Return true if it exists
   */
  [[nodiscard]] bool table_exists(const std::string &table_name);

  /**
   * @brief Drop the table
   * @param table_name: Table name
   */
  void drop_table(const std::string &table_name);

  /**
   * @brief If the table exists, drop the table
   * @param table_name: Table name
   * @return Return true if dropped
   */
  bool drop_table_if_exists(const std::string &table_name);

  /**
   * @brief Count the number of lines in a given table
   * @param table_name: Table name
   * @return The number of lines
   */
  [[nodiscard]] std::int64_t table_line_count(const std::string &table_name);

  /**
   * @brief Execute a one-step query with no expected result
   * @param sql: SQL statement
   * @return The number of rows modified, inserted or deleted by the most
   * recently completed INSERT, UPDATE or DELETE statement
   */
  std::int32_t exec(std::string_view sql);

 private:
  class SqlDatabaseImpl;
  std::experimental::propagate_const<std::unique_ptr<SqlDatabaseImpl>> impl_;
};

}  // namespace klib
