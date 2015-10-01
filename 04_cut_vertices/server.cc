#include "server.h"
#include <iostream>
#include <list>
#include <unordered_map>
#include <unordered_set>

namespace train {

Server::iterator& Server::iterator::operator++() {
  pos_++;
  return *this;
}

bool Server::iterator::operator==(const Server::iterator& other) const {
  return server_ == other.server_ && pos_ == other.pos_;
}

bool Server::iterator::operator!=(const Server::iterator& other) const {
  return !(*this == other);
}

Server::SP Server::iterator::operator*() {
  auto size = server_->connected_.size();
  if (pos_ < size) {
    return server_->connected_[pos_];
  }
  return server_->connected_weak_[pos_ - size].lock();
}

size_t Server::size() {
  return connected_.size() + connected_weak_.size();
}

bool Server::is_connected_to(const Server::SP& other) const {
  auto ptr = shared_from_this();
  for (auto& c : connected_) {
    if (c == ptr) {
      return true;
    }
  }
  for (auto& c : connected_weak_) {
    if (c.lock() == ptr) {
      return true;
    }
  }
  return false;
}

Server::iterator Server::begin() {
  return Server::iterator(shared_from_this(), 0);
}

Server::iterator Server::end() {
  return Server::iterator(shared_from_this(), size());
}

void Server::link(const Server::SP& other) {
  if (is_connected_to(other)) {
    return;
  }
  connected_.push_back(other);
  other->connected_weak_.push_back(shared_from_this());
}

void Server::dfs(
    const std::function<void(const SP& node, const SP& parent)>& visit) {
  std::list<std::pair<SP, SP>> lifo;
  lifo.emplace_back(shared_from_this(), nullptr);
  std::unordered_set<SP> visited;
  while (!lifo.empty()) {
    auto u = lifo.back();
    lifo.pop_back();
    if (visited.find(u.first) != visited.end()) {
      continue;
    }
    visited.insert(u.first);
    // call the specified function to do the useful work
    visit(u.first, u.second);
    // add adjacent vertices
    for (auto v : *u.first) {
      // this check is not strictly neccessary
      if (visited.find(v) == visited.end()) {
        lifo.emplace_back(v, u.first);
      }
    }
  }
}

namespace {
  /// Service information needed by get_cut_vertices()
  struct ServerServiceInfo {
    int date;
    int up;
    std::unordered_set<Server::SP> tree_nodes;
    Server::SP parent;
  };
}  // anonymous namespace

std::vector<Server::SP> Server::get_cut_vertices() {
  std::vector<Server::SP> results;
  int date = 1;
  std::list<Server::SP> traversal;
  std::unordered_map<Server::SP, ServerServiceInfo> info;

  // do depth-first scan, recording the vertices in reverse order
  dfs([&](auto visited, auto parent) {
    traversal.push_front(visited);  // mind push_front => reverse order
    auto& iv = info[visited];
    iv.date = date++;
    iv.parent = parent;
    info[parent].tree_nodes.insert(visited);
  });

  // analyze each vertex
  for (auto& u : traversal) {
    int up = info[u].date;
    auto& iu = info[u];
    for (auto v : *u) {
      if (v == iu.parent) {  // skip the edge to parent
        continue;
      }
      // is the corresponding edge (u, v) a back edge?
      bool is_back = iu.tree_nodes.find(v) == iu.tree_nodes.end();
      up = std::min(up, is_back? info[v].date : info[v].up);
    }
    iu.up = up;
    // check the condition for the cut vertex
    for (auto& v : iu.tree_nodes) {
      if (iu.date <= info[v].up) {
        results.push_back(u);
        break;
      }
    }
  }

  // this is a special condition for the traversal root (this)
  if (info[traversal.back()].tree_nodes.size() == 1) {
    results.pop_back();
  }

  return results;
}

}  // namespace train

using Server = train::Server;  // using-s are allowed in definitions (*.cc)

int main() {
  auto s1 = std::make_shared<Server>(1);
  auto s2 = std::make_shared<Server>(2);
  auto s3 = std::make_shared<Server>(3);
  auto s4 = std::make_shared<Server>(4);
  auto s5 = std::make_shared<Server>(5);
  auto s6 = std::make_shared<Server>(6);

  s1->link(s2);
  s2->link(s3);
  s3->link(s4);
  s2->link(s4);
  s4->link(s5);
  s1->link(s6);
  s6->link(s4);

  std::cout << "Depth-first traversal:" << std::endl;

  s1->dfs([](auto s, auto p) {
    std::cout << s->number() << std::endl;
  });

  std::cout << std::endl << "Found cut vertices:" << std::endl;

  auto cv = s1->get_cut_vertices();
  for (auto& s : cv) {
    std::cout << s->number() << std::endl;
  }
  return 0;
}
