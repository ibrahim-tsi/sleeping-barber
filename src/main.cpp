#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <chrono>
#include <queue>
#include <iostream>
#include <string_view>
#include <sstream>

#include "util.hpp"

std::condition_variable cv{};
std::mutex cv_mtx{};
std::mutex cout_mtx{};
std::mutex customers_mtx{};
bool barber_notified{false};

class Customer {
  std::string _name{};

public:
  const std::string name() { return _name; }
  
  Customer(std::string name) : _name(name) {}
};

void print(const std::stringstream& ss) {
  std::lock_guard<std::mutex> cout_lock(cout_mtx);
  std::cout << ss.str();
}

void print(std::string str) {
  std::lock_guard<std::mutex> cout_lock(cout_mtx);
  std::cout << str;
}

class Barber {
  std::string _name{};

public:
  const std::string name() { return _name; }

  Barber(std::string name) : _name(name) {}

  void work(std::queue<Customer>& customers) {
    while (true) {
      std::unique_lock<std::mutex> cv_lock(cv_mtx);

      while (!barber_notified)
        cv.wait(cv_lock);

      barber_notified = false;
      cv_lock.unlock();
      
      std::stringstream output{};
      output << "Barber " << _name << " awoke\n";
      print(output);

      while (!customers.empty()) {
        std::unique_lock<std::mutex> customers_lock(customers_mtx);

        std::string customer_name{customers.front().name()};

        customers.pop();
        customers_lock.unlock();
        output.str("");
        
        output << "Barber " << _name << " cutting " << customer_name <<
          "'s hair...\n";
        
        print(output);

        int seconds{util::rnd_range(5, 10)};

        std::this_thread::sleep_for(std::chrono::seconds(seconds));
        output.str("");
        output << "Barber " << _name << " finished cutting (" << std::to_string(seconds) << " seconds)\n";
        print(output);
      }

      output.str("");
      output << "Barber " << _name << " sleeping\n";
      print(output);
    }
  }
};

void notify_barber() {
  std::unique_lock<std::mutex> lock(cv_mtx);

  barber_notified = true;

  cv.notify_one();
}

const std::string_view names[] {
  "Leo",
  "Eli",
  "Ian",
  "Kai",
  "Max",
  "Ace",
  "Jax",
  "Ali",
  "Jay",
  "Ari",
  "Dax",
  "Asa",
  "Roy",
  "Koa",
  "Sam",
  "Noe",
  "Rex",
  "Ray",
  "Joe",
  "Axl",
  "Lee",
  "Ben",
  "Rio",
  "Ira",
  "Kye",
  "Jon",
  "Van",
  "Rey",
  "Avi",
  "Fox",
  "Zev",
  "Mac",
  "Cal"
};

int main() {
  const int barbers_count{5};
  const int waiting_seats_count{5};
  std::queue<Customer> customers{};

  auto rnd_name = []() {
    return names[util::rnd_range(0, ARR_SIZE(names) - 1)];
  };

  std::vector<std::future<void>> workers(barbers_count);

  for (int i = 0; i < barbers_count; i++) {
    workers.push_back(std::async(
      std::launch::async,
      Barber::work, Barber(std::string(rnd_name())), std::ref(customers)
    ));
  }

  while(true) {
    if (customers.size() == waiting_seats_count)
      continue;

    int seconds{util::rnd_range(0, 10)};

    std::this_thread::sleep_for(std::chrono::seconds(seconds));

    std::string name{std::string(rnd_name())};
    std::stringstream output{};
    output << "Customer " << name << " is entering the shop\n";
    print(output);
    
    {
      std::lock_guard<std::mutex> customers_lock(customers_mtx);
      customers.push({std::string(name)});
    }

    notify_barber();
  }
  
  return 0;
}