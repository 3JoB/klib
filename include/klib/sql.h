#pragma once

#include <cstddef>
#include <cstdint>
#include <experimental/propagate_const>
#include <memory>
#include <string>
#include <string_view>

namespace klib {

class SqlQuery;

class Column {
  friend class SqlQuery;

 public:
  Column(const Column &) = delete;
  Column(Column &&) = delete;
  Column &operator=(const Column &) = delete;
  Column &operator=(Column &&) = delete;
  ~Column();

  [[nodiscard]] bool is_null() const;

  [[nodiscard]] std::int32_t as_int32() const;
  [[nodiscard]] std::int64_t as_int64() const;
  [[nodiscard]] double as_double() const;
  [[nodiscard]] std::string as_string() const;
  [[nodiscard]] std::string as_blob() const;

 private:
  Column(SqlQuery &sql_query, std::int32_t index);

  class ColumnImpl;
  std::experimental::propagate_const<std::unique_ptr<ColumnImpl>> impl_;
};

class SqlDatabase;

class SqlQuery {
  friend class SqlDatabase;
  friend class Column::ColumnImpl;

 public:
  explicit SqlQuery(const SqlDatabase &database);

  SqlQuery(const SqlQuery &) = delete;
  SqlQuery(SqlQuery &&) = delete;
  SqlQuery &operator=(const SqlQuery &) = delete;
  SqlQuery &operator=(SqlQuery &&) = delete;
  ~SqlQuery();

  void finalize();

  std::string get_column_name(std::int32_t index);

  void prepare(std::string_view sql);

  void bind(std::int32_t index, std::int32_t value);
  void bind(std::int32_t index, std::int64_t value);
  void bind(std::int32_t index, double value);
  void bind(std::int32_t index, const std::string &value);
  void bind(std::int32_t index, const char *value, std::size_t size);

  void step();

  [[nodiscard]] bool next();

  [[nodiscard]] Column get_column(std::int32_t index);

 private:
  class SqlQueryImpl;
  std::experimental::propagate_const<std::unique_ptr<SqlQueryImpl>> impl_;
};

class SqlDatabase {
  friend SqlQuery::SqlQueryImpl;

 public:
  enum OpenType { ReadOnly, ReadWrite };

  SqlDatabase(const std::string &table_name, OpenType open_type,
              const std::string &key);

  SqlDatabase(const SqlDatabase &) = delete;
  SqlDatabase(SqlDatabase &&) = delete;
  SqlDatabase &operator=(const SqlDatabase &) = delete;
  SqlDatabase &operator=(SqlDatabase &&) = delete;
  ~SqlDatabase();

  void transaction();
  void commit();
  void rollback();
  void vacuum();

  void drop_table(const std::string &table_name);
  void drop_table_if_exists(const std::string &table_name);

  [[nodiscard]] bool table_exists(const std::string &table_name);
  std::int64_t table_line_count(const std::string &table_name);

  std::int32_t exec(std::string_view sql);

 private:
  class SqlDatabaseImpl;
  std::experimental::propagate_const<std::unique_ptr<SqlDatabaseImpl>> impl_;
};

}  // namespace klib