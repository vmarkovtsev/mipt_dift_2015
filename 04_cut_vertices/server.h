#ifndef CUT_VERTICES_SERVER_H_
#define CUT_VERTICES_SERVER_H_

#include <functional>  // std::function
#include <memory>  // std::shared_ptr
#include <vector>

namespace train {

class Server : public std::enable_shared_from_this<Server> {
 public:
  using SP = std::shared_ptr<Server>;

  /// Iterator class returned by Server::begin() and Server::end()
  class iterator {
   public:
    iterator& operator++();
    bool operator==(const iterator& other) const;
    bool operator!=(const iterator& other) const;
    SP operator*();

   private:
    friend class Server;
    iterator(SP server, size_t pos) : server_(server), pos_(pos) {}

    SP server_;
    size_t pos_;
  };

  Server(int number) : number_(number) {}
  /// Useful payload - a number.
  int number() { return number_; }
  /// The number of servers this one is connected to.
  size_t size();
  bool is_connected_to(const SP& other) const;
  // the following two methods allow to iterate Server
  iterator begin();
  iterator end();
  /// Link one server to another.
  void link(const SP& other);
  /// Do depth-first graph traversal.
  void dfs(const std::function<void(const SP& node, const SP& parent)>& visit);
  /// Returns the cut vertices of the graph this server belongs to.
  std::vector<SP> get_cut_vertices();

 private:
  std::vector<SP> connected_;
  std::vector<std::weak_ptr<Server>> connected_weak_;
  int number_;
};

}  // namespace train

#endif  // CUT_VERTICES_SERVER_H_
